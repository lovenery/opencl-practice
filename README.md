# opencl-practice

## Build

```bash
cd refs/
gcc -o hello hello.c -lOpenCL
./hello

g++ -lOpenCL serial-histogram.cpp -o serial-histogram
./serial-histogram

g++ -o histogram histogram.cpp -lOpenCL
./histogram
```

## input file for serial-histogram.cpp & histogram.cpp

```bash
# Example: input image has three pixels, (255 0 0), (0 100 255), (255 255 255)
9 # The size of the one-dimension array (N), N/3 lines
255 0 0 # R
0 100 255 # G
255 255 255 # B
# separated by a space
```

## Refs

- hello.c, hello.cl: https://us.fixstars.com/opencl/book/OpenCLProgrammingBook/first-opencl-program/
- `#define CL_USE_DEPRECATED_OPENCL_1_2_APIS`: https://stackoverflow.com/questions/28500496/opencl-function-found-deprecated-by-visual-studio
