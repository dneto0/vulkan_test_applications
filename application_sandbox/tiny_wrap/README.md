# Stripe transition

A compute shader that writes a sequence of 9.0s followed by a sequence of 1.0s to
a storage buffer with 40 cells.

The transition point is selected by a program variable `transition_base`.
Buffer cells up to and including `transition_base` should get 9.0s.
Cells after `transition_base` should get 1.0s.

The code uses shifts to test hypotheses about whether the signed comparison
is implemented correctly.  See stripe.comp for a GLSL version of the code.

This version of the code uses a transition base of -3, so the outputs should be
a sequence of 40 1.0s.

Shader sources:
   stripe.comp:    GLSL version showing essential logic
   stripe.cl:      OpenCL C version, the original motivating example
   stripe.spvasm:  SPIR-V assembly, hand tuned for experimentation
   `stripe_reg.spvasm`:  SPIR-V assembly, same as stripe.spvasm but
      passes the computed `%index` value through a 32-bit store-load pair
      before using it in the signed comparison.
   `stripe_opt0.spvasm`:
      stripe.comp compiled with glslc, no optimizations at all.
      (Same as glslangValidator would do, but adds OpSourceExtension, and
      changes generator word.)
   `stripe_mem2reg.spvasm`:
      Result of running stripe_opt0.spvasm through
            spirv-opt --eliminate-local-single-block
      This eliminates redundant loads and stores in a single basic block.
      The SSA result IDs for instructions that survive are the same as
      for `stripe_opt0.spvasm`, so it's easy to verify logical equivalency.
