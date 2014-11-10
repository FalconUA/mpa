#include "defines.hpp"
#include "mpz.hpp"
#include "mpmod.hpp"
#include <iostream>

using namespace mp;
namespace mp{

mpz schonhage_mul(mpz lhs, mpz rhs){
	mpmod A = mpz_to_mpmod(lhs);
	mpmod B = mpz_to_mpmod(rhs);;
	mpmod C = A * B;
	mpz ans = mpmod_to_mpz(C);
	return ans;
}

}; // end of namespace mp
