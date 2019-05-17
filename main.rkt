#lang racket/base
(require (for-syntax racket/base
                     syntax/parse)
         syntax/parse/define
         racket/lazy-require
         racket/list
         racket/undefined
         racket/match
         racket/contract
         struct-define
         (prefix-in gr: gregor))
(module+ test
  (require chk))

;; Format

(define SIGNED #t)
(define UNSIGNED #f)
(define LITTLE-ENDIAN #f)

(define   8bit  1)
(define  16bit  2)
(define  32bit  4)
(define  64bit  8)
(define 128bit 16)
(define 256bit 32)

(define shebang-len 128)
(define lang-len 12)
(define filemeta-len 82)

(define-syntax define-offset-sequence
  (syntax-parser
    [(_ s:expr x:id) #'(define x s)]
    [(_ s:expr [x:id sz:expr] . more)
     #'(begin (define x s)
              (define-offset-sequence (+ x sz) . more))]))
(define-offset-sequence 0
  [shebang-off shebang-len]
  [lang-off lang-len]
  [version-off 8bit]
  [flags-off 8bit]
  [free-bpt-off 64bit]
  [free-opt-off 64bit]
  [root-obj-off 64bit]
  [glob-obj-off 64bit]
  [filemeta-off filemeta-len]
  first-block-off)
(module+ test
  (chk first-block-off 256))

(define shebang-re
  (byte-pregexp
   (bytes-append #"^#![^\n]{"
                 (string->bytes/utf-8 (number->string (- shebang-len 3)))
                 #"}\n")))
(define (shebang-pad s)
  (bytes-append #"#!" s (make-bytes (- shebang-len (+ (bytes-length s) 3))) #"\n"))
(define shebang-default-bs
  (shebang-pad #"/usr/bin/env runic"))
(module+ test
  (chk #:t (regexp-match shebang-re shebang-default-bs)
       (bytes-length shebang-default-bs) shebang-len))

(define lang-bs #"#lang runic ")
(module+ test
  (chk (bytes-length lang-bs) lang-len))
(define version-bs (bytes 0))

(define flag-offset-mask  #b11000000)
(define flag-offset-16bit #b00000000)
(define flag-offset-32bit #b01000000)
(define flag-offset-64bit #b10000000)
(define flags-default-bs (bytes flag-offset-16bit))

(define block-size (expt 2 12))

(define filemeta-default-bs (make-bytes filemeta-len 0))

(define block-header-size (* 2 64bit))

;; Value Helpers

(define time-s? gr:datetime?)
(define (time-s-in bs) (gr:posix->datetime (integer-bytes->integer bs SIGNED LITTLE-ENDIAN)))
(define (time-s-out x) (integer->integer-bytes (floor (gr:->posix x)) 64bit SIGNED LITTLE-ENDIAN))

(define time-s+ns? gr:datetime?)
(define (time-s+ns-in bs)
  (define dt (gr:posix->datetime (integer-bytes->integer bs SIGNED LITTLE-ENDIAN 0 3)))
  (gr:+nanoseconds dt (integer-bytes->integer bs SIGNED LITTLE-ENDIAN 4 7)))
(define (time-s+ns-out x)
  (bytes-append (time-s-out x)
                (integer->integer-bytes (gr:->nanoseconds x) 64bit SIGNED LITTLE-ENDIAN)))

;; Type definers
(define-simple-macro (type-constant (quote t:id) bits:bytes val:expr)
  (begin
    (define (? x) (eq? val x))
    (define (in bs) val)
    (define (out x) #"")
    (type-atom 't bits 0 ? in out)))
(define-simple-macro (type-int (quote t:id) bits:bytes signed?:boolean len:id)
  (begin
    (define (? x) (and (integer? x) (<= (integer-length x) len)))
    (define (in bs) (integer-bytes->integer bs signed? LITTLE-ENDIAN))
    (define (out x) (integer->integer-bytes x len signed? LITTLE-ENDIAN))
    (type-atom 't bits len ? in out)))
(define-simple-macro (type-flo (quote t:id) bits:bytes len:id)
  (begin
    (define ?
      (match len
        [4 single-flonum?]
        [8 flonum?]
        [_ (error 'type-flo "Doesn't support len ~e" len)]))
    (define (in bs) (floating-point-bytes->real bs LITTLE-ENDIAN))
    (define (out x) (real->floating-point-bytes x len LITTLE-ENDIAN))
    (type-atom 't bits len ? in out)))

(define ATOM-ts '())
(define ATOM->? (make-hasheq))
(define ATOM->IN (make-hasheq))
(define ATOM->OUT (make-hasheq))
(define-simple-macro (type-atom (quote t:id) bits:bytes len:expr ?:id in:id out:id)
  (begin
    (set! ATOM-ts (append ATOM-ts (list 't)))
    (hash-set! ATOM->? 't ?)
    (hash-set! ATOM->IN 't in)
    (hash-set! ATOM->OUT 't out)
    (type-tag! 't bits len)))

(define RAW-TYPES '())
(define-simple-macro (type-raw (quote t:id) bits:bytes len:expr)
  (begin
    (set! RAW-TYPES (cons 't RAW-TYPES))
    (type-tag! 't bits len)))

(define-simple-macro (type (quote t:id) bits:bytes len:expr ?:id)
  (begin
    (define (? rv) (and (Rval? rv) (eq? (Rval-type rv) 't)))
    (type-tag! 't bits len)))

(struct tag-info (mask tag type))
(struct len:vlq (bits))
(struct len:con (n))
(struct len:offset (k))
(define TYPE-TAGS '())
(define TYPE->LEN (make-hasheq))

(define (tag-insert ti l)
  (cond
    [(empty? l) (list ti)]
    [(> (tag-info-mask ti)
        (tag-info-mask (first l)))
     (cons ti l)]
    [else
     (cons (first l) (tag-insert ti (rest l)))]))
(define (type-tag! ty bits len)
  (define-values (m t vlq _)
    (for/fold ([m 0] [t 0] [vlq 0] [i (expt 2 7)])
              ([b (in-bytes bits)])
      (match (integer->char b)
        [#\0     (values (+ m i)    t       vlq    (/ i 2))]
        [#\1     (values (+ m i) (+ t i)    vlq    (/ i 2))]
        [#\_     (values    m       t    (+ vlq 1) (/ i 2))]
        [#\space (values    m       t       vlq       i   )])))
  (define ti (tag-info m t ty))
  (hash-set! TYPE->LEN
             t
             (match len
               ['VLQ (len:vlq vlq)]
               ['W (len:offset 1)]
               ['2W (len:offset 2)]
               [(? number? n) (len:con n)]))
  (set! TYPE-TAGS (tag-insert ti TYPE-TAGS)))

;; Types

#;(define (type-raw bit-string rlen t)
    (define (? x) (and (Rraw? x) (eq? (Rraw-t x) t)))
    (define (in f o len)
      (if (integer? rlen)
        (Rraw f o t)
        (Rrw* f o t len)))
    (type #:? ? #:bit bit-string #:length rlen #:in in))

(type-constant 'FALSE #"0000 0000" #f)
(type-constant  'TRUE #"0000 0001" #t)
(type-int         'U8 #"0000 0010" #f   8bit)
(type-int         'S8 #"0000 0011" #t   8bit)
(type-int        'U16 #"0000 0100" #f  16bit)
(type-int        'S16 #"0000 0101" #t  16bit)
(type-int        'U32 #"0000 0110" #f  32bit)
(type-int        'S32 #"0000 0111" #t  32bit)
(type-int        'U64 #"0000 1000" #f  64bit)
(type-int        'S64 #"0000 1001" #t  64bit)
(type-raw       'U128 #"0000 1010"    128bit)
(type-raw       'S128 #"0000 1011"    128bit)
(type-raw       'U256 #"0000 1100"    256bit)
(type-raw       'S256 #"0000 1101"    256bit)
(type-atom    'TIME64 #"0000 1110" 64bit time-s? time-s-in time-s-out)
(type-atom   'TIME128 #"0000 1111" 128bit time-s+ns? time-s+ns-in time-s+ns-out)
(type-constant  'NULL #"0001 0000" '())
(type-constant  'VOID #"0001 0001" (void))
(type-constant 'UNDEF #"0001 0010" undefined)
(type            'BOX #"0001 0011" 'W Rbox?)
(type-raw        'F16 #"0001 0100"  16bit)
(type-flo        'F32 #"0001 0101"  32bit)
(type-flo        'F64 #"0001 0110"  64bit)
(type-raw       'F128 #"0001 0111" 128bit)
(type-raw       'F256 #"0001 1000" 256bit)
(type-raw       'FD32 #"0001 1001"  32bit)
(type-raw       'FD64 #"0001 1010"  64bit)
(type-raw      'FD128 #"0001 1011" 128bit)
(type-raw       'FB16 #"0001 1100"  16bit)
(type           'CONS #"0001 1101" '2W Rcons?)
;; Unallocated        #"0001 1110"
;; Unallocated        #"0001 1111"
(type-raw     'BIGNUM #"0010 ____" 'VLQ)
(type-raw     'BIGFLO #"0011 ____" 'VLQ)
(type            'VEC #"01__ ____" 'VLQ Rvec?)
(type            'STR #"10__ ____" 'VLQ Rstr?)
(type          'BYTES #"11__ ____" 'VLQ Rbytes?)

(define Rval-type/c
  (apply symbols (map tag-info-type TYPE-TAGS)))

;; Values

(struct Roff ())
(struct Rval Roff (f o))
(struct Rglobal Roff (i))

(define (Rval-tagb rv)
  (match-define (Rval f o) rv)
  (Rfile-read1 f o))
(define (Rval-type rv)
  (define b (Rval-tagb rv))
  (for/or ([m*t (in-list TYPE-TAGS)])
    (match-define (tag-info m t ty) m*t)
    (and (= (bitwise-and m b) t)
         ty)))
(define (Rval-bs-start rv) 'XXX)
(define (Rval-len rv) 'XXX)
(define (Rval-bs rv) (Rval-bs-ref* rv 0 (Rval-len rv)))
(define (Rval-bs-ref* rv off len)
  (match-define (Rval f _) rv)
  (Rfile-read f (+ (Rval-bs-start rv) off) len))
(define (Rval-bs-set*! rv off bs)
  (match-define (Rval f _) rv)
  (Rfile-write! f (+ (Rval-bs-start rv) off) bs))
(define (Rval-bs-oref rv idx)
  (match-define (Rval f _) rv)
  (define offset-len (Rfile-offset-len f))
  (bs->Roff f (Rval-bs-ref* rv (* idx offset-len) offset-len)))
(define (Rval-bs-oset! rv idx o)
  (match-define (Rval f _) rv)
  (define offset-len (Rfile-offset-len f))
  (Rval-bs-ref* rv (* idx offset-len) (Roff->bs f o)))
(define (Rval-bs-ref rv off) (bytes-ref (Rval-bs-ref* rv off 1) 0))
(define (Rval-bs-set! rv off b) (Rval-bs-set*! rv off (bytes b)))

;; XXX Make functions that override what an Rval currently is and turn
;; it into something else, if the space is big enough.

(define (Ratom? rv) (and (Rval? rv) (hash-has-key? ATOM->IN (Rval-type rv))))
(define (Ratom f v)
  (define t
    (for/or ([t (in-list ATOM-ts)])
      (and ((hash-ref ATOM->? t) v) t)))
  (define rv (Rfile-alloc! f t))
  (Ratom-val! rv v)
  rv)
(define (Ratom-val rv)
  ((hash-ref ATOM->IN (Rval-type rv)) (Rval-bs rv)))
(define (Ratom-val! rv v)
  (Rval-bs-set*! rv 0 ((hash-ref ATOM->OUT (Rval-type rv)) v)))
(define Ratom/c (apply or/c (hash-values ATOM->?)))

(define (Rcons f oa od)
  (define rv (Rfile-alloc! f 'CONS))
  (Rset-car! rv oa)
  (Rset-car! rv od)
  rv)
(define (Rcar rv) (Rval-bs-oref rv 0))
(define (Rcdr rv) (Rval-bs-oref rv 1))
(define (Rset-car! rv o) (Rval-bs-oset! rv 0 o))
(define (Rset-cdr! rv o) (Rval-bs-oset! rv 1 o))
(define (Rcons->cons rv) (cons (Rcar rv) (Rcdr rv)))

(define (Rbox f ro)
  (define rv (Rfile-alloc! f 'BOX))
  (Rset-box! rv ro)
  rv)
(define (Runbox rv) (Rval-bs-oref rv 0))
(define (Rset-box! rv o) (Rval-bs-oset! rv 0 o))

(define Rraw-type/c (apply symbols RAW-TYPES))
(define (Rraw? rv) (and (Rval? rv) (member (Rval-type rv) RAW-TYPES) #t))
(define (Rraw-type rv) (Rval-type rv))
(define (Rraw f t bs)
  (define rv (Rfile-alloc! f t (bytes-length bs)))
  (Rraw-bs! rv bs)
  rv)
(define (Rraw-bs rv) (Rval-bs rv))
(define (Rraw-bs! rv bs) (Rval-bs-set*! rv 0 bs))

(define (Rvec f len)
  (define offset-len (Rfile-offset-len f))
  (define rv (Rfile-alloc! f 'VEC (* offset-len len)))
  rv)
(define (Rvec-len rv)
  (/ (Rval-len rv) (Rfile-offset-len (Rval-f rv))))
(define (Rvec-ref rv idx)
  (Rval-bs-oref rv idx))
(define (Rvec-set! rv idx o)
  (Rval-bs-oset! rv idx o))
(define (Rvec->vector rv)
    (build-vector (Rvec-len rv) (λ (i) (Rvec-ref rv i))))

(define (Rstr f s)
  (define rv (Rfile-alloc! f 'STRING (string-utf-8-length s)))
  (Rstr-replace! rv s)
  rv)
(define (Rstr-utf8-len rv) (Rval-len rv))
(define (Rstr-len rv) (bytes-utf-8-length (Rval-bs rv)))
(define (Rstr-ref rv idx)
  (bytes-utf-8-ref (Rval-bs rv) idx))
(define (Rstr-replace! rv s)
  (Rval-bs-set*! rv 0 (string->bytes/utf-8 s)))
(define (Rstr-set! rv idx nc)
  (Rval-bs-set*! rv (bytes-utf-8-index (Rval-bs rv) idx)
                 (string->bytes/utf-8 (string nc))))
(define (Rstr->string rv)
  (bytes->string/utf-8 (Rval-bs rv)))

(define (Rbytes f bs)
  (define rv (Rfile-alloc! f 'BYTES (bytes-length bs)))
  (Rbytes-replace! rv bs)
  rv)
(define (Rbytes-len rv) (Rval-len rv))
(define (Rbytes-ref rv idx)
  (Rval-bs-ref rv idx))
(define (Rbytes-set! rv idx nb)
  (Rval-bs-set! rv idx nb))
(define (Rbytes-replace! rv nbs)
  (Rval-bs-set*! rv 0 nbs))
(define (Rbytes->bytes rv)
  (Rval-bs rv))

;; Helpers

(define (write-bytes@! fp off bs)
  (file-position fp off)
  (write-bytes bs fp)
  (flush-output fp))
(define (read-bytes@ fp off len)
  (file-position fp off)
  (read-bytes len fp))
(define (Rfile-read f off len)
  (ensure-open! f)
  (read-bytes@ (Rfile-ip f) off len))
(define (Rfile-write! f off bs)
  (ensure-open! f)
  (write-bytes@! (Rfile-op f) off bs))

(define (->unsigned-bs len x)
  (integer->integer-bytes x len UNSIGNED LITTLE-ENDIAN))
(define (->unsigned bs)
  (integer-bytes->integer bs UNSIGNED LITTLE-ENDIAN))
(define (Rfile-read1 f off)
  (bytes-ref (Rfile-read f off 1) 0))
(define (Rfile-readU f bitw off)
  (->unsigned (Rfile-read f off bitw)))
(define (Rfile-writeU! f bitw off n)
  (Rfile-write! f off (->unsigned-bs bitw n)))

(define (Roff->bs f ro)
  (define offset-len (Rfile-offset-len f))
  (define o
    (match ro
      [(Rglobal i) i]
      [(Rval _ o) o]))
  (->unsigned-bs offset-len o))
(define (bs->Roff f bs)
  (define o (->unsigned bs))
  (if (byte? o)
    (Rglobal o)
    (Rval f o)))

(define (ensure-open! f)
  (unless (unbox (Rfile-open?-b f))
    (error 'runic "File closed\n")))

;; XXX need to deal with OOM/GC during allocation
(define (Rfile-alloc! f t) 'XXX)

;; Library

(define-struct-define define-Rfile Rfile)
(struct Rfile (open?-b path ip op roots offset-len))

;; XXX runic-read (no path)
;; XXX runic-open read-only
(define (runic-open path #:create? [create? #t])
  (define exists? (file-exists? path))
  (when (and (not exists?) (not create?))
    (error 'runic-open "File does not exist"))
  (define op (open-output-file path #:exists (if exists? 'can-update 'update)))
  (define ip (open-input-file path))
  (unless exists?
    (write-bytes@! op  shebang-off shebang-default-bs)
    (write-bytes@! op     lang-off lang-bs)
    (write-bytes@! op  version-off version-bs)
    (write-bytes@! op    flags-off flags-default-bs)
    (write-bytes@! op free-bpt-off (->unsigned-bs 64bit block-size))
    (write-bytes@! op free-opt-off (->unsigned-bs 64bit (+ first-block-off block-header-size)))
    (write-bytes@! op root-obj-off (->unsigned-bs 64bit 0))
    (write-bytes@! op glob-obj-off (->unsigned-bs 64bit 0))
    (write-bytes@! op filemeta-off filemeta-default-bs)
    (write-bytes@! op    first-block-off        (->unsigned-bs 64bit 0))
    (write-bytes@! op (+ first-block-off 64bit) (->unsigned-bs 64bit 0))
    (void))
  (define roots (make-weak-hasheq))
  (define offset-len #f)
  (let ()
    (let ([x (read-bytes@ ip shebang-off shebang-len)])
      (unless (regexp-match shebang-re x)
        (error 'runic-open "Invalid shebang: ~e" x)))
    (let ([x (read-bytes@ ip lang-off (bytes-length lang-bs))])
      (unless (bytes=? lang-bs x)
        (error 'runic-open "Invalid lang line: ~e" x)))
    (let ([x (read-bytes@ ip version-off (bytes-length version-bs))])
      (unless (bytes=? version-bs x)
        (error 'runic-open "Invalid version: ~e" x)))
    (let ([x (bytes-ref (read-bytes@ ip flags-off (bytes-length flags-default-bs)) 0)])
      (set! offset-len
            (match (bitwise-and x flag-offset-mask)
              [(== flag-offset-16bit) 16bit]
              [(== flag-offset-32bit) 32bit]
              [(== flag-offset-64bit) 64bit]
              [_ (error 'runic-open "Invalid flags: ~e" x)]))))
  (unless offset-len
    (error 'runic-open "offets-len didn't get set"))
  (Rfile (box #t) path ip op roots offset-len))

(define (Rfile-open? f)
  (define-Rfile f)
  (unbox open?-b))
(define (Rofile? x)
  (and (Rfile? x) (Rfile-open? x)))

(define (runic-close f)
  (define-Rfile f)
  (set-box! open?-b #f)
  (close-input-port ip)
  (close-output-port op))

(define (runic-shebang f)
  (Rfile-read f shebang-off shebang-len))
(define (runic-shebang! f bs)
  (Rfile-write! f shebang-off bs))

(define (runic-filemeta f)
  (Rfile-read f filemeta-off filemeta-len))
(define (runic-filemeta! f bs)
  (Rfile-write! f filemeta-off bs))

(define (runic-root f)
  (Rfile-readU f 64bit root-obj-off))
(define (runic-root! f ob)
  (Rfile-writeU! f 64bit root-obj-off ob))

(define (runic-global f)
  (Rfile-readU f 64bit glob-obj-off))
(define (runic-global! f ob)
  (Rfile-writeU! f 64bit glob-obj-off ob))

;; Interface

;; XXX Rval?/etc functions don't check that the file is open

(define (bytes/c len [cmp =])
  (λ (x) (and (bytes? x) (cmp (bytes-length x) len))))

(provide
 (contract-out
  [Rfile? (-> any/c boolean?)]
  [runic-open (->* (path-string?) (#:create? boolean?) Rfile?)]
  [Rfile-open? (-> Rfile? boolean?)]
  [runic-close (-> Rofile? void?)]
  
  [runic-shebang (-> Rofile? (bytes/c shebang-len))]
  [shebang-pad (-> (bytes/c shebang-len <=) (bytes/c shebang-len =))]
  [runic-shebang! (-> Rofile? (bytes/c shebang-len) void?)]
  [runic-filemeta (-> Rofile? (bytes/c filemeta-len))]
  [runic-filemeta! (-> Rofile? (bytes/c filemeta-len) void?)]
  [runic-root (-> Rofile? Roff?)]
  [runic-root! (-> Rofile? Roff? void?)]
  [runic-global (-> Rofile? Roff?)]
  [runic-global! (-> Rofile? Roff? void?)]
  
  [Roff? (-> any/c boolean?)]
  [Rval? (-> any/c boolean?)]
  [struct Rglobal ([i byte?])]
  [Rval-type (-> Rval? Rval-type/c)]
  
  [Ratom? (-> any/c boolean?)]
  [Ratom/c contract?]
  [Ratom (-> Rofile? Ratom/c Rval?)]
  [Ratom-val (->i ([rv Ratom?]) [v (rv) (hash-ref ATOM->? (Rval-type rv))])]
  [Ratom-val! (->i ([rv Ratom?] [av (rv) (hash-ref ATOM->? (Rval-type rv))]) [v void?])]

  [Rcons? (-> any/c boolean?)]
  [Rcons (-> Rofile? Roff? Roff? Rval?)]
  [Rcar (-> Rcons? Roff?)]
  [Rcdr (-> Rcons? Roff?)]
  [Rset-car! (-> Rcons? Roff? void?)]
  [Rset-cdr! (-> Rcons? Roff? void?)]
  [Rcons->cons (-> Rcons? (cons/c Roff? Roff?))]

  [Rbox? (-> any/c boolean?)]
  [Rbox (-> Rofile? Roff? Rval?)]
  [Runbox (-> Rbox? Roff?)]
  [Rset-box! (-> Rbox? Roff? void?)]

  [Rraw? (-> any/c boolean?)]
  [Rraw-type (-> Rraw? Rraw-type/c)]
  [Rraw (-> Rofile? Rraw-type/c bytes? Rval?)]
  [Rraw-bs (-> Rraw? bytes?)]
  [Rraw-bs! (-> Rraw? bytes? void?)]

  [Rvec? (-> any/c boolean?)]
  [Rvec (-> Rofile? exact-nonnegative-integer? Rval?)]
  [Rvec-len (-> Rvec? exact-nonnegative-integer?)]
  [Rvec-ref (->i ([r Rvec?] [idx (r) (integer-in 0 (sub1 (Rvec-len r)))]) [o Roff?])]
  [Rvec-set! (->i ([r Rvec?] [idx (r) (integer-in 0 (sub1 (Rvec-len r)))] [o Roff?]) [v void?])]
  [Rvec->vector (-> Rvec? (vector/c #:immutable #t #:flat? #t Roff?))]

  [Rstr? (-> any/c boolean?)]
  [Rstr (-> Rofile? string? Rval?)]
  [Rstr-utf8-len (-> Rstr? exact-nonnegative-integer?)]
  [Rstr-len (-> Rstr? exact-nonnegative-integer?)]
  [Rstr-ref (->i ([r Rstr?] [idx (r) (integer-in 0 (sub1 (Rstr-len r)))]) [c char?])]
  [Rstr-replace! (->i ([r Rstr?] [s (r) (λ (x) (= (string-utf-8-length x) (Rstr-utf8-len r)))]) [v void?])]
  [Rstr-set! (->i ([r Rstr?] [idx (r) (integer-in 0 (sub1 (Rstr-len r)))] [c char?]) [v void?])]
  [Rstr->string (-> Rstr? string?)]

  [Rbytes? (-> any/c boolean?)]
  [Rbytes (-> Rofile? bytes? Rval?)]
  [Rbytes-len (-> Rbytes? exact-nonnegative-integer?)]
  [Rbytes-ref (->i ([r Rbytes?] [idx (r) (integer-in 0 (sub1 (Rbytes-len r)))]) [b byte?])]
  [Rbytes-set! (->i ([r Rbytes?] [idx (r) (integer-in 0 (sub1 (Rbytes-len r)))] [c byte?]) [v void?])]
  [Rbytes-replace! (->i ([r Rbytes?] [bs (r) (bytes/c (Rbytes-len r))]) [v void?])]
  [Rbytes->bytes (-> Rbytes? bytes?)]))
