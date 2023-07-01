#include "Pixel.h"

Pixel::Pixel(unsigned char red, unsigned char green, unsigned char blue) {
    r = red;
    g = green;
    b = blue;
}

bool Pixel::isBlackAndWhite() {
    return r == g && g == b;
}

bool Pixel::isBlack() {
    return r == 0 && g == 0 && b == 0;
}

bool Pixel::isWhite() {
    return r == 255 && g == 255 && b == 255;
}

unsigned char Pixel::getBrightness() {
    unsigned char brightness = ((int)r + (int)g + (int)b) / 3;
    return brightness;
}

void Pixel::setBrightness(unsigned char brightness) {
    r = brightness;
    g = brightness;
    b = brightness;
}
