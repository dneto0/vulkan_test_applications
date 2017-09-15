# Tiny Compute

This sample runs a computer shader with a specialized workgroup size.

The shader is compiled with workgroup size (1,1,1), but the host program
will override it with specialization constants to get an effective
workgroup size of (2,3,7).

The shader is run over 1 workgroup.

Each invocation computes its own linearized index, and then writes
that index to its own slot in the output array.  This fills entries
0 through 41.

Also, the observed workgroup size x, y, z components are written to
entries 50, 51, and 52.
