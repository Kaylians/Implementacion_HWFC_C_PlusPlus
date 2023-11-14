#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
    std::vector<Pixel> pixeles;
    std::vector<Pixel> tmpPixeles;
    std::vector<int> id_N;
    std::vector<int> id_S;
    std::vector<int> id_E;
    std::vector<int> id_W;

    //constructor
    Tile(int id, int size) : id(id), size(size) {}

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

void initializeRandomSeed() {
    std::srand(std::time(0));
}
int getRandom(int min, int max) {
    std::cout << "numero aleatorio entre: " << min << " " << max << std::endl;
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

std::vector<std::vector<int>> propagateTile(std::vector<std::vector<int>> unCollapseMap, std::vector<Tile> tilesArray, int pos) {
    int max = sqrt(tilesArray.size());
    int id = unCollapseMap[pos].front();
    //std::cout << "Arriba" << std::endl;
    //Arriba
    if (pos - max > 0 ) 
        if (unCollapseMap[pos - max].size() > 1) {
            unCollapseMap[pos - max].clear();
            unCollapseMap[pos - max].resize(tilesArray[id].id_N.size());
            unCollapseMap[pos - max] = tilesArray[id].id_N;
        }
    //Abajo
    //std::cout << "Abajo" << std::endl; 
    if (pos + max < unCollapseMap.size())
        if (unCollapseMap[pos + max].size() > 1) {
            unCollapseMap[pos + max].clear();
            unCollapseMap[pos + max].resize(tilesArray[id].id_S.size());
            unCollapseMap[pos + max] = tilesArray[id].id_S;
        }
    //Derecha
    //std::cout << "Derecha" << std::endl;
    if ((pos % max) > 0)
        if (unCollapseMap[pos - 1].size() > 1) {
            unCollapseMap[pos - 1].clear();
            unCollapseMap[pos - 1].resize(tilesArray[id].id_W.size());
            unCollapseMap[pos - 1] = tilesArray[id].id_W;
        }
    //Izquierda
    //std::cout << "Izquierda" << std::endl;
    if ((pos % max) != max - 1) 
        if (unCollapseMap[pos + 1].size() > 1) {
            unCollapseMap[pos + 1].clear();
            unCollapseMap[pos + 1].resize(tilesArray[id].id_E.size());
            unCollapseMap[pos + 1] = tilesArray[id].id_E;
        }
    return unCollapseMap;
}

std::vector<std::vector<int>> collapseTile(std::vector<std::vector<int>> unCollapseMap, std::vector<Tile> tilesArray,int id) {
    int selectedID;
    if (unCollapseMap[id].size() == tilesArray.size())
        selectedID = id;
    else
        selectedID = unCollapseMap[id][getRandom(0, unCollapseMap[id].size())];
    std::cout << "opciones para colapsar: ";
    for (int i = 0; i < unCollapseMap[id].size(); i++) {
        std::cout << unCollapseMap[id][i] << " ";
    }
    std::cout << "Selected: "<< selectedID  << std::endl;

    unCollapseMap[id].clear();
    unCollapseMap[id].push_back(selectedID);

    return unCollapseMap;
}

void printCollapsedTile(std::vector<std::vector<int>> unCollapseMap, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (unCollapseMap[j + i * size].size() == 1) {
                std::cout << unCollapseMap[j + i * size].front() << "*" << "|| ";
            }
            else if (unCollapseMap[j + i * size].size() < size*size) {
                std::cout << unCollapseMap[j + i * size].size() << " " << "|| ";
            }
            else
                std::cout << "X" << " || ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
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
        std::cout << "selección aleatoria----------------------------------------------------" << std::endl;
        lowestID = getRandom(0, unCollapseMap.size());
    }
    return lowestID;
}

int main(int argc, char* argv[]) {
    //1 image.ppm
    initializeRandomSeed();
    std::string imageName = argv[1];
    std::string filename = "./" + imageName, PPM_Identifier;
    std::ifstream file(filename, std::ios::binary);

    int inputImageWidth, inputImageHeight, pixelMaxValue;
    int size = std::atoi(argv[2]);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }
    //guardado de tipo de archivo, ancho, alto y valor maximo de rgb
    file >> PPM_Identifier >> inputImageWidth >> inputImageHeight >> pixelMaxValue;

    if (PPM_Identifier != "P6") {
        std::cerr << "El archivo no es un archivo PPM válido." << std::endl;
        return 1;
        //covertir el archivo antes de proceder
    }
    //guardado de la información en un vector y cierre de archivo
    std::vector<Pixel> pixelVector(inputImageWidth * inputImageHeight);
    std::vector<Pixel> pixelVectorSalida;
    file.read(reinterpret_cast<char*>(pixelVector.data()), sizeof(Pixel) * pixelVector.size());
    file.close();

    //division y guardado del vector original en vectores que actuaran como casillas
    std::vector<Pixel> tmpVector;
    std::vector<Tile> tilesArray;
    for (int y = 0; y < inputImageHeight / size; y++) {
        for (int x = 0, b = 0; x < inputImageWidth / size; x++, b++) {
            if (b > inputImageWidth / size) {
                b = 0;
            }
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    tmpVector.push_back(pixelVector[j + (inputImageWidth * i) + (b * size) + (y * inputImageHeight * size)]);
                }
            }
            //guardado de una nueva casilla
            Tile newTile(tilesArray.size(), size);
            newTile.addPixelVector(tmpVector);
            tilesArray.push_back(newTile);
            tmpVector.clear();
        }
    }

    //definición de vecinos dentro de una matriz, -1 indica "sin vecino"
    //modificar con restriccion
    int max = sqrt(tilesArray.size());
    for (int i = 0; i < max; i++) {
        for (int j = 0; j < max; j++) {
            //guardado de si mismo como vecino potencial
            tilesArray[j + i * max].id_W.push_back(tilesArray[j + i * max].id);
            tilesArray[j + i * max].id_E.push_back(tilesArray[j + i * max].id);
            tilesArray[j + i * max].id_N.push_back(tilesArray[j + i * max].id);
            tilesArray[j + i * max].id_S.push_back(tilesArray[j + i * max].id);

            //Izquierda
            if (j > 0) {
                tilesArray[j + i * max].id_W.push_back(tilesArray[(j - 1) + i * max].id);
            }

            //Derecha
            if (j < max - 1) {
                tilesArray[j + i * max].id_E.push_back(tilesArray[(j + 1) + i * max].id);
            }
            
            //Arriba
            if (i > 0) {
                tilesArray[j + i * max].id_N.push_back(tilesArray[j + (i - 1) * max].id);
            }
            
            //Abajo
            if (i < max - 1) {
                tilesArray[j + i * max].id_S.push_back(tilesArray[j + (i + 1) * max].id);
            }
        }
    }
    std::vector<std::vector<int>> unCollapseMap;
    unCollapseMap.resize(tilesArray.size());
    

    for (int i = 0; i < tilesArray.size(); i++) {
        for (int j = 0; j < tilesArray.size(); j++) {
            unCollapseMap[i].push_back(j);
        }
    }
    //implementacion basica de WFC
    int lowestEntropyTile;
    while (!mapCompleted (unCollapseMap)) {
        
        //seleccionar, buscar la casilla con la menor entropia posible
        lowestEntropyTile = selectLowestEntropyTile(unCollapseMap);
        std::cout << "seleccionar : " << lowestEntropyTile  << std::endl;
       
        //colapsar
        std::cout << "Colapsar" << std::endl;
        unCollapseMap = collapseTile(unCollapseMap, tilesArray, lowestEntropyTile);
        printCollapsedTile(unCollapseMap, inputImageWidth / size);

        //propagar
        std::cout << "propagar" << std::endl;
        unCollapseMap = propagateTile(unCollapseMap, tilesArray, lowestEntropyTile);
        printCollapsedTile(unCollapseMap, inputImageWidth / size);
    }

    std::vector<Tile> newTilesArray;
    for (int i = 0, pos = 0; i < tilesArray.size(); i++) {
        pos = unCollapseMap[i].front();
        newTilesArray.push_back(tilesArray[pos]);
    }

    //reconstrucción de la imagen en el vector de salida
    for (int y = 0; y < inputImageHeight / size; y++) {
        for (int x = 0; x < size; x++) {
            for (int i = 0; i < inputImageWidth / size; i++) {
                for (int j = 0; j < size; j++) {//i+y j+x
                    pixelVectorSalida.push_back(newTilesArray[i + (y * (inputImageHeight/size))].pixeles[j + (x * size)]);
                }
            }
        }
    }

    // Guardado del vector en una nueva imagen modificada
    std::ofstream output_file("./image_modificada.ppm", std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return 1;
    }
    output_file << "P6\n" << inputImageWidth << " " << inputImageHeight << "\n" << pixelMaxValue << "\n";
    output_file.write(reinterpret_cast<char*>(pixelVectorSalida.data()), sizeof(Pixel) * pixelVectorSalida.size());
    output_file.close();
    
    return 0;
}
