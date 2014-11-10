#include "defines.hpp"
#include "mpz.hpp"
#include <iostream>
using namespace mp;


static mpz Jakobi(const mpz& A, const mpz& N){
	mpz a(A), n(N);
    mpz J, s;
    s = (n-1)/2;
    integer e_s;
    if(a < 0){
        if(s%2 == 0)
            e_s = 1;
        else
            e_s = -1;
        J = Jakobi(-a,n) * e_s;
        return J;
    }
    if(a % 2 == 0){
        if(((fft_mul(n,n)-1)/8)%2 == 0)
            e_s = 1;
        else
            e_s = -1;
        J = Jakobi(a/2,n) * e_s;
        return J;
    }
    if(a == 1){
        J = 1;
        return J;
    }
    if(a < n){
        if( ( ((a-1)/2)*((n-1)/2) ) % 2 == 0 )
            e_s = 1;
        else
            e_s = -1;
        J = Jakobi(n,a) * e_s;
        return J;
    }
    J = Jakobi(a % n, n);
    return J;
}


namespace mp{

bool solovey_strassen_test(mpz num, int k){
	mpz A(num);
    if(A < 2){
		return false;
    }
    if((A == 2)||(A == 3)){
		return true;
    }
    for(int i = 0; i < k; i++){
        mpz a = random(A,1000);
        if(gcd(a,A) != mp::ONE){
            return false;
        }
        mpz s = (A - 1)/2;
        mpz j;
        j = deg(a,s,A);
        //j = fft_mul(a,s) % A;
        mpz J = Jakobi(a,A);
        while(J < mp::ZERO)
            J = J + A;

        if(j != J){
		    return false;
        }
    }
    return true;
}
bool strassen_solovey_test(const mpz& num, int r)
{
	solovey_strassen_test(num, r);
}

}// end of namespace mp
