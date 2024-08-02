#ifndef HWFC_H
#define HWFC_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

std::vector<Pattern> definePatternsHWFC(const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N);
bool HPattValidTile(const int pos, const int X, const int Y, const int N);


#endif 