# Copy Struct

A compute shader that copies from one storage buffer to another.  Each contains
an array of structs of 4 floats.  The copy swaps members 0 and 3, and forwards
members 1 and 2.

`copy_struct.comp`: a GLSL compute shader to prove the pipeline is set up
correctly.
`copy_struct.spvasm`: Compiled from `copy_struct.cl` with 
  `clspv copy_struct.cl -o a.spv -hack-undef -hack-inserts`
  `spirv-dis a.spv >copy_struct.spvasm`

  Note that `copy_struct.spvasm` does *not* use OpCompositeInsert.

Expected output is:

Output: <shader-name> 3 1 2 0 7 5 6 4 11 9 10 8 15 13 14 12 19 17 18 16 23 21 22
20 27 25 26 24 31 29 30 28 35 33 34 ...
