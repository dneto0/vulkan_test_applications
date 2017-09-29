
#if defined(INLINE)
#define KERNEL_NAME write_inline
#else
#define KERNEL_NAME write_call
#endif


void inner(global float4* buf) {
  buf[0] = (float4)(23.0f, 24.0f, 25.0f, 26.0f);
}

__attribute__((reqd_work_group_size(1,1,1)))
kernel void KERNEL_NAME(global float4* buf) {
#if defined(INLINE)
  buf[0] = (float4)(23.0f, 24.0f, 25.0f, 26.0f);
#else
  inner(buf);
#endif
}
