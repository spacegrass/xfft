#include"hfft.h"

#if SM==37
#define NUM_CTA_2048 9
#elif (SM>=30)&&(SM!=37)
#define NUM_CTA_2048 5
#else
#define NUM_CTA_2048  4
#endif

__global__ void __launch_bounds__(128,NUM_CTA_2048) d_hfft2048x( float2* d_o, const float2* __restrict__ d_i, const float2* __restrict__ d_RF )
{																	
	__shared__ float smem[16*161];									
	float2 c[16], RF[15], temp;										
	d_i+=((blockIdx.x<<11)+threadIdx.x);							
	d_o+=((blockIdx.x<<11)+threadIdx.x);							
	unsigned int lane=threadIdx.x&15;								
	unsigned int slot=threadIdx.x>>4;								
	float* sp0=&smem[threadIdx.x];									
	float* sp1=&smem[144*slot+lane];	
	float* sp2=&smem[15*(slot&1)+161*slot+lane];
	float* sp3=&smem[15*(lane&1)+161*lane+17*slot];					
	RF[0]=d_RF[threadIdx.x];										
	mLOAD16(c,d_i,128,)												
	mCALRF16(RF)													
	mFFT16(c,)													
	mHMRF16(c,RF)													
	RF[0]=d_RF[lane<<4];											
	mPERMUTE_S16_L8x2(sp0,sp1,c,144,1152,16,0xf);							
	mCALRF8(RF)														
	mFFT8(&c[0],)												
	mFFT8(&c[8],)												
	mHMRF8(&c[0],RF)
	mHMRF8(&c[8],RF)													
	mPERMUTE_S8x2_L16(sp2,sp3,c,1288,17,1,0x7);		
	mFFT16(c,)													
	mISTORE16(d_o,c,128,)											
}
__global__ void __launch_bounds__(128,NUM_CTA_2048) d_hifft2048x( float2* d_o, const float2* __restrict__ d_i, const float2* __restrict__ d_RF )
{																	
	__shared__ float smem[16*128];									
	float2 c[16], RF[15], temp;										
	d_i+=((blockIdx.x<<11)+threadIdx.x);							
	d_o+=((blockIdx.x<<11)+threadIdx.x);							
	unsigned int lane=threadIdx.x&15;								
	unsigned int slot=threadIdx.x>>4;								
	float* sp0=&smem[threadIdx.x];									
	float* sp1=&smem[144*slot+lane];	
	float* sp2=&smem[15*(slot&1)+161*slot+lane];
	float* sp3=&smem[15*(lane&1)+161*lane+17*slot];					
	RF[0]=d_RF[threadIdx.x];
	RF[0].y=-RF[0].y;
	mLOAD16(c,d_i,128,)												
	mCALRF16(RF)													
	mFFT16(c,i)													
	mHMRF16(c,RF)													
	RF[0]=d_RF[lane<<4];
	RF[0].y=-RF[0].y;
	mPERMUTE_S16_L8x2(sp0,sp1,c,144,1152,16,0xf);							
	mCALRF8(RF)														
	mFFT8(&c[0],i)												
	mFFT8(&c[8],i)												
	mHMRF8(&c[0],RF)
	mHMRF8(&c[8],RF)													
	mPERMUTE_S8x2_L16(sp2,sp3,c,1288,17,1,0x7);		
	mFFT16(c,i)													
	mISTORE16(d_o,c,128,)											
}

