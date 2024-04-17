#include "WFC.h"
#include "Pixel.h"
#include "Pattern.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

int hammingMetric(const std::vector<Pixel>& Map1, const std::vector<Pixel>& Map2) {
    int coincidence = 0;
    if (Map1.size() != Map2.size())
        return 0;
    for (int i = 0; i < Map1.size(); i++) {
        if (Map1[i] == Map2[i]) {
            coincidence++;
        }
    }
    return coincidence;
}
int KL_Divergence(std::vector<Pattern>& PA1, std::vector<Pattern>& PA2) {
    

    
    return 0;
}