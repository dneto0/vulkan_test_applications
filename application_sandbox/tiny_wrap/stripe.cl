kernel void foo(global float* out) {
  int i = (int)get_global_id(0);
  out[i] = 9.0f;
  if (i >= -3) {
	  out[i] = 1.0f;
  }
}
