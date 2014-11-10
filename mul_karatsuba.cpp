#include "defines.hpp"
#include "mpz.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace mp;

static void gradeschool	(integer* a, integer* b, integer* r, size_t d);
static void normalize		(integer* a, size_t d, uinteger b);

// This karatsuba code is an improved version of www.cburch.com/proj/karat/karat.txt
// written by Tran Ha Vu without recursive calls.
// the use of ret array (that have 6*d integers) is pretty  creative:
// | al * bl | ah * bh | asum*bsum | lower-recursion space | asum | bsum |
//  d digits  d digits   d digits     3d digits              d/2    d/2
// the result will be stored only in the first 2d digits

typedef struct {
	bool t;			// false means we just have to do recursive step
					// true means we have to sum some results
	integer* a;
	integer* b;
	integer* r;
	size_t d;
} stackel;

void karatsuba_im (integer* a, integer* b, integer* r, size_t d, uinteger base)
{	
	stackel*			stack;
	register stackel*	back;

	stack = (stackel*) malloc(d * sizeof(stackel)); 
	
	integer* ret = (integer*) malloc(6*d * sizeof(integer));

	integer*	al;		// low-order half of a
	integer*	ah;		// high-order half of a
	integer*	bl;		// low-order half of b
	integer*	bh;		// high-order half of b
	integer*	asum;	// sum of a's halves
	integer*	bsum;	// sum of b's halves
	integer*	x1;		// al * bl location
	integer*	x2;		// ah * bh location
	integer*	x3;		// asum * bsum location

	integer*	a_;		// a in the recursive step
	integer*	b_;		// b in the recursive step
	integer*	r_;		// ret in the recursive step
	size_t d_;			// d in the recursive step

	stackel start = (stackel){.t = false, .a = a, .b = b, .r = ret, .d = d}, tmp;
	*(stack) = start;

	size_t k, i, j;	
	for(back = stack;back >= stack; ){

		r_ = back->r;
		d_ = back->d;	
		x1 = r_;
		x2 = r_ + d_;
		x3 = r_ + (d_<<1);

		if (back->t == false){
			a_		= back->a;	
			b_		= back->b;
			al		= a_;
			ah		= a_ + (d_>>1);
			bl		= b_;
			bh		= b_ + (d_>>1);
			asum	= r_ + d_*5;
			bsum	= r_ + d_*5 + (d_>>1);
	
			// it is better to multiply small numbers with school 
			// grade algorithm (that have complexity O(N^2))
			if (d_ < KARATSUBA_CUTOFF){
				back--;
				gradeschool(a_, b_, r_, d_);
				continue;
			}

			// compute local asum & bsum
			for (k=0; k<(d_>>1); k++){
				*(asum+k) = (*(al+k)) + (*(ah+k));
				*(bsum+k) = (*(bl+k)) + (*(bh+k));
			}
				
			// now we add next recursive steps into the stack
			// it means that after processing x1, x2, x3, it will sum
			// x1 + (x3<<d_/2) + (x2<<d_)
			back--;
			tmp = (stackel) { .t = true,  .a = a_,   .b = b_,   .r = r_, .d = d_    };	*(++back) = tmp;
			tmp = (stackel) { .t = false, .a = asum, .b = bsum, .r = x3, .d = d_>>1 };	*(++back) = tmp;
			tmp = (stackel) { .t = false, .a = ah,   .b = bh,   .r = x2, .d = d_>>1 };	*(++back) = tmp;
			tmp = (stackel) { .t = false, .a = al,   .b = bl,   .r = x1, .d = d_>>1 };	*(++back) = tmp;
		}
		else {
			back--;
			for (k=0; k<d_; k++) *(x3+k) = *(x3+k) - *(x1+k) - *(x2+k);
			for (k=0; k<d_; k++) *(r_+k+d_/2) += *(x3+k);
		}
	}	

	for (k=0; k<(d<<1); k++) *(r+k) = *(ret+k);

	normalize(r, d<<1, base);
	free(stack);
	free(ret);
}

namespace mp
{

mpz karatsuba_mul(mpz lhs, mpz rhs)
{
	mpz A(lhs), B(rhs);
	#ifndef USING_IMPROVED_KARATSUBA
		size_t newlen = std::max(A.length(), B.length());
		if (newlen < KARATSUBA_CUTOFF)
			return schoolgrade_mul(A, B);		

		if (newlen % 2 != 0) newlen++;

		A.value.resize(newlen), B.value.resize(newlen);
		size_t partlen = newlen / 2;

		mpz a[2], b[2], x[3];
		for (int i=0; i<2; i++){
			a[i].value.resize(partlen,0); std::copy(A.value.begin() + (i*partlen), A.value.begin() + (i+1)*partlen, a[i].value.begin());
			b[i].value.resize(partlen,0); std::copy(B.value.begin() + (i*partlen), B.value.begin() + (i+1)*partlen, b[i].value.begin());
		}
	
		x[0] = karatsuba_mul(a[0], b[0]);
		x[2] = karatsuba_mul(a[1], b[1]);
		x[1] = karatsuba_mul(a[0]+a[1], b[0]+b[1]) - x[0] - x[2];
	
		mpz answer = x[0] + (x[1] << partlen) + (x[2] << (partlen<<1));
		answer.Sign = MUL_SIGN(A.Sign, B.Sign);
		answer.EraseLeadingZeros();
		return answer;
	#else
		#ifndef USING_256SYMBOL_IO
			unsigned int base = 10;
		#else
			unsigned int base = 16;
		#endif

		for (; BASE_BYSPLITDEPTH[A.SplitDepth] != base; A.split2());
		for (; BASE_BYSPLITDEPTH[B.SplitDepth] != base; B.split2());
		size_t n = std::max(A.length(), B.length());		
		size_t d;
		for (d = 1; d<n; d<<=1);

		integer* a = (integer*) malloc(d      * sizeof(integer));
		integer* b = (integer*) malloc(d      * sizeof(integer));
		integer* r = (integer*) malloc(d*2    * sizeof(integer));

		A.value.resize(d, 0), B.value.resize(d, 0);
		std::copy(A.value.begin(), A.value.end(), a);
		std::copy(B.value.begin(), B.value.end(), b);

		karatsuba_im(a, b, r, d, base);
		
		mpz answer;
		answer.value.assign(r, r+(d<<1));
		answer.Sign = MUL_SIGN(A.Sign, B.Sign);
		answer.EraseLeadingZeros();
		for (; BASE_BYSPLITDEPTH[answer.SplitDepth] != STORING_BASE; answer.merge2());

		free(a), free(b), free(r);
		return answer;
	#endif	
}

}; // end of namespace mp

static void normalize(integer* a, size_t d, uinteger b)
{
	integer c = (integer) 0;
	size_t i;
	for (i=0; i<d; i++){
		*(a+i) += c;
		if (*(a+i) < 0){
			c = -(-(*(a+i) + 1) / b + 1);		
		} else {
			c = *(a+i) / b;
		}
		*(a+i) -= c*b;
		}
	if (c != 0){
		for (i=0; i<d; i++) fprintf(stderr, "%d ", a[i]); fprintf(stderr, "\n");
		fprintf(stderr, "Overflow! %d\n", c);
	}
}

static void gradeschool(integer* a, integer* b, integer* r, size_t d)
{
	size_t i, j;
	for (i=0; i<2*d; i++) *(r+i) = 0;
	for (i=0; i<d; i++)
		for (j=0; j<d; j++)
			*(r+i+j) += (*(a+i)) * (*(b+j));
}


