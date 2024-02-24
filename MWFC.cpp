#include "MWFC.h"
#include "WFC.h"
#include "Pixel.h"
#include "Pattern.h"
#include "DebugUtility.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

//funcion para ver si dos patrones son iguales
bool comparePatternMWFC(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}
//funcion para separar la imagen en los diferentes patrones que la componen
void definePatternsMWFC(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N, bool midPattern) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    //seperacion de la imagen en multiples patrones
    //for (int y = 0; y <= inputImageHeight - N; y++)
    int pos = 0;
    for (int z = 0; z < N.size(); z++) {
        for (int y = 0; y <= inputImageHeight - N[z]; y++) {
            for (int x = 0; x <= inputImageWidth - N[z]; x++) {
                for (int i = 0; i < N[z]; i++) {
                    for (int j = 0; j < N[z]; j++) {
                        pos = (x + j + y * inputImageHeight + i * inputImageHeight);
                        auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[pos]);
                        tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                        tmpVector.push_back(pixelVector[pos]);
                    }
                }

                Pattern newPattern(pattArray.size(), N[z]);
                newPattern.addPixelVector(tmpVector);
                newPattern.addPixelCooVector(tmpCooVector);
                newPattern.midPattern = midPattern;
                pattArray.push_back(newPattern);
                tmpVector.clear();
                tmpCooVector.clear();
            }
        }
        findUniquePattern(pattArray);
        makeMirroRotPattern(pattArray);
    }

    std::cout << "Patrones base obtenidos de la imagen: " << pattArray.size() << std::endl;
    std::sort(pattArray.begin(), pattArray.end(), comparePatternMWFC);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
