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
void ConvertValuesToPercent(std::vector<float>& values) {
    std::vector<float> tmp_values;
    int totalWeight = 0, tmp_weight;

    ControlString("convertir valores a porcentaje ");
    for (int i = 0; i < values.size(); i++) {
        tmp_weight = static_cast<int>(values[i]);
        ControlPoint(tmp_weight);
        totalWeight += tmp_weight;
    }
    std::cout << "peso total: " << totalWeight << std::endl;
    for (int i = 0; i < values.size(); i++) {
        tmp_values.push_back((values[i] / totalWeight) * 100);
        ControlString(" porcentaje: " + std::to_string(tmp_values[i]));
    }
    values = tmp_values;
    float test = 0;
    for (int i = 0; i < tmp_values.size(); i++) {
        test += tmp_values[i];
    }
    ControlPoint(test);

}
double KL_Formula(double p, double q) {
    if (p == 0.0 || q == 0.0) {
        return 0.0;
    }
    return p * log(p / q);
}
void KL_Divergence_UnmatchPercentAdd( std::vector<Pattern>& P, std::vector<Pattern>& matches, std::vector<float>& P_values) {
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
    unmatches.clear();
}
double KL_Divergence(std::vector<Pattern> P, std::vector<Pattern> Q) {
    std::vector<float> P_values, Q_values;
    std::vector<Pattern> matches;
    double result = 0.0;
    ControlString("KL Divergence control start");
    //revisar cuales patrones estan en el mismo vector
    //
    for (int i = 0; i < P.size(); i++) {
        for (int j = 0; j < Q.size(); j++) {
            if (P[i].pixelesCoo.size() == Q[j].pixelesCoo.size()) {
                if (P[i].compareCooPattern(Q[j].pixelesCoo)) {
                    std::cout << "equal pattern find, P" << i << ": " << P[i].weight << "/ Q" << j << ": " << Q[j].weight << std::endl;
                    PrintPixelCoo(P[i]);
                    PrintPixelCoo(Q[j]);
                    P_values.push_back(P[i].weight);
                    Q_values.push_back(Q[j].weight);
                    matches.push_back(P[i]);
                }
               
            }
        }
    }

    KL_Divergence_UnmatchPercentAdd(P, matches, P_values);
    KL_Divergence_UnmatchPercentAdd(Q, matches, Q_values);
    ConvertValuesToPercent(P_values);
    ConvertValuesToPercent(Q_values);

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