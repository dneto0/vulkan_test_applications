# Tiny Wrap

This sample runs a computer shader with one invocation.
It writes the value vec4(23,24,25,26) into the first part
of a storage buffer.

The main variations are:

* `tiny_wrap_glsl`- in GLSL
* `tiny_wrap_call_asm`- in SPIR-V assembly, where the pointer to the
  storage buffer is passed from the kernel entry point into a helper
  function, and the helper function does the write.
* `tiny_wrap_inline_asm`- in SPIR-V assembly, where the entry point
  is the only function, and it does the write.


The SPIR-V assembly was derived from wrap.cl, except:
* Remove OpExtension for `SPV_KHR_variable_pointers`.
* Remove VariablePointers capability.
* Change OpSource to `GLSL` 430 instead of `OpenCL_C` 120.
* Change entry point names to `write_call` and `write_inline` as appropriate.

Additional variations:
* `tiny_wrap_bufferblock_call_asm` - same as `tiny_wrap_call_asm` but use the
  old representation for a storage buffer: Uniform storage class, and
  BufferBlock decoration.
* `tiny_wrap_bufferblock_inline_asm` - same as `tiny_wrap_inline_asm` but use the
  old representation for a storage buffer: Uniform storage class, and
  BufferBlock decoration.
