#ifndef DEFINES_H
#define DEFINES_H

/* Custom defines section!
 * List of names that you can define:
 * - USING_DECIMAL_DEBUG_MODE   -   Using base 10 in both IO and calculations
 * - USING_256SYMBOL_IO         -   Using base 256 in IO, you can use it for cryptographic 
 *                                  stuffs, since each symbol is a digit
 * - USING_IMPROVED_KARATSUBA   -   Using karatsuba without recursion calls and object 
 *                                  creations. Be careful: the longer is your number, the 
 *                                  larger possibility of standart type overflow!
 * - USING_IMPROVED_TOOM3WAY    -   Using toom 3-way without recursion calls and object
 *                                  creations. Be careful: the longer is your number, the	
 *                                  larger possibility of standart type overflow!
 * - USING_IMPROVED_TOMMATH	    -   Defining two of the above.
 * - ENABLE_FFT_MULTIPLICATION  -   Enabling fft in default multiplication method (by
 *                                  default, the A*B operation is a hybrid algorithm of
 *                                  grade school multiplication and karatsuba multiplication.
 *                                  enabling fft will change default multiplication method
 *                                  to fft (cause it is way faster, but less stable).
 * - ENABLE_TIME_COUNT          _   Enabling timer to measure the calculation time of 
 *                                  most arithmetic and number theory algorithms.
 * - ENABLE_RTCSC_TIMECOUNT     -   Same as ENABLE_TIME_COUNT, but using RTCSC. Be careful:
 *                                  do not use this along with ENABLE_TIME_COUNT!!
 * - ENABLE_GPU_FFT             -   Using GPU for calculating radix 2 fft multiplication
 * - ENABLE_GPU_MODULAR         -   Using GPU for modular arithmetic calculations
 * - ENABLE_GPUCOMPUTATION      -   Using parallel computation on available GPU.
 *
 * (!) caution: USING_DECIMAL_DEBUG_MODE and USING_256SYMBOL_IO musn't be defined in the
 * same time!!! It will cause undefined behaviour in the most unexpected places in your
 * program. 
 */


	#define USING_DECIMAL_DEBUG_MODE
	#define USING_IMPROVED_TOMMATH
	#define ENABLE_TIME_COUNT
//	#define ENABLE_GPU_MODULAR

/* end of custom defines section */

#include <stdint.h>
#ifdef ENABLE_TIME_COUNT
	#include <time.h>
#endif
namespace mp
{

#ifndef USING_DECIMAL_DEBUG_MODE
	#if __x86_64__ || __ppc64__
		typedef uint64_t			uinteger;
		typedef int64_t				integer;
		#ifdef USING_256SYMBOL_IO
			#define STORING_BASE	0x100000000ll
			#define DIGIT_IN_WORD	4
			#define MAX_SPLITDEPTH	3
			#define FFT_BASE		0x10000ll
			const uinteger BASE_BYSPLITDEPTH[] = { 0x100000000ll, 0x10000ll, 0x100ll };
		#else
			#define STORING_BASE	100000000 // 8 zeros
			#define DIGIT_IN_WORD	8
			#define MAX_SPLITDEPTH	3
			#define FFT_BASE		10000ll
			const uinteger BASE_BYSPLITDEPTH[] = { 100000000ll, 10000ll, 100ll, 10ll };
		#endif
	#else
		typedef uint32_t			uinteger;
		typedef int32_t				integer;
		#ifdef USING_256SYMBOL_IO
			#define STORING_BASE	0x10000
			#define DIGIT_IN_WORD	2
			#define MAX_SPLITDEPTH	2
			#define FFT_BASE		0x100
			const uinteger BASE_BYSPLITDEPTH[] = { 0x10000, 0x100, 0x10};
		#else
			#define STORING_BASE	10000 // 4 zeros
			#define DIGIT_IN_WORD	4
			#define MAX_SPLITDEPTH	2
			#define FFT_BASE		100
			const uinteger BASE_BYSPLITDEPTH[] = { 10000, 100, 10 };
		#endif
	#endif
#else
	typedef unsigned int	uinteger;
	typedef int				integer;
	#define STORING_BASE	10
	#define DIGIT_IN_WORD	1
	#define MAX_SPLITDEPTH	0
	#define FFT_BASE		10
	const uinteger BASE_BYSPLITDEPTH[] = { 10 };
#endif

#ifdef USING_256SYMBOL_IO
	#define INPUT_OUTPUT_BASE 0x100
#else
	#define INPUT_OUTPUT_BASE 10
#endif

#ifdef USING_IMPROVED_TOMMATH
	#define USING_IMPROVED_KARATSUBA
	#define USING_IMPROVED_TOOM3WAY
#endif

#ifdef ENABLE_GPUCOMPUTATION
	#define ENABLE_GPU_FFT
	#define ENABLE_GPU_MODULAR
#endif

#define KARATSUBA_CUTOFF		5
#define TOOM3WAY_CUTOFF			5

#define MUL_SIGN(x,y)		((x == y)?(PLUS):(MINUS))
#define ISINTEGER(x)		(bool)(fabs(x - round(x)) < 0.0000001)

typedef enum {
	MINUS = '-',
	PLUS = '+'
} TSign;

typedef enum MulMethod{
	DEFAULT,
	SCHOOLGRADE,
	KARATSUBA,
	TOOM3WAY,
	FFT,
	SCHONHAGE
} TMethod;

/* global switcher */
static TMethod GLOBAL_MULTIPLICATIONMETHOD_SWITCHER = SCHOOLGRADE;

#ifdef ENABLE_TIME_COUNT
	#define INIT_TIMER \
		timespec __res, __t0, __t1; \
		double __lasttime;
	
	#define START_TIMER \
		clock_getres(CLOCK_REALTIME, &__res); \
		clock_gettime(CLOCK_REALTIME, &__t0);
		
	#define STOP_TIMER \
		clock_gettime(CLOCK_REALTIME, &__t1); \
		__lasttime = \
			(double) (__t1.tv_sec - __t0.tv_sec) + \
			(double) (__t1.tv_nsec - __t0.tv_nsec) / 1000000000.0;
	
	#define GET_LAST_EXECUTION_TIME __lasttime
#else
	#define START_TIMER
	#define STOP_TIMER 
#endif

#define LEHMANN_DEFAULT_ROUNDNUM 10
#define RABIN_MILLER_DEFAULT_ROUNDNUM 5
#define SOLOVEY_STRASSEN_DEFAULT_ROUNDNUM 10

class mpz;
class mpr;
class mpmod;

}; //end of namespace mp
#endif
