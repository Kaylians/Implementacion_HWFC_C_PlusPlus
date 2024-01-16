// cd /mnt/d/Memoria\ HWFC/Code/test2/src

// g++ -g -Wall -Wextra -o WFC WFC.cpp

// valgrind ./WFC
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./WFC
// valgrind --tool=cachegrind ./WFC

//problemas encontrados:
/*
* //despues de terminar la ejecucion
* double free or corruption (out)
* free(): invalid size
* munmap_chunk(): invalid pointer
* Segmentation fault (core dumped)
* munmap_chunk(): invalid pointer
* 
* //durante la ejecucion
* corrupted size vs. prev_size Aborted (core dumped)  reducir mapa(ultimo print)  
* free(): invalid next size (fast)  (despues de terminar la iteración, no se llego a un print nuevo) (M)
* munmap_chunk(): invalid pointer (misma situacion a la anterior) (no se tenia ningun RPP) (M)
* malloc(): invalid next size (unsorted) (fallo durante la propagacion)
* double free or corruption (!prev) (fallo durante la propagacion) (ultimo print reducir mapa)
* malloc(): invalid next size (unsorted) (durante  el guardado de backtracking)
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstdio>

#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include "WFC.h"
#include "Pixel.h"
#include <thread>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

//colores de prueba
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define WHITE    "\033[37m"
#define PURPLE "\033[35m"

class Pattern {
public:
    int id;
    int N;
    int weight;
    bool pattern = true;
    std::vector<Pixel> pixeles;
    std::vector<int> pixelesCoo;
    std::vector<int> coordinate;
    //constructor
    Pattern(int id, int N) : id(id), N(N) {}
    bool comparePixelPattern(const std::vector<Pixel>& otherPixeles) {

        if (otherPixeles.size() != pixeles.size()) {
            std::cout << "Error, tamaño de tiles no compatible 1" << std::endl;
            return false;
        }

        return otherPixeles == pixeles;
    }
    bool compareCooPattern(const std::vector<int>& otherPixeles) {

        if (otherPixeles.size() != pixelesCoo.size()) {
            std::cout << "Error, tamaño de tiles no compatible 2" << std::endl;
            return false;
        }

        return otherPixeles == pixelesCoo;
    }

    std::vector<Pixel> rotatePattern(const std::vector<Pixel>& Pattern) {
        std::vector<Pixel> newPattern;
        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[N * j + i]);
            }
        }
        return newPattern;
    }
    std::vector<int> rotatePatternCoo(const std::vector<int>& Pattern) {
        std::vector<int> newPattern;
        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[N * j + i]);
            }
        }
        return newPattern;
    }
    std::vector<Pixel> mirrorPattern(const std::vector<Pixel>& Pattern) {
        std::vector<Pixel> newPattern;

        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[j + i * N]);
            }
        }
        return newPattern;
    }
    std::vector<int> mirrorPatternCoo(const std::vector<int>& Pattern) {
        std::vector<int> newPattern;

        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[j + i * N]);
            }
        }
        return newPattern;
    }

    void addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
    void addPixelCooVector(std::vector<int> newPixeles) {
        pixelesCoo = newPixeles;
    }
};
//funcion para inicializar la generación de numeros aleatorios
void initializeRandomSeed() {
    std::srand(std::time(0));
}
//funcion para obtener numeros aleatorios con min y max
int getRandom(int min, int max) {
    int rnd = 0;
    rnd = std::rand() % max;
    return rnd;
}
//funcion random para elegir el patron que más se repite
int getRandomPatternWeighted(const std::vector<Pattern> pattern) {

    std::vector<double> pesos(pattern.size());
    for (size_t i = 0; i < pattern.size(); i++) {
        pesos[i] = pattern[i].weight;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution(pesos.begin(), pesos.end());
    return distribution(gen);
}
//funcion para la lectura de la imagen de ejemplo
bool readImagePPM(const std::string& r, int& w, int& h, std::vector<Pixel>& pixeles) {
    std::ifstream archivo(r, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << r << std::endl;
        return false;
    }

    std::string encabezado;
    archivo >> encabezado;

    if (encabezado != "P6") {
        std::cerr << "Formato de archivo PPM no válido." << std::endl;
        return false;
    }

    archivo >> w >> h;
    int maxValor;
    archivo >> maxValor;

    archivo.ignore(); // Ignorar el espacio en blanco después del valor máximo

    pixeles.resize(w* h);
    archivo.read(reinterpret_cast<char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return true;
}
//funcion para la escritura de una nueva imagen
bool writeImagePPM(const std::string& r, int w, int h, const std::vector<Pixel>& pixeles) {
    std::ofstream archivo(r, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << r << std::endl;
        return false;
    }

    archivo << "P6\n" << w << " " << h << "\n255\n";
    archivo.write(reinterpret_cast<const char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return true;
}
//definir cuantas posibles estados puede tomar un pixel y cuantas veces aparece
void defineTiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
}
//funciona para imprimir el mapa que son valores int
void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi) {
    for (int i = -1; i < size; i++) {
        if (i == -1) {
            std::cout << BLUE << " X" << " " << RESET << "|| ";
            for (int z = 0; z < size; z++) {
                if (z < 10) {

                    std::cout << BLUE << " " << z << " " << RESET << "|| ";
                }
                else {

                    std::cout << BLUE << z << " " << RESET << "|| ";
                }
            }
        }
        else {
            for (int j = -1; j < size; j++) {
                if (j == -1) {
                    if (i < 10) {

                        std::cout << " " << BLUE << i << RESET << " || ";
                    }
                    else {

                        std::cout << BLUE << i << RESET << " || ";
                    }
                }
                else if (unCollapseMap[j + i * size].size() == 1) {
                    if (unCollapseMap[j + i * size].front() < 10) {
                        if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << GREEN << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 1)
                            std::cout << RED << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 2)
                            std::cout << BLUE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 3)
                            std::cout << WHITE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 4)
                            std::cout << YELLOW << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                    else {
                        std::cout << GREEN << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                }
                else if (unCollapseMap[j + i * size].size() == 0) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() == posibi) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() < posibi)
                    std::cout << PURPLE << " " << unCollapseMap[j + i * size].size() << RESET << " || ";
                else
                    std::cout << "X ||";
            }
        }

        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
//funcion para ver si dos patrones son iguales
bool comparePattern(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}
//funcion para separar la imagen en los diferentes patrones que la componen
void defineHPatterns(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N, std::string mode){
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    N = { 5 };

    for (int z = 0; z < N.size(); z++) {
        for (int x = 0; x <= inputImageWidth * inputImageHeight - (inputImageWidth * (N[z] - 1)) - N[z]; x++) {
            for (int i = 0; i < N[z]; i++) {
                for (int j = 0; j < N[z]; j++) {
                    if (i % N[z]-1 == 0 || j == 0 || j == N[z]-1) {
                        auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[(x + j + i * inputImageWidth)]);
                        tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                        tmpVector.push_back(pixelVector[(x + j + i * inputImageWidth)]);
                    }
                    else {
                        tmpCooVector.push_back(posibleTiles.size());
                        tmpVector = pixelVector;
                    }
                }
            }
            Pattern newPattern(pattArray.size(), N[z]);
            newPattern.addPixelVector(tmpVector);
            newPattern.addPixelCooVector(tmpCooVector);
            pattArray.push_back(newPattern);

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
    std::sort(pattArray.begin(), pattArray.end(), comparePattern);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}
void definePatterns(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, std::vector<int> N, std::string mode) {
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
    std::sort(pattArray.begin(), pattArray.end(), comparePattern);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
void initializePosMap(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& posibleTiles, int Y) {
    Y = Y * Y;
    unCollapseMap.resize(Y);
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < posibleTiles.size(); j++) {
            unCollapseMap[i].push_back(j);
        }
    }
}
//funcion para elegir la casilla con entropia (posibles colores) disponible en el mapa
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int size, int LastLowestEntropyTilePos, const std::vector<int>& RPP) {
    int lowestValue = size + 1, lowestID = -1;
    std::cout << "cantidad de regiones de propagacion: " << RPP.size() << std::endl;
    if (RPP.size() > 0) {
        std::cout << RED << "seleccionar entre regiones colapsadas" << RESET << std::endl;
        lowestID = RPP[getRandom(0, RPP.size())];
        lowestValue = unCollapseMap[lowestID].size();
        for (int i = 0; i < RPP.size(); i++) {
            if (lowestValue > unCollapseMap[RPP[i]].size()) {
                lowestID = RPP[i];
                lowestValue = unCollapseMap[RPP[i]].size();
            }
        }
    }
    else {
        std::cout << RED << "Buscar entre posiciones de baja entropia" << RESET << std::endl;
        for (int i = 0; i < unCollapseMap.size(); i++) {
            //si es que hay más de una posibilidad para la casilla
            if (unCollapseMap[i].size() > 1)
                if (unCollapseMap[i].size() < lowestValue) {
                    lowestID = i;
                    lowestValue = unCollapseMap[i].size();
                }
        }
    }
    return lowestID;
}
//funciones para confirmar si el patron coincide en cada uno de sus puntos con el espacio alrededor del punto entregado
bool selectPatternAnalizer(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
    bool contains = false;
    std::vector<int> mapCoo;
    int count = 0;
    for (int i = i_min, x = 0; i <= i_max; i++, x++) {
        for (int j = j_min, y = 0; j <= j_max; j++, y++) {
            for (int z = 0; z < unCollapseMap[pos + j + i * Y].size(); z++) {
                if (unCollapseMap[pos + j + i * Y].size() == 1) {
                    count++;
                    if (count == N * N) {
                        return false;
                    }
                }
                if (pattern.pixelesCoo[y + x * N] == unCollapseMap[pos + j + i * Y][z]) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                mapCoo.clear();
                return false;
            }
            else {
                contains = false;
            }

        }
    }
    pattern.coordinate = mapCoo;
    return true;
}
bool selectPattern(Pattern& pattern, std::vector<Pattern>& pattArray, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos, bool forceCenter) {

    bool U = false, S = false, E = false, W = false, C = false, contains = false;

    //center, norte, sur, este , oeste
    if (N % 2 == 1)
        if (pos % Y >= (N / 2) && pos % Y < Y - (N / 2) && pos / Y >= (N / 2) && pos / Y < Y - (N / 2)) {
            if (forceCenter) {
                for (int i = 0; i < pattArray.size(); i++) {
                    if (selectPatternAnalizer(pattArray[i], unCollapseMap, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                        pattern = pattArray[i];
                        return true;
                    }
                }
            }
            else if (selectPatternAnalizer(pattern, unCollapseMap, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                return true;
            }
        }

    if (pos / Y >= N - 1)
        U = true;
    if (pos / Y < Y - (N - 1))
        S = true;
    if (pos % Y >= N - 1)
        W = true;
    if (pos % Y < Y - (N - 1))
        E = true;

    if (U) {
        for (int i = 0; i < N; i++) {
            if (W) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, N, Y, pos, -(N - 1), 0, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, N, Y, pos, -(N - 1), 0, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    else if (S) {
        for (int i = 0; i < N; i++) {
            if (W) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, N, Y, pos, 0, N - 1, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, N, Y, pos, 0, N - 1, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
//funcion para colapsar una posicion a una patron en concreto que coincida por cada pixel con los valores adyacentes al punto
bool Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, const int Y, std::vector<Pattern>& pattern, Pattern& selectedPatt, int pos, int posibleTilesN) {

    auto newPattern = pattern.front();
    int stuck_Counter = 0;

    std::vector<int> usedPatterns;
    auto iterador = usedPatterns.end();

    bool finded;

    do {
        do {
            if (usedPatterns.size() == pattern.size()) {
                std::cout << RED << "!!!!!!!!!!!!!!!!!!!!!!!!! NO HAY SOLUCIONES DISPONIBLES PARA LA POSICION: " << pos << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << pattern.size() << std::endl;
                auto i = std::find(RPP.begin(), RPP.end(), pos);
                if (i != RPP.end())
                    RPP.erase(i);
                std::cout << "Posiciones restantes: ";
                for (int i = 0; i < RPP.size(); i++) {
                    std::cout << RPP[i] << " ";
                }
                std::cout << std::endl;
                return false;
            }
            //newPattern = pattern[getRandomPatternWeighted(pattern)];
            newPattern = pattern[getRandom(0, pattern.size())];
            iterador = std::find(usedPatterns.begin(), usedPatterns.end(), newPattern.id);
        } while (iterador != usedPatterns.end());
        usedPatterns.push_back(newPattern.id);
        //std::cout << "patron seleccionado: " << newPattern.id << std::endl;

        if (selectPattern(newPattern, pattern, unCollapseMap, newPattern.N, Y, pos, false)) {
            std::cout << "patron util encontrado: " << newPattern.id << std::endl;
            finded = true;
        }
        else {
            finded = false;
        }
        stuck_Counter++;
    } while (!finded);
    std::cout << "iteraciones realizadas buscando el patron: " << stuck_Counter << std::endl,
        stuck_Counter = 0;

    //RPP.clear();
    for (int i = 0; i < newPattern.N; i++) {
        for (int j = 0; j < newPattern.N; j++) {
            if (unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].size() > 1) {
                unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].clear();
                //convierte cada pixel del arreglo a su respectivo valor numerico que lo representa dentro de un collapseMap
                unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].push_back(newPattern.pixelesCoo[j + newPattern.N * i]);
                RPP.push_back(newPattern.coordinate[j + newPattern.N * i]);
            }
        }
    }
    selectedPatt = newPattern;
    bool U = false, S = false, E = false, O = false;
    int colide;
    std::vector<int> tmpRPP;
    std::vector<int> tmpEraseRPP;
    for (int i = 0; i < RPP.size(); i++) {
        std::cout << RPP[i];
        if (unCollapseMap[RPP[i]].size() < posibleTilesN) {
            U = false, S = false, E = false, O = false, colide = 0;
            if (RPP[i] / Y > 0) {
                if (unCollapseMap[RPP[i] - Y].size() > 1) {
                    tmpRPP.push_back(RPP[i] - Y);
                    U = true;
                }
            }
            if (RPP[i] / Y < Y - 1) {
                if (unCollapseMap[RPP[i] + Y].size() > 1) {
                    tmpRPP.push_back(RPP[i] + Y);
                    S = true;
                }
            }
            if (RPP[i] % Y > 1 && RPP[i] % Y < Y - 1) {
                if (unCollapseMap[RPP[i] + 1].size() > 1) {
                    tmpRPP.push_back(RPP[i] + 1);
                    E = true;
                }
            }
            if (RPP[i] % Y < Y - 1 && RPP[i] % Y > 0) {
                if (unCollapseMap[RPP[i] - 1].size() > 1) {
                    tmpRPP.push_back(RPP[i] - 1);
                    O = true;
                }
            }

            if (!U && !S && !E && !O) {
                tmpEraseRPP.push_back(RPP[i]);
                std::cout << RED << "X" << RESET;
            }
        }
        std::cout << "  ";
    }
    for (int i = 0; i < tmpEraseRPP.size(); i++) {
        auto it = std::find(RPP.begin(), RPP.end(), tmpEraseRPP[i]);

        if (it != RPP.end())
            RPP.erase(it);
    }
    std::cout << std::endl;
    for (int i = 0; i < tmpRPP.size(); i++) {
        RPP.push_back(tmpRPP[i]);
    }
    tmpRPP.clear();

    std::sort(RPP.begin(), RPP.end());
    RPP.erase(std::unique(RPP.begin(), RPP.end()), RPP.end());

    std::cout << "regiones para propagar: " << RPP.size() << " == ";
    if (RPP.size() == 0) {
        std::cout << "no se colapsaron nuevas regiones " << std::endl;
        return false;
    }
    else {
        for (int i = 0; i < RPP.size(); i++) {
            std::cout << RPP[i] << " ";
        }
        std::cout << std::endl;
    }
    return true;
}
//funcion para crear un mapa con los valores de los patrones candidatos para la propagacion
void reduceMap(const std::vector<Pattern>& pattern, std::vector<std::vector<int>>& unCollapseMap, const int Y, int pos) {
    for (int i = 0; i < pattern.size(); i++) {
        for (int z = 0; z < pattern[i].coordinate.size(); z++) {
            unCollapseMap[pattern[i].coordinate[z]].push_back(pattern[i].pixelesCoo[z]);
        }
    }
}
//funcion para ver y guardar que patrones son compatibles alrededor de los posibles candidatos
void propagate(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, std::vector<Pattern>& patternArray, int pos, const int Y) {

    //distancia de N + 1 para buscar posibles candidatos
    std::vector<int> posiblePos = RPP;
    std::vector<Pattern> propagationPattern;
    std::vector<std::vector<int>> tmpUnCollapseMap;
    int resize = Y * Y;
    tmpUnCollapseMap.resize(resize);
    auto newPattern = patternArray.front();
    std::cout << "analizar patrones de RPP para propagacion" << std::endl;
    for (int i = 0; i < posiblePos.size(); i++) {
        propagationPattern.clear();
        //std::cout << "RPP: " << posiblePos[i] << " /";
        for (int j = 0; j < patternArray.size(); j++) {
            if (selectPattern(patternArray[j], patternArray, unCollapseMap, patternArray[j].N, Y, posiblePos[i], false)) {
                propagationPattern.push_back(patternArray[j]);
            }
        }
        //std::cout << "patrones compatibles obtenidos: " << propagationPattern.size() << std::endl;
        if (propagationPattern.size() == 0) {
            std::cout << "eliminar posicion sin patrones:" << posiblePos[i] <<std::endl;
            auto it = std::find(RPP.begin(),RPP.end(), posiblePos[i]);
            // Verificar si se encontró el elemento
            if (it != RPP.end()) {
                RPP.erase(RPP.begin() + i);
                std::cout << "eliminacion exitosa" << std::endl;
            }
        }else {
            std::cout << "reducir mapa" << std::endl;
            reduceMap(propagationPattern, tmpUnCollapseMap, Y, posiblePos[i]);
        }
    }
    for (auto& e : tmpUnCollapseMap) {
        std::sort(e.begin(), e.end());
        auto it = std::unique(e.begin(), e.end());
        e.erase(it, e.end());
    }
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (tmpUnCollapseMap[i].size() > 1 && unCollapseMap[i].size() > 1) {
            unCollapseMap[i] = tmpUnCollapseMap[i];
        }
    }
    posiblePos.clear();
    propagationPattern.clear();
    tmpUnCollapseMap.clear();
}
//funcion para reconstruir una imagen a partir del mapa generado
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        pixelVectorSalida.push_back(tiles[unCollapseMap[i].front()]);
    }
}
//verifica si todas las posiciones del mapa estan colapsadas
bool mapCompleted(const std::vector<std::vector<int>>& unCollapseMap) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}
//creación de una imagen con un mosaico de los patrones
void createPatternDraw(const std::vector<Pattern>& pattern, std::vector<Pixel>& pixelVector, int& Y) {
    int lenght = pattern.front().N;
    int wAmount = lenght * 10 + 10;

    Pixel pixelNegro = { 0,0,0 };

    int Width = 0;

    do {
        Width++;
    } while (Width * Width < pattern.size());

    int z = 0;
    Y = lenght * Width + Width;
    do {
        for (int x = 0; x < lenght; x++) {
            for (int j = 0; j < Width; j++) {
                for (int i = 0; i < lenght; i++) {
                    if ((z * Width + j) < pattern.size()) {
                        pixelVector.push_back(pattern[z * Width + j].pixeles[x * lenght + i]);
                    }
                    else {
                        pixelVector.push_back(pixelNegro);
                    }
                }
                pixelVector.push_back(pixelNegro);
            }
        }
        for (int i = 0; i < lenght * Width + Width; i++) {
            pixelVector.push_back(pixelNegro);
        }
        z++;
    } while (Width > z);


}
void ControlPoint(int i) {
    std::cout << "Control point " << i << std::endl;
}
int main(int argc, char* argv[]) {
    std::vector<int> N;
    std::string imageName;
    std::string mode;
    int Y;

    po::options_description desc("Opciones permitidas");
    desc.add_options()
        ("help", "Mostrar información de ayuda")
        ("mode", po::value<std::string>(&mode), "Ingresar modo del algoritmo: WFC, MWFC, HWFC")
        ("pattern", po::value<std::vector<int>>(&N)->multitoken(), "Ingresar tamano de patrones a usar como valores enteros")
        ("image", po::value<std::string>(&imageName), "Ingresar nombre del archivo en formato .PPM")
        ("size", po::value<int>(&Y), "Ingresar tamano de la imagen de salida");
        try {
            po::variables_map vm;
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);

            if (vm.count("help")) {
                std::cout << desc << std::endl;
                return 1;
            }

            if (vm.count("mode")) {
                std::cout << "Variante del algoritmo a utilizar: " << mode << std::endl;
            }
            else {
                std::cout << "modo no valido." << std::endl;
            }

            if (vm.count("pattern")) {
                std::cout << "Usar patrones de: ";
                for (int val : N) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }
            else {
                std::cout << "Valores no ingresados." << std::endl;
            }

            if (vm.count("image")) {
                std::cout << "Valor de cadena ingresado: " << imageName << std::endl;
            }
            else {
                std::cout << "Valor de cadena no ingresado." << std::endl;
            }

            if (vm.count("size")) {
                std::cout <<"tamano imagen de salida: " << Y << std::endl;
            }
            else {
                std::cout << "Valor de tamano no ingresado." << std::endl;
            }
        }catch (const boost::program_options::error& e) {
            std::cerr << "Error al analizar las opciones: " << e.what() << std::endl;
            return 1;
        }

    initializeRandomSeed();
    std::string filename = "./" + imageName, PPM_Identifier;
    std::ifstream file(filename, std::ios::binary);
    int inputImageWidth, inputImageHeight;
    
    std::vector<Pixel> pixelVector, pixelVectorSalida, patterVectorSalida;
    //lectura de la imagen de entrada
    if (readImagePPM(imageName, inputImageWidth, inputImageHeight, pixelVector)) {
        std::cout << "Imagen PPM leída exitosamente." << std::endl;
    }
    //inicio de cronometro
    auto start_time = std::chrono::high_resolution_clock::now();

    //defincion de las casillas
    std::vector<Pixel> PosibleTiles;
    defineTiles(pixelVector, PosibleTiles);

    //defincion de los patrones posibles que puede adoptar el mapa
    std::vector<Pattern> patternArray;
    definePatterns(patternArray, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, N, mode);


    //mapa de superposiciones posibles
    std::vector<std::vector<int>> unCollapseMap;
    std::vector<int> RPP;
    RPP.resize(0);

    initializePosMap(unCollapseMap, PosibleTiles, Y);
    int lowestEntropyTilePos;
    //inicio del algoritmo WFC
    bool randomStart = true;

    //almacen de información para backtracking
    std::vector<int> BT_pos;
    std::vector<std::vector<std::vector<int>>> BT_cMap;
    std::vector<std::vector<int>> BT_RPP;
    int step = 0, toStep = 0, lastStep = 0, backtrackUses = 0;
    bool backtrackingRequested = true;
    Pattern lastSelectedPattern(0, 0);

    int controlPointN = 0;
    while (!mapCompleted(unCollapseMap)) {
        std::cout << PURPLE << "step: " << step << RESET << std::endl;
        //seleccionar una casilla para colpasar
        std::cout << GREEN << "SELECCIONAR " << RESET << std::endl;

        if (randomStart) {
            lowestEntropyTilePos = getRandom(0, unCollapseMap.size());
            randomStart = false;
            std::cout << YELLOW << "Posicion aleatoria: " << lowestEntropyTilePos << RESET << std::endl;
        }
        else {
            lowestEntropyTilePos = selectLowestEntropyTile(unCollapseMap, PosibleTiles.size(), lowestEntropyTilePos, RPP);
            std::cout << YELLOW << "Posicion con la entropia más baja: " << lowestEntropyTilePos << RESET << std::endl;
        }

        std::cout << GREEN << "COLAPSAR" << RESET << std::endl;
        if (Collapse(unCollapseMap, RPP, Y, patternArray, lastSelectedPattern, lowestEntropyTilePos, PosibleTiles.size())) {
            printMap(unCollapseMap, Y, PosibleTiles.size());

            std::cout << " " << std::endl;
            std::cout << GREEN << "PROPAGAR" << RESET << std::endl;
            propagate(unCollapseMap, RPP, patternArray, lowestEntropyTilePos, Y);
            printMap(unCollapseMap, Y, PosibleTiles.size());

            if (RPP.size() > 0) {
                std::cout << "Guardar nuevo estado de backtracking" << std::endl;
                BT_cMap.push_back(unCollapseMap);
                std::cout << "Guardar nuevo estado de backtracking" << std::endl;
                BT_pos.push_back(lowestEntropyTilePos);
                std::cout << "Guardar nuevo estado de backtracking" << std::endl;
                BT_RPP.push_back(RPP);
                std::cout << "Guardado completado" << std::endl;
                step++;

                std::cout << "Posiciones restantes: ";
                for (int i = 0; i < RPP.size(); i++) {
                    std::cout << RPP[i] << " ";
                }
                std::cout << std::endl;
                backtrackingRequested = false;
            }
        }
        if (RPP.size() == 0 && !mapCompleted(unCollapseMap)) {
            //añadir un do while que vaya eliminando las posiciones en el backtracking anterior al usado antes de reemplazarlo 
            std::cout << YELLOW << "BACKTRACKING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
            int posToDelete;
            
            do {
                step = BT_cMap.size() - 1;
                unCollapseMap = BT_cMap.back();
                RPP = BT_RPP.back();
                if (!backtrackingRequested){
                    posToDelete = lowestEntropyTilePos;
                }
                auto i = std::find(RPP.begin(), RPP.end(), posToDelete);
                std::cout << "elemento a eliminar " << posToDelete << ": ";
                for (int j = 0; j < RPP.size(); j++) {
                    std::cout << RPP[j] << " ";
                }
                std::cout << std::endl;

                if (i != RPP.end()) {
                    RPP.erase(i);
                }
                else {
                    std::cout << "elemento no encontrado " << std::endl;
                }

                posToDelete = BT_pos.back();
                BT_cMap.pop_back();
                BT_pos.pop_back();
                BT_RPP.pop_back();

                backtrackUses++;
                backtrackingRequested = true;
            } while (RPP.empty());

            //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            
        }
        else {
            std::cout << YELLOW << "reintentar con otro punto" << RESET << std::endl;
        }

        std::cout << "posibles regiones de propagacion restantes: " << RPP.size() << std::endl;
        std::cout << "usos del backtracking: " << backtrackUses << std::endl;
        std::cout << PURPLE << "punto de iteracion" << RESET << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << GREEN << "MAPA COMPLETADO EXITOSAMENTE" << RESET << std::endl;
    printMap(unCollapseMap, Y, PosibleTiles.size());
    std::cout << "posible pixel color: " << PosibleTiles.size() << std::endl;
    std::cout << "usos del backtracking: " << backtrackUses << std::endl;
    //construccion de una nueva imagen
    reconstructMap(pixelVectorSalida, unCollapseMap, PosibleTiles);
    std::cout << "Mapa reconstruido exitosamente." << std::endl;
    //guardado de la imagen en un nuevo archivo
    if (writeImagePPM("imagen_Generada.ppm", Y, Y, pixelVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    //dibujar los patrones en una imagen aparte
    createPatternDraw(patternArray, patterVectorSalida, Y);
    if (writeImagePPM("patron_Generada.ppm", Y, Y, patterVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    //fin del cronometro del tiempo de ejecucion
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Tiempo de ejecución: " << duration << " ms" << std::endl;

    return 0;
}
