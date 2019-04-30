#lang racket/base
(require (for-syntax racket/base
                     syntax/parse)
         racket/undefined
         racket/match)
(module+ test
  (require chk))

;; Format

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
  [version-off 1]
  [flags-off 1]
  [block-sz-off 2]
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

(struct -type (tag len in out))

(define (id x) x)
(define (xxx x) (error 'xxx))

(define t-bool-f (type-constant #"0000 0000" #f))
(define t-bool-t (type-constant #"0000 0001" #t))
(define t-u8          (type-int #"0000 0010" #f   8bit))
(define t-s8          (type-int #"0000 0011" #t   8bit))
(define t-u16         (type-int #"0000 0100" #f  16bit))
(define t-s16         (type-int #"0000 0101" #t  16bit))
(define t-u32         (type-int #"0000 0110" #f  32bit))
(define t-s32         (type-int #"0000 0111" #t  32bit))
(define t-u64         (type-int #"0000 1000" #f  64bit))
(define t-s64         (type-int #"0000 1001" #t  64bit))
(define t-u128        (type-int #"0000 1010" #f 128bit))
(define t-s128        (type-int #"0000 1011" #t 128bit))
(define t-u256        (type-int #"0000 1100" #f 256bit))
(define t-s256        (type-int #"0000 1101" #t 256bit))
(define t-time-s    (type #:tag #"0000 1110" #:length  64bit #:in xxx #:out xxx))
(define t-time-s+ns (type #:tag #"0000 1111" #:length 128bit #:in xxx #:out xxx))
(define t-null   (type-constant #"0001 0000" '()))
(define t-void   (type-constant #"0001 0001" (void)))
(define t-undef  (type-constant #"0001 0010" undefined))
(define t-ref       (type #:tag #"0001 0011" #:length 'W #:in xxx #:out xxx))
(define t-f16       (type #:tag #"0001 0100" #:length 16bit #:in xxx #:out xxx))
(define t-f32       (type #:tag #"0001 0101" #:length 32bit #:in xxx #:out xxx))
(define t-f64       (type #:tag #"0001 0110" #:length 64bit #:in xxx #:out xxx))
(define t-f128      (type #:tag #"0001 0111" #:length 128bit #:in xxx #:out xxx))
(define t-f256      (type #:tag #"0001 1000" #:length 256bit #:in xxx #:out xxx))
(define t-fd32      (type #:tag #"0001 1001" #:length  32bit #:in xxx #:out xxx))
(define t-fd64      (type #:tag #"0001 1010" #:length  64bit #:in xxx #:out xxx))
(define t-fd128     (type #:tag #"0001 1011" #:length 128bit #:in xxx #:out xxx))
(define t-fb16      (type #:tag #"0001 1100" #:length  16bit #:in xxx #:out xxx))
;; Unallocated for extensions   #"0001 1101"
;; Unallocated for extensions   #"0001 1110"
;; Unallocated for extensions   #"0001 1111"
(define t-bignum    (type #:tag #"0010 ____" #:length 'VLQ #:in xxx #:out xxx))
(define t-bigfloat  (type #:tag #"0011 ____" #:length 'VLQ #:in xxx #:out xxx))
(define t-vector    (type #:tag #"01__ ____" #:length 'VLQ_W #:in xxx #:out xxx))
(define t-string    (type #:tag #"10__ ____" #:length 'VLQ #:in bytes->string/utf-8 #:out string->bytes/utf-8))
(define t-bytes     (type #:tag #"11__ ____" #:length 'VLQ #:in id #:out id))

;; Interface

(struct -file (path port roots offset-sz block-sz))
(struct -atom (f o))
(struct -cons (f o))
