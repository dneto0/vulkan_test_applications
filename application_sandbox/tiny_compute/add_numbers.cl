kernel void 
adder(global uint* A) {
  A[0] = get_local_size(0);
  A[1] = get_local_size(1);
  A[2] = get_local_size(2);
}
