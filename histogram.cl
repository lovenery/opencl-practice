__kernel void histogram(__global unsigned int *image, __global unsigned int *result, const unsigned int _size) {
	int id = get_global_id(0);
	int x = image[id];
	int y = id % 3;
	atomic_inc(result+((256 * y) + x)); // result[(256 * y) + x)]++
}
