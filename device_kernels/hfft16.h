#include"hfft.h"

__global__ void d_hfft16x( float2* d_o, float2* d_i, int bat )	
{												
	float2 c[16], temp;	
	int tidx=blockIdx.x*blockDim.x+threadIdx.x;
	if(tidx>=bat) return;
	d_i+=(tidx<<4);
	d_o+=(tidx<<4);
	mLOAD16(c,d_i,1,)					
	mFFT16(c,)								
	mISTORE16(d_o,c,1,)
}
__global__ void d_hifft16x( float2* d_o, float2* d_i, int bat )	
{												
	float2 c[16], temp;	
	int tidx=blockIdx.x*blockDim.x+threadIdx.x;
	if(tidx>=bat) return;
	d_i+=(tidx<<4);
	d_o+=(tidx<<4);
	mLOAD16(c,d_i,1,)					
	mFFT16(c,i)								
	mISTORE16(d_o,c,1,)
}