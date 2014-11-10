#include "defines.hpp"

#if defined(ENABLE_GPUCOMPUTATION) || defined(ENABLE_GPU_FFT) || defined(ENABLE_GPU_MODULAR)

#include "modules.hpp"
#include "gpu_op.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "CL/cl.h"


#define MAX_SOURCE_SIZE (0x100000)
#define CHECK_FOR_ERRORS(x, y, z)					\
	if (debug_mode){								\
		if (ret != 0)								\
			fprintf(stderr, "%d %s%s\n", z, x, y);	\
	}

namespace mp{
	const char* KERNEL_CODE[PROGRAM_COUNT]  = { 
		"kernelsrc/vadd_kernel.cl", 
		"kernelsrc/vsub_kernel.cl", 
		"kernelsrc/vmul_kernel.cl",
		"kernelsrc/vadd_mod_kernel.cl",
		"kernelsrc/vsub_mod_kernel.cl",
		"kernelsrc/vmul_mod_kernel.cl",
		"kernelsrc/fft_radix2_forward_kernel.cl"
	 };
	const char* PROGRAM_NAME[PROGRAM_COUNT] = { 
		"vector_add", 
		"vector_sub", 
		"vector_mul",
		"vector_add_mod",
		"vector_sub_mod",
		"vector_mul_mod",
		"fft_radix2_forward"
	};

	static cl_kernel		kernel[PROGRAM_COUNT];
	static cl_program		program[PROGRAM_COUNT];
	static cl_context		context;
	static cl_command_queue command_queue[PROGRAM_COUNT];

	static cl_platform_id	platform_id;
	static cl_device_id		device_id;
	static cl_uint			ret_num_devices;
	static cl_uint			ret_num_platforms;
	static cl_int			ret;

	void InitGPUKernel(bool debug_mode)
	{
		platform_id = NULL;
		device_id = NULL;
		ret_num_devices = 0;
		ret_num_platforms = 0;		

		// get platform and device information 
		ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

		// create an OpenCL context
		context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
		CHECK_FOR_ERRORS("clCreateContext ", "", ret);
					
		// loading all source into device 
		for (size_t T=0; T<PROGRAM_COUNT; T++){
			// load the kernel source code
			FILE* fp;
			char* source_str;
			size_t source_size;
			
			fp = fopen( KERNEL_CODE[T], "r");
			if (!fp){
				fprintf(stderr, "Failed to load kernel module (%s).\n", KERNEL_CODE[T]);
				exit(1);
			}
			source_str = (char*) malloc(MAX_SOURCE_SIZE);
			source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
			fclose(fp);

			// create a program for kernel source code
			program[T] = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
			CHECK_FOR_ERRORS("clCreateProgramWithSource ", KERNEL_CODE[T], ret);			

			// Build the program
			ret = clBuildProgram(program[T], 1, &device_id, NULL, NULL, NULL);
			CHECK_FOR_ERRORS("clBuildProgram ", KERNEL_CODE[T], ret);

			// create the OpenCL kernel
			kernel[T] = clCreateKernel(program[T], PROGRAM_NAME[T], &ret);
			CHECK_FOR_ERRORS("clCreateKernel ", KERNEL_CODE[T], ret);

			// create a command queue
			command_queue[T] = clCreateCommandQueue(context, device_id, 0, &ret);
			CHECK_FOR_ERRORS("clCreateCommandQueue ", KERNEL_CODE[T], ret);
		}
	}
	
	void FreeGPUKernel(bool debug_mode)
	{
		for (size_t T=0; T<PROGRAM_COUNT; T++){
			ret = clFlush(command_queue[T]);
			ret = clFinish(command_queue[T]);
			ret = clReleaseCommandQueue(command_queue[T]);
			ret = clReleaseKernel(kernel[T]);	CHECK_FOR_ERRORS("clReleaseKernel ",  KERNEL_CODE[T], ret);
			ret = clReleaseProgram(program[T]); CHECK_FOR_ERRORS("clReleaseProgram ", KERNEL_CODE[T], ret); 
		}
		ret = clReleaseContext(context); CHECK_FOR_ERRORS("clReleaseContext ", "", ret);

	}

	void OPS0TO2(int* A, int* B, int* C, size_t len, int T)
	{		
		INIT_TIMER
		START_TIMER	

		// create a memory buffer on the device for each vector
		cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len*sizeof(int), NULL, &ret);
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len*sizeof(int), NULL, &ret);
		cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len*sizeof(int), NULL, &ret);

		// copy the list A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue[T], a_mem_obj, CL_TRUE, 0, len*sizeof(int), A, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue[T], b_mem_obj, CL_TRUE, 0, len*sizeof(int), B, 0, NULL, NULL);
		
		// set the arguments of the kernel
		int i = 0;
		ret = clSetKernelArg(kernel[T], i++, sizeof(cl_mem), (void*)&a_mem_obj);
		ret = clSetKernelArg(kernel[T], i++, sizeof(cl_mem), (void*)&b_mem_obj);
		ret = clSetKernelArg(kernel[T], i++, sizeof(cl_mem), (void*)&c_mem_obj);
		STOP_TIMER
		printf("copy time : %23.20f\n", GET_LAST_EXECUTION_TIME);
		
		START_TIMER
		// execute the OpenCL kernel
		size_t global_item_size = len;
		size_t local_item_size = 64;
		ret = clEnqueueNDRangeKernel(command_queue[T], kernel[T], 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
		STOP_TIMER
		printf("execution : %23.20f\n", GET_LAST_EXECUTION_TIME);		

		START_TIMER
		// read the memory buffer c_mem_obj to the host list C
		ret = clEnqueueReadBuffer(command_queue[T], c_mem_obj, CL_TRUE, 0, len*sizeof(int), C, 0, NULL, NULL);
	
		// free the memory after processing
		ret = clReleaseMemObject(a_mem_obj);
		ret = clReleaseMemObject(b_mem_obj);
		ret = clReleaseMemObject(c_mem_obj);
		STOP_TIMER
		printf("disclaim  : %23.20f\n", GET_LAST_EXECUTION_TIME);
	}

}; // end of namespace mp

#endif
