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

class Pattern {
public:
    int id;
    int size;
    int weight;
    bool pattern = true;
    std::vector<Pixel> pixeles;
    //constructor
    Pattern(int id, int size) : id(id), size(size) {}
    bool comparePattern2(const std::vector<Pixel>& otherPixeles) {

        if (otherPixeles.size() != pixeles.size()) {
            std::cout << "Error, tamaño de tiles no compatible" << std::endl;
            return false;
        }

        return otherPixeles == pixeles;
    }
    void addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
};
void initializeRandomSeed() {
    std::srand(std::time(0));
}
int getRandom(int min, int max) {
    int rnd = 0;
    rnd = std::rand() % max;
    return rnd;
}
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

    pixeles.resize(w * h);
    archivo.read(reinterpret_cast<char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return true;
}
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
void llenarVectorUnico(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
}
//definir cuantas posibles estados puede tomar un pixel y cuantas veces aparece
void defineTiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    llenarVectorUnico(pixelVector, posibleTiles );
}
//definir cuantas combinaciones de N*N pueden pueden adoptar los pixeles
bool compararPorTamano(const Pattern& a, const Pattern& b){
    return a.weight > b.weight;
}
void printCollapsedTile(const std::vector<std::vector<int>>& unCollapseMap, int size) {
    for (int i = -1; i < size; i++) {
        if (i == -1) {
            std::cout << "X" << " " << "|| ";
            for (int z = 0; z < 10; z++) {
                std::cout << z << " " << "|| ";
            }
        }
        else {
            for (int j = -1; j < size; j++) {
                if (j == -1) {
                    std::cout << i << " || ";
                }
                else if (unCollapseMap[j + i * size].size() == 1) {
                    std::cout << unCollapseMap[j + i * size].front() << "*" << "|| ";
                }
                else if (unCollapseMap[j + i * size].size() < 5) {
                    std::cout << unCollapseMap[j + i * size].size() << " " << "|| ";
                }
                else
                    std::cout << "X" << " || ";
            }
        }
        
        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
void verifyPos(int& pos, const int Y, const int N) {
    if (pos % Y > Y - N) {
        pos -= (N - (Y - pos % Y));
    }
    if (pos / Y > Y - N) {
        pos -= (Y * (N - (Y - pos / Y)));
    }
}
void definePatterns(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const int inputImageHeight, const int inputImageWidth, int N){
    std::vector<Pixel> tmpVector;
    //seperacion de la imagen en multiples patrones
    for (int y = 0; y <= inputImageHeight - N; y++)
        for (int x = 0; x <= inputImageWidth - N; x++) {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    tmpVector.push_back(pixelVector[(x + j) + (y + i) * N]);
            Pattern newPattern(pattArray.size(), N);
            newPattern.addPixelVector(tmpVector);
            pattArray.push_back(newPattern);
            tmpVector.clear();
        }

    int weight = 0;
    for (int i = 0; i < pattArray.size(); i++) {
        if (pattArray[i].pattern) {
            weight++;
            for (int j = i + 1; j < pattArray.size(); j++) {
                if (pattArray[j].pattern && pattArray[i].comparePattern2(pattArray[j].pixeles)) {
                    pattArray[j].pattern = false;
                    weight++;
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

    std::sort(pattArray.begin(), pattArray.end(), compararPorTamano);

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
}
//inicializar el mapa de coordenadas con la cantidad de posibles formas que tienen los pixeles, representadas en integer
void initializePosMap(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& posibleTiles, int Y) {
    Y = Y * Y;
    unCollapseMap.resize(Y);
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < posibleTiles.size(); j++) {
            unCollapseMap[i].push_back(j);
        }
    }
}
//falta implementar los pesos
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int size) {
    int lowestValue = size, lowestID = -1;
    for (int i = 0; i < unCollapseMap.size(); i++) {
        //si es que hay más de una posibilidad para la casilla
        if (unCollapseMap[i].size() > 1) {
            if (lowestValue > unCollapseMap[i].size()) {
                lowestID = i;
                lowestValue = unCollapseMap[i].size();
            }
        }
    }
    if (lowestID == -1) {
        //std::cout << "selección aleatoria con pesos----------------------------------------------------" << std::endl;
        lowestID = getRandom(0, unCollapseMap.size());
    }
    return lowestID;
}
//se selecciona un patron
bool selectPattern(const Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, const std::vector<Pixel>& tiles, int pos) {
    int x;
    std::vector<Pixel> tmpVector;
    verifyPos(pos, Y, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int z = 0; z < unCollapseMap[pos + j + Y * i].size(); z++) {
                /*
                if (pattern.pixeles[j + i * N] == tiles[unCollapseMap[pos + j + Y * i][z]]) {
                    tmpVector.push_back(tiles[unCollapseMap[pos + j + Y * i][z]]);
                    break;
                }
                */
                
            }
        }
    }
    if (tmpVector == pattern.pixeles) {
        tmpVector.clear();
        return true;
    }
    else return false;
}
void reduceCollapseMap(const std::vector<Pattern>& pattern, std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, const std::vector<Pixel>& tiles, int pos) {
    verifyPos(pos, Y, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            unCollapseMap[pos + j + i * Y].clear();
            for (int z = 0; z < pattern.size(); z++) {
                auto e = std::find(tiles.begin(), tiles.end(), pattern[z].pixeles[j + N * i]);
                unCollapseMap[pos + j + Y * i].push_back(std::distance(tiles.begin(), e));
            }
        }
    }
    //std::cout << "posible reduced Map" << std::endl;
    //printCollapsedTile(unCollapseMap, Y);
}
void Collapse(std::vector<std::vector<int>>& unCollapseMap, const int Y, const std::vector<Pattern>& pattern, const std::vector<Pixel>& tiles, int pos) {
    int N = pattern.front().size;
    auto newPattern = pattern.front();
    verifyPos(pos,Y,N);
    int count = 0;
    do {
        count++;
        newPattern = pattern[getRandomPatternWeighted(pattern)];
        //newPattern = pattern[getRandom(0, pattern.size())];
    } while (!selectPattern(newPattern, unCollapseMap, N, Y, tiles, pos) && count < 10);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            unCollapseMap[pos + j + Y * i].clear();
            auto e = std::find(tiles.begin(), tiles.end(), newPattern.pixeles[j + N * i]);
            unCollapseMap[pos + j + Y * i].push_back(std::distance(tiles.begin(),e));
        }
    }
}
void getMapPropagationPos(std::vector<int>& posiblePos, const std::vector<std::vector<int>>& unCollapseMap,const int N, const int Y) {
    int distance = 1;
    posiblePos.clear();
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < Y; j++) {
            if (unCollapseMap[j + i * Y].size() == 1) {
                
                //norte
                if (((j + i * Y) - (Y * distance)) >= 0 && unCollapseMap[(j + i * Y) - (Y * distance)].size() > 1) {
                    posiblePos.push_back((j + i * Y) - (Y * distance));
                }
                //sur
                if (((j + i * Y) + Y* distance) < unCollapseMap.size() && unCollapseMap[(j + i * Y) + (Y * distance)].size() > 1) {
                    posiblePos.push_back((j + i * Y) + (Y * distance));
                }
                //este
                if ( j + (distance) < Y && unCollapseMap[j + (distance) + i * Y].size() > 1) {
                    posiblePos.push_back(j + (distance) + i * Y);
                }
                //oeste
                if (j - (distance) >= 0 && unCollapseMap[j - (distance) + i * Y].size() > 1) {
                    posiblePos.push_back(j - (distance) + i * Y);
                }
            }
        }
    }
    std::sort(posiblePos.begin(), posiblePos.end());
    posiblePos.erase(std::unique(posiblePos.begin(), posiblePos.end()), posiblePos.end());

    std::cout << "posible pos" << std::endl;
    for (int i = 0; i < posiblePos.size(); i++) {
        std::cout << posiblePos[i] << " ";
    }
    std::cout << std::endl;
}
void propagate(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pattern>& pattern, const std::vector<Pixel>& tiles, int pos, const int N, const int Y) {

    //distancia de N + 1 para buscar posibles candidatos
    std::vector<int> posiblePos;
    getMapPropagationPos(posiblePos,unCollapseMap,N,Y);
    std::vector<Pattern> propagationPattern;
    std::vector<std::vector<int>> tmpUnCollapseMap = unCollapseMap;
    auto newPattern = pattern.front();

    for (int i = 0; i < posiblePos.size(); i++) {
        propagationPattern.clear();
        for (int j = 0; j < pattern.size(); j++) {
            if (selectPattern(pattern[j], unCollapseMap, N, Y, tiles, posiblePos[i])) {
                propagationPattern.push_back(pattern[j]);
            }
        }
        if(propagationPattern.size() > 0)
            reduceCollapseMap(propagationPattern, tmpUnCollapseMap, N, Y, tiles, posiblePos[i]);
    } 

    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() > 1 && tmpUnCollapseMap[i].size() > 0) {
            unCollapseMap[i] = tmpUnCollapseMap[i];
        }
    }
}
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        pixelVectorSalida.push_back(tiles[unCollapseMap[i].front()]);
    }
}
bool mapCompleted(const std::vector<std::vector<int>>& unCollapseMap) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    //1 image.ppm
    initializeRandomSeed();
    std::string imageName = argv[1];
    std::string filename = "./" + imageName, PPM_Identifier;
    std::ifstream file(filename, std::ios::binary);
    int inputImageWidth, inputImageHeight;
    int N = std::atoi(argv[2]);
    int Y = std::atoi(argv[3]);
    std::vector<Pixel> pixelVector, pixelVectorSalida;
    if (readImagePPM(imageName, inputImageWidth, inputImageHeight, pixelVector)) {
        std::cout << "Imagen PPM leída exitosamente." << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    //defincion de las casillas
    std::vector<Pixel> PosibleTiles;
    defineTiles(pixelVector, PosibleTiles);

    //defincion de los patrones posibles que puede adoptar el mapa
    std::vector<Pattern> patternArray;
    definePatterns(patternArray, pixelVector, inputImageHeight, inputImageWidth, N);

    //mapa de superposiciones posibles
    std::vector<std::vector<int>> unCollapseMap;
    initializePosMap(unCollapseMap, PosibleTiles, Y);

    int lowestEntropyTilePos;
    //inicio del algoritmo WFC
    while (!mapCompleted(unCollapseMap)) {

        //seleccionar, buscar la casilla con la menor entropia posible
        lowestEntropyTilePos = selectLowestEntropyTile(unCollapseMap, PosibleTiles.size());
        std::cout << "posicion con la entropia más baja: " << lowestEntropyTilePos << std::endl;

        //seleccionar patron compatible y collapsar
        std::cout << "colapsar" << std::endl;
        Collapse(unCollapseMap, Y, patternArray, PosibleTiles, lowestEntropyTilePos);
        printCollapsedTile(unCollapseMap, Y);
        std::cout << "propagar" << std::endl;
        propagate(unCollapseMap, patternArray, PosibleTiles, lowestEntropyTilePos, PosibleTiles.size(), Y);
        printCollapsedTile(unCollapseMap, Y);

        break;
    }
    
    std::cout <<"posible pixel color: " << PosibleTiles.size() << std::endl;

    reconstructMap(pixelVectorSalida, unCollapseMap, PosibleTiles);

    if (writeImagePPM("imagen_Generada.ppm", Y, Y, pixelVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Tiempo de ejecución: " << duration << " ms" << std::endl;

    return 0;
}