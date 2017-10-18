kernel void foo(global float4* out) {
  float4 value;
  value.w = 1000.0f;
  *out = value;
}
