#ifndef GPU_OP_HPP
#define GPU_OP_HPP

#include "defines.hpp"

#if defined(ENABLE_GPUCOMPUTATION) || defined(ENABLE_GPU_FFT) || defined(ENABLE_GPU_MODULAR)

#include "CL/cl.h"
#include "stdio.h"
#include "stdlib.h"

namespace mp{

	#define PROGRAM_COUNT 7
	#define VECTOR_ADD_PROGRAM 0
	#define VECTOR_SUB_PROGRAM 1
	#define VECTOR_MUL_PROGRAM 2
	#define VECTOR_ADD_MOD_PROGRAM 3
	#define VECTOR_SUB_MOD_PROGRAM 4
	#define VECTOR_MUL_MOD_PROGRAM 5
	#define FFT_PROGRAM 6

	void InitGPUKernel(bool debug_mode = false); // call only once at start
	void FreeGPUKernel(bool debug_mode = false); // call only once at finish
	void OPS0TO2(int* A, int* B, int* C, size_t len, int T);
	void OPS3TO5(int* A, int* B, int* M, int* C, size_t len, int T);

}

#endif
#endif
