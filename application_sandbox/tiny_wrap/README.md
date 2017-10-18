# Tiny Wrap

This sample runs a computer shader with one invocation.
It writes the a vec4 value into the first part of a storage buffer.

The main variations are:

* `tiny_wrap_write_w`- in SPIR-V assembly. Write (undef, undef, undef, 1000.0).
* `tiny_wrap_write_w0`- in SPIR-V assembly. Write (0, 0, 0, 1000.0).

The SPIR-V assembly was derived from orig.cl, except:
* Remove OpExtension for `SPV_KHR_variable_pointers`.
* Remove VariablePointers capability.
