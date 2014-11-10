#include "defines.hpp"

#ifndef ENABLE_GPUCOMPUTATION

#include "modules.hpp"
#include "mpz.hpp"
#include "mpmod.hpp"
#include <stdio.h>
#include <iostream>

static int gcd (int a, int b, int& x, int& y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcd (b%a, a, x1, y1);
    x = y1 - (b / a)* x1;
    y = x1;
    return d;
}



namespace mp{

mpmod mpz_to_mpmod(mpz num)
{
	mpz A(num);
	mpmod B;
	for (size_t j = 0; j<MODULAR_BASE; j++){
		size_t deg_mod = 1;
		size_t a_mod = A.value[0] % MODULES[j];
		for (size_t i=1; i<A.length(); i++){
			deg_mod = (deg_mod * STORING_BASE) % MODULES[j];
			a_mod = (a_mod + deg_mod*A.value[i]) % MODULES[j];
		}
		B.m[j] = a_mod;
	}
	return B;
}

mpz mpmod_to_mpz(mpmod num)
{
	mpmod A(num);
    mpz c;
    int R[MODULAR_BASE - 1][MODULAR_BASE];
    int y = 0;
    for(unsigned int i = 0; i < MODULAR_BASE - 1; i++)
        for(unsigned int j = i+1; j < MODULAR_BASE; j++){
            if (gcd(MODULES[i],MODULES[j],R[i][j],y) != 1){
                fprintf(stderr, "Error, gcd is not 1!\n");
                return c;
            }
            R[i][j] = (R[i][j] % MODULES[j] + MODULES[j]) % MODULES[j];
        }
	long long int x[MODULAR_BASE];
    for (unsigned int i=0; i<MODULAR_BASE; ++i) {
        x[i] = A.m[i];
        for (unsigned int j=0; j<i; ++j) {
            x[i] = R[j][i] * (x[i] - x[j]);

            x[i] = x[i] % MODULES[i];
            if (x[i] < 0)  x[i] += MODULES[i];
        }
    }

    mpz P;
    P = 1;
    for(unsigned int i = 0; i < MODULAR_BASE; i++){
        c = c + P*((int)x[i]);
        P = P*MODULES[i];
    }

	c.EraseLeadingZeros();
    return c;
}

mpmod operator* (mpmod& lhs, mpmod& rhs)
{
	mpmod ans;
	for (size_t i=0; i<MODULAR_BASE; i++){
		ans.m[i] = lhs.m[i] * rhs.m[i];
	}
	return ans;
}

mpmod operator+ (mpmod& lhs, mpmod& rhs)
{
	mpmod ans;
	for (size_t i=0; i<MODULAR_BASE; i++){
		ans.m[i] = lhs.m[i] + rhs.m[i];
	}
	return ans;
}

mpmod operator- (mpmod& lhs, mpmod& rhs)
{
	mpmod ans;
	for (size_t i=0; i<MODULAR_BASE; i++){
		ans.m[i] = lhs.m[i] - rhs.m[i];
	}
	return ans;
}


} // end of namespace mp

#endif
