#include "defines.hpp"
#include "mpa.hpp"
#include <stdio.h>

int main()
{
	srand(time(0));
	INIT_TIMER

	#ifdef USING_IMPROVED_KARATSUBA
		printf("USING_IMPROVED_KARATSUBA\n");
	#endif
	#ifdef USING_IMPROVED_TOOM3WAY
		printf("USING_IMPROVED_TOOM3WAY\n");
	#endif
	#ifdef ENABLE_GPUCOMPUTATION
		printf("ENABLE_GPUCOMPUTATION\n");
	#endif

	printf("Multiplication algorithms testing\n");
	printf("         schoolgrade    karatsuba      toom 3 way     schonhage      fft         \n");
	mp::mpz a, b, c, d;
	size_t D;
	for (D=1; D<100000; D<<=1){
		std::string as = "", bs = "";
		size_t i;
		for (i=0; i<D; i++) as += (char)('0' + rand()%10);
		for (i=0; i<D; i++) bs += (char)('0' + rand()%10);
		a.Read(as), b.Read(bs);
		printf("%7d  ", D);
		START_TIMER c = mul(a, b, mp::SCHOOLGRADE); STOP_TIMER printf("%13.9f  ", GET_LAST_EXECUTION_TIME);
		START_TIMER c = mul(a, b, mp::KARATSUBA  ); STOP_TIMER printf("%13.9f  ", GET_LAST_EXECUTION_TIME);
		START_TIMER c = mul(a, b, mp::TOOM3WAY   ); STOP_TIMER printf("%13.9f  ", GET_LAST_EXECUTION_TIME);
		START_TIMER c = mul(a, b, mp::SCHONHAGE  ); STOP_TIMER printf("%13.9f  ", GET_LAST_EXECUTION_TIME);
		START_TIMER c = mul(a, b, mp::FFT        ); STOP_TIMER printf("%13.9f  ", GET_LAST_EXECUTION_TIME);

		printf("\n");
	}
}
