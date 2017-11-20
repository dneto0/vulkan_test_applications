
typedef struct {
 float a, b, c, d;
} S;

float4 FromS(S in) {
  return (float4)(0.0f, in.b, in.c + 100.f, in.d+100.0f);
}

S ToS(float4 in) {
  S val = {1.0f, in.y, in.z, 3.0f};
  return val;
}

kernel void foo(global S* out, global S* in) {
  uint i = get_global_id(0);
  out[i] = ToS(FromS(in[i]));
}
