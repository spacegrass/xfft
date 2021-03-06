#include"../include/xfft_calcRF.h"

#define PI 3.1415926535897931e+0

#define mMakeRF(p,s){				\
	(p).x=(float)cos((s)*-2.0*PI);	\
	(p).y=(float)sin((s)*-2.0*PI);	\
}

static void __calcVRF4( float2* p, int n, double rt )
{
	int i;
	for( i=0; i<n; ++i, p+=4 )
	{
		mMakeRF(p[0],i*rt*2.0)
		mMakeRF(p[2],i*rt*1.0)
		mMakeRF(p[3],i*rt*3.0)
	}
}
static void __calcVRF8( float2* p, int n, double rt )
{
	int i;
	for( i=0; i<n; ++i, p+=8 )
	{
		mMakeRF(p[0],i*rt*4.0)
		mMakeRF(p[2],i*rt*2.0)
		mMakeRF(p[3],i*rt*6.0)
		mMakeRF(p[4],i*rt*1.0)
		mMakeRF(p[5],i*rt*5.0)
		mMakeRF(p[6],i*rt*3.0)
		mMakeRF(p[7],i*rt*7.0)
	}
}
static void __calcVRF16( float2* p, int n, double rt )
{
	int i;
	for( i=0; i<n; ++i, p+=16 )
	{
		mMakeRF(p[ 0],i*rt* 8.0)
		mMakeRF(p[ 2],i*rt* 4.0)
		mMakeRF(p[ 3],i*rt*12.0)
		mMakeRF(p[ 4],i*rt* 2.0)
		mMakeRF(p[ 5],i*rt*10.0)
		mMakeRF(p[ 6],i*rt* 6.0)
		mMakeRF(p[ 7],i*rt*14.0)
		mMakeRF(p[ 8],i*rt* 1.0)
		mMakeRF(p[ 9],i*rt* 9.0)
		mMakeRF(p[10],i*rt* 5.0)
		mMakeRF(p[11],i*rt*13.0)
		mMakeRF(p[12],i*rt* 3.0)
		mMakeRF(p[13],i*rt*11.0)
		mMakeRF(p[14],i*rt* 7.0)
		mMakeRF(p[15],i*rt*15.0)
	}
}
static void __calcVRF32( float2* p, int n )
{
	__calcVRF8( p, n>>3, 1.0/n );
	if(n>32){
		__calcVRF4( &p[n], n>>5, 1.0/(n>>3) );
	}
}
static void __calcVRF64( float2* p, int n )
{
	__calcVRF8( p, n>>3, 1.0/n );
}
static void __calcVRF128( float2* p, int n )
{
	__calcVRF16( p, n>>4, 1.0/n );
	if(n>128){
		__calcVRF8( &p[n], n>>7, 1.0/(n>>4) );
	}
}
static void __calcVRF256( float2* p, int n )
{
	__calcVRF16( p, n>>4, 1.0/n );
}
static void (*p_calcVRF[])( float2*, int )={ __calcVRF32, __calcVRF64, __calcVRF128, __calcVRF256 };

int xfft_get_size_HRF( int e )
{
	int n[]={0,0,0,0,16,8,8,16,64,256,256,256,2048,2048,4096,4096};
	return n[e];
}
int xfft_get_size_VRF( int e )
{
	int ue, ve, s, n;
	if(e<4) return 0;
	ue=1+e;
	n=1<<ue;
	ve=(ue>>1)+(ue&1);
	if((ve==5)|(ve==7)){
		s=(ve==5)?3:4;
		n+=(n>>s);
	}
	return n;
}
void xfft_calcHRF( float2* const p, int n, double rt )
{
	int i=0;
	do{ mMakeRF(p[i],i*rt) }while((++i)<n);
}
void xfft_calcVRF( float2* const p, int e )
{
	int radix=e+1, i=radix;
	if(radix>8){ i=(radix>>1)+(radix&1); }
	p_calcVRF[i-5]( p, 1<<radix );
}