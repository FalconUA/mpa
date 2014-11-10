#include "defines.hpp"
#include "mpz.hpp"

namespace mp{

mpz schoolgrade_mul(mpz lhs, mpz rhs)
{
	mpz answer, A(lhs), B(rhs);
	for(; A.SplitDepth != 0; A.split2());
	for(; B.SplitDepth != 0; B.split2());

	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = mp::PLUS;
	if ((A.length() == 1)&&(A.value[0] == 0)) A.Sign = B.Sign;
	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = A.Sign;

	answer.value.resize(A.value.size() + B.value.size() + 1, 0);
	for (size_t i=0; i<A.value.size(); i++){
		unsigned long long delta = 0;
		for (size_t j=0; (j<B.length())||(delta > 0); j++){
			unsigned long long cur = answer.value[i+j] + A.value[i] * 1ll * ((j<B.value.size())?(B.value[j]):(0)) + delta;
			answer.value[i+j] = cur % STORING_BASE;
			delta = cur / STORING_BASE;
		}									
	} 
	answer.Sign = MUL_SIGN(A.Sign, B.Sign);
	answer.EraseLeadingZeros();
	return answer;
}

} // end of namespace mp
