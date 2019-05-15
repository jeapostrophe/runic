#lang racket/base
(require (for-syntax racket/base
                     syntax/parse)
         racket/lazy-require
         racket/undefined
         racket/match)
(lazy-require [gregor (datetime? posix->datetime ->posix ->nanoseconds +nanoseconds)])
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

(define-syntax define-offset-sequence
  (syntax-parser
    [(_ s:expr x:id) #'(define x s)]
    [(_ s:expr [x:id sz:expr] . more)
     #'(begin (define x s)
              (define-offset-sequence (+ x sz) . more))]))
(define-offset-sequence 0
  [shebang-off 128]
  [lang-off 12]
  [version-off 8bit]
  [flags-off 8bit]
  [block-sz-off 16bit]
  [free-ptr-off 64bit]
  [free-abk-off 64bit]
  [free-cbk-off 64bit]
  [root-obj-off 64bit]
  [glob-obj-off 64bit]
  [filemeta-off 72]
  first-block-off)
(module+ test
  (chk first-block-off 256))

(define shebang-re
  (byte-pregexp #"^#![^\n]{125}\n"))
(define (shebang-pad s)
  (bytes-append #"#!" s (make-bytes (- 128 (+ (bytes-length s) 3))) #"\n"))
(define shebang-default
  (shebang-pad #"/usr/bin/env runic"))
(module+ test
  (chk #:t (regexp-match shebang-re shebang-default)
       (bytes-length shebang-default) 128))

(define lang-bs #"#lang runic ")
(module+ test
  (chk (bytes-length lang-bs) 12))
(define version-bs (bytes 0))

;; Runic values

(struct Rraw (t bs))

(struct Aref (o))
(define (Aref-in bs) (Aref (-obj (current-runic-file) (integer-bytes->integer bs UNSIGNED LITTLE-ENDIAN))))
(define (Aref-out x)
  (match-define (Aref (-obj f o)) x)
  (integer->integer-bytes o (file-offset-len f) UNSIGNED LITTLE-ENDIAN))

(struct Rvec (v))
(define (Rvec-in bs)
  (define f (current-runic-file))
  (define offset-len (file-offset-len f))
  (define-values (len should-be-0) (quotient/remainder (bytes-length bs) offset-len))
  (unless (zero? should-be-0) (error 'Rvec-in "bytes is wrong size"))
  (define v
    (for/vector #:length len ([i (in-range len)])
      (-obj f
            (integer-bytes->integer bs UNSIGNED LITTLE-ENDIAN
                                    (* i offset-len) (* (add1 i) offset-len)))))
  (Rvec v))
(define (Rvec-out x)
  (match-define (Rvec v) x)
  (define f (current-runic-file))
  (define offset-len (file-offset-len f))
  (define bs (make-bytes (* offset-len (vector-length v)) 0))
  (for ([ve (in-vector v)] [i (in-naturals)])
    (match-define (-obj (== f) o) ve)
    (integer->integer-bytes o offset-len UNSIGNED LITTLE-ENDIAN
                            (* i offset-len) (* (add1 i) offset-len)))
  bs)

(define time-s? datetime?)
(define (time-s-in bs) (posix->datetime (integer-bytes->integer bs SIGNED LITTLE-ENDIAN)))
(define (time-s-out x) (integer->integer-bytes (floor (->posix x)) 64bit SIGNED LITTLE-ENDIAN))

(define time-s+ns? datetime?)
(define (time-s+ns-in bs)
  (define dt (posix->datetime (integer-bytes->integer bs SIGNED LITTLE-ENDIAN 0 3)))
  (+nanoseconds dt (integer-bytes->integer bs SIGNED LITTLE-ENDIAN 4 7)))
(define (time-s+ns-out x)
  (bytes-append (time-s-out x)
                (integer->integer-bytes (->nanoseconds x) 64bit SIGNED LITTLE-ENDIAN)))

(struct Rcons (a d))

;; Atom Interpretation

(struct -type (mask tg ? len in out))

(define (type-constant bit-string value)
  (define (? x) (eq? value x))
  (define (in bs) value)
  (define (out x) (error 'type-constant "No output bytes"))
  (type #:? ? #:bit bit-string #:length 0 #:in in #:out out))
(define (type-int bit-string signed? len)
  (unless (member len '(1 2 4 8)) (error 'type-int "Doesn't support len ~e" len))
  (define (? x) (and (integer? x) (<= (integer-length x) len)))
  (define (in bs) (integer-bytes->integer bs signed? LITTLE-ENDIAN))
  (define (out x) (integer->integer-bytes x len signed? LITTLE-ENDIAN))
  (type #:? ? #:bit bit-string #:length len #:in in #:out out))
(define (type-flo bit-string len)
  (define ?
    (match len
      [4 single-flonum?]
      [8 flonum?]
      [_ (error 'type-flo "Doesn't support len ~e" len)]))
  (define (in bs) (floating-point-bytes->real bs LITTLE-ENDIAN))
  (define (out x) (real->floating-point-bytes x len LITTLE-ENDIAN))
  (type #:? ? #:bit bit-string #:length len #:in in #:out out))
(define (type-raw bit-string len t)
  (define (? x) (and (Rraw? x) (eq? (Rraw-t x) t)))
  (define (in bs) (Rraw t bs))
  (define (out x) (Rraw-bs x))
  (type #:? ? #:bit bit-string #:length len #:in in #:out out))
(define (type #:? ? #:bit bit-string #:length len #:in in #:out out)
  (define-values (mask tg)
    (for/fold ([m 0] [t 0] [i (expt 2 7)] #:result (values m t)) ([b (in-bytes bit-string)])
      (match (integer->char b)
        [#\0     (values (+ m i)    t    (/ i 2))]
        [#\1     (values (+ m i) (+ t i) (/ i 2))]
        [#\_     (values    m       t    (/ i 2))]
        [#\space (values    m       t       i   )])))
  (-type mask tg ? len in out))

(define TYPES
  (list
   (cons 't-bool-f (type-constant #"0000 0000" #f))
   (cons 't-bool-t (type-constant #"0000 0001" #t))
   (cons 't-u8          (type-int #"0000 0010" #f   8bit))
   (cons 't-s8          (type-int #"0000 0011" #t   8bit))
   (cons 't-u16         (type-int #"0000 0100" #f  16bit))
   (cons 't-s16         (type-int #"0000 0101" #t  16bit))
   (cons 't-u32         (type-int #"0000 0110" #f  32bit))
   (cons 't-s32         (type-int #"0000 0111" #t  32bit))
   (cons 't-u64         (type-int #"0000 1000" #f  64bit))
   (cons 't-s64         (type-int #"0000 1001" #t  64bit))
   (cons 't-u128        (type-raw #"0000 1010" 128bit 'U128))
   (cons 't-s128        (type-raw #"0000 1011" 128bit 'S128))
   (cons 't-u256        (type-raw #"0000 1100" 256bit 'U256))
   (cons 't-s256        (type-raw #"0000 1101" 256bit 'S256))
   (cons 't-time-s    (type #:bit #"0000 1110" #:length  64bit #:? time-s? #:in time-s-in #:out time-s-out))
   (cons 't-time-s+ns (type #:bit #"0000 1111" #:length 128bit #:? time-s+ns? #:in time-s+ns-in #:out time-s+ns-out))
   (cons 't-null   (type-constant #"0001 0000" '()))
   (cons 't-void   (type-constant #"0001 0001" (void)))
   (cons 't-undef  (type-constant #"0001 0010" undefined))
   (cons 't-ref       (type #:bit #"0001 0011" #:length 'W #:? Rref? #:in Rref-in #:out Rref-out))
   (cons 't-f16         (type-raw #"0001 0100"  16bit 'F16))
   (cons 't-f32         (type-flo #"0001 0101"  32bit))
   (cons 't-f64         (type-flo #"0001 0110"  64bit))
   (cons 't-f128        (type-raw #"0001 0111" 128bit 'F128))
   (cons 't-f256        (type-raw #"0001 1000" 256bit 'F256))
   (cons 't-fd32        (type-raw #"0001 1001"  32bit 'FD32))
   (cons 't-fd64        (type-raw #"0001 1010"  64bit 'FD64))
   (cons 't-fd128       (type-raw #"0001 1011" 128bit 'FD128))
   (cons 't-fb16        (type-raw #"0001 1100"  16bit 'FB16))
   ;; Unallocated for extensions  #"0001 1101"
   ;; Unallocated for extensions  #"0001 1110"
   ;; Unallocated for extensions  #"0001 1111"
   (cons 't-bignum      (type-raw #"0010 ____" 'VLQ 'BIGNUM))
   (cons 't-bigfloat    (type-raw #"0011 ____" 'VLQ 'BIGFLOAT))
   (cons 't-vector    (type #:bit #"01__ ____" #:length 'VLQ_W #:? Rvec? #:in Rvec-in #:out Rvec-out))
   (cons 't-string    (type #:bit #"10__ ____" #:length 'VLQ #:? string? #:in bytes->string/utf-8 #:out string->bytes/utf-8))
   (cons 't-bytes     (type #:bit #"11__ ____" #:length 'VLQ #:? bytes? #:in (λ (x) x) #:out (λ (x) x)))))

;; Interface

(define current-runic-file (make-parameter #f))
(struct -file (path port roots offset-sz block-sz))
(define file-offset-len -file-offset-sz)
(struct -obj (f o))
