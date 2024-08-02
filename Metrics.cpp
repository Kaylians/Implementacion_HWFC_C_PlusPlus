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
#include <set>
#include <cmath>

int hammingMetric(const std::vector<Pixel>& Map1, const std::vector<Pixel>& Map2) {
    int difference = 0;
    if (Map1.size() != Map2.size())
        return 0;
    for (int i = 0; i < Map1.size(); i++) {
        if (Map1[i] == Map2[i]) {
            
        }
        else {
            difference++;
        }
    }
    return difference;
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
    float test = 0.0f;
    //std::cout << "peso total: " << totalWeight << std::endl;
    for (int i = 0; i < lenght; i++) {
        tmp_values.push_back((values[i] / totalWeight));
        //std::cout << i << "tiene un valor de "<< values[i] << " el valor es: " << values[i] / totalWeight << std::endl;
        //test += (values[i] / totalWeight);
    }
    //std::cout << test << std::endl;
    //stopExecute(5000, "test");
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
    std::vector<Pattern> matches, unmatches;
    double result = 0.0;
    float constant_value = 0.001f;

    if (P.size() == 0 || Q.size() == 0) {
        return result;
    }

    int p = 0, q = 0;
    do {
        if (P[p].id == Q[q].id) {
            P_values.push_back(P[p].weight);
            Q_values.push_back(Q[q].weight);
            p++;
            q++;
        }
        else if (P[p].id < Q[q].id) {
            P_values.push_back(P[p].weight);
            Q_values.push_back(constant_value);
            p++;
        }
        else {
            P_values.push_back(constant_value);
            Q_values.push_back(Q[q].weight);
            q++;
        }
    } while (p < P.size() && q < Q.size());

    //std::cout << P_values.size() << " " << Q_values.size() << " parecidos" << matches.size() << std::endl;
    //KL_Divergence_UnmatchPercentAdd(P, matches, P_values);
    //KL_Divergence_UnmatchPercentAdd(Q, matches, Q_values);
    
    ConvertValuesToPercent(P_values, false);
    ConvertValuesToPercent(Q_values, false);


    if (P_values.size() != Q_values.size()) {
        ControlString("ERROR, P y Q no son iguales para la metrica");
    }
    else {
        for (int i = 0; i < P_values.size(); i++) {
            result += KL_Formula(P_values[i], Q_values[i]);
        }
    }
    return result;
}