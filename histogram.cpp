#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>

// https://stackoverflow.com/questions/28500496/opencl-function-found-deprecated-by-visual-studio
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define MAX_SOURCE_SIZE (0x100000)

#include <CL/cl.h>

unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	/* init params */
	size_t input_image_size = _size * sizeof(unsigned int);
	size_t histogram_array_size = 256 * 3 * sizeof(unsigned int);
	unsigned int *ref_histogram_results;
	ref_histogram_results = (unsigned int *)malloc(histogram_array_size);
	memset (ref_histogram_results, 0x0, histogram_array_size);

	/* Load the source code containing the kernel*/
	FILE *fp;
	char fileName[] = "./histogram.cl";
	char *source_str;
	size_t source_size;
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* Get Platform and Device Info */
	cl_int ret;
	cl_uint ret_num_platforms;
	cl_uint ret_num_devices;
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	cl_context context = NULL;
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	cl_command_queue command_queue = NULL;
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	cl_mem input_memobj = NULL;
	cl_mem result_memobj = NULL;
	input_memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, input_image_size, NULL, &ret);
	result_memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, histogram_array_size, NULL, &ret);

	/* Write Memory Buffer */
	ret = clEnqueueWriteBuffer(command_queue, input_memobj, CL_TRUE, 0, input_image_size, image_data, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, result_memobj, CL_TRUE, 0, histogram_array_size, ref_histogram_results, 0, NULL, NULL);

	/* Create Kernel Program from the source */
	cl_program program = NULL;
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	cl_kernel kernel = NULL;
	kernel = clCreateKernel(program, "histogram", &ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input_memobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&result_memobj);
	ret = clSetKernelArg(kernel, 2, sizeof(_size), (void *)&_size);

	/* Execute OpenCL Kernel */
	cl_uint work_dim = 1;
	size_t global_work_size = _size;
	size_t local_work_size = 1;
	ret = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, 0, &global_work_size, &local_work_size, 0, NULL, NULL);

	/* Caculating ... */

	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, result_memobj, CL_TRUE, 0, histogram_array_size, ref_histogram_results, 0, NULL, NULL);

	/* Display Result */
	// for(unsigned int i = 0; i < 256 * 3; ++i) {
	// 	if (i % 256 == 0 && i != 0)
	// 		std::cout << std::endl;
	// 	std::cout << ref_histogram_results[i]<< ' ';
	// }

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(input_memobj);
	ret = clReleaseMemObject(result_memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(source_str);

	return ref_histogram_results;
}

int main(int argc, char const *argv[])
{

	unsigned int * histogram_results;
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0756132.out", std::ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}

	histogram_results = histogram(image, input_size);
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();

	return 0;
}
