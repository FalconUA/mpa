#include "defines.hpp"

#ifdef ENABLE_GPU_MODULAR

#include "modules.hpp"
#include "mpz.hpp"
#include "mpmod.hpp"
#include <CL/cl.c>
#include <iostream>

static int gcd (int a, int b, int& x, int& y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcd (b%a, a, x1, y1);
    x = y1 - (b / a)* x1;
    y = x1;
    return d;
}


namespace mp {

void InitModularArithmeticData()
{
	cl_int ret;
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    int y = 0;
    for(unsigned int i = 0; i < MODULAR_BASE - 1; i++)
        for(unsigned int j = i+1; j < MODULAR_BASE; j++){
            if (gcd(MODULES[i],MODULES[j],R[i*MODULAR_BASE + j],y) != 1){
                fprintf(stderr, "Error, gcd is not 1!\n");
                return c;
            }
            R[i*MODULAR_BASE + j] = (R[i*MODULAR_BASE + j] % MODULES[j] + MODULES[j]) % MODULES[j];
        }
	r_mem_obj =				clCreateBuffer(context, CL_MEM_READ_ONLY, (MODULAR_BASE-1)*(MODULAR_BASE)*sizeof(int), NULL, ret);
	modular_base_mem_obj =  clCreateBuffer(context, CL_MEM_READ_ONLY, MODULAR_BASE*sizeof(int), NULL, ret);
	ret = clEnqueueWriteBuffer(command_queue, r_mem_obj, CL_TRUE, 0, (MODULAR_BASE-1)*(MODULAR_BASE)*sizeof(int), R, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, modular_base_mem_obj, CL_TRUE, 0, MODULAR_BASE*sizeof(int), MODULES, 0, NULL, NULL);			
}

mp::mpmod::mpmod()
{
	cl_int ret;
	mod_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, MODULAR_BASE*sizeof(int), NULL, &ret);
}

mpz mpmod_to_mpz(mpmod num)
{
	
}

}// end of namespace mp

#endif
