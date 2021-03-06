#ifndef __xfft_op_h__
#define __xfft_op_h__

#include"xfft_context.h"
#include"xfft_kernel.h"
#include"xfft_calcRF.h"

typedef struct __xfftOp{
	xfft_kernel_t*	p_kernel;
	CUtexref		texref;
	CUdeviceptr		d_HRF;	
	CUdeviceptr		d_URF;
	CUdeviceptr		d_VRF;
	int				nk;
	int				nx;
	int				ny;
	int				nz;
	int				bat;
	int				eh;
	int				eu;
	int				ev;
	char			padding[56];
} xfftOp_t;

int			xfft_createOp_1d( xfftOp_t* const __restrict, xfft_context_t* const __restrict, int, int, float2* __restrict );
int			xfft_createOp_2d( xfftOp_t* const __restrict, xfft_context_t* const __restrict, int, int, int, float2* __restrict );
int			xfft_createOp_3d( xfftOp_t* const __restrict, xfft_context_t* const __restrict, int, int, int, int, float2* __restrict );
CUdeviceptr	xfft_exec( xfftOp_t* const, CUdeviceptr, CUdeviceptr, int, CUstream );
void		xfft_release( xfftOp_t* const );

#endif