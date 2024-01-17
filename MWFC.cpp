#include "MWFC.h"
#include "Pixel.h"
#include "Pattern.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

//funcion para ver si dos patrones son iguales
bool comparePatternMWFC(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}
//funcion para separar la imagen en los diferentes patrones que la componen
void definePatternsMWFC(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    //seperacion de la imagen en multiples patrones
    //for (int y = 0; y <= inputImageHeight - N; y++)
    for (int z = 0; z < N.size(); z++) {
        for (int x = 0; x <= inputImageWidth * inputImageHeight - (inputImageWidth * (N[z] - 1)) - N[z]; x++) {
            for (int i = 0; i < N[z]; i++) {
                for (int j = 0; j < N[z]; j++) {
                    auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[(x + j + i * inputImageWidth)]);
                    tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                    tmpVector.push_back(pixelVector[(x + j + i * inputImageWidth)]);
                }
            }
            for (int i = 0; i < 4; i++) {
                if (i == 0) {
                    //definir y añadir patron base al arreglo
                    Pattern newPattern(pattArray.size(), N[z]);
                    newPattern.addPixelVector(tmpVector);
                    newPattern.addPixelCooVector(tmpCooVector);
                    pattArray.push_back(newPattern);

                    //añadir espejo del patron inicial
                    Pattern newPatternMirror(pattArray.size(), N[z]);
                    newPatternMirror.addPixelVector(newPatternMirror.mirrorPattern(tmpVector));
                    newPatternMirror.addPixelCooVector(newPatternMirror.mirrorPatternCoo(tmpCooVector));
                    pattArray.push_back(newPatternMirror);
                }
                else {

                    //añdir rotacion del patron base
                    Pattern newPatternRot(pattArray.size(), N[z]);
                    tmpVector = newPatternRot.rotatePattern(tmpVector);
                    tmpCooVector = newPatternRot.rotatePatternCoo(tmpCooVector);
                    newPatternRot.addPixelVector(tmpVector);
                    newPatternRot.addPixelCooVector(tmpCooVector);
                    pattArray.push_back(newPatternRot);

                    //rot espejo de la rotacion
                    Pattern newPatternRotMirror(pattArray.size(), N[z]);
                    newPatternRotMirror.addPixelVector(newPatternRotMirror.mirrorPattern(tmpVector));
                    newPatternRotMirror.addPixelCooVector(newPatternRotMirror.mirrorPatternCoo(tmpCooVector));
                    pattArray.push_back(newPatternRotMirror);
                }
            }
            tmpVector.clear();
            tmpCooVector.clear();

        }
    }

    int weight = 0;
    for (int i = 0; i < pattArray.size(); i++) {
        if (pattArray[i].pattern) {
            weight++;
            for (int j = i + 1; j < pattArray.size(); j++) {
                if (pattArray[j].N == pattArray[i].N) {
                    if (pattArray[j].pattern && pattArray[i].comparePixelPattern(pattArray[j].pixeles)) {
                        pattArray[j].pattern = false;
                        weight++;
                    }
                }

            }
            pattArray[i].weight = weight;
        }
        weight = 0;
    }

    std::vector<Pattern> tmpPattArray;
    for (int i = 0; i < pattArray.size(); i++)
        if (pattArray[i].pattern) {
            tmpPattArray.push_back(pattArray[i]);
        }
    pattArray.clear();
    pattArray = tmpPattArray;

    std::cout << "Patrones totales obtenidos de la imagen: " << pattArray.size() << std::endl;
    std::sort(pattArray.begin(), pattArray.end(), comparePatternMWFC);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
