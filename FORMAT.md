```
  256-byte Header = 
   - 128 bytes, starting with "#!" and ending with "\n"; in between,
     no "\n"s are allowed. Recommend "#!/usr/bin/env runic\0*\n"
   - 12 bytes for "#lan" "g ru" "nic "
   - 1 byte  for version
   ^-- just a counter of changes to interpretation
   - 1 byte  for flags
   ^-- WW__ ___
   ^-- W = 00 -> 2 bytes
    --     01 -> 4 bytes
    --     10 -> 8 bytes
    -- Other bits are not used.
   - 8 bytes for offset of free pointer for blocks
   - 8 bytes for offset of free pointer for objects
   - 8 bytes for offset of root object
   - 8 bytes for offset to global object
   ^-- These offsets are always 8 meaningful bytes even if in 16bit mode
   - 82 bytes of file-specific meta data

   Block Header =
   - 8 bytes for offset to last block of same type
   - 8 bytes for offset to next block of same type
   ^-- In both cases, an offset value of 0 means none.

   Object =
   - 1 byte = tag
   ^-- 000_ ____ => Primitive (32 kinds)
   ^---  2 Booleans: T, F
   ^--- 12 Integers: [US](8,16,32,64,128,256)
   ^---  2 Times: 64b (s), 128b (s+ns)
   ^---  5 Others: Null (empty list), Void, Undefined, Ref (W size), Cons (2*W size)
   ^---  9 Floats: ([HSDQO], d32, d64, d128, bfloat16)
   ^---  2 Unused
   ^-- 0010 ____ => BigNum (GnuMP), 1+3 VLQ length
   ^-- 0011 ____ => BigFloat (GnuMPFR), 1+3 VLQ length
   ^-- 01__ ____ => Vector, 1+5 VLQ length (of W size)
   ^-- 10__ ____ => UTF-8, 1+5 VLQ length
   ^-- 11__ ____ => Bytes, 1+5 VLQ length
   - Data follows

   Issues/notes:
   - Bytes are always little-endian
   - 1st block is always shorter by 256 bytes
   - An offset of [0-255] is an (Indirect i)
   ---> Because those offsets are in the header
   ---> The standard interpretation of this is (vec/list-ref global i)
   - Individual objects can extend past the end of their block but the
     next block won't be usable. This means that there is effectively
     no limit the size of things.
```
