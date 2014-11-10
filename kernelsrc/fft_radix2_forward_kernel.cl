#define PI 3.14159265358979323846	
#define PI_2 1.57079632679489661923	

// Return A*B
float2 mul(float2 a,float2 b)
{
#if USE_MAD
  return (float2)(mad(a.x,b.x,-a.y*b.y),mad(a.x,b.y,a.y*b.x)); // mad
#else
  return (float2)(a.x*b.x-a.y*b.y,a.x*b.y+a.y*b.x); // no mad
#endif
}

// Return A * exp(K*ALPHA*i)
float2 twiddle(float2 a,int k,float alpha)
{
  float cs,sn;
  sn = sincos((float)k*alpha,&cs);
  return mul(a,(float2)(cs,sn));
}

// In-place DFT-2, output is (a,b). Arguments must be variables.
#define DFT2(a,b) { float2 tmp = a - b; a += b; b = tmp; }

// Compute T x DFT-2.
// T is the number of threads.
// N = 2*T is the size of input vectors.
// X[N], Y[N]
// P is the length of input sub-sequences: 1,2,4,...,T.
// Each DFT-2 has input (X[I],X[I+T]), I=0..T-1,
// and output Y[J],Y|J+P], J = I with one 0 bit inserted at postion P. */
__kernel void fft_radix2_forward(__global const float2 * x,__global float2 * y,int p)
{
  int t = get_global_size(0); // thread count
  int i = get_global_id(0);   // thread index
  int k = i&(p-1);            // index in input sequence, in 0..P-1
  int j = ((i-k)<<1) + k;     // output index
  float alpha = -PI*(float)k/(float)p;

  // Read and twiddle input
  x += i;
  float2 u0 = x[0];
  float2 u1 = twiddle(x[t],1,alpha);

  // In-place DFT-2
  DFT2(u0,u1);

  // Write output
  y += j;
  y[0] = u0;
  y[p] = u1;1
}
