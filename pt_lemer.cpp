#include "defines.hpp"
#include "mpz.hpp"

using namespace mp;

static mpz mercen(const size_t p)
{
	mpz M;
	M = 2;
	if (p<1) return M;
	M = (M^p) - 1;
	return M;
}

namespace mp
{

bool lemer_test(size_t p){
    if(p < 2){
        return false;
    }
    if(p == 2){
        return true;
    }

    for(size_t i = 2, end = p >> 1; i <= end; i++)
        if(p % i == 0){
            return false;
        }
    mpz L;
    L = 4;
    mpz M = mercen(p);
    while(p > 2){
	        L = (fft_mul(L,L) - 2) % M;
		 p--; //??
		}
    if((L.length() == 1)&&(L.value[0] == 0)){
        return true;
    }
    return false;
}

} // end of namespace mp
