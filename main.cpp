#include "mpz.hpp"
#include "mpr.hpp"
#include <iostream>
#include <stdio.h>
using namespace mp;

inline char compare(mpz A, mpz B){
	return (A==B)?('C'):('I');
}

int main()
{
	srand(time(0));
	INIT_TIMER
	std::string hello("\
******************************\n\
*                            *\n\
* mpz testing prorgram       *\n\
*                            *\n\
******************************\n\n");

	std::cout << hello;
	mp::mpz a,b,c,d;

	std::cout << "io test:\n";
	a.Read("123456789987654321");
	std::cout << "raw:   "<<a.PrintRaw(" ") << "\n";
	std::cout << "print: "<<a.Print() << "\n";

	mp::GLOBAL_MULTIPLICATIONMETHOD_SWITCHER = mp::DEFAULT;

	std::cout << "\nsum testing:\n";
	a.Read("1111111111111111111111111111111100000");
	b.Read("1111111111111111111111111111111100000");	
	c = a + b;
	std::cout << a.Print() << "\n" << b.Print() << "\n" << c.Print() << "\n";


	a.Read("111111111");
	b.Read("222222222222222222222222222222222222222222");
	
	mpz corr;
	std::cout << "\nmultiplications:\n";
	std::cout << "1. simple mul: ";
	START_TIMER	c = mul(a, b, mp::SCHOOLGRADE);	STOP_TIMER corr = c;
	printf("%.20f %c %s\n", GET_LAST_EXECUTION_TIME, compare(corr,c), c.Print().c_str());
	std::cout << "2. karatsuba : ";
	START_TIMER	c = mul(a, b, mp::KARATSUBA); STOP_TIMER
	printf("%.20f %c %s\n", GET_LAST_EXECUTION_TIME, compare(corr,c), c.Print().c_str());
	std::cout << "3. toom 3-way: ";
	START_TIMER c = mul(a, b, mp::TOOM3WAY); STOP_TIMER
	printf("%.20f %c %s\n", GET_LAST_EXECUTION_TIME, compare(corr,c), c.Print().c_str());
	std::cout << "4. schonhage : ";
	START_TIMER c = mul(a, b, mp::SCHONHAGE); STOP_TIMER
	printf("%.20f %c %s\n", GET_LAST_EXECUTION_TIME, compare(corr,c), c.Print().c_str());
	std::cout << "5. fft       : ";
	START_TIMER c = mul(a, b, mp::FFT); STOP_TIMER
	printf("%.20f %c %s\n", GET_LAST_EXECUTION_TIME, compare(corr,c), c.Print().c_str());


    mp::mpr af,bf,iaf,ibf,afbf;
    af = "1000.00";
    bf = "1000";
	std::cout << af.PrintRaw(" ") << "\n";
    iaf = inverse(af); ibf = inverse(bf); afbf = af/bf;
    std::cout << "af: " << af.Print() << std::endl;
    std::cout << "1/af: " << iaf.Print() << std::endl;
    std::cout << "bf: " << bf.Print() << std::endl;
    std::cout << "1/bf: " << ibf.Print() << std::endl;
    std::cout << "af/bf: " << afbf.Print() << std::endl << std::endl;
	a.Read("59693");
//	a.Read("2222222222222222222");
    size_t p = 31;
//	a.lemer(p);
    bool lres = true;

//	lres = lemer_test(p);
	std::cout << "lemer: ";
	if (lres)
		std::cout << p << "-th mersen number is Prime!\n";
	else
		std::cout << p << "-th mersen number is not Prime!\n";

	lres = rabin_miller_test(a);
	std::cout << "rabin miller    : " << a.Print() << " ";
	if (lres)
		std::cout << "IS PRIME" << "\n";
	else
		std::cout << "not prime\n";

	lres = lehmann_test(a);
	std::cout << "lehmann         : " << a.Print() << " ";
	if (lres)
		std::cout << "IS PRIME" << "\n";
	else
		std::cout << "not prime\n";
	lres = solovey_strassen_test(a);
	std::cout << "solovey strassen: " << a.Print() << " ";
	if (lres)
		std::cout << "IS PRIME" << "\n";
	else
		std::cout << "not prime\n";
	lres = agrawal_test(a);
	std::cout << "agrawal         : " << a.Print() << " ";
	if (lres)
		std::cout << "IS PRIME" << "\n";
	else
		std::cout << "not prime\n";



	return 0;
}
