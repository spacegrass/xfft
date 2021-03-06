#ifndef __cuintrin_h__
#define __cuintrin_h__

#if defined(_M_X64)||defined(_M_AMD64)||defined(__x86_64)||defined(_M_IA64)||defined(__LP64__)
#define PTX_PTR "l"
#else
#define PTX_PTR "r"
#endif

__device__ __forceinline__ static float2 __fldu2( const float2* p )
{
	float2 val;
	asm volatile ("ldu.global.v2.f32 {%0,%1}, [%2];" : "=f"(val.x), "=f"(val.y) : PTX_PTR(p));
	return val;
}
__device__ __forceinline__ static float4 __fldu4( const float4* p )
{
	float4 val;
	asm volatile ("ldu.global.v4.f32 {%0,%1,%2,%3}, [%4];" : "=f"(val.x), "=f"(val.y), "=f"(val.z), "=f"(val.w) : PTX_PTR(p));
	return val;
}

#endif