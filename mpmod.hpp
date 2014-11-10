#ifndef MPMOD_HPP
#define MPMOD_HPP

#include "defines.hpp"
#include "mpz.hpp"
#include "modules.hpp"

namespace mp{

#ifndef ENABLE_GPU_MODULAR

	class mpmod
	{
	private:
		size_t r;
		int* m;
	public:
		mpmod(){ this->m = (int*) malloc(MODULAR_BASE * sizeof(int)); }
		mpmod(const mpmod& A){ 
			this->m = (int*) malloc(MODULAR_BASE * sizeof(int));
			for (size_t i=0; i<MODULAR_BASE; i++) this->m[i] = A.m[i]; 
		}
		friend mpmod mpz_to_mpmod (mpz   num);
		friend mpz   mpmod_to_mpz (mpmod num);
	
		friend mpmod operator* (mpmod& lhs, mpmod& rhs);	
		friend mpmod operator+ (mpmod& lhs, mpmod& rhs);
		friend mpmod operator- (mpmod& lhs, mpmod& rhs);
	};
#else
	#include <CL/cl.h>

	static cl_mem modular_base_mem_obj;
	static cl_mem r_mem_obj

	static int R[(MODULAR_BASE-1)*(MODULAR_BASE)];	
	void InitModularArithmeticData();

	class mpmod
	{
	private:
		cl_mem mod_mem_obj;
	public:
		mpmod();
		mpmod(const mpmod& A);

		friend mpmod mpz_to_mpmod (mpz   num);
		friend mpz   mpmod_to_mpz (mpmod num);
	
		friend mpmod operator* (mpmod& lhs, mpmod& rhs);	
		friend mpmod operator+ (mpmod& lhs, mpmod& rhs);
		friend mpmod operator- (mpmod& lhs, mpmod& rhs);
	};
#endif

}; // end of namespace mp

#endif
