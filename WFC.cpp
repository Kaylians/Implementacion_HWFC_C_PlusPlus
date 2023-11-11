#include <iostream>
#include <fstream>
#include <vector>

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

    //constructor
    Tile(int id, int size) : id(id), size(size) {}

    void addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
};

int main(int argc, char* argv[]) {
    //1 image.ppm

    int inputImageWidth, inputImageHeight, pixelMaxValue;

    std::string imageName = argv[1];

    std::string filename = "./" + imageName, PPM_Identifier;

    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

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

    //cambiarlo para ser pasado como argumento
    int size = 25;
    //int i = 0, j = 0;

    std::vector<Pixel> tmpVector;
    std::vector<Pixel> tmpVector2;
    std::vector<Tile> tilesArray;

    for (int y = 0; y < inputImageHeight / size; y++) {
        for (int x = 0, b = 0; x < inputImageWidth / size; x++, b++) {
            if (b > inputImageWidth / size) {
                b = 0;
            }
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    tmpVector.push_back(pixelVector[j + (inputImageWidth * i) + (b * size) + (y * inputImageHeight * size)]);
                    //std::cout << j << " + " << (inputImageWidth * i) << " + " << (b * size) << " + " << ( y * inputImageHeight * size); 
                    //std::cout << " = " << j + (inputImageWidth * i) + (b * size) + (y * inputImageHeight * size) << std::endl;
                }
            }
            std::cout << "control point --------------------------------" << std::endl;
            Tile newTile(0, 0);
            newTile.addPixelVector(tmpVector);
            tilesArray.push_back(newTile);
            tmpVector.clear();
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    std::cout << "Tiles Array Size: " << tilesArray.size() << std::endl;
    std::cout << "Tiles Array pixel Size: " << tilesArray[0].pixeles.size() << std::endl;
    std::shuffle(tilesArray.begin(), tilesArray.end(), rng);
    /*
    for (Pixel& pixel : pixelVector) {
        unsigned char promedio = (pixel.R + pixel.G + pixel.B) / 3;
        pixel.R = pixel.G = pixel.B = promedio;
    }

    */
    
    tilesArray.size();
    for (int y = 0; y < inputImageHeight / size; y++) {
        for (int x = 0; x < size; x++) {
            for (int i = 0; i < inputImageWidth / size; i++) {
                for (int j = 0; j < size; j++) {//i+y j+x
                    pixelVectorSalida.push_back(tilesArray[i + (y * (inputImageHeight/size))].pixeles[j + (x * size)]);
                }
            }
        }
    }
    
    
    

   std::cout << pixelVector.size() << std::endl;
   std::cout << pixelVectorSalida.size() << std::endl;





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