```
  256-byte Header = 
   - 128 bytes, starting with "#!" and ending with "\n"; in between,
     no "\n"s are allowed. Recommend "#!/usr/bin/env runic *\n"
   - 12 bytes for "#lan" "g ru" "nic "
   - 1 byte  for version
   ^-- just a counter of changes to interpretation
   - 1 byte  for flags
   ^-- W = offset size (16, 32, or 64)
   - 2 bytes for (block size >> 8)
   - 8 bytes for offset of start of next available free block
   - 8 bytes for offset of free spot in free atom block
   - 8 bytes for offset of free spot in free obj block
   - 8 bytes for offset of root object
   - 8 bytes for offset to global object
   ^-- only 2 used if in 16bit mode
   ^-- only 4 used if in 32bit mode
   - 72 bytes of file-specific meta data

   Block Header =
   - W bytes for offset to last block of same type
   ^-- last eight bits are all 0, so they have a special meaning
       the bottom one identifies this as an ATOM or CONS block
   - W bytes for offset to next block of same type
   ^-- In both cases, an offset value of 0 means none.

   Atom =
   - 1 byte = tag
   ^-- 000_ ____ => Primitive (32 kinds)
   ^---  2 Booleans: T, F
   ^--- 12 Integers: [US](8,16,32,64,128,256)
   ^--- 11 Floats: ([HSDQOex], d32, d64, d128, bfloat16)
   ^---  2 Times: 64b (s), 128b (s+ns)
   ^---  5 Others: Null (empty list), Void, Undefined, Ref (W size), Extension
   ^-- 0010 ____ => BigNum (GnuMP), 1+3 VLQ length
   ^-- 0011 ____ => BigFloat (GnuMPFR), 1+3 VLQ length
   ^-- 01__ ____ => Vector, 1+5 VLQ length (of W size)
   ^-- 10__ ____ => UTF-8, 1+5 VLQ length
   ^-- 11__ ____ => Bytes, 1+5 VLQ length
   - Data follows

   Cons =
   - W bytes for offset to car
   - W bytes for offset to cdr

   Issues/notes:
   - Bytes are always little-endian
   - 1st block is always shorter by 256 bytes
   - An offset of [0-255] is an (Indirect i)
   ---> Because those offsets are in the header
   ---> The standard interpretation of this is (vec/list-ref global i)
   - Individual ATOMs can extend past the end of their block but the
     next block won't be usable. This means that there is effectively
     no limit the size of things.
```
