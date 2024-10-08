#ifndef WFC_H
#define WFC_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>
bool comparePatternWFC(const Pattern& a, const Pattern& b);
std::vector<std::vector<Pattern>> definePatternsWFC(const std::vector<Pixel>& pixelVector, const std::vector<Pixel>& posibleTiles, const int inputImageHeight, const int inputImageWidth, const std::vector<int>& N);
void findUniquePatternCSV(std::vector<Pattern>& pattArray);
void findUniquePattern(std::vector<Pattern>& pattArray);
void makeMirroRotPattern(std::vector<Pattern>& pattArray);
void infoPatternUpdateID(std::vector<Pattern>& pattern, std::vector<Pattern>& pHigh);

#endif 