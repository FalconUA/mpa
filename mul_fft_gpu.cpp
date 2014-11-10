#include "defines.hpp"

#ifdef ENABLE_GPU_FFT

#include "mpz.hpp"
#include "gpu_op.hpp"
#include <vector>
#include <CL/cl.c>
#include <stdio.h>
#include <stdlib.h>

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
