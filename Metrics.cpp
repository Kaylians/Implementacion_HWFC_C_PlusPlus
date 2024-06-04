#include "WFC.h"
#include "Pixel.h"
#include "Pattern.h"
#include "Metrics.h"
#include "DebugUtility.h"
#include "ReadWrite.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

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
void ConvertValuesToPercent(std::vector<float>& values, bool includeIncompatibilityPercent) {
    std::vector<float> tmp_values;
    float totalWeight = 0, tmp_weight;
    int lenght = 0;
    if (includeIncompatibilityPercent) {
        lenght = values.size();
    }
    else {
        lenght = values.size() - 1;
    }

    for (int i = 0; i < lenght; i++) {
        tmp_weight = values[i];
        totalWeight += tmp_weight;
    }
    for (int i = 0; i < lenght; i++) {
        tmp_values.push_back((values[i] / totalWeight) * 100);
    }
    values = tmp_values;
}
double KL_Formula(double p, double q) {
    if (p == 0.0 || q == 0.0) {
        return 0.0;
    }
    return p * log(p / q);
}
void KL_Divergence_UnmatchPercentAdd(std::vector<Pattern>& P, std::vector<Pattern>& matches, std::vector<float>& P_values) {
    std::vector<Pattern> unmatches;
    int incompatibilityCounter = 0;
    for (int i = 0; i < P.size(); i++) {
        bool difference = true;
        for (int j = 0; j < matches.size(); j++) {
            if (P[i].compareCooPattern(matches[j].pixelesCoo)) {
                difference = false;
            }
        }
        if (difference) {
            unmatches.push_back(P[i]);
        }
    }
    for (int i = 0; i < unmatches.size(); i++) {
        incompatibilityCounter += unmatches[i].weight;
    }
    P_values.push_back(incompatibilityCounter);
}
double KL_Divergence(std::vector<Pattern> P, std::vector<Pattern> Q) {
    std::vector<float> P_values, Q_values;
    std::vector<Pattern> matches;
    double result = 0.0;
    //
    for (int i = 0; i < P.size(); i++) {
        for (int j = 0; j < Q.size(); j++) {
            if (P[i].pixelesCoo.size() == Q[j].pixelesCoo.size()) {
                if (P[i].compareCooPattern(Q[j].pixelesCoo)) {
                    P_values.push_back(P[i].weight);
                    Q_values.push_back(Q[j].weight);
                    matches.push_back(P[i]);
                }
                
            }
        }
    }

    KL_Divergence_UnmatchPercentAdd(P, matches, P_values);
    KL_Divergence_UnmatchPercentAdd(Q, matches, Q_values);
    ConvertValuesToPercent(P_values, false);
    ConvertValuesToPercent(Q_values, false);

    if (P_values.size() != Q_values.size()) {
        ControlString("ERROR nigga, P y Q no son iguales para la metrica");
    }
    else {
        for (int i = 0; i < P_values.size(); i++) {
            result += KL_Formula(P_values[i], Q_values[i]);
        }
    }
    return result;
}