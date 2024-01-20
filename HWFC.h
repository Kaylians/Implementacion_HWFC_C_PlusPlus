#ifndef HWFC_H
#define HWFC_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

void definePatternsHWFC(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N);
bool HPatternTileSelection(const int pos, const int Y, const int N);


#endif 