#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

class Pixel {
private:
    unsigned char r, g, b;

public:
    Pixel(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0) {
        r = red;
        g = green;
        b = blue;
    }

    __host__ __device__ bool isBlackAndWhite() {
        return r == g && g == b;
    }

    __host__ __device__ bool isBlack() {
        return r == 0 && g == 0 && b == 0;
    }

    __host__ __device__ bool isWhite() {
        return r == 255 && g == 255 && b == 255;
    }

    __host__ __device__ unsigned char getBrightness() {
        unsigned char brightness = ((int)r + (int)g + (int)b) / 3;
        return brightness;
    }

    __host__ __device__ void setBrightness(unsigned char brightness) {
        r = brightness;
        g = brightness;
        b = brightness;
    }
};

// CUDA kernel function to convert the image to grayscale and create a binary image
__global__ void convert_to_grayscale_kernel(Pixel* pixels, long N) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < N) {
        if (pixels[idx].isBlack() || pixels[idx].isWhite() || pixels[idx].isBlackAndWhite()) {
            return;
        }

        unsigned char brightness = pixels[idx].getBrightness();
        pixels[idx].setBrightness(brightness);
    }
}

void cuda_implementation(std::string& load_image)
{
    static int run = 0;
    clock_t start = clock();

    // Load image file
    std::ifstream file(load_image, std::ios::binary);
    if (!file) {
        printf("Could not open image file.\n");
        return;
    }

    std::string format;
    std::getline(file, format);

    int width, height, maxval;
    file >> width >> height >> maxval;
    long N = width * height;
    file.ignore(1);

    // Read pixel data
    Pixel* host_pixels = new Pixel[N];
    file.read((char*)host_pixels, N * sizeof(Pixel));

    // Allocate memory on the device
    Pixel* device_pixels;
    cudaMalloc(&device_pixels, N * sizeof(Pixel));

    // Copy data to to device
    cudaMemcpy(device_pixels, host_pixels, N * sizeof(Pixel), cudaMemcpyHostToDevice);
    
    // Launch the kernel to convert the image to grayscale
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    convert_to_grayscale_kernel << <blocksPerGrid, threadsPerBlock >> > (device_pixels, N);
    cudaDeviceSynchronize();

    /*
        // Functioneaza

        // Define the grid structure
        dim3 block_size(256, 1, 1);
        dim3 grid_size(N / block_size.x + 1, 1, 1);
        convert_to_grayscale_kernel << <block_size, grid_size >> > (device_pixels, N);
        cudaDeviceSynchronize();
        
    
        // NU functioneaza

        // Define the grid structure
        dim3 block_size(32, 32, 1);
        dim3 grid_size(N / block_size.x + 1, N / block_size.y + 1, 1);
        convert_to_grayscale_kernel << <block_size, grid_size >> > (device_pixels, N);
        cudaDeviceSynchronize();
    */
    
    // Copy the results back to the host
    cudaMemcpy(host_pixels, device_pixels, N * sizeof(Pixel), cudaMemcpyDeviceToHost);

    // Ignore the first call
    if (run == 0) {
        run++;
        return;
    }

    // Save binary image to file
    std::string save_image = load_image;
    save_image.resize(save_image.length() - 4);
    save_image += "_cuda.ppm";
    std::ofstream outFile(save_image, std::ios::binary);
    outFile << format << "\n" << width << " " << height << "\n" << maxval << "\n";
    outFile.write((const char*)host_pixels, N * sizeof(Pixel));

    // Free memory
    cudaFree(device_pixels);
    delete[] host_pixels;

    clock_t end = clock();

    printf("Time elapsed: %.5f seconds (CUDA implementation)\n", float(end - start) / CLOCKS_PER_SEC);
}

int main() {

    // Ignore the first call as it causes elapsed time to increase
    std::string ignore = "ignore.ppm";
    cuda_implementation(ignore);

    std::string img256px = "image256px.ppm";
    std::cout << "Smaller images:\n";
    cuda_implementation(img256px);

    std::string img4k = "image4k.ppm";
    std::cout << "\nLarger images:\n";
    cuda_implementation(img4k);
    return 0;
}