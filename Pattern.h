#ifndef PATTERN_H
#define PATTERN_H

#include <iostream>
#include <vector>
#include "Pixel.h"

class Pattern {
public:
    int id;
    int N;
    int weight;
    bool pattern = true;
    bool highPattern = false;
    std::vector<Pixel> pixeles;
    std::vector<int> pixelesCoo;
    std::vector<int> coordinate;

    // Constructor
    Pattern(int id, int N);

    // Métodos de la clase
    bool comparePixelPattern(const std::vector<Pixel>& otherPixeles);
    bool compareCooPattern(const std::vector<int>& otherPixeles);
    std::vector<Pixel> rotatePattern(const std::vector<Pixel>& Pattern);
    std::vector<int> rotatePatternCoo(const std::vector<int>& Pattern);
    std::vector<Pixel> mirrorPattern(const std::vector<Pixel>& Pattern);
    std::vector<int> mirrorPatternCoo(const std::vector<int>& Pattern);
    void addPixelVector(std::vector<Pixel> newPixeles);
    void addPixelCooVector(std::vector<int> newPixeles);
};

#endif 