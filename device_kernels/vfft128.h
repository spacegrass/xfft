#include"vfft.h"

#define CUDA_VFFT128(LB,dir,e,bdx,op) CUDA_VFFT_DECL(128,LB,dir,e,PRF)         \
{                                                                              \
    __shared__ float smem[128*bdx];                                            \
    float2 c[16], temp;                                                        \
    d_i+=(blockIdx.y*128*(1<<e)+threadIdx.y*(1<<e)+blockIdx.x*bdx+threadIdx.x);\
    d_o+=(blockIdx.y*128*(1<<e)+threadIdx.y*(1<<e)+blockIdx.x*bdx+threadIdx.x);\
    float* sst=&smem[16*bdx*threadIdx.y+threadIdx.x];                          \
    float* sld=&smem[   bdx*threadIdx.y+threadIdx.x];                          \
    mLOAD16(c,d_i,8*(1<<e),)                                                   \
    mFFT16(c,dir)                                                              \
    mVMRF16(&d_RF[threadIdx.y<<4],dir,op)                                      \
    mPERMUTE_S16_L8x2(sst,sld,c,bdx,8*bdx,16*bdx,7)                            \
    mFFT8(&c[0],dir)                                                           \
    mFFT8(&c[8],dir)                                                           \
    mISTORE8x2(d_o,c,8*(1<<e),16*(1<<e),)                                      \
}
#define CUDA_UFFT128X(LB,dir,n,e,bdx,op) CUDA_UFFTX_DECL(128,LB,dir,n,e)                   \
{                                                                                          \
    __shared__ float smem[128*bdx];                                                        \
    float2 c[16], temp;                                                                    \
    unsigned int slot=blockIdx.x/((1<<e)/bdx);                                             \
    unsigned int bidx=blockIdx.x&((1<<e)/bdx-1);                                           \
    size_t p=blockIdx.y*n*128*(1<<e)+slot*(1<<e)+threadIdx.y*n*(1<<e)+bidx*bdx+threadIdx.x;\
    d_i+=p;	d_o+=p;                                                                        \
    mLOAD16(c,d_i,8*n*(1<<e),)                                                             \
    slot+=threadIdx.y*n;                                                                   \
    float* sst=&smem[16*bdx*threadIdx.y+threadIdx.x];                                      \
    float* sld=&smem[   bdx*threadIdx.y+threadIdx.x];                                      \
    mFFT16(c,dir)                                                                          \
    mVMRF16(&d_RF[slot<<4],dir,op)                                                         \
    mPERMUTE_S16_L8x2(sst,sld,c,bdx,8*bdx,16*bdx,7)                                        \
    mFFT8(&c[0],dir)                                                                       \
    mFFT8(&c[8],dir)                                                                       \
    mVMRF8x2(&d_RF[n*128+((slot&(n-1))<<3)],dir,op)                                        \
    mISTORE8x2(d_o,c,8*n*(1<<e),16*n*(1<<e),)                                              \
}
#define CUDA_VFFT128X(LB,dir,n,e,bdx,op) CUDA_VFFTX_DECL(128,LB,dir,n,e,PRF)\
{                                                                           \
    __shared__ float smem[128*bdx];                                         \
    float2 c[16], temp;                                                     \
    unsigned int slot=blockIdx.x/((1<<e)/bdx);                              \
    unsigned int bidx=blockIdx.x&((1<<e)/bdx-1);                            \
    size_t p=blockIdx.y*128*n*(1<<e)+bidx*bdx+threadIdx.x;                  \
    d_i+=(p+slot*128*(1<<e)+threadIdx.y*(1<<e));                            \
    d_o+=(p+slot*(1<<e)+threadIdx.y*n*(1<<e));                              \
    mLOAD16(c,d_i,8*(1<<e),)                                                \
    float* sst=&smem[16*bdx*threadIdx.y+threadIdx.x];                       \
    float* sld=&smem[   bdx*threadIdx.y+threadIdx.x];                       \
    mFFT16(c,dir)                                                           \
    mVMRF16(&d_RF[16*n*threadIdx.y],dir,op)                                 \
    mPERMUTE_S16_L8x2(sst,sld,c,bdx,8*bdx,16*bdx,7)                         \
    mFFT8(&c[0],dir)                                                        \
    mFFT8(&c[8],dir)                                                        \
    mISTORE8x2(d_o,c,8*n*(1<<e),16*n*(1<<e),)                               \
}

#if SM==37
#define NUM_CTA_V128 6
#else
#define NUM_CTA_V128 3
#endif

CUDA_VFFT128(,,1, 2,g)
CUDA_VFFT128(,,2, 4,g)
CUDA_VFFT128(,,3, 8,g)
CUDA_VFFT128(,,4,16,g)
CUDA_VFFT128(,, 5,32,u)
CUDA_VFFT128(,, 6,32,u)
CUDA_VFFT128(,, 7,32,u)
CUDA_VFFT128(,, 8,32,u)
CUDA_VFFT128(,, 9,32,u)
CUDA_VFFT128(,,10,32,u)
CUDA_VFFT128(,,11,32,u)
CUDA_VFFT128(,,12,32,u)
CUDA_VFFT128(,,13,32,u)
CUDA_VFFT128(,,14,32,u)
CUDA_VFFT128(,,15,32,u)
CUDA_VFFT128(,,16,32,u)

CUDA_VFFT128(,i,1, 2,g)
CUDA_VFFT128(,i,2, 4,g)
CUDA_VFFT128(,i,3, 8,g)
CUDA_VFFT128(,i,4,16,g)
CUDA_VFFT128(,i, 5,32,u)
CUDA_VFFT128(,i, 6,32,u)
CUDA_VFFT128(,i, 7,32,u)
CUDA_VFFT128(,i, 8,32,u)
CUDA_VFFT128(,i, 9,32,u)
CUDA_VFFT128(,i,10,32,u)
CUDA_VFFT128(,i,11,32,u)
CUDA_VFFT128(,i,12,32,u)
CUDA_VFFT128(,i,13,32,u)
CUDA_VFFT128(,i,14,32,u)
CUDA_VFFT128(,i,15,32,u)
CUDA_VFFT128(,i,16,32,u)

CUDA_UFFT128X(,,64,1, 2,g)
CUDA_UFFT128X(,,64,2, 4,g)
CUDA_UFFT128X(,,64,3, 8,g)
CUDA_UFFT128X(,,64,4,16,g)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64, 5,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64, 6,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64, 7,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64, 8,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64, 9,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,10,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,11,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,12,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,13,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,14,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,64,15,32,u)

CUDA_UFFT128X(,i,64,1, 2,g)
CUDA_UFFT128X(,i,64,2, 4,g)
CUDA_UFFT128X(,i,64,3, 8,g)
CUDA_UFFT128X(,i,64,4,16,g)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64, 5,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64, 6,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64, 7,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64, 8,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64, 9,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,10,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,11,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,12,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,13,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,14,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,64,15,32,u)

CUDA_UFFT128X(,,128,1, 2,g)
CUDA_UFFT128X(,,128,2, 4,g)
CUDA_UFFT128X(,,128,3, 8,g)
CUDA_UFFT128X(,,128,4,16,g)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128, 5,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128, 6,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128, 7,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128, 8,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128, 9,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128,10,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128,11,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128,12,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128,13,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),,128,14,32,u)

CUDA_UFFT128X(,i,128,1, 2,g)
CUDA_UFFT128X(,i,128,2, 4,g)
CUDA_UFFT128X(,i,128,3, 8,g)
CUDA_UFFT128X(,i,128,4,16,g)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128, 5,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128, 6,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128, 7,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128, 8,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128, 9,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128,10,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128,11,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128,12,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128,13,32,u)
CUDA_UFFT128X(LB(256,NUM_CTA_V128),i,128,14,32,u)

CUDA_VFFT128X(,,128,1, 2,g)
CUDA_VFFT128X(,,128,2, 4,g)
CUDA_VFFT128X(,,128,3, 8,g)
CUDA_VFFT128X(,,128,4,16,g)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128, 5,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128, 6,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128, 7,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128, 8,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128, 9,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128,10,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128,11,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128,12,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128,13,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,128,14,32,u)

CUDA_VFFT128X(,i,128,1, 2,g)
CUDA_VFFT128X(,i,128,2, 4,g)
CUDA_VFFT128X(,i,128,3, 8,g)
CUDA_VFFT128X(,i,128,4,16,g)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128, 5,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128, 6,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128, 7,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128, 8,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128, 9,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128,10,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128,11,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128,12,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128,13,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,128,14,32,u)

CUDA_VFFT128X(,,256,1, 2,g)
CUDA_VFFT128X(,,256,2, 4,g)
CUDA_VFFT128X(,,256,3, 8,g)
CUDA_VFFT128X(,,256,4,16,g)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256, 5,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256, 6,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256, 7,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256, 8,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256, 9,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256,10,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256,11,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256,12,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),,256,13,32,u)

CUDA_VFFT128X(,i,256,1, 2,g)
CUDA_VFFT128X(,i,256,2, 4,g)
CUDA_VFFT128X(,i,256,3, 8,g)
CUDA_VFFT128X(,i,256,4,16,g)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256, 5,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256, 6,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256, 7,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256, 8,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256, 9,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256,10,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256,11,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256,12,32,u)
CUDA_VFFT128X(LB(256,NUM_CTA_V128),i,256,13,32,u)