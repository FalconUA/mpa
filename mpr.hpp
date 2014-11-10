#ifndef MPRHEADER_HPP
#define MPRHEADER_HPP

#include "defines.hpp"
#include "mpz.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace mp
{
	class mpr: private mpz
	{
	protected:
		size_t Exp;

	/* Overloaded digit manipulation methods */
		void EraseLeadingZeros(){
			this->mpz::EraseLeadingZeros();
			size_t i = 0;
			for (; (i<this->Exp) && (this->value[i]==0); i++);
			this->value.erase(this->value.begin(), this->value.begin() + std::min(i, this->length()));
			this->Exp -= i;
			if (this->length() < this->Exp){
				this->value.resize(this->Exp, 0);
			}
		};
		void split2(){		
			if (this->SplitDepth == MAX_SPLITDEPTH) return;
			this->Exp *= 2;
			this->mpz::split2();
		};
		void merge2(){
			if (this->SplitDepth == 0) return;
			if (this->Exp % 2 == 1){
				this->value.insert(this->value.begin(), 0);
				this->Exp += 1;
			}
			this->Exp /= 2;
			this->mpz::merge2();
		};

	public:
		mpr():mpz(), Exp(0) {};
		mpr(std::string val){ this->Read(val); };  
		mpr(const mpz& A){ *this = A; };
		mpr(const mpr& A){ *this = A; };
	
	/* Overloaded IO operations */
		void Read(std::string strValue)
		{
			#ifndef USING_256SYMBOL_IO
				this->Exp = 0;
				size_t dotpos = strValue.find('.');
				if (dotpos < strValue.length()){
					this->Exp = strValue.length() - dotpos - 1;
					strValue.erase(strValue.begin() + dotpos);
				}
			#endif	
			for(; BASE_BYSPLITDEPTH[this->SplitDepth] != INPUT_OUTPUT_BASE; this->split2());
			this->mpz::Read(strValue);
			for(; BASE_BYSPLITDEPTH[this->SplitDepth] != STORING_BASE; this->merge2());
		};

		std::string Print(){
			std::string res;
			for(; BASE_BYSPLITDEPTH[this->SplitDepth] != INPUT_OUTPUT_BASE; this->split2());
			res = this->mpz::Print();
			res.insert(res.begin() + res.length() - this->Exp, '.');
			for(; BASE_BYSPLITDEPTH[this->SplitDepth] != STORING_BASE; this->merge2());
			return res;
		}
	
		std::string PrintRaw(std::string separator){
			std::stringstream ss;
			ss << this->mpz::PrintRaw(separator) << " * " << BASE_BYSPLITDEPTH[this->SplitDepth] << "^-" << this->Exp;
			return ss.str();
		}


	/* overloaded assignment operators */
		mpr& operator= (const mpr& A){
			this->Sign = A.Sign;
			this->value = A.value;
			this->Exp = A.Exp;
			this->SplitDepth = A.SplitDepth;
			return (*this);
		}
		mpr& operator= (const mpz& A){
			this->Exp = 0;
			this->mpz::operator= (A);
			return (*this);
		}
		mpr& operator= (const std::string& strValue){
			std::string s = strValue;
			this->Read(s);	
			return (*this);
		}
		mpr& operator= (const integer& A){
			this->Exp = 0;
			this->mpz::operator= (A);
			return (*this);
		}

	/* overloaded comparison operators */
		friend bool operator== (const mpr& lhs, const mpr& rhs);
		friend bool operator!= (const mpr& lhs, const mpr& rhs);
		friend bool operator<  (const mpr& lhs, const mpr& rhs);
		friend bool operator>  (const mpr& lhs, const mpr& rhs);
		friend bool operator<= (const mpr& lhs, const mpr& rhs);
		friend bool operator>= (const mpr& lhs, const mpr& rhs);

	/* binary operations with integers */
		friend mpr operator* (const mpr& lhs, integer rhs){mpr tmp(rhs); return lhs*tmp;}
		friend mpr operator* (integer lhs, const mpr& rhs){mpr tmp(lhs); return tmp*rhs;}

	/* binary operations */
		friend mpr operator+ (const mpr& lhs, const mpr& rhs);
		friend mpr operator- (const mpr& lhs, const mpr& rhs);
		friend mpr operator* (const mpr& lhs, const mpr& rhs);
		friend mpr operator/ (const mpr& lhs, const mpr& rhs);

		friend mpr mul(const mpr& lhs, const mpr& rhs, mp::TMethod method){
			mpr A(lhs), B(rhs), res;
			mpz* a = &A;
			mpz* b = &B;
			mpz* c = &res;
		
			*c = mul(*a, *b, method);

			res.Exp = A.Exp + B.Exp;
			res.EraseLeadingZeros();
			return res;	
		}

	/* overloaded operators << and >> */ 
		mpr& operator<< (const size_t n);
		mpr& operator>> (const size_t n);

	/* inverse cook-newton */
		friend mpr cooknewton	(const mpr& V, const mpr& z, size_t depth);
		friend mpr inverse		(const mpr& num); 
		
	/* idk how it is working... */
		friend class mpz;
	};
}


#endif
