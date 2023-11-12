#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <algorithm>
#include <random>
#include <cmath>

#include "WFC.h"
#include "Pixel.h"

class Tile {
public:
    int id;
    int size;
    std::vector<Pixel> pixeles;
    std::vector<int> idVecinos;

    //constructor
    Tile(int id, int size) : id(id), size(size) {}

    void addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
};

int main(int argc, char* argv[]) {
    //1 image.ppm

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
    int max = sqrt(tilesArray.size());
    for (int i = 0; i < max; i++) {
        for (int j = 0; j < max; j++) {
            if (j > 0) {//Izquierda
                tilesArray[j + i * max].idVecinos.push_back(tilesArray[(j-1) + i * max].id);
            }
            else tilesArray[j + i * max].idVecinos.push_back(-1);
            if (j < max - 1) {//Derecha
                tilesArray[j + i * max].idVecinos.push_back(tilesArray[(j+1) + i * max].id);
            }
            else tilesArray[j + i * max].idVecinos.push_back(-1);
            if (i > 0) {//Arriba
                tilesArray[j + i * max].idVecinos.push_back(tilesArray[j + (i-1) * max].id);
            }
            else tilesArray[j + i * max].idVecinos.push_back(-1);
            if (i < max -1 ) {//Abajo
                tilesArray[j + i * max].idVecinos.push_back(tilesArray[j + (i+1) * max].id);
            }
            else tilesArray[j + i * max].idVecinos.push_back(-1);
        }
    }

    
    //mezcla aleatorio de las casillas (prueba) 
    /*
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(tilesArray.begin(), tilesArray.end(), rng);
    */

    /*
    for (Pixel& pixel : pixelVector) {
        unsigned char promedio = (pixel.R + pixel.G + pixel.B) / 3;
        pixel.R = pixel.G = pixel.B = promedio;
    }

    */
    
    for (int y = 0; y < inputImageHeight / size; y++) {
        for (int x = 0; x < size; x++) {
            for (int i = 0; i < inputImageWidth / size; i++) {
                for (int j = 0; j < size; j++) {//i+y j+x
                    pixelVectorSalida.push_back(tilesArray[i + (y * (inputImageHeight/size))].pixeles[j + (x * size)]);
                }
            }
        }
    }

    // Guardar la imagen modificada
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
