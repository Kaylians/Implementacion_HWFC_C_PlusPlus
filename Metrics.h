#ifndef METRICS
#define METRICS

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

int hammingMetric(const std::vector<Pixel>& Map1, const std::vector<Pixel>& Map2);
double KL_Divergence(std::vector<Pattern> P, std::vector<Pattern> Q);

#endif 