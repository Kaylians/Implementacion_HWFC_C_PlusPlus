#include "WFC.h"
#include "Pixel.h"
#include "Pattern.h"
#include "Metrics.h"

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
int KL_Divergence(std::vector<Pattern>& P, std::vector<Pattern>& Q) {
    std::vector<Pattern> tmp_vector;
    //p1 * log (p1/q1)
    std::vector<int> result;
    int min_size = P.size();
    if (min_size > Q.size()) {
        min_size = Q.size();
    }
    for (int i = 0; i < min_size; i++) {
        for (int j = 0; j < min_size; j++) {
            if (P[i].compareCooPattern(Q[j].pixelesCoo)) {
                tmp_vector.push_back(P[i]);
                break;
            }
        }
    }
    for (int i = 0; i )
    
    return 0;
}
void SaveFileKLD() {

}
void PerformMetrics() {

}