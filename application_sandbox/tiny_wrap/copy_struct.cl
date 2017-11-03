
typedef struct {
 float a, b, c, d;
} S;

float4 FromS(S in) {
  return (float4)(in.a, in.b, in.c, in.d);
}

S ToS(float4 in) {
  S val = {in.x, in.y, in.z, in.w};
  return val;
}

kernel void foo(global S* out, global float4* in) {
  uint i = get_global_id(0);
  out[i] = ToS(in[i]);
}
