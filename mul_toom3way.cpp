#include "defines.hpp"
#include "mpz.hpp"
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace mp;

static void gradeschool		(integer* a, integer* b, integer* r, size_t d, integer c);
static void normalize		(integer* a, size_t d, uinteger b);
static integer trynormalize	(integer* a, size_t d, uinteger b);
static void reverse			(integer* a, size_t d);
static integer divide(integer* a, size_t d, integer n, integer base);

#define DIVIDE(x, y, z)	                                   \
	{                                                      \
		bool positive = (trynormalize(x, y, base) == 0);   \
		if (!positive){                                    \
			reverse(x, y);                                 \
			normalize(x, y, base);                         \
			divide(x, y, z, base);                         \
			reverse(x, y);                                 \
		} else {                                           \
			normalize(x, y, base);                         \
			divide(x, y, z, base);                         \
		}                                                  \
	}
#undef DIVIDE
#define DIVIDE(x, y, z) divide(x, y, z, base);

#define PRINT(x, y, z) \
	std::cout << z; \
	for (size_t __l=0; __l<y; __l++) std::cout << x[__l] << " "; \
	std::cout << "\n";

// in un-normalized state our result will look like this:
//
//      high                                        low
//     +-------+-------+
//     |       w4      |
//     +-------+-------+
//             +-------+-------+
//             |       w3      |
//             +-------+-------+
//                     +-------+-------+
//                     |       w2      |
//                     +-------+-------+
//                             +-------+-------+
//                             |       w1      |
//                             +-------+-------+
//                                     +-------+-------+
//                                     |       w0      |
//                                     +-------+-------+
//
// in our array, we have to store next values: w0 -> w4, suma, sumb, some free spaces, 
// the calculation order must be like this:
// w0, w4, wat1, watm1, wat2, w2, w1, w3
//
//
// so, in our array, we can store these number like
// | w0    | free  | w4    | wat1  | watm1 | wat2  | w2    | w1    | w3    |    free   | sum0a | sum0b | sum1a | sum1b | sum2a | sum2b |
//   2/3 d   2/3 d   2/3 d   2/3 d   2/3 d   2/3 d   2/3 d   2/3 d   2/3 d     6d        1/3 d   1/3 d   1/3 d   1/3 d   1/3 d   1/3 d
// [  RESULT       2d      ][           4d                                 ]           [                    2d                         ]
// lets see what happen if we set the length of ret equal to 12d:
// w0 -> w1 will be calculated without any problems. 
// for w3, it needs at least 6/3d space, the free space length is 14-8 = 6d >= 14/3d 
//
// while the result (not normalized) will be stored at first 2/3*3 == 2d digits
// to calculate 1 multiplication d/3 * d/3, we need at least 6d extra space, 

typedef struct 
{	
	bool t;		// false means we have to do recursive step
				// true means we have to calculate some results
	integer* a;
	integer* b;
	integer* r;
	size_t d;		
} stckel;

void toom3way_im (integer* a, integer* b, integer* r, size_t d, uinteger base)
{
	stckel*				stack;	
	register stckel*	back;
	stack	= (stckel*) malloc(d * sizeof(stckel));
	back	= NULL;

	integer* ret = (integer*) malloc(14*d * sizeof(integer));

	integer *al, *bl, *am, *bm, *ah, *bh, *asum0, *bsum0, *asum1, *bsum1, 
			*asum2, *bsum2, *w0, *w1, *w2, *w3, *w4, *freesp, *wat1, *watm1, *wat2;
		
	integer* a_;		// a in current recursive step
	integer* b_;		// b in current recursive step
	integer* r_;		// ret in current recursive step
	size_t d_;			// d in current recursive step
		
	size_t partlen;		// d/3 in current recursive step

	stckel start = (stckel){ .t = false, .a = a, .b = b, .r = ret, .d = d}, tmp;
	*stack = start;

	size_t k, i, j;
	for (back = stack; back >= stack; ){
		a_ = back->a;
		b_ = back->b;
		r_ = back->r;
		d_ = back->d;
		
		partlen		= d_/3;
		al			= a_;
		am			= a_		+ partlen;
		ah			= a_		+ (partlen<<1);
		bl			= b_;
		bm			= b_		+ partlen;
		bh			= b_		+ (partlen<<1);
		
		w0			= r_;	
		freesp		= r_		+ (partlen<<1);
		w4			= freesp	+ (partlen<<1);
		wat1		= w4		+ (partlen<<1);
		watm1		= wat1		+ (partlen<<1);
		wat2		= watm1		+ (partlen<<1);
		w2			= wat2		+ (partlen<<1);
		w1			= w2		+ (partlen<<1);
		w3			= w1		+ (partlen<<1);			
	
		asum0		= r_	+ d_*12;
		bsum0		= asum0 + partlen;
		asum1		= bsum0 + partlen;
		bsum1		= asum1 + partlen;
		asum2		= bsum1 + partlen;
		bsum2		= asum2 + partlen;
		
		if (back->t == false){
	
			// it is better to multiply small numbers with school grade
			// algorithm (which have complexity O(N^2))
			if (d_ < TOOM3WAY_CUTOFF){
				back--;
				gradeschool(a_, b_, r_, d_, 1);	
				continue;
			}
		
			for (i=0; i<partlen; i++){
				*(asum0 + i) = *(al+i) + *(am+i) + *(ah+i);
				*(bsum0 + i) = *(bl+i) + *(bm+i) + *(bh+i);
			
				*(asum1 + i) = *(al+i) - *(am+i) + *(ah+i);
				*(bsum1 + i) = *(bl+i) - *(bm+i) + *(bh+i);
		
				*(asum2 + i) = *(al+i) + 2*(*(am+i)) + 4*(*(ah+i));
				*(bsum2 + i) = *(bl+i) + 2*(*(bm+i)) + 4*(*(bh+i));
			}
			
			// here we have to be careful about the calculation order
			// the right order is: w0 -> w4 -> wat1 -> watm1 -> wat2 -> w2 -> w1 -> w3
			// the formula for each variable is:
			// w0    = al * bl
			// w4    = ah * bh
			// w(1)  = (al + am + ah)*(bl + bm + bh)
			// w(-1) = (al - am + ah)*(bl - bm + bh)
			// w(2)  = (al + 2*am + 4*ah)*(bl + 2*bm + 4*bh)
			// w2    = (w(1) + w(-1) - 2*(w0 + w4)) / 2
			// w1    = (24*w4 + 12*w2 + 9*w0 - wat2 - 8*w(-1)) / 6
			// w3    = (w(1) - w(-1) - 2*w1) / 2			
			
			back--;			
			tmp = (stckel){ .t = true,  .a = a_,    .b = b_,    .r = r_,    .d = d_      };	*(++back) = tmp;
			tmp = (stckel){ .t = false, .a = asum2, .b = bsum2, .r = wat2,  .d = partlen }; *(++back) = tmp;					
			tmp = (stckel){ .t = false, .a = asum1, .b = bsum1, .r = watm1, .d = partlen }; *(++back) = tmp;
			tmp = (stckel){ .t = false, .a = asum0, .b = bsum0, .r = wat1,  .d = partlen }; *(++back) = tmp;
			tmp = (stckel){ .t = false, .a = ah,    .b = bh,    .r = w4,    .d = partlen }; *(++back) = tmp;		
			tmp = (stckel){ .t = false, .a = al,    .b = bl,    .r = w0,    .d = partlen }; *(++back) = tmp;
		}					
		else {
			uinteger dd;
			integer carry;
			for (i=0; i<(partlen<<1); i++)
				freesp[i] = 0;

			for (i=0; i<(partlen<<1); i++)
				w2[i] = (wat1[i] + watm1[i] - 2*(w0[i] + w4[i])) ;
				dd = DIVIDE(w2, partlen<<1, 2); //std::cout << dd << " ";
				
			for (i=0; i<(partlen<<1); i++)
				w1[i] = (24*w4[i] + 12*w2[i] + 9*w0[i] - wat2[i] - 8*watm1[i]);						
				dd = DIVIDE(w1, partlen<<1, 6); //std::cout << dd << " ";

			for (i=0; i<(partlen<<1); i++)
				w3[i] = (wat1[i] - watm1[i] - 2*w1[i]);
				dd = DIVIDE(w3, partlen<<1, 2); //std::cout << dd << " ";
				
			for (i=0; i<(partlen<<1); i++){
				ret[i + partlen]	+= w1[i];
				ret[i + partlen*2]	+= w2[i];
				ret[i + partlen*3]	+= w3[i];
			}

			back--;
		}				
	}
//	std::cout << "\n";

	for (i=0; i<d<<1; i++) r[i] = ret[i];
	normalize(r, d<<1, base);	
	free(stack);
	free(ret);
}





namespace mp
{

mpz toom_3way_mul (mpz A, mpz B)
{
	#ifndef USING_IMPROVED_TOOM3WAY
		size_t newlen = std::max(A.length(), B.length());
		if (newlen < TOOM3WAY_CUTOFF)
			return schoolgrade_mul(A, B);		

		if (newlen % 3 != 0) newlen += 3-(newlen % 3);
		
		A.value.resize(newlen,0), B.value.resize(newlen,0);
		size_t partlen = newlen / 3;
		mpz a[3], b[3], w[5], w1, w2, w_1;
		for (int i=0; i<3; i++){
			a[i].value.resize(partlen, 0), b[i].value.resize(partlen, 0);			
			std::copy(A.value.begin()+(i*partlen), A.value.begin()+(i+1)*partlen, a[i].value.begin());
			std::copy(B.value.begin()+(i*partlen), B.value.begin()+(i+1)*partlen, b[i].value.begin());
		}
		w[0] = toom_3way_mul(a[0], b[0]);
		w[4] = toom_3way_mul(a[2], b[2]);
		w1   = toom_3way_mul(a[0]+a[1]+a[2], b[0]+b[1]+b[2]);
		w_1  = toom_3way_mul(a[0]-a[1]+a[2], b[0]-b[1]+b[2]);
		w2   = toom_3way_mul(a[0] + 2*a[1] + 4*a[2], b[0] + 2*b[1] + 4*b[2]);	
		w[2] = (w1 + w_1 - 2*(w[0] + w[4])) / 2;
		w[1] = (24*w[4] + 12*w[2] + 9*w[0] - w2 - 8*w_1) / 6;
		w[3] = (w1 - w_1 - 2*w[1]) / 2;
	
		for (int i=1; i<5; i++)
			w[i] = w[i] << partlen*i;
		mpz answer = w[0] + w[1] + w[2] + w[3] + w[4];

		answer.Sign = MUL_SIGN(A.Sign, B.Sign);
		answer.EraseLeadingZeros();
		return answer;
	#else
		/* in toom 3-way, as you see, we have to use division operation.
		 * so, we'll multiply w[i] (0 <= i <= 5) to 6, and then divide it by 6.
		 */
		
		#ifndef USING_256SYMBOL_IO
			unsigned int base = 10;
		#else
			unsigned int base = 16;
		#endif
	
		for (; BASE_BYSPLITDEPTH[A.SplitDepth] != base; A.split2());
		for (; BASE_BYSPLITDEPTH[B.SplitDepth] != base; B.split2());
		size_t n = std::max(A.length(), B.length());		
		size_t d;
		for (d = 1; d<n; d *= 3);

		integer* a = (integer*) malloc(d      * sizeof(integer));
		integer* b = (integer*) malloc(d      * sizeof(integer));
		integer* r = (integer*) malloc(d*2    * sizeof(integer));

		A.value.resize(d, 0), B.value.resize(d, 0);
		std::copy(A.value.begin(), A.value.end(), a);
		std::copy(B.value.begin(), B.value.end(), b);
	
		toom3way_im(a, b, r, d, base);

		mpz answer;
		answer.value.assign(r, r+(d<<1));
		answer.Sign = MUL_SIGN(A.Sign, B.Sign);
		//answer = answer / 6;
		answer.EraseLeadingZeros();
		for (; BASE_BYSPLITDEPTH[answer.SplitDepth] != STORING_BASE; answer.merge2());

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
	if (c != 0);
//		fprintf(stderr, "Overflow! %d\n", c);
}
static integer trynormalize(integer* a, size_t d, uinteger b)
{
	integer c = (integer) 0, bc;
	size_t i;
	for (i=0; i<d; i++){
		*(a+i) += c; bc = c;
		if (*(a+i) < 0){
			c = -(-(*(a+i) + 1) / b + 1);		
		} else {
			c = *(a+i) / b;
		}
		*(a+i) -= bc;
	}
	return c;
}

static void reverse(integer* a, size_t d)
{
	size_t i;
	for (i=0; i<d; i++) *(a+i) = -*(a+i);
}

static void gradeschool(integer* a, integer* b, integer* r, size_t d, integer c)
{
	size_t i, j;
	for (i=0; i<2*d; i++) *(r+i) = 0;
	for (i=0; i<d; i++)
		for (j=0; j<d; j++)
			*(r+i+j) += (*(a+i)) * (*(b+j)) * c;
}

static integer divide(integer* a, size_t d, integer n, integer base)
{
	if (n > base) return 0;
	integer carry = 0;
	for (size_t i=d; i-- > 0; ) {
		integer cur = a[i] + carry * 1ll * base;
		a[i] = integer (cur / n);
		carry = integer (cur % n);
	}
	return carry;
}

