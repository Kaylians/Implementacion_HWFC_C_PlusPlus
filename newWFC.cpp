// cd /mnt/d/Memoria\ HWFC/Code/test2/src
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
            std::cout << "Error, tamaño de tiles no compatible" << std::endl;
            return false;
        }

        return otherPixeles == pixeles;
    }
    bool compareCooPattern(const std::vector<int>& otherPixeles) {

        if (otherPixeles.size() != pixelesCoo.size()) {
            std::cout << "Error, tamaño de tiles no compatible" << std::endl;
            return false;
        }

        return otherPixeles == pixelesCoo;
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

    pixeles.resize(w * h);
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
                else if (unCollapseMap[j + i * size].size() == 0) {
                    std::cout << " " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() == posibi) {
                    std::cout << " " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() < posibi)
                    std::cout << unCollapseMap[j + i * size].size() << " || ";
                else
                    std::cout << "X ||";
            }
        }
        
        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
//funcion para ver si el patron de N*N, no excede los limites del mapa, en caso de que si, mueve el punto para que abarque solo hasta el final del mapa
void verifyPos(int& pos, const int Y, const int N) {


    
    
}
//funcion para ver si dos patrones son iguales
bool comparePattern(const Pattern& a, const Pattern& b) {
    return a.weight > b.weight;
}
//funcion para separar la imagen en los diferentes patrones que la componen
void definePatterns(std::vector<Pattern>& pattArray, const std::vector<Pixel>& pixelVector, const std::vector<Pixel> posibleTiles, const int inputImageHeight, const int inputImageWidth, int N){
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    //seperacion de la imagen en multiples patrones
    //for (int y = 0; y <= inputImageHeight - N; y++)
        for (int x = 0; x <= inputImageWidth * inputImageHeight - (inputImageWidth*(N-1))-N; x++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    auto e = std::find(posibleTiles.begin(), posibleTiles.end(), pixelVector[(x + j + i * inputImageWidth)]); 
                    tmpCooVector.push_back(std::distance(posibleTiles.begin(), e));
                    tmpVector.push_back(pixelVector[(x + j + i * inputImageWidth)]);
                }
            }

            
            Pattern newPattern(pattArray.size(), N);
            newPattern.addPixelVector(tmpVector);
            newPattern.addPixelCooVector(tmpCooVector);
            pattArray.push_back(newPattern);
            tmpVector.clear();
            tmpCooVector.clear();
        }

    int weight = 0;
    for (int i = 0; i < pattArray.size(); i++) {
        if (pattArray[i].pattern) {
            weight++;
            for (int j = i + 1; j < pattArray.size(); j++) {
                if (pattArray[j].pattern && pattArray[i].comparePixelPattern(pattArray[j].pixeles)) {
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
            tmpPattArray[tmpPattArray.size() - 1].id = tmpPattArray.size() - 1;
        }
    pattArray.clear();
    pattArray = tmpPattArray;
    std::cout << "Patrones totales obtenidos de la imagen: " << pattArray.size() << std::endl;

    std::sort(pattArray.begin(), pattArray.end(), comparePattern);

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
//funcion para elegir la casilla con entropia (posibles colores) disponible en el mapa
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int size, int LastLowestEntropyTilePos) {
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
//funciones para confirmar si el patron coincide en cada uno de sus puntos con el espacio alrededor del punto entregado
bool selectPatternAnalizer(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& mapCoo, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
    bool contains = false;
    mapCoo.clear();
    for (int i = i_min, x = 0; i <= i_max; i++, x++) {
        for (int j = j_min, y = 0; j <= j_max; j++, y++) {
            for (int z = 0; z < unCollapseMap[pos + j + i * N].size(); z++) {
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
bool selectPattern(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& mapCoo, const int N, const int Y, int pos, bool forceCenter) {
   
    bool U = false, S = false, E = false, W = false, C = false, contains = false;
    
    //center, norte, sur, este , oeste
    if (N % 2 == 1)
        if (pos % Y >= (N / 2) && pos % Y < Y - (N / 2) && pos / Y >= (N / 2) && pos / Y < Y - (N / 2)) {
            if (selectPatternAnalizer(pattern, unCollapseMap, mapCoo, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
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
                    if (selectPatternAnalizer(pattern, unCollapseMap, mapCoo, N, Y, pos, -(N - 1), 0, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, mapCoo, N, Y, pos, -(N-1), 0, 0, N - 1)) {
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
                    if (selectPatternAnalizer(pattern, unCollapseMap, mapCoo, N, Y, pos, 0, N - 1, -(N-1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, mapCoo, N, Y, pos, 0, N - 1, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;   
}
//funcion para colapsar una posicion a una patron en concreto que coincida por cada pixel con los valores adyacentes al punto
void Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& mapCoo, std::vector<int>& regionesRecienColapsadas, const int Y, const std::vector<Pattern>& pattern, int pos) {
    int N = pattern.front().N;
    auto newPattern = pattern.front();

    do {
        newPattern = pattern[getRandomPatternWeighted(pattern)];
        //newPattern = pattern[getRandom(0, pattern.size())];
        std::cout << "Buscar nuevo patron para colapsar: " << std::endl;
    } while (!selectPattern(newPattern, unCollapseMap, mapCoo, N, Y, pos, false));
    
    regionesRecienColapsadas.clear();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (unCollapseMap[mapCoo[j + N * i]].size() > 1) {
                unCollapseMap[mapCoo[j + N * i]].clear();
                //convierte cada pixel del arreglo a su respectivo valor numerico que lo representa dentro de un collapseMap
                unCollapseMap[mapCoo[j + N * i]].push_back(newPattern.pixelesCoo[j + N * i]);
                regionesRecienColapsadas.push_back(mapCoo[j + N * i]);
            }
        }
    }
    
    std::cout << "regiones recien colapsadas totales: " << regionesRecienColapsadas.size() << " == ";
    if (regionesRecienColapsadas.size() == 0) {
        std::cout << "no se colapsaron nuevas regiones " << std::endl;
    }
    else {
        for (int i = 0; i < regionesRecienColapsadas.size(); i++) {
            std::cout << regionesRecienColapsadas[i] << " ";
        }
        std::cout << std::endl;
    }
}
//funcion para obtener los posibles candidatos para donde empezar a propagar
void getMapPropagationPos(std::vector<int>& posiblePos, const std::vector<std::vector<int>>& unCollapseMap, const std::vector<int>& regionesRecienColapsadas,const int N, const int Y) {
    int distance = 1;
    posiblePos.clear();
    bool U = false, S = false, E = false, W = false, C = false, contains = false;

    std::vector<int> borders;
    //bordes en la zona de colapso
    int begin = regionesRecienColapsadas.front(), end = regionesRecienColapsadas.back();
    for (int i = 0, j = 1; i < regionesRecienColapsadas.size(); i++, j++) {
        if ((j % N) == 1 || (j % N) == 0) {
            borders.push_back(regionesRecienColapsadas[i]);
        }
        else if (regionesRecienColapsadas[i] <= begin + N) {
            borders.push_back(regionesRecienColapsadas[i]);
        }
        else if (regionesRecienColapsadas[i] >= end - N) {
            borders.push_back(regionesRecienColapsadas[i]);
        }
        
    }
    /*
    for (int i = 0; i < borders.size(); i++) {
        U = false, S = false, W = false, E = false;
        if (borders[i] / Y > 0)
            if (unCollapseMap[borders[i] - Y].size() > 1) {
                posiblePos.push_back(borders[i] - Y); U = true;
            }
        if (borders[i] / Y < Y - 1)
            if (unCollapseMap[borders[i] + Y].size() > 1) {
                posiblePos.push_back(borders[i] + Y); S = true;
            }
        if (borders[i] % Y < Y - 1)
            if (unCollapseMap[borders[i] + 1].size() > 1) {
                posiblePos.push_back(borders[i] + 1);E = true;
            }
        if (borders[i] % Y > 0)
            if (unCollapseMap[borders[i] - 1].size() > 1) {
                posiblePos.push_back(borders[i] - 1);W = true;
            }

        if (U) {
            if (E) {
                if(unCollapseMap[borders[i] - Y + 1].size() > 1)
                    posiblePos.push_back(borders[i] - Y + 1);
            }
            if (W) {
                if (unCollapseMap[borders[i] - Y - 1].size() > 1)
                    posiblePos.push_back(borders[i] - Y - 1);
            }
        }
        if (S) {
            if (E) {
                if (unCollapseMap[borders[i] + Y + 1].size() > 1)
                    posiblePos.push_back(borders[i] + Y + 1);
            }
            if (W) {
                if (unCollapseMap[borders[i] + Y - 1].size() > 1)
                    posiblePos.push_back(borders[i] + Y - 1);
            }
        }

        //std::cout << "el borde " << borders[i] << " tiene disponible N " << U << " S " << S << " E " << E << " W " << W << std::endl;
    }
    */
    posiblePos = borders;
    std::cout << "posiciones de propagacion posibles : ";
    for (int i = 0; i < posiblePos.size(); i++) {
        std::cout << posiblePos[i] << " ";
    }
    std::cout << std::endl;   
}
//funcion para crear un mapa con los valores de los patrones candidatos para la propagacion
void reduceMap(const std::vector<Pattern>& pattern, std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos) {
    for (int i = 0; i < pattern.size(); i++) {
        for (int z = 0; z < pattern[i].coordinate.size(); z++) {
            unCollapseMap[pattern[i].coordinate[z]].push_back(pattern[i].pixelesCoo[z]);
            if (unCollapseMap[pattern[i].coordinate[z]].size() > 1) {
                
            }
        }
    }
}
//funcion para ver y guardar que patrones son compatibles alrededor de los posibles candidatos
//requiere revision////requiere revision//////requiere revision///////requiere revision//////////////////requiere revision//////////////////
void propagate(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& mapCoo, const std::vector<int>& regionesRecienColapsadas, std::vector<Pattern>& pattern, int pos, const int N, const int Y) {

    //distancia de N + 1 para buscar posibles candidatos
    std::vector<int> posiblePos;
    getMapPropagationPos(posiblePos, unCollapseMap, regionesRecienColapsadas, N, Y);
    std::vector<Pattern> propagationPattern;
    std::vector<std::vector<int>> tmpUnCollapseMap;
    tmpUnCollapseMap.resize(Y * Y);
    auto newPattern = pattern.front();
    for (int i = 0; i < posiblePos.size(); i++) {
        propagationPattern.clear();
        std::cout << "posicion posible de propagacion " << posiblePos[i] << " /";
        for (int j = 0; j < pattern.size(); j++) {
            if (selectPattern(pattern[j], unCollapseMap, mapCoo, N, Y, posiblePos[i],true)) {
                propagationPattern.push_back(pattern[j]);
            }
        }
        std::cout << "patrones compatibles obtenidos: " << propagationPattern.size() << std::endl;
        if (propagationPattern.size() > 0) {
            reduceMap(propagationPattern, tmpUnCollapseMap, N, Y, posiblePos[i]);
        }
    } 
    for (auto& e : tmpUnCollapseMap) {
        std::sort(e.begin(), e.end());
        auto it = std::unique(e.begin(), e.end());
        e.erase(it, e.end());
    }
    std::cout << "final reduced Map" << std::endl;
    printMap(tmpUnCollapseMap, Y, 5);

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! la propagacion puede colapsar??????
    //! !!!!!!!!!!!!!!!!!
    //! 
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (tmpUnCollapseMap[i].size() != 0 && unCollapseMap[i].size() > 1 ) {
            unCollapseMap[i] = tmpUnCollapseMap[i];
        }
    }
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
    } while (Width*Width < pattern.size());
    
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

int main(int argc, char* argv[]) {
    //1 image.ppm
    initializeRandomSeed();
    std::string imageName = argv[1];
    std::string filename = "./" + imageName, PPM_Identifier;
    std::ifstream file(filename, std::ios::binary);
    int inputImageWidth, inputImageHeight;
    int N = std::atoi(argv[2]);
    int Y = std::atoi(argv[3]);
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
    definePatterns(patternArray, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, N);


    //mapa de superposiciones posibles
    std::vector<std::vector<int>> unCollapseMap;
    std::vector<int> regionesRecienColapsadas;
    regionesRecienColapsadas.resize(0);
    std::vector<int> mapCoo;

    initializePosMap(unCollapseMap, PosibleTiles, Y);
    int lowestEntropyTilePos;
    //inicio del algoritmo WFC
    bool TestingFixedStart = true;
    
    while (!mapCompleted(unCollapseMap)) {

        //seleccionar, buscar la casilla con la menor entropia posible
        if (TestingFixedStart) {
            int fixedPos = 89;
            unCollapseMap[fixedPos].clear();
            unCollapseMap[fixedPos].push_back(1);
            unCollapseMap[fixedPos].push_back(0);
            lowestEntropyTilePos = fixedPos;
            TestingFixedStart = false;
        }
        else {
            lowestEntropyTilePos = selectLowestEntropyTile(unCollapseMap, PosibleTiles.size(), lowestEntropyTilePos);
        }
        std::cout << "posicion con la entropia más baja: " << lowestEntropyTilePos << std::endl;

        //seleccionar patron compatible y collapsar
        std::cout << "colapsar" << std::endl;
        Collapse(unCollapseMap, mapCoo, regionesRecienColapsadas, Y, patternArray, lowestEntropyTilePos);
        printMap(unCollapseMap, Y, PosibleTiles.size());

        std::cout << "propagar" << std::endl;
        propagate(unCollapseMap, mapCoo, regionesRecienColapsadas, patternArray, lowestEntropyTilePos, N, Y);
        printMap(unCollapseMap, Y, PosibleTiles.size());

        break;
    }
   
    
    std::cout <<"posible pixel color: " << PosibleTiles.size() << std::endl;
    //construccion de una nueva imagen
    reconstructMap(pixelVectorSalida, unCollapseMap, PosibleTiles);

    //guardado de la imagen en un nuevo archivo
    if (writeImagePPM("imagen_Generada.ppm", Y, Y, pixelVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }


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
