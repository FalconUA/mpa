#include "defines.hpp"
#include "mpz.hpp"
#include <algorithm>

namespace mp{

/* number theory algorithms */

mpz abs(mpz num){
	mpz ans(num);
	ans.Sign = mp::PLUS;
	return ans;
}

mpz random(mpz upperbound, int iterations)
{
    //generates numbers from 0 to mod-1 by fibonachchi method: [0,mod-1];
	mpz mod(upperbound);
    if(mod == mp::ZERO)
        return mp::ZERO;
    size_t n;
    n = rand() % iterations;
    mpz X, X_1, Y, M = STORING_BASE*mod;
    X = 1;
    for(size_t i = 0; i < n; i++){
        Y = X;
        X = (X + X_1) % M;
        X_1 = Y;
    }
    return X/STORING_BASE;
}


mpz gcd(mpz lhs, mpz rhs){
	mpz a(lhs), b(rhs);
	mpz zz = mp::ONE % mp::ZERO;
    mpz t;
	while (b > mp::ZERO){
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

mpz sqrt(mpz num){
	mpz a(num);
    mpz cur;

    integer pos = (a.value.size() + 1) / 2;
    cur.value.resize(pos);
    pos--;
    while (pos >= 0){
        integer l = 0, r = STORING_BASE;
        integer curDigit = 0;
        while (l <= r){
            integer m = (l+r)>>1;
            cur.value[pos] = m;
            if (cur * cur <= a){
                curDigit = m;
                l = m + 1;
            }
            else
                r = m - 1;
        }
        cur.value[pos] = curDigit;
        pos--;
    }
    cur.EraseLeadingZeros();
    return cur;
}

mpz deg (mpz A, mpz B, mpz MOD){
	mpz ans(mp::ONE), a(A), b(B), mod(MOD);
	for (; b > mp::ZERO; b = b/2 ){
		if(b%2 == 1){
			ans = fft_mul(ans, a);
			ans = ans % mod;
		}
		a = fft_mul(a, a);
		a = a % mod;
	}
	return ans;
}

}; // end of namespace mp
