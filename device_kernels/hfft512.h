#include"hfft.h"

texture<float2,1,cudaReadModeElementType> d_tex;

#if SM>=21
#define NUM_CTA_512 16
#else
#define NUM_CTA_512  8
#endif

__global__ void __launch_bounds__(64,NUM_CTA_512) d_hfft512x( float2* d_o, const float2* __restrict__ d_i, const float2* d_RF )
{
	__shared__ float smem[97*8];
	float2 c[8], RF[7], temp;
	d_o+=(blockIdx.x<<9)+threadIdx.x;
	d_i+=(blockIdx.x<<9)+threadIdx.x;
	const unsigned int lane=threadIdx.x&7;
	const unsigned int slot=threadIdx.x>>3;
	float* spu=&smem[threadIdx.x];
	float* spv=&smem[72*slot+lane];
	RF[0]=d_RF[threadIdx.x];
	mLOAD8(c,d_i,64,)
	mCALRF8(RF)
	mFFT8(c,)
	mHMRF8(c,RF)
	RF[0]=d_RF[lane<<3];
	mPERMUTE(8,spu,spv,c,72,8,0xf)
	mCALRF8(RF)	
	spu=&smem[7*(slot&3)+97*slot+lane];
	spv=&smem[7*(lane&3)+97*lane+9*slot];
	mFFT8(c,)
	mHMRF8(c,RF)
	mPERMUTE(8,spu,spv,c,9,1,0x7)
	mFFT8(c,)
	mISTORE8(d_o,c,64,)	
}
__global__ void __launch_bounds__(64,NUM_CTA_512) d_hifft512x( float2* d_o, const float2* __restrict__ d_i, const float2* d_RF )
{
	__shared__ float smem[97*8];
	float2 c[8], RF[7], temp;
	d_o+=(blockIdx.x<<9)+threadIdx.x;
	d_i+=(blockIdx.x<<9)+threadIdx.x;
	const unsigned int lane=threadIdx.x&7;
	const unsigned int slot=threadIdx.x>>3;
	float* spu=&smem[threadIdx.x];
	float* spv=&smem[72*slot+lane];
	RF[0]=d_RF[threadIdx.x];
	RF[0].y=-RF[0].y;
	mLOAD8(c,d_i,64,)
	mCALRF8(RF)
	mFFT8(c,i)
	mHMRF8(c,RF)
	RF[0]=d_RF[lane<<3];
	RF[0].y=-RF[0].y;
	mPERMUTE(8,spu,spv,c,72,8,0xf)
	mCALRF8(RF)	
	spu=&smem[7*(slot&3)+97*slot+lane];
	spv=&smem[7*(lane&3)+97*lane+9*slot];
	mFFT8(c,i)
	mHMRF8(c,RF)
	mPERMUTE(8,spu,spv,c,9,1,0x7)
	mFFT8(c,i)
	mISTORE8(d_o,c,64,)	
}
__global__ void __launch_bounds__(64,8) d_hfft512x_tex( float2* d_c, const float2* d_RF )
{
	__shared__ float smem[97*8];
	float2 c[8], RF[7], temp;
	const unsigned int ofs=(blockIdx.x<<9)+threadIdx.x;
	d_c+=ofs;
	const unsigned int lane=threadIdx.x&7;
	const unsigned int slot=threadIdx.x>>3;
	float* spu=&smem[threadIdx.x];
	float* spv=&smem[72*slot+lane];
	RF[0]=d_RF[threadIdx.x];
	mTLD8(c,ofs,64)
	mCALRF8(RF)
	mFFT8(c,)
	mHMRF8(c,RF)
	RF[0]=d_RF[lane<<3];
	mPERMUTE(8,spu,spv,c,72,8,0xf)
	mCALRF8(RF)	
	spu=&smem[7*(slot&3)+97*slot+lane];
	spv=&smem[7*(lane&3)+97*lane+9*slot];
	mFFT8(c,)
	mHMRF8(c,RF)
	mPERMUTE(8,spu,spv,c,9,1,0x7)
	mFFT8(c,)
	mISTORE8(d_c,c,64,)	
}
__global__ void __launch_bounds__(64,8) d_hifft512x_tex( float2* d_c, const float2* d_RF )
{
	__shared__ float smem[97*8];
	float2 c[8], RF[7], temp;
	const unsigned int ofs=(blockIdx.x<<9)+threadIdx.x;
	d_c+=ofs;
	const unsigned int lane=threadIdx.x&7;
	const unsigned int slot=threadIdx.x>>3;
	float* spu=&smem[threadIdx.x];
	float* spv=&smem[72*slot+lane];
	RF[0]=d_RF[threadIdx.x];
	RF[0].y=-RF[0].y;
	mTLD8(c,ofs,64)
	mCALRF8(RF)
	mFFT8(c,i)
	mHMRF8(c,RF)
	RF[0]=d_RF[lane<<3];
	RF[0].y=-RF[0].y;
	mPERMUTE(8,spu,spv,c,72,8,0xf)
	mCALRF8(RF)	
	spu=&smem[7*(slot&3)+97*slot+lane];
	spv=&smem[7*(lane&3)+97*lane+9*slot];
	mFFT8(c,i)
	mHMRF8(c,RF)
	mPERMUTE(8,spu,spv,c,9,1,0x7)
	mFFT8(c,i)
	mISTORE8(d_c,c,64,)	
}

//__global__ void d_hfft512Sx( float2* d_o, const float2* __restrict__ d_i, const float2* d_RF )
//{
//	__shared__ float smem[8*512];
//	float2 c[8], RF[7], temp;
//	size_t tidx=(blockIdx.y<<18)+(threadIdx.y<<9)+(blockIdx.x<<3)+threadIdx.x;
//	d_i+=tidx;
//	d_o+=tidx;
//	float* spu=&smem[  8*threadIdx.y+threadIdx.x];
//	float* spv=&smem[512*threadIdx.y+threadIdx.x];
//	RF[0]=d_RF[threadIdx.x];
//	mLOAD8(c,d_i,32768,)
//	mCALRF8(RF)
//	mFFT8(c,)
//	mHMRF8(c,RF)
//	RF[0]=d_RF[(tidx&4097)<<3];
//	mPERMUTE(8,spu,spv,c,512,64,0xf)
//	mCALRF8(RF)
//	mFFT8(c,)
//	mHMRF8(c,RF)
//	RF[0]=d_RF[(tidx&511)<<6];
//	spu=&smem[64*threadIdx.y+threadIdx.x];
//	mPERMUTE(8,spv,spu,c,64,8,0x7)
//	mFFT8(c,)
//	mHMRF8(c,RF)
//	mISTORE8(d_o,c,32768,)	
//}
//__global__ void d_hfft512Tx( float2* d_o, const float2* __restrict__ d_i, const float2* d_RF )
//{
//	__shared__ float smem[97*8];
//	float2 c[8], RF[7], temp;
//	unsigned int lane=threadIdx.x&7;
//	unsigned int slot=threadIdx.x>>3;
//	d_i+=(blockIdx.y<<18)+(blockIdx.x<<15)+threadIdx.x;
//	d_o+=(blockIdx.y<<18)+(blockIdx.x<< 3)+(slot<<9)+lane;
//	float* spu=&smem[threadIdx.x];
//	float* spv=&smem[72*slot+lane];
//	RF[0]=d_RF[threadIdx.x];
//	mLOAD8(c,d_i,64,)
//	mCALRF8(RF)
//	mFFT8(c,)
//	mHMRF8(c,RF)
//	RF[0]=d_RF[lane<<3];
//	mPERMUTE(8,spu,spv,c,72,8,0x8)
//	mCALRF8(RF)
//	mFFT8(c,)
//	mHMRF8(c,RF)
//	spu=&smem[7*(slot&3)+97*slot+lane];
//	spv=&smem[7*(lane&3)+97*lane+9*slot];
//	mPERMUTE(8,spu,spv,c,9,1,0x7)
//	mFFT8(c,)
//	mISTORE8(d_o,c,32768,)	
//}