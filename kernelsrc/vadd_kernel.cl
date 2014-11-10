__kernel void vector_add(__global int *A, __global int *B, __global int *C)
{
	// get the index of the current element to be processed
	unsigned int i = get_global_id(0);

	// do the operation
	C[i] = A[i] + B[i];
} 
