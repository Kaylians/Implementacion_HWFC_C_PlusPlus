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

class Tile {
public:
    int id;
    int size;
    bool pattern = true;
    int idPattern = id;

    std::vector<Pixel> pixeles;
    std::vector<Pixel> tmpPixeles;

    std::vector<int> id_N;
    std::vector<int> id_S;
    std::vector<int> id_E;
    std::vector<int> id_W;

    //constructor
    Tile(int id, int size) : id(id), size(size) {}
    bool comparePattern2(const std::vector<Pixel>& tile1) {

        if (tile1.size() != pixeles.size()) {
            std::cout << "Error, tamaño de tiles no compatible" << std::endl;
            return false;
        }

        return tile1 == pixeles;
    }

    void addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
    void rotateToRigth() {
        for (int i = 0; i < size; i++) {
            for (int j = size - 1; j >= 0; j--) {
                tmpPixeles.push_back(pixeles[i + j * size]);
            }
        }
        pixeles = tmpPixeles;
    }
    void rotateToLeft() {
        for (int i = size - 1; i >= 0; i--) {
            for (int j = 0; j < size; j++) {
                tmpPixeles.push_back(pixeles[i + j * size]);
            }
        }
        pixeles = tmpPixeles;
    }
};

bool comparePattern(const std::vector<Pixel>& tile1, const std::vector<Pixel>& tile2) {

    if (tile1.size() != tile2.size()) {
        std::cout << "Error, tamaño de tiles no compatible" << std::endl;
        return false;
    }

    return tile1 == tile2;
}

void initializeRandomSeed() {
    std::srand(std::time(0));
}
int getRandom(int min, int max) {
    //std::cout << "numero aleatorio entre: " << min << " " << max << std::endl;
    //static std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    //std::uniform_int_distribution<int> distr(min, max);
    //return distr(rnd);
    int rnd = 0;
    rnd = std::rand() % max;
    return rnd;
}
bool mapCompleted(std::vector<std::vector<int>> unCollapseMap) {

    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}

void propagateTile(std::vector<std::vector<int>>& unCollapseMap, std::vector<Tile> tilesArray, int pos) {
    int max = sqrt(unCollapseMap.size());
    int id = unCollapseMap[pos].front(), patternID;

    if (tilesArray[id].pattern)
        patternID = tilesArray[id].id;
    else
        patternID = tilesArray[id].idPattern;

    id = patternID;

    std::cout << "Arriba " << tilesArray[id].id_N.size() << std::endl;
    //Arriba
    if (pos - max > 0 ) 
        if (unCollapseMap[pos - max].size() > 1) {
            unCollapseMap[pos - max].clear();
            unCollapseMap[pos - max].resize(tilesArray[id].id_N.size());
            unCollapseMap[pos - max] = tilesArray[id].id_N;
        }
    //Abajo
    std::cout << "Abajo " << tilesArray[id].id_S.size() << std::endl;
    if (pos + max < unCollapseMap.size())
        if (unCollapseMap[pos + max].size() > 1) {
            unCollapseMap[pos + max].clear();
            unCollapseMap[pos + max].resize(tilesArray[id].id_S.size());
            unCollapseMap[pos + max] = tilesArray[id].id_S;
        }
    //Derecha
    std::cout << "Izquierda " << tilesArray[id].id_W.size() << std::endl;
    if ((pos % max) > 0)
        if (unCollapseMap[pos - 1].size() > 1) {
            unCollapseMap[pos - 1].clear();
            unCollapseMap[pos - 1].resize(tilesArray[id].id_W.size());
            unCollapseMap[pos - 1] = tilesArray[id].id_W;
        }
    //Izquierda
    std::cout << "Derecha " << tilesArray[id].id_E.size() << std::endl;
    if ((pos % max) != max - 1) 
        if (unCollapseMap[pos + 1].size() > 1) {
            unCollapseMap[pos + 1].clear();
            unCollapseMap[pos + 1].resize(tilesArray[id].id_E.size());
            unCollapseMap[pos + 1] = tilesArray[id].id_E;
        }
}

void collapseTile(std::vector<std::vector<int>>& unCollapseMap, std::vector<Tile> tilesArray, int tilePos) {
    int selectedID, pos;
    
    do {
        if (selectedID == -1) {
            if (unCollapseMap[tilePos].size() > 1) {
                unCollapseMap[tilePos].erase(unCollapseMap[tilePos].begin() + pos);
            }
            else
                break;
        }
        pos = getRandom(0, unCollapseMap[tilePos].size());
        selectedID = unCollapseMap[tilePos][pos];


    } while (selectedID == -1);

    if (selectedID == -1)
        selectedID = 0;  
    unCollapseMap[tilePos].clear();
    unCollapseMap[tilePos].push_back(selectedID);
}

int selectLowestEntropyTile(std::vector<std::vector<int>> unCollapseMap) {
    int lowestValue = unCollapseMap.size();
    int lowestID = -1;

    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() > 1) {
            if (lowestValue > unCollapseMap[i].size()) {
                lowestID = i;
                lowestValue = unCollapseMap[i].size();
            }
        }
    }
    if (lowestID == -1) {
        //std::cout << "selección aleatoria----------------------------------------------------" << std::endl;
        lowestID = getRandom(0, unCollapseMap.size());
    }
    return lowestID;
}
void printCollapsedTile(std::vector<std::vector<int>> unCollapseMap, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (unCollapseMap[j + i * size].size() == 1) {
                std::cout << unCollapseMap[j + i * size].front() << "*" << "|| ";
            }
            else if (unCollapseMap[j + i * size].size() < size * size) {
                std::cout << unCollapseMap[j + i * size].size() << " " << "|| ";
            }
            else
                std::cout << "X" << " || ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
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

int main(int argc, char* argv[]) {
    //1 image.ppm
    initializeRandomSeed();
    std::string imageName = argv[1];
    std::string filename = "./" + imageName, PPM_Identifier;
    std::ifstream file(filename, std::ios::binary);
    int inputImageWidth, inputImageHeight;
    int tileSize = std::atoi(argv[2]);
    std::vector<Pixel> pixelVector;
    if (readImagePPM(imageName, inputImageWidth, inputImageHeight, pixelVector)) {
        std::cout << "Imagen PPM leída exitosamente." << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    //division y guardado del vector original en vectores que actuaran como casillas
    std::vector<Pixel> tmpVector;
    std::vector<Tile> tilesArray;
    for (int y = 0; y < inputImageHeight / tileSize; y++) {
        for (int x = 0, b = 0; x < inputImageWidth / tileSize; x++, b++) {
            if (b > inputImageWidth / tileSize) {
                b = 0;
            }
            for (int i = 0; i < tileSize; i++) {
                for (int j = 0; j < tileSize; j++) {
                    tmpVector.push_back(pixelVector[j + (inputImageWidth * i) + (b * tileSize) + (y * inputImageHeight * tileSize)]);
                }
            }
            //guardado de una nueva casilla
            Tile newTile(tilesArray.size(), tileSize);
            newTile.addPixelVector(tmpVector);
            tilesArray.push_back(newTile);
            tmpVector.clear();
        }
    }

    //definición de vecinos dentro de una matriz, -1 indica "sin vecino"
    int max = sqrt(tilesArray.size());
    for (int i = 0; i < max; i++) {
        for (int j = 0; j < max; j++) {

            //Izquierda
            if (j > 0) {
                tilesArray[j + i * max].id_W.push_back(tilesArray[(j - 1) + i * max].id);
            }
            else tilesArray[j + i * max].id_W.push_back(-1);

            //Derecha
            if (j < max - 1) {
                tilesArray[j + i * max].id_E.push_back(tilesArray[(j + 1) + i * max].id);
            }
            else tilesArray[j + i * max].id_E.push_back(-1);
            //Arriba
            if (i > 0) {
                tilesArray[j + i * max].id_N.push_back(tilesArray[j + (i - 1) * max].id);
            }
            else tilesArray[j + i * max].id_N.push_back(-1);
            //Abajo
            if (i < max - 1) {
                tilesArray[j + i * max].id_S.push_back(tilesArray[j + (i + 1) * max].id);
            }
            else tilesArray[j + i * max].id_S.push_back(-1);
        }
    }

    int patternN = 0;
    for (int i = 0; i < tilesArray.size(); i++) {
        if (tilesArray[i].pattern) {
            patternN++;
            for (int j = i + 1; j < tilesArray.size(); j++) {
                if (tilesArray[j].pattern && tilesArray[i].comparePattern2(tilesArray[j].pixeles)) {
                    
                    auto iterador = std::find(tilesArray[i].id_E.begin(), tilesArray[i].id_E.end(), tilesArray[j].id_E.front());
                    if (iterador == tilesArray[i].id_E.end())
                        tilesArray[i].id_E.push_back(tilesArray[j].id_E.front());
                    
                    iterador = std::find(tilesArray[i].id_W.begin(), tilesArray[i].id_W.end(), tilesArray[j].id_W.front());
                    if (iterador == tilesArray[i].id_W.end())
                        tilesArray[i].id_W.push_back(tilesArray[j].id_W.front());
                    
                    iterador = std::find(tilesArray[i].id_S.begin(), tilesArray[i].id_S.end(), tilesArray[j].id_S.front());
                    if (iterador == tilesArray[i].id_S.end())
                        tilesArray[i].id_S.push_back(tilesArray[j].id_S.front());
                    
                    iterador = std::find(tilesArray[i].id_N.begin(), tilesArray[i].id_N.end(), tilesArray[j].id_N.front());
                    if (iterador == tilesArray[i].id_N.end())
                        tilesArray[i].id_N.push_back(tilesArray[j].id_N.front());

                    tilesArray[j].pattern = false;
                    tilesArray[j].idPattern = tilesArray[i].id;
                }
            }
        }
    }
    std::cout << "numero de patrones " << patternN << " de "<< tilesArray.size() << std::endl;
    std::vector<std::vector<int>> unCollapseMap;
    unCollapseMap.resize(tilesArray.size());

    for (int i = 0; i < tilesArray.size(); i++) {
        for (int j = 0; j < tilesArray.size(); j++) {
            unCollapseMap[i].push_back(j);
        }
    }
    int lowestEntropyTile;
    while (!mapCompleted(unCollapseMap)) {

        //seleccionar, buscar la casilla con la menor entropia posible
        lowestEntropyTile = selectLowestEntropyTile(unCollapseMap);
        std::cout << "seleccionar : " << lowestEntropyTile  << std::endl;

        //colapsar
        std::cout << "Colapsar" << std::endl;
        collapseTile(unCollapseMap, tilesArray, lowestEntropyTile);
        printCollapsedTile(unCollapseMap, inputImageWidth / tileSize);

        //propagar
        std::cout << "propagar" << std::endl;
        propagateTile(unCollapseMap, tilesArray, lowestEntropyTile);
        printCollapsedTile(unCollapseMap, inputImageWidth / tileSize);
    }
    std::cout << "mapa completado" << std::endl;

    std::vector<Tile> newTilesArray;
    for (int i = 0, pos = 0; i < tilesArray.size(); i++) {
        pos = unCollapseMap[i].front();
        newTilesArray.push_back(tilesArray[pos]);
    }

    //reconstrucción de la imagen en el vector de salida
    std::vector<Pixel> pixelVectorSalida;
    for (int y = 0; y < inputImageHeight / tileSize; y++) {
        for (int x = 0; x < tileSize; x++) {
            for (int i = 0; i < inputImageWidth / tileSize; i++) {
                for (int j = 0; j < tileSize; j++) {//i+y j+x
                    pixelVectorSalida.push_back(newTilesArray[i + (y * (inputImageHeight/tileSize))].pixeles[j + (x * tileSize)]);
                }
            }
        }
    }

    if (writeImagePPM("imagen_Generada.ppm", inputImageWidth, inputImageHeight, pixelVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Tiempo de ejecución: " << duration << " ms" << std::endl;

    return 0;
}
