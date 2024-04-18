#ifndef WFC_H
#define WFC_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

void definePatternsWFC(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, int N);
void findUniquePattern(std::vector<Pattern>& pattArray);
void makeMirroRotPattern(std::vector<Pattern>& pattArray);

void infoPatternUpdateID(std::vector<Pattern>& pBase, std::vector<Pattern>& pLow, std::vector<Pattern>& pMid, std::vector<Pattern>& pHigh);
#endif 