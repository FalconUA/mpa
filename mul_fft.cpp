#include "defines.hpp"

#ifndef ENABLE_GPU_FFT

#include "mpz.hpp"
#include <vector>
#include <complex>
#include <cmath>

typedef std::complex< double > base;

static int rev (int num, int lg_n) {
	int res = 0;
	for (int i=0; i<lg_n; ++i)
		if (num & (1<<i))
			res |= 1<<(lg_n-1-i);
	return res;
}
 
static void fft (std::vector<base> & a, bool invert) {
	int n = (int) a.size();
	int lg_n = 0;
	while ((1 << lg_n) < n)  ++lg_n;
 
	for (int i=0; i<n; ++i)
		if (i < rev(i,lg_n))
			std::swap (a[i], a[rev(i,lg_n)]);
 
	for (int len=2; len<=n; len<<=1) {
		double ang = 2*M_PI/len * (invert ? -1 : 1);
		base wlen (cos(ang), sin(ang));
		for (int i=0; i<n; i+=len) {
			base w (1);
			for (int j=0; j<len/2; ++j) {
				base u = a[i+j],  v = a[i+j+len/2] * w;
				a[i+j] = u + v;
				a[i+j+len/2] = u - v;
				w *= wlen;
			}
		}
	}
	if (invert)
		for (int i=0; i<n; ++i)
			a[i] /= n;
}
 
namespace mp{
mpz fft_mul(mpz lhs, mpz rhs)
{
	mpz A(lhs), B(rhs), answer;

	for (; BASE_BYSPLITDEPTH[A.SplitDepth] != FFT_BASE; A.split2());
	for (; BASE_BYSPLITDEPTH[B.SplitDepth] != FFT_BASE; B.split2());

	std::vector< base > fa(A.value.begin(), A.value.end());
	std::vector< base > fb(B.value.begin(), B.value.end());
	size_t n = 1;
	while (n < std::max(fa.size(), fb.size())) n <<= 1;
	n <<= 1;
	fa.resize(n),	fb.resize(n);

	fft(fa, false), fft(fb, false);
	for (size_t i=0; i<n; ++i)
		fa[i] *= fb[i];
	fft(fa, true);

	answer.value.resize(n, 0);
	for (size_t i=0; i<n; ++i)
		answer.value[i] = (int) (fa[i].real() + 0.5);
	answer.SplitDepth = A.SplitDepth;
	answer.Normalize();

	answer.Sign = MUL_SIGN(A.Sign, B.Sign);
	answer.EraseLeadingZeros();
	for (; BASE_BYSPLITDEPTH[answer.SplitDepth] != STORING_BASE; answer.merge2());

	return answer;	
}

}; // end of namespace mp

#endif
