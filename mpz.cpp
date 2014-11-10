#include "defines.hpp"
#include "mpz.hpp"
#include "mpr.hpp"
#include "mpmod.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#ifdef USING_256SYMBOL_IO
	#define VAL_FROM_CHAR(x)	(uinteger)(x)
	#define CHAR_FROM_VAL(x)	(char)(x+'\0')
#else
	#define VAL_FROM_CHAR(x)	(uinteger)(x-'0')
	#define CHAR_FROM_VAL(x)	(char)(x+'0')
#endif

namespace mp
{

mp::mpz::mpz(integer A)
{
	this->SplitDepth = 0;
	this->Sign = (A > 0)?(mp::PLUS):(mp::MINUS);
	this->value.push_back(A);
	this->Normalize();
}

void mp::mpz::Read(std::string strValue)
{	
	this->value.clear();
	this->SplitDepth = 0;
	this->Sign = mp::PLUS;

	#ifndef USING_256SYMBOL_IO
		size_t start = 0;		
		if (strValue[0] == PLUS) {this->Sign = PLUS; start = 1;}
		else if (strValue[0] == MINUS) {this->Sign = MINUS; start = 1;}
		else {this->Sign = PLUS; start = 0;}
	#else
		size_t start = 0;
	#endif

	for (; BASE_BYSPLITDEPTH[this->SplitDepth] != INPUT_OUTPUT_BASE; this->split2());
	for (size_t i=strValue.length(), exp = 0; i-- > start; exp++){
		this->value.push_back(VAL_FROM_CHAR(strValue[i]));
	}
	for (; BASE_BYSPLITDEPTH[this->SplitDepth] != STORING_BASE; this->merge2());
}

std::string mp::mpz::Print()
{
	std::string result("");

	if ( ((this->length() == 1)&&(this->value[0]==0)) || (this->length() == 0)){
		result = CHAR_FROM_VAL(0);
		return result;
	}

	for (; BASE_BYSPLITDEPTH[this->SplitDepth] != INPUT_OUTPUT_BASE; this->split2());
	for (size_t i=0; i<this->value.size(); i++){
		result = CHAR_FROM_VAL(this->value[i]) + result;
	}
	for (; BASE_BYSPLITDEPTH[this->SplitDepth] != STORING_BASE; this->merge2());

	result = ((char) this->Sign) + result;
	return result;
}

std::string mp::mpz::PrintRaw(std::string separator)
{
	std::stringstream ss("");
	ss << (char)this->Sign << separator;
	for (size_t i=this->value.size(); i-- > 0; ){
		ss << this->value[i] << separator;
	}
	return ss.str();	
}

size_t mp::mpz::length()
{	
	return this->value.size();
}

void mp::mpz::EraseLeadingZeros()
{	
	while ((this->value.size() > 1) && (this->value.back() == 0)) this->value.pop_back();
}

void mp::mpz::split2()
{	
	if (this->SplitDepth == MAX_SPLITDEPTH) return ;

	std::vector< integer > newvec(0);
	this->SplitDepth++;
	for (size_t i=0; i<this->length(); i++){	
		newvec.push_back(this->value[i] % BASE_BYSPLITDEPTH[this->SplitDepth]);
		newvec.push_back(this->value[i] / BASE_BYSPLITDEPTH[this->SplitDepth]);
	}			
	this->value = newvec;
}

void mp::mpz::merge2()
{
	if (this->SplitDepth == 0) return ;
	std::vector< integer > newvec(0);
	
	if (this->length() % 2 == 1) this->value.push_back(0);
	for (size_t i = 0; i<this->length() / 2; i++){
		newvec.push_back(this->value[i<<1] + BASE_BYSPLITDEPTH[this->SplitDepth]*this->value[(i<<1)+1]);
	}
	this->value = newvec;
	this->SplitDepth--;
}

void mp::mpz::Normalize()
{
	if (this->length() == 0){
		this->Sign = mp::PLUS; return;
	}
	if ((this->length() == 1)&&(this->value[0] == 0)){
		this->Sign = mp::PLUS; return;
	}

	integer carry = 0;
	for (size_t i=0; (i<this->length()) || carry; i++){
		this->value[i] += carry;
		if (this->value[i] < 0)
			carry  = -(-(this->value[i]+1)/BASE_BYSPLITDEPTH[this->SplitDepth] + 1);		
		else 
			carry = this->value[i] / BASE_BYSPLITDEPTH[this->SplitDepth];		
		this->value[i] -= carry * BASE_BYSPLITDEPTH[this->SplitDepth];		
		if ((i == this->length()-1)&&(carry != 0)) this->value.push_back(0);	
	}
	if (carry != 0) fprintf(stderr, "Overflow! %d\n", carry);
}




/* Here comes the overloaded operators... */

mpz& mp::mpz::operator=  (const mpz& A)
{
	if (this == &A){
		return *this;
	}
	this->Sign = A.Sign;
	this->value = A.value;
	this->SplitDepth = A.SplitDepth;
	return *this;
}

mpz& mp::mpz::operator=  (const mpr& A)
{
	this->Sign = A.Sign;
	this->value.assign(A.value.begin()+A.Exp, A.value.end());
	this->SplitDepth = A.SplitDepth;
	return *this;
}


mpz& mp::mpz::operator=(const integer& A)
{
	this->Sign = (A<0)?(mp::MINUS):(mp::PLUS);
	this->SplitDepth = 0;
	this->value.push_back(A);
	this->Normalize();
	return *this;
}

mpz& mp::mpz::operator=(const std::string& strValue)
{
	this->SplitDepth = 0;
	this->Read(strValue);
}

/* unary operators */
mpz operator+ (mpz& A)
{
	return A;
}
mpz operator- (mpz& A)
{
	mpz answer = A;
	answer.Sign = MUL_SIGN(A.Sign, mp::MINUS);
	return answer;
}


/* comparison friend operators */

bool operator== (const mpz& lhs, const mpz& rhs)
{
	mpz A(lhs), B(rhs);
	A.EraseLeadingZeros(), B.EraseLeadingZeros();	
	if ((A.value.size() != B.value.size())||(A.Sign != B.Sign)) return false;
	for (size_t i=0; i<A.value.size(); i++){
		if (A.value[i] != B.value[i]) return false;
	}
	return true;
}

bool operator!= (const mpz& lhs, const mpz& rhs)
{
	return (!( lhs == rhs ));
}

bool operator>  (const mpz& lhs, const mpz& rhs)
{
	mpz A(lhs), B(rhs);
	A.EraseLeadingZeros(), B.EraseLeadingZeros();	
	A.Normalize(), B.Normalize();

	if ((A.Sign == mp::PLUS)&&(B.Sign == mp::MINUS)) return true;
	if ((A.Sign == mp::MINUS)&&(B.Sign == mp::PLUS)) return false;
	bool reverse = ((A.Sign == mp::MINUS)&&(B.Sign == mp::MINUS));

	if (A.value.size() != B.value.size()){
		bool ans = (A.value.size() > B.value.size());
		return (reverse)?(!ans):(ans);
	}

	for (size_t i=std::max(A.value.size(), B.value.size()); i-- >0; ){
		if (A.value[i] >  B.value[i]) return (reverse)?(!true):(true);
		if (A.value[i] == B.value[i]) continue;
		if (A.value[i] <  B.value[i]) return (reverse)?(!false):(false);
	}
	return false;
}

bool operator<  (const mpz& lhs, const mpz& rhs)
{
	return (rhs > lhs);
}

bool operator>= (const mpz& lhs, const mpz& rhs)
{
	return (!(lhs < rhs));
}
	
bool operator<= (const mpz& lhs, const mpz& rhs)
{
	return (!(lhs > rhs));
}

/* binary operations with short integers */

mpz operator+ (const mpz& A, integer n)
{
	mpz tmp(n);
	return A + tmp;
}
mpz operator+ (integer n, const mpz& A)
{
	mpz tmp(n);
	return tmp + A;
}
mpz operator- (const mpz& A, integer n)
{
	mpz tmp(n);
	return A - tmp;
}
mpz operator- (integer n, const mpz& A)
{
	mpz tmp(n);
	return tmp - A;
}

mpz operator* (integer n, const mpz&A)
{
	return A*n;
}

mpz operator* (const mpz& A, integer n)
{
	integer carry = 0;
	mp::TSign sgna = A.Sign;
	mp::TSign sgnb = (n>=0)?(mp::PLUS):(mp::MINUS);
	uinteger tmp = abs((integer) n);
	mpz answer = A;
	answer.Sign = MUL_SIGN(sgna, sgnb);
	for (size_t i = 0; (i<answer.value.size())||(carry != 0); i++){
		if (i==answer.value.size()) answer.value.push_back(0);
		integer cur = carry + answer.value[i] * tmp;
		answer.value[i] = (integer)(cur % STORING_BASE);
		carry = (integer)(cur / STORING_BASE);
	}	
	answer.EraseLeadingZeros();
	return answer;
}

/* private shits */
uinteger mp::mpz::div(uinteger A)
{
	std::vector< integer > answer(0);
	unsigned long long tmp = 0;
	bool bound = false;
	for (std::vector< integer >::reverse_iterator it = this->value.rbegin(); it != this->value.rend(); ++it){
		tmp = tmp*1ll*BASE_BYSPLITDEPTH[this->SplitDepth] + (*it)*1ll;
		if ((tmp >= A)||(tmp == 0)){
			bound = true;
			answer.push_back((integer) tmp / A);
			tmp = tmp % A;
		}		
		else { if (bound) answer.push_back(0); }
	}
	if (answer.size() == 0) answer.push_back(0);

	std::reverse(answer.begin(), answer.end());
	this->value = answer;
	return (uinteger) tmp;
}

uinteger mp::mpz::mod(uinteger A)
{
	std::vector< integer > swap(this->value);
	uinteger answer = this->div(A);
	this->value = swap;	
	return answer;
}

mpz operator/ (const mpz& A, integer n)
{
	mpz answer = A;
	answer.Sign = MUL_SIGN(A.Sign, ((n>=0)?(PLUS):(MINUS)));
	answer.div((uinteger)(abs(n)));
	return answer;
}
mpz operator/ (integer n, const mpz& A)
{
	mpz tmp, answer;
	tmp = n;
	answer = tmp/A;
	return answer;
}

mpz operator% (const mpz& A, integer n)
{
	mpz tmp(A);
	return tmp.mod(n);
}

mpz operator<< (mpz& A, const size_t n)
{
	mpz answer;
	answer.Sign = A.Sign;
	answer.SplitDepth = A.SplitDepth;
	answer.value.resize(A.length() + n, 0);
	std::copy(A.value.begin(), A.value.end(), answer.value.begin() + n);
	return answer;
}

mpz operator>> (mpz& A, const size_t n)
{	
	mpz answer;
	if (n == 0){
		answer = A;
		return answer;
	}

	answer.Sign = A.Sign;
	answer.SplitDepth = A.SplitDepth;
	if (n >= A.length()){
		answer.value.clear();
		return answer;
	}
	answer.value.resize(A.length() - n);
	std::copy(A.value.begin() + n, A.value.end(), answer.value.begin());
	return answer;
}

mpz operator^ (mpz& A, const size_t n)
{
	mpz ans = 1;
	size_t p = n;
	while (p) {
		if (p & 1)
			ans = mul(ans, A, mp::FFT);
		ans = mul(ans, ans, mp::FFT);
		p >>= 1;
	}
	return ans;
}

/* binary arithmetic operators */

mpz operator+ (const mpz& lhs, const mpz& rhs)
{	
	mpz A(lhs), B = (rhs);
	
	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = PLUS;
	if ((A.length() == 1)&&(A.value[0] == 0)) A.Sign = B.Sign;
	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = A.Sign;	

	if (B.Sign != A.Sign){
		TSign bckb = B.Sign;	
		B.Sign = A.Sign;
		mpz answer = A - B;		
		return answer;
	}

	integer delta = 0;
	mpz answer;
	answer.Sign = A.Sign;	

	answer.value.resize(std::max(A.value.size(), B.value.size()) + 1, 0);
	for (size_t i=0; (i < std::max(A.value.size(), B.value.size())) || (delta>0); i++){
		delta += ((i<A.value.size())? (A.value[i]):(0)) + ((i<B.value.size())?(B.value[i]):(0));
		answer.value[i] = delta % STORING_BASE;
		delta /= STORING_BASE;
	}
	answer.EraseLeadingZeros();
	return answer;
}

mpz operator-  (const mpz& lhs, const mpz& rhs)
{
	mpz A(lhs), B(rhs), l(lhs), r(rhs);
	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = PLUS;
	if ((A.length() == 1)&&(A.value[0] == 0)) A.Sign = B.Sign;
	if ((B.length() == 1)&&(B.value[0] == 0)) B.Sign = A.Sign;

	if (B.Sign != A.Sign){
		B.Sign = A.Sign;
		mpz answer = A + B;
		answer.Sign = A.Sign;
		return answer;	
	}

	if (abs(B) > abs(A)){
		mpz answer = B - A;
		answer.Sign = MUL_SIGN(A.Sign, MINUS);
		return answer;
	}

	integer carry = 0;
	for (size_t i=0; (i<B.value.size())||(carry != 0); i++){
		A.value[i] -= (integer)((i<B.value.size())?(B.value[i]):(0)) + carry;
		carry = (A.value[i] < 0)?(1):(0);
		if (carry) A.value[i] += STORING_BASE;
	}
	A.EraseLeadingZeros();
	return A;
}

mpz operator*  (const mpz& A, const mpz& B)
{
	switch (GLOBAL_MULTIPLICATIONMETHOD_SWITCHER){
		case mp::DEFAULT		: return schoolgrade_mul(A, B);
		case mp::FFT			: return fft_mul(A, B);
		case mp::SCHOOLGRADE	: return schoolgrade_mul(A, B);
		case mp::KARATSUBA		: return karatsuba_mul(A, B);
		case mp::TOOM3WAY		: return toom_3way_mul(A, B);
		case mp::SCHONHAGE		: return schonhage_mul(A, B);

		default: return schoolgrade_mul(A, B);
	}
}

mpz mul(const mpz& A, const mpz& B, TMethod method){
	switch (method){
		case mp::DEFAULT		: return schoolgrade_mul(A, B);
		case mp::FFT			: return fft_mul(A, B);
		case mp::SCHOOLGRADE	: return schoolgrade_mul(A, B);
		case mp::KARATSUBA		: return karatsuba_mul(A, B);
		case mp::TOOM3WAY		: return toom_3way_mul(A, B);
		case mp::SCHONHAGE		: return schonhage_mul(A, B);

		default: return schoolgrade_mul(A, B);
	}
}

mpz operator/ (const mpz& lhs, const mpz& rhs)
{
	mpz a(lhs), b(rhs);
	a.EraseLeadingZeros();
	b.EraseLeadingZeros();
	if (a < b) return a;
	mpz res;
	res.value.resize(a.length());
	mpz curValue;
	for (long long i = (long long)a.length() - 1; i>=0; i--){
		curValue.value.insert(curValue.value.begin(),0); // * osn
		curValue.value[0] = a.value[i];
		// подбираем максимальное число x, такое что b * x <= curValue
		integer x = 0;
		integer l = 0, r = STORING_BASE;
		while (l <= r){
			int m = (l + r) >> 1;
			mpz cur;
			cur = b * m;
			if (cur <= curValue){
				x = m;
				l = m+1;
			}
			else
				r = m-1;
		}
		res.value[i] = x;
		curValue = curValue - b * x;
	}
	res.EraseLeadingZeros();
	return res;
}

mpz operator % (const mpz& lhs, const mpz& rhs)
{
	mpz a(lhs), b(rhs);
	a.EraseLeadingZeros();
	b.EraseLeadingZeros();
	if (a < b) return a;
	mpz res;
	res.value.resize(a.length());
	mpz curValue;
	for (long long i = (long long)a.length() - 1; i>=0; i--){
		curValue.value.insert(curValue.value.begin(),0); // * osn
		curValue.value[0] = a.value[i];
		// подбираем максимальное число x, такое что b * x <= curValue
		integer x = 0;
		integer l = 0, r = STORING_BASE;
		while (l <= r){
			int m = (l + r) >> 1;
			mpz cur;
			cur = b * m;
			if (cur <= curValue){
				x = m;
				l = m+1;
			}
			else
				r = m-1;
		}
		res.value[i] = x;
		curValue = curValue - b * x;
	}
	curValue.EraseLeadingZeros();
	return curValue;
}



}; // end of namespace mp
