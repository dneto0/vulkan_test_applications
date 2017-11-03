#!/bin/bash

# Conversion script from OpenCL C to SPIR-V assembly.
clspv copy_struct.cl -hack-undef -o a0.spv
spirv-opt --inline-entry-points-exhaustive --eliminate-dead-functions a0.spv -o a.spv
spirv-dis a.spv >a.spvasm

# Then copy a.spvasm to copy_struct.spvasm
# Optional: Edit copy_struct.spvasm
