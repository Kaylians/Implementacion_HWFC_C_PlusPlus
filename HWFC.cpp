#include "WFC.h"
#include "HWFC.h"
#include "Main.h"
#include "Pixel.h"
#include "Pattern.h"
#include "DebugUtility.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>




//funcion para ver si dos patrones son iguales
bool comparePatternHWFC(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}
bool HPattValidTile(const int pos, const int X, const int Y, const int N) {
    if (pos % Y - (N - 1) >= 0) 
        if (pos / Y - (N - 1) >= 0 || pos / Y + (N - 1) < Y)
            return true;
    if (pos % Y + (N - 1) < Y)
        if (pos / Y - (N - 1) >= 0 || pos / Y + (N - 1) < Y) 
            return true;
    return false;
}
//funcion para separar la imagen en los diferentes patrones que la componen
std::vector<Pattern> definePatternsHWFC(const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;
    std::vector<Pattern> hPattArray;
    int counter = 0, pos = 0;

    // X e Y dan el punto de inicio
    // I y J dan el las areas del patron

    for (int z = 0; z < N.size(); z++) {
        for (int y = 0; y <= inputImageHeight - N[z]; y++) {
            for (int x = 0; x <= inputImageWidth - N[z]; x++) {
                for (int i = 0; i < N[z]; i++) {
                    for (int j = 0; j < N[z]; j++) {
                        pos = (x + j + y * inputImageHeight + i * inputImageHeight);
                        if (i == 0 || i == N[z] - 1 || j == 0 || j == N[z] - 1) {
                            auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[pos]);
                            tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                            tmpVector.push_back(pixelVector[pos]);
                            //std::cout << tmpCooVector.back();
                        }
                        else {
                            tmpCooVector.push_back(-1);
                            tmpVector.push_back(posibleTiles.back());
                            //std::cout << " ";
                        }

                    }
                    //std::cout << std::endl;
                }

                Pattern newPattern(hPattArray.size(), N[z]);
                newPattern.addPixelVector(tmpVector);
                newPattern.addPixelCooVector(tmpCooVector);
                newPattern.highPattern = true;
                hPattArray.push_back(newPattern);
                tmpVector.clear();
                tmpCooVector.clear();
            }
        }
        //findUniquePattern(hPattArray);
        //makeMirroRotPattern(hPattArray);
    }
    

    std::cout << "Patrones totales obtenidos de la imagen: " << hPattArray.size() << std::endl;
    std::sort(hPattArray.begin(), hPattArray.end(), comparePatternHWFC);

    for (int i = 0; i < hPattArray.size(); i++) {
        hPattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << hPattArray.size() << std::endl;

    return hPattArray;
}//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
