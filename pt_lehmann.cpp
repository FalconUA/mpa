#include "defines.hpp"
#include "mpz.hpp"
#include <iostream>
using namespace mp;


namespace mp{

bool lehmann_test(mpz num, int t){
	mpz A(num);
    if(A < 2){
        return false;
    }

	mpz a, s;
    for(; t > 0; t--){
        a = random(A,1000);
        s = deg(a,(A-1)/2,A);
        if((s != mp::ONE)&&(s != (A - mp::ONE))){
			return false;
        }
    }
    return true;
}

} // end of namespace mp
