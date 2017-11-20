
typedef struct {
 float a, b, c, d;
} S;


typedef S InType;

S rep_ends_struct(InType in) {
  S val = {in.d, in.b, in.c, in.a};
  return val;
}


kernel void foo(global S* out, global InType* in) {
  uint i = get_global_id(0);
  out[i] = rep_ends_struct(in[i]);
}
