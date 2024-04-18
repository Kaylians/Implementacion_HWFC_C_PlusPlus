#include "WFC.h"
#include "Pixel.h"
#include "Pattern.h"
#include "DebugUtility.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

//funcion para ver si dos patrones son iguales
bool comparePatternWFC(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}

void findUniquePattern(std::vector<Pattern>& pattArray) {
    std::vector<Pattern> tmpPattArray;
    int weight = 1;
    for (int i = 0; i < pattArray.size(); i++) {
        if (pattArray[i].pattern) {
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
        weight = 1;
    }
    for (int i = 0; i < pattArray.size(); i++)
        if (pattArray[i].pattern) {
            tmpPattArray.push_back(pattArray[i]);
            tmpPattArray[tmpPattArray.size() - 1].id = tmpPattArray.size() - 1;
        }
    pattArray.clear();
    pattArray = tmpPattArray;
}
void makeMirroRotPattern(std::vector<Pattern>& pattArray) {
    std::vector<Pattern> tmpPattArray, finalPattArray;
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    for (int i = 0; i < pattArray.size(); i++) {
        tmpVector = pattArray[i].pixeles;
        tmpCooVector = pattArray[i].pixelesCoo;
        finalPattArray.push_back(pattArray[i]);
        for (int i = 0; i < 4; i++) {
            if (i == 0) {
                //añadir espejo del patron inicial
                Pattern newPatternMirror(tmpPattArray.size(), pattArray[i].N);
                newPatternMirror.addPixelVector(newPatternMirror.mirrorPattern(tmpVector));
                newPatternMirror.addPixelCooVector(newPatternMirror.mirrorPatternCoo(tmpCooVector));
                tmpPattArray.push_back(newPatternMirror);

                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();
            }
            else {

                //añdir rotacion del patron base
                Pattern newPatternRot(tmpPattArray.size(), pattArray[i].N);
                tmpVector = newPatternRot.rotatePattern(tmpVector);
                tmpCooVector = newPatternRot.rotatePatternCoo(tmpCooVector);
                newPatternRot.addPixelVector(tmpVector);
                newPatternRot.addPixelCooVector(tmpCooVector);
                tmpPattArray.push_back(newPatternRot);

                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();

                //rot espejo de la rotacion
                Pattern newPatternRotMirror(tmpPattArray.size(), pattArray[i].N);
                newPatternRotMirror.addPixelVector(newPatternRotMirror.mirrorPattern(tmpVector));
                newPatternRotMirror.addPixelCooVector(newPatternRotMirror.mirrorPatternCoo(tmpCooVector));
                tmpPattArray.push_back(newPatternRotMirror);

                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();
            }
        }
    }
    findUniquePattern(finalPattArray);
    pattArray.clear();
    pattArray = finalPattArray;
}
//funcion para separar la imagen en los diferentes patrones que la componen
void definePatternsWFC(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, int N) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;
    int pos = 0, counter = 0;
    //seperacion de la imagen en multiples patrones
    for (int y = 0; y <= inputImageHeight - N; y++) {
        for (int x = 0; x <= inputImageWidth - N; x++) {
            //definición de patron de tamaño N*N
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    pos = (x + j + y * inputImageHeight + i * inputImageHeight);
                    auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[pos]);
                    tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                    tmpVector.push_back(pixelVector[pos]);
                }
            }
            //guardado del patron y creación de espejo y rotaciones
            //definir y añadir patron base al arreglo
            Pattern newPattern(pattArray.size(), N);
            newPattern.addPixelVector(tmpVector);
            newPattern.addPixelCooVector(tmpCooVector);
            pattArray.push_back(newPattern);

            tmpVector.clear();
            tmpCooVector.clear();
        }
    }
    std::cout << "patrones obtenidos para tamaño de patron " << N << ": " << pattArray.size() << std::endl;

    //definicion de patrones unicos y marcado/eliminacion de elementos repetidos
    findUniquePattern(pattArray);

    std::cout << "Patrones base obtenidos de la imagen: " << pattArray.size() << std::endl;
    std::sort(pattArray.begin(), pattArray.end(), comparePatternWFC);

    //creacion de patrones espejo y rotaciones
    makeMirroRotPattern(pattArray);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
        //std::cout << "conteo de patrones: patron " << pattArray[i].id << " se repite: " << pattArray[i].weight;
        if (pattArray[i].pattern) {
            //std::cout << "  es patron";
        }
        //std::cout << std::endl;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
void infoPatternUpdateID(std::vector<Pattern>& pBase, std::vector<Pattern>& pLow, std::vector<Pattern>& pMid, std::vector<Pattern>& pHigh) {
    pBase.reserve(pLow.size() + pMid.size() + pHigh.size());
    pBase.insert(pBase.end(), pLow.begin(), pLow.end());
    if (pMid.size() > 0)
        pBase.insert(pBase.end(), pMid.begin(), pMid.end());
    if (pHigh.size() > 0)
        pBase.insert(pBase.end(), pHigh.begin(), pHigh.end());
    findUniquePattern(pBase);

    for (int i = 0; i < pHigh.size(); i++) {
        for (int j = 0; j < pBase.size(); j++) {
            if (pHigh[i].comparePixelPattern(pBase[j].pixeles)) {
                pHigh[i].id = pBase[j].id;
            }
        }
    }
    for (int i = 0; i < pMid.size(); i++) {
        for (int j = 0; j < pBase.size(); j++) {
            if (pMid[i].comparePixelPattern(pBase[j].pixeles)) {
                pMid[i].id = pBase[j].id;
            }
        }
    }
    for (int i = 0; i < pLow.size(); i++) {
        for (int j = 0; j < pBase.size(); j++) {
            if (pLow[i].comparePixelPattern(pBase[j].pixeles)) {
                pLow[i].id = pBase[j].id;
            }
        }
    }
}
