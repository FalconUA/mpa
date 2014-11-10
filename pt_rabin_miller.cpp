#include "defines.hpp"
#include "mpz.hpp"

using namespace mp;


namespace mp{

bool rabin_miller_test(mpz num, int r){
	mpz A(num);

	if(A == 1){
		return false;
	}
	
    if(A == 2){
        return true;
    }

	if(A == 3){
		return true;
	}

    if(A % 2 == 0){
        return false;
    }

    mpz m_1 = A - 1;
    mpz t(m_1), x;
    x = 1;
    size_t s;
    for(s = 0; (t % 2) == 0; s++)
        t = t / 2;

    for(; r > 0; r--){
        mpz a = random(A,10000);
        mpz z;
        z = deg(a,t,A);
        if((z == mp::ONE)||(z == m_1)){
            return true;
        }
        for(size_t j = 0; j < s; j++){
            if((z == mp::ONE)&&(j > 0)){
                return false;
            }
            if(z == m_1){
                return true;
            }
            if(j != s - 1)
                z = (z*z) % A;
            else{
                return false;
            }
        }
    }
    return true;
}

bool miller_rabin_test(const mpz& num, int r)
{
	rabin_miller_test(num, r);
}

} // end of namespace mp


