#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <omp.h>

#include "Pixel.h"

void sequential_implementation(std::string& load_image);
void parallel_for_implementation(std::string& load_image);
void parallel_implementation(std::string& load_image);

int main() {
    std::string img256px = "image256px.ppm";
    std::cout << "Smaller images:\n";
    sequential_implementation(img256px);
    parallel_for_implementation(img256px);
    parallel_implementation(img256px);

    std::string img4k = "image4k.ppm";
    std::cout << "\nLarger images:\n";
    sequential_implementation(img4k);
    parallel_for_implementation(img4k);
    parallel_implementation(img4k);
    return 0;
}

void sequential_implementation(std::string& load_image)
{
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
    file.ignore(1);

    // Read pixel data
    Pixel* pixels = new Pixel[width * height];
    file.read((char*)pixels, width * height * sizeof(Pixel));

    // Convert to grayscale and create binary image
    for (int i = 0; i < width * height; ++i) {
        if (pixels[i].isBlack() || pixels[i].isWhite() || pixels[i].isBlackAndWhite())
            continue;
        else
        {
            unsigned char brightness = pixels[i].getBrightness();
            pixels[i].setBrightness(brightness);
        }
    }

    // Save binary image to file
    std::string save_image = load_image;
    save_image.resize(save_image.length() - 4);
    save_image += "_sequential.ppm";
    std::ofstream outFile(save_image, std::ios::binary);
    outFile << format << "\n" << width << " " << height << "\n" << maxval << "\n";
    outFile.write((const char*)pixels, width * height * sizeof(Pixel));

    delete[] pixels;

    clock_t end = clock();
    printf("Time elapsed: %.5f seconds (sequential implementation)\n", float(end - start) / CLOCKS_PER_SEC);
}

void parallel_for_implementation(std::string& load_image)
{
    clock_t start = clock();

    // Load image file
    std::ifstream file(load_image, std::ios::binary);
    if (!file) {
        printf("Error: Could not open image file.\n");
        return;
    }

    std::string format;
    std::getline(file, format);

    int width, height, maxval;
    file >> width >> height >> maxval;
    file.ignore(1);

    // Read pixel data
    Pixel* pixels = new Pixel[width * height];
    file.read((char*)pixels, width * height * sizeof(Pixel));

    // Convert to grayscale and create binary image
    int num_threads = 8;
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for (int i = 0; i < width * height; ++i) {
        if (pixels[i].isBlack() || pixels[i].isWhite() || pixels[i].isBlackAndWhite())
            continue;
        else
        {
            unsigned char brightness = pixels[i].getBrightness();
            pixels[i].setBrightness(brightness);
        }
    }

    // Save binary image to file
    std::string save_image = load_image;
    save_image.resize(save_image.length() - 4);
    save_image += "_parallel_for.ppm";
    std::ofstream outFile(save_image, std::ios::binary);
    outFile << format << "\n" << width << " " << height << "\n" << maxval << "\n";
    outFile.write((const char*)pixels, width * height * sizeof(Pixel));

    delete[] pixels;

    clock_t end = clock();
    printf("Time elapsed: %.5f seconds (parallel for implementation, %d threads)\n", float(end - start) / CLOCKS_PER_SEC, num_threads);
}

void parallel_implementation(std::string& load_image)
{
    clock_t start = clock();

    // Load image file
    std::ifstream file(load_image, std::ios::binary);
    if (!file) {
        printf("Error: Could not open image file.\n");
        return;
    }

    std::string format;
    std::getline(file, format);

    int width, height, maxval;
    file >> width >> height >> maxval;
    file.ignore(1);

    // Read pixel data
    Pixel* pixels = new Pixel[width * height];
    file.read((char*)pixels, width * height * sizeof(Pixel));

    // Convert to grayscale and create binary image
    int num_threads = 8;
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        int tId = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        long long chunk_size = (width * height + nthreads - 1) / nthreads;

        int start = tId * chunk_size;
        int end = (tId + 1) * chunk_size;
        if (end > width * height) {
            end = width * height;
        }

        for (int i = start; i < end; ++i) {
            if (pixels[i].isBlack() || pixels[i].isWhite() || pixels[i].isBlackAndWhite())
                continue;
            else
            {
                unsigned char brightness = pixels[i].getBrightness();
                pixels[i].setBrightness(brightness);
            }
        }
    }

    // Save binary image to file
    std::string save_image = load_image;
    save_image.resize(save_image.length() - 4);
    save_image += "_parallel.ppm";
    std::ofstream outFile(save_image, std::ios::binary);
    outFile << format << "\n" << width << " " << height << "\n" << maxval << "\n";
    outFile.write((const char*)pixels, width * height * sizeof(Pixel));

    delete[] pixels;

    clock_t end = clock();
    printf("Time elapsed: %.5f seconds (parallel implementation, %d threads)\n", float(end - start) / CLOCKS_PER_SEC, num_threads);
}

