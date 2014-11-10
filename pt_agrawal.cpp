#include "defines.hpp"
#include "mpz.hpp"
#include "mpr.hpp"
#include <iostream>
using namespace mp;

static mpz phi(const mpz& num){//Euler
	mpz n(num);
    mpz ret(mp::ONE);
    mpz i;
    for(i = 2; fft_mul(i, i) <= n; i = i + 1) {
		mpz p(mp::ONE);
        while(n % i == 0) {
            p = mul(p, i, mp::FFT);
            n = n / i;
        }
        if((p = p / i) >= 1) ret = mul(ret, mul(p, (i - 1), mp::FFT), mp::FFT);
    }
    n = n - 1;
    return (n != 0) ? mul(n, ret, mp::FFT) : ret;
}

namespace mp{

bool agrawal_test(mpz num){
    mpz a(num), A(num);
    mpz Log_2_a, Log2_2_a;

    for(Log_2_a = 0; a >= 2; Log_2_a = Log_2_a + 1, a = a / 2);
    Log2_2_a = fft_mul(Log_2_a, Log_2_a);
	mpz	 r, k;
    bool nextR = true;
    for(r = 2; nextR; r = r + 1){
        nextR=false;
        a = 1;
        for(k = 1; (!nextR) && (k<=Log2_2_a); k = k + 1){
            a = fft_mul(a, A) % r;//deg((*this),k,r);
            nextR = ((a==mp::ONE) || (a==mp::ZERO) || (a==mp::EMPTY));
        }
    }
    r = r-1;

    //======================3=====================//
    mpz GCD;
    for(a = r; a > 1; a = a - 1)
        if(((GCD = gcd(a,A)) > 1) && (GCD < A)){
            return false;
        }
    //======================4=====================//
    if(A <= r){
        return true;
    }
    //======================5=====================//

    k = 2;
    mpz max = fft_mul( sqrt(phi(r)) , Log_2_a );
    for(; k <= max; k = k + 1)
        if(deg(k,A,A) != k){
            return false;
        }

    //======================6=====================//
    return true;
}

} // end of namespace mp
