#include "defines.hpp"
#include "mpr.hpp"
#include "mpz.hpp"

namespace mp
{
	bool operator== (const mpr& lhs, const mpr& rhs){
		mpr A(lhs), B(rhs);
		A.EraseLeadingZeros(), B.EraseLeadingZeros();
		mpz* a = &A;
		mpz* b = &B;
		return ((*a == *b) && (A.Exp == B.Exp));						
	}
	bool operator!= (const mpr& lhs, const mpr& rhs){return (!(lhs == rhs));}		
	bool operator<  (const mpr& lhs, const mpr& rhs){return (rhs > lhs);}
	bool operator>  (const mpr& lhs, const mpr& rhs){
		mpr A(lhs), B(rhs);
		mpz* a = &A;
		mpz* b = &B;
		size_t comexp = std::max(A.Exp, B.Exp);
		*a = *a << (comexp - A.Exp);
		*b = *b << (comexp - B.Exp);
		return (*a > *b);
	} 
	bool operator<= (const mpr& lhs, const mpr& rhs){return ((lhs<rhs)||(rhs==lhs));}
	bool operator>= (const mpr& lhs, const mpr& rhs){return ((lhs>rhs)||(rhs==lhs));}

/* binary operations */
	mpr operator+ (const mpr& lhs, const mpr& rhs){
		mpr A(lhs), B(rhs), res;	
		mpz* a = &A;
		mpz* b = &B;
		mpz* c = &res;

		size_t comexp = std::max(A.Exp, B.Exp);
		*a = *a << (comexp - A.Exp);
		*b = *b << (comexp - B.Exp);

		*c = *a + *b;
		
		res.Exp = comexp;	
		return res;
	};
	mpr operator- (const mpr& lhs, const mpr& rhs){
		mpr A(lhs), B(rhs), res;	
		mpz* a = &A;
		mpz* b = &B;
		mpz* c = &res;

		size_t comexp = std::max(A.Exp, B.Exp);
		*a = *a << (comexp - A.Exp);
		*b = *b << (comexp - B.Exp);	 

		*c = *a - *b;
		
		res.Exp = comexp;	
		return res;
	}
	mpr operator* (const mpr& lhs, const mpr& rhs){
		mpr A(lhs), B(rhs), res;
		mpz* a = &A;
		mpz* b = &B;
		mpz* c = &res;

		*c = (*a) * (*b);

		res.Exp = A.Exp + B.Exp;
		res.EraseLeadingZeros();
		return res;
	}
	mpr operator/ (const mpr& lhs, const mpr& rhs){
		return lhs * inverse(rhs);	
	};

	mpr& mpr::operator << (size_t digits){
	    if(digits == 0)
	        return *this;
	    std::vector<integer> value;
	    long long int zeros = (long long int) this->Exp - (long long int)digits;
	    if(zeros >= 0){
	        this->Exp = zeros;
	        this->EraseLeadingZeros();
	        return *this;
	    }
	    this->Exp = 0;
	    zeros = -zeros;
	    for(long long int i = 0; i < zeros; i++)
	        value.push_back(0);
	    for(long long int i = 0; i < (int)this->value.size(); i++)
	        value.push_back(this->value[i]);
	    this->value = value;
	    this->EraseLeadingZeros();
	    return *this;
	}
	
	mpr& mpr::operator >> (size_t digits){
	    if(digits == 0)
	        return *this;
	    this->Exp += digits;
	    while(this->value.size() <= this->Exp)
	        this->value.push_back(0);
	    this->EraseLeadingZeros();
	    return *this;
	}
	
}; // end of namespace mp											
