
#ifndef PIXEL_H
#define PIXEL_H

#include <vector>

struct Pixel {
    unsigned char R, G, B;

    Pixel() : R(0), G(0), B(0) {}

    Pixel(unsigned char red, unsigned char green, unsigned char blue)
        : R(red), G(green), B(blue) {}

    bool operator==(const Pixel& otro) const {
        return R == otro.R && G == otro.G && B == otro.B;
    }

    static bool contienePixel(const std::vector<Pixel>& vector, const Pixel& pixel) {
        for (const auto& elemento : vector) {
            if (elemento.R == pixel.R && elemento.G == pixel.G && elemento.B == pixel.B) {
                return true; // Se encontró un elemento igual
            }
        }
        return false; // No se encontró ningún elemento igual
    }

};


#endif 
