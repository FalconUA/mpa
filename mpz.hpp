#ifndef MPZHEADER_HPP
#define MPZHEADER_HPP

#include "defines.hpp"
#include <vector>
#include <string>

namespace mp
{
	class mpz
	{
	private:
	/* things that we dont want to be implemented in mpr */	
		uinteger div(uinteger A);
		uinteger mod(uinteger A);
	protected:
		std::vector< integer >	value;
		mp::TSign				Sign;
		size_t					SplitDepth;
	
	/* digit manipulation */
		void EraseLeadingZeros();
		void Normalize();
		void split2();
		void merge2();
		
	public:
		mpz(): SplitDepth(0), Sign(mp::PLUS) {this->value.clear();};
		mpz(integer A);
		mpz(std::string val){ this->Read(val); };

	/* copy constructors */
		mpz(const mpz& A): SplitDepth(A.SplitDepth), value(A.value), Sign(A.Sign) {};

	/* IO operations */
		void			Read(std::string strValue);
		std::string		Print();
		std::string		PrintRaw(std::string separator = "");
		size_t			length();

	/* assignment operators */
		mpz& operator=  (const mpz& A);
		mpz& operator=  (const mpr& A);
		mpz& operator=  (const integer& A);
		mpz& operator=  (const std::string& strValue);
		friend mpz tompz(const mpr& A);

	/* unary operators */
		friend mpz operator+ (mpz& A);
		friend mpz operator- (mpz& A);
	
	/* comparison operators */
		friend bool operator== (const mpz& lhs, const mpz& rhs);
		friend bool operator!= (const mpz& lhs, const mpz& rhs);	
		friend bool operator<  (const mpz& lhs, const mpz& rhs);
		friend bool operator>  (const mpz& lhs, const mpz& rhs);
		friend bool operator<= (const mpz& lhs, const mpz& rhs);
		friend bool operator>= (const mpz& lhs, const mpz& rhs);

	/* operations with short numbers (both unsigned and signed) */
		friend mpz operator+  (const mpz& A, integer n);	friend mpz operator+ (integer n, const mpz& A);
		friend mpz operator-  (const mpz& A, integer n);	friend mpz operator- (integer n, const mpz& A);
		friend mpz operator*  (const mpz& A, integer n);	friend mpz operator* (integer n, const mpz& A);
		friend mpz operator/  (const mpz& A, integer n);	friend mpz operator/ (integer n, const mpz& A);
		friend mpz operator%  (const mpz& A, integer n);	friend mpz operator% (integer n, const mpz& A);
		friend mpz operator<< (mpz& A, const size_t n);
		friend mpz operator>> (mpz& A, const size_t n);
		friend mpz operator^  (mpz& A, const size_t n);
	
	/* binary operations */
		friend mpz operator+ (const mpz& lhs, const mpz& rhs);
		friend mpz operator- (const mpz& lhs, const mpz& rhs);
		friend mpz operator* (const mpz& A,   const mpz& B);
		friend mpz operator/ (const mpz& A,   const mpz& B);
		friend mpz operator% (const mpz& lhs, const mpz& rhs);	
		
	/* number theory algorithms, only work for mpz */
		friend mpz abs		(mpz num);
		friend mpz gcd		(mpz lhs, mpz rhs);
		friend mpz random	(mpz upperbound, int iterations);
		friend mpz sqrt		(mpz num);
		friend mpz deg		(mpz num, mpz pow, mpz mod); 

		friend mpmod mpz_to_mpmod (mpz   num);
		friend mpz   mpmod_to_mpz (mpmod num);

	/* Here is 10 algorithms required in the task */
	/* multiplication algorithms */
		friend mpz mul				(const mpz& lhs, const mpz& rhs, mp::TMethod method);
		friend mpz schoolgrade_mul	(mpz lhs, mpz rhs);
		friend mpz karatsuba_mul	(mpz lhs, mpz rhs);
		friend mpz toom_3way_mul 	(mpz lhs, mpz rhs);
		friend mpz fft_mul			(mpz lhs, mpz rhs);
		friend mpz schonhage_mul	(mpz lhs, mpz rhs);	

	/* primality tests */
		friend bool lemer_test				(size_t p);
		friend bool lehmann_test			(mpz num, int r = LEHMANN_DEFAULT_ROUNDNUM);
		friend bool solovey_strassen_test	(mpz num, int r = SOLOVEY_STRASSEN_DEFAULT_ROUNDNUM);
		friend bool strassen_solovey_test	(mpz num, int r = SOLOVEY_STRASSEN_DEFAULT_ROUNDNUM);
		friend bool rabin_miller_test		(mpz num, int r = RABIN_MILLER_DEFAULT_ROUNDNUM);
		friend bool miller_rabin_test		(mpz num, int r = RABIN_MILLER_DEFAULT_ROUNDNUM);
		friend bool agrawal_test			(mpz num);
				
	};
	
	const mpz ZERO(0);
	const mpz ONE(1);
	const mpz EMPTY;	
};

#endif
