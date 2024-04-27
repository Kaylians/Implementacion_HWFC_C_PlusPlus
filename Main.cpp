/*funcion o lo que sea para ver cuanta distancia minima puede tener de grosor un color.
si por ejemplo un pixel se ve siempre solo, nunca esta solo, quizas si es lineal o en que formas se usa
valor que guarde cuantos pixeles de cada tipo tiene un patron, ver como usar esa informacion
ver distancia entre patrones, quizas con el punto inicial de donde estaban en el mapa original
*/
//siguiente tarea: 

//aumentar la capacidad para hacer encajar los patrones, dar flexibilidad.
//dar prioridad a encajar los patrones
//revisar que no se queden puntos atras del 
// 
// problema principal, se pierden RPP aun utiles
// 
//link de reunion: https://reuna.zoom.us/my/nbarriga

//revisar que los alta jerarquia no coincidan en el mismo espacio

//Fallo colocar los rpp extendidos en los puntos que quedaron atras, realizar busqueda de todos los RPP utiles antes de cerrar ciclo medio

//
// puntero, revisar que RPP se esta eliminando
//  
// A�adir que no se toquen colores que no deben
// 
// A�adir un RPP para colapsados y no colapsados, primero se debe usar el de colapsados
// 

// Ruta: cd /mnt/d/Memoria\ HWFC/Code/test2/src



// g++ -g -Wall -Wextra -o WFC WFC.cpp

//medir tiempo

// g++ -O3 Main.cpp pattern.cpp HWFC.cpp MWFC.cpp WFC.cpp DebugUtility.cpp ReadWrite.cpp Metrics.cpp -o Main -lboost_program_options

// Ejecuci�n: 

// ./Main --mode "WFC" --LP 2 3 --example "example2.ppm" --size 10

// ./Main --mode "HWFC" --LP 2 3 --MP 5 --HP 8 HP_i --example "example.ppm" --size 10 --metric --serial_i 100

// gdb ./Main

// r (argumento)

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
* free(): invalid next size (fast)  (despues de terminar la iteraci�n, no se llego a un print nuevo) (M)
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

#include "Pattern.h"
#include "Main.h"
#include "Pixel.h"
#include "WFC.h"
#include "MWFC.h"
#include "HWFC.h"
#include "ReadWrite.h"
#include "DebugUtility.h"
#include "Metrics.h"
#include <thread>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

//funcion para inicializar la generaci�n de numeros aleatorios
void initializeRandomSeed() {
    std::srand(std::time(0));
}
//funcion para obtener numeros aleatorios con min y max
int getRandom(int min, int max) {
    int rnd = 0;
    rnd = std::rand() % max;
    return rnd;
}
//funcion para verificar si un vector contiene un valor int
bool searchVectorInt(const std::vector<int>& vec, int value) {
    auto iter = std::find(vec.begin(), vec.end(), value);
    if (iter != vec.end()) {
        return true;
    }
    else {
        return false;
    }
}
//funcion para detener la ejecucion
void stopExecute(int times, std::string texto) {
    ControlString(texto);
    //std::this_thread::sleep_for(std::chrono::milliseconds(times));
}
//funcion random para elegir el patron que m�s se repite
int getRandomPatternWeighted(const std::vector<Pattern> pattern, int weightMultiplier) {

    std::vector<double> pesos(pattern.size());
    for (size_t i = 0; i < pattern.size(); i++) {
        pesos[i] = pattern[i].weight * weightMultiplier;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution(pesos.begin(), pesos.end());
    return distribution(gen);
}
//definir cuantas posibles estados puede tomar un pixel y cuantas veces aparece
void defineTiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
}
//funcion para inicializar el mapa
void initializePosMap(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& posibleTiles, int Y) {
    std::vector<int> tmp;
    unCollapseMap.clear();
    for (int i = 0; i < Y * Y; i++) {
        for (int j = 0; j < posibleTiles.size(); j++) {
            tmp.push_back(j);
        }
        unCollapseMap.push_back(tmp);
        tmp.clear();
    }
}
//funcion para elegir la casilla con entropia (posibles colores) disponible en el mapa
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int Posibi, int LastLowestEntropyTilePos, const std::vector<int>& RPP) {
    int lowestValue = Posibi + 1, lowestID = -1;
    //std::cout << "cantidad de regiones de propagacion: " << RPP.size() << std::endl;
    if (RPP.size() > 0) {
        //std::cout << RED << "seleccionar entre regiones colapsadas" << RESET << std::endl;
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
        //std::cout << RED << "Buscar entre posiciones de baja entropia" << RESET << std::endl;
        for (int i = 0; i < unCollapseMap.size(); i++) {
            //si es que hay m�s de una posibilidad para la casilla
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
                //seccion para patrones de HWFC
                if (pattern.pixelesCoo[y + x * N] == -1) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
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
bool Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, std::vector<int>& reserveRPP, const int Y, std::vector<Pattern>& pattern, Pattern& selectedPatt, int pos, int posibleTilesN, bool extendRPP) {
    auto newPattern = pattern.front();
    int stuck_Counter = 0;
    std::vector<int> usedPatterns;
    std::vector<int> uncollapseNode;
    for (int h = 0; h < posibleTilesN; h++)
        uncollapseNode.push_back(h);
    auto iterador = usedPatterns.end();
    bool finded;
    //if (unCollapseMap[x].size() == 1)
    do {
        do {
            if (usedPatterns.size() == pattern.size()) {
                //std::cout << RED << "!!!!!!!!!!!!!!!!!!!!!!!!! NO HAY SOLUCIONES DISPONIBLES PARA LA POSICION: " << pos << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
                auto i = std::find(RPP.begin(), RPP.end(), pos);
                if (i != RPP.end()) {
                    int posicion = std::distance(RPP.begin(), i);
                    //ControlPoint(posicion);
                    if (unCollapseMap[posicion].size() > 1)
                        reserveRPP.push_back(posicion);
                    RPP.erase(i);
                }

                /*
                std::cout << "Posiciones restantes: ";
                for (int i = 0; i < RPP.size(); i++) {
                    std::cout << RPP[i] << " ";
                }
                std::cout << std::endl;
                */
                return false;
            }
            //newPattern = pattern[getRandomPatternWeighted(pattern)];
            newPattern = pattern[getRandomPatternWeighted(pattern, 5)];
            iterador = std::find(usedPatterns.begin(), usedPatterns.end(), newPattern.id);
        } while (iterador != usedPatterns.end());
        usedPatterns.push_back(newPattern.id);
        //std::cout << "patron seleccionado: " << newPattern.id << std::endl;

        if (selectPattern(newPattern, pattern, unCollapseMap, newPattern.N, Y, pos, false)) {
            //std::cout << "patron util encontrado: " << newPattern.id << " tama�o :" << newPattern.N << std::endl;
            finded = true;
        }
        else {
            finded = false;
        }
        stuck_Counter++;
    } while (!finded);
   // std::cout << "iteraciones realizadas buscando el patron: " << stuck_Counter << std::endl,
        stuck_Counter = 0;

    //RPP.clear();
    for (int i = 0; i < newPattern.N; i++) {
        for (int j = 0; j < newPattern.N; j++) {
            if (unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].size() > 1) {
                unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].clear();
                //convierte cada pixel del arreglo a su respectivo valor numerico que lo representa dentro de un collapseMap
                if (newPattern.pixelesCoo[j + newPattern.N * i] == -1) {
                    unCollapseMap[newPattern.coordinate[j + newPattern.N * i]] = uncollapseNode;
                }
                else {
                    unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].push_back(newPattern.pixelesCoo[j + newPattern.N * i]);
                    RPP.push_back(newPattern.coordinate[j + newPattern.N * i]);
                }
            }
        }
    }

    selectedPatt = newPattern;
    bool U = false, S = false, E = false, O = false;
    int colide;
    std::vector<int> tmpRPP;
    std::vector<int> tmpEraseRPP;
    for (int i = 0; i < RPP.size(); i++) {
        //std::cout << RPP[i];
        if (unCollapseMap[RPP[i]].size() < posibleTilesN) {
            U = false, S = false, E = false, O = false, colide = 0;
            if (RPP[i] / Y > 0) {
                if (unCollapseMap[RPP[i] - Y].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] - Y);
                    tmpRPP.push_back(RPP[i]);
                    U = true;
                }
            }
            if (RPP[i] / Y < Y - 1) {
                if (unCollapseMap[RPP[i] + Y].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] + Y);
                    tmpRPP.push_back(RPP[i]);
                    S = true;
                }
            }
            if (RPP[i] % Y < Y-1) {
                if (unCollapseMap[RPP[i] + 1].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] + 1);
                    tmpRPP.push_back(RPP[i]);
                    E = true;
                }
            }
            if (RPP[i] % Y < Y && RPP[i] % Y > 0) {
                if (unCollapseMap[RPP[i] - 1].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] - 1);
                    tmpRPP.push_back(RPP[i]);
                    O = true;
                }
            }

            if (!U && !S && !E && !O && unCollapseMap[RPP[i]].size() == 1) {
                tmpEraseRPP.push_back(RPP[i]);
                //std::cout << RED << "X" << RESET;
            }
        }
        //std::cout << "  ";
    }
    for (int i = 0; i < tmpEraseRPP.size(); i++) {
        auto it = std::find(RPP.begin(), RPP.end(), tmpEraseRPP[i]);

        if (it != RPP.end())
            RPP.erase(it);
    }
    //std::cout << std::endl;
    for (int i = 0; i < tmpRPP.size(); i++) {
        RPP.push_back(tmpRPP[i]);
    }
    tmpRPP.clear();

    std::sort(RPP.begin(), RPP.end());
    RPP.erase(std::unique(RPP.begin(), RPP.end()), RPP.end());

    /*
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
    */
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
void searchForRPP(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() > 1) {
            RPP.push_back(i);
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
            //std::cout << "eliminar posicion sin patrones:" << posiblePos[i] << std::endl;
            auto it = std::find(RPP.begin(), RPP.end(), posiblePos[i]);
            // Verificar si se encontr� el elemento
            if (it != RPP.end()) {
                RPP.erase(RPP.begin() + i);
                //std::cout << "eliminacion exitosa" << std::endl;
            }
        }
        else {
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
//verifica si todas las posiciones del mapa estan colapsadas
bool mapCompleted(const std::vector<std::vector<int>>& unCollapseMap) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}


int main(int argc, char* argv[]) {
    std::vector<int> N;
    std::vector<int> MN;
    std::vector<int> HN;
    std::string exampleMap, MetricMap;
    std::string mode;
    int Y, X, HN_i, serial_it_max, serial_it = 0;
    int testPos;

    po::options_description desc("Opciones permitidas");
    desc.add_options()
        ("help", "Mostrar informaci�n de ayuda")
        ("mode", po::value<std::string>(&mode), "Ingresar modo del algoritmo: WFC, MWFC, HWFC")
        ("LP", po::value<std::vector<int>>(&N)->multitoken(), "Ingresar tamano de patrones a usar como valores enteros")
        ("MP", po::value<std::vector<int>>(&MN)->multitoken(), "Ingresar tamano de patrones de jerarquia a usar como valores enteros")
        ("HP", po::value<std::vector<int>>(&HN)->multitoken(), "Ingresar tamano de patrones de jerarquia a usar como valores enteros")
        ("HP_i", po::value<int>(&HN_i), "Ingresar iteraciones para patron alto")
        ("example", po::value<std::string>(&exampleMap), "Ingresar nombre del archivo en formato .PPM")
        ("size", po::value<int>(&Y), "Ingresar tamano de la imagen de salida")
        ("test", po::value<int>(&testPos), "valor para ejecutar pruebas")
        ("metric", po::value<std::string>(&MetricMap), "Ingresar nombre del archivo en formato .PPM")
        ("serial_i", po::value<int>(&serial_it_max), "Ingresar iteraciones para patron alto");
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
        if (vm.count("LP")) {
            std::cout << "Usar patrones de baja jerarquia: ";
            for (int val : N) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Valores de patrones bajos no ingresados." << std::endl;
        }
        if (vm.count("MP")) {
            std::cout << "Usar patrones de media jerarquia: ";
            for (int val : HN) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Valores de patrones medianos no ingresados." << std::endl;
        }
        if (vm.count("HP")) {
            std::cout << "Usar patrones de alta jerarquia: ";
            for (int val : MN) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Valores de patrones altos no ingresados." << std::endl;
        }
        if (vm.count("HP_i")) {
            std::cout << "iteraciones de alta jerarquia: " << HN_i << std::endl;
        }
        else {
            std::cout << "Valores de patrones altos no ingresados." << std::endl;
        }
        if (vm.count("example")) {
            std::cout << "Valor de cadena ingresado: " << exampleMap << std::endl;
        }
        else {
            std::cout << "Valor de cadena no ingresado." << std::endl;
        }
        if (vm.count("size")) {
            std::cout << "tamano imagen de salida: " << Y << std::endl;
        }
        else {
            std::cout << "Valor de tamano no ingresado." << std::endl;
        }
        if (vm.count("metric")) {
            std::cout << "Valor de cadena ingresado: " << MetricMap << std::endl;
        }
        else {
            std::cout << "Valor de cadena no ingresado." << std::endl;
        }
        if (vm.count("serial_i")) {
            std::cout << "iteracion serial: " << serial_it_max << std::endl;
        }
        else {
            std::cout << " " << std::endl;
        }

    }
    catch (const boost::program_options::error& e) {
        std::cerr << "Error al analizar las opciones: " << e.what() << std::endl;
        return 1;
    }

    initializeRandomSeed();
    std::string filename = "./" + exampleMap, PPM_Identifier; std::ifstream file(filename, std::ios::binary); int inputImageWidth, inputImageHeight;

    std::vector<Pixel> pixelVector, pixelVectorSalida, patterVectorSalida, PosibleTiles;
    std::vector<std::vector<int>> unCollapseMap; std::vector<int> RPP, reserveRPP; RPP.resize(0); reserveRPP.resize(0);

    //lectura de la imagen de entrada
    if (readImagePPM(exampleMap, inputImageWidth, inputImageHeight, pixelVector)) {
        std::cout << "Imagen PPM leida exitosamente." << std::endl;
    }
    //inicio de cronometro
    auto start_time = std::chrono::high_resolution_clock::now();

    //defincion de las casillas
    defineTiles(pixelVector, PosibleTiles);

    initializePosMap(unCollapseMap, PosibleTiles, Y);
    int lowestEntropyTilePos;
    //inicio del algoritmo WFC
    bool randomStart = true;

    //almacen de informaci�n para backtracking
    std::vector<int> BT_pos; std::vector<std::vector<std::vector<int>>> BT_cMap; std::vector<std::vector<int>> BT_RPP;
    int step = 0, toStep = 0, lastStep = 0, backtrackUses = 0; bool backtrackingRequested = true;
    Pattern lastSelectedPattern(0, 0);

    std::vector<Pattern> patternArrayBase, patternArrayLow, patternArrayMid, patternArrayHigh, usedPatternArray;

    //DEFINICION DE LOS PATRONES
    if (mode == "WFC") {
        //Patrones de un unico tama�o
        ControlString("Obtener patrones WFC");
        definePatternsWFC(patternArrayLow, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, N[0]);
    }
    else {
        //Patrones de multiples tama�os
        ControlString("Obtener patrones MWFC");
        definePatternsMWFC(patternArrayLow, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, N, false);
    }
    if (mode == "HWFC") {
        //patrones de multiples tama�os para uso medio
        ControlString("Obtener patrones medios HWFC");
        definePatternsMWFC(patternArrayMid, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, MN, true);
        //patrones de multiples tama�os para uso alto
        ControlString("Obtener patrones altos HWFC");
        definePatternsHWFC(patternArrayHigh, pixelVector, PosibleTiles, inputImageHeight, inputImageWidth, HN);
    }
    infoPatternUpdateID(patternArrayBase,patternArrayLow,patternArrayMid,patternArrayHigh);
    do {
        //EJECUCION DE JERARQUIA ALTA EN HWFC
        if (mode == "HWFC") {
            //colapso de patrones de alta jerarquia iniciales
            int HN_max = 0;
            for (int i = 0; i < HN.size(); i++)
                if (HN_max < HN[i])
                    HN_max = HN[i];
            //Collapse(unCollapseMap, RPP, reserveRPP, Y, patternArrayHigh, lastSelectedPattern, 20, PosibleTiles.size(), false);
            //printMapWithCollapseMark(unCollapseMap, lastSelectedPattern.coordinate, Y, PosibleTiles.size(), RPP, false);

            for (int i = 0; i < HN_i; i++) {
                //busqueda y posicionamiento de patrones de alta jerarquia
                do {
                    lowestEntropyTilePos = getRandom(0, unCollapseMap.size());
                } while (!HPattValidTile(lowestEntropyTilePos, Y, Y, HN_max) && unCollapseMap[lowestEntropyTilePos].size() > 1);

                if (Collapse(unCollapseMap, RPP, reserveRPP, Y, patternArrayHigh, lastSelectedPattern, lowestEntropyTilePos, PosibleTiles.size(), false)) {
                    //printMapWithCollapseMark(unCollapseMap, lastSelectedPattern.coordinate, Y, PosibleTiles.size(), RPP, false, false);
                }
                else i--;
            }
            //stopExecute(2000, "patrones de media jerarquia...");
            bool condition = true;
            //colapso de patrones de tama�o mediano
            do {
                lowestEntropyTilePos = selectLowestEntropyTile(unCollapseMap, PosibleTiles.size(), lowestEntropyTilePos, RPP);
                //std::cout << YELLOW << "Posicion con la entropia m�s baja: " << lowestEntropyTilePos << RESET << std::endl;
                //std::cout << GREEN << "COLAPSAR mediano" << RESET << std::endl;
                if (Collapse(unCollapseMap, RPP, reserveRPP, Y, patternArrayMid, lastSelectedPattern, lowestEntropyTilePos, PosibleTiles.size(), false))
                {
                    usedPatternArray.push_back(lastSelectedPattern);
                    //printMapWithCollapseMark(unCollapseMap, lastSelectedPattern.coordinate, Y, PosibleTiles.size(), RPP, true, false);
                }
                //ControlPoint(RPP.size());
            } while (RPP.size() > 1);

        }
        for (int i = 0; i < reserveRPP.size(); i++) {
            RPP.push_back(reserveRPP[i]);
        }
        searchForRPP(unCollapseMap, RPP);
        std::sort(RPP.begin(), RPP.end());
        RPP.erase(std::unique(RPP.begin(), RPP.end()), RPP.end());
        //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, true);
        //ControlPoint(0);
        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        //std::exit(0);
        int controlPointN = 0;
        while (!mapCompleted(unCollapseMap)) {
            //std::cout << PURPLE << "step: " << step << RESET << std::endl;
            //seleccionar una casilla para colpasar
            //std::cout << GREEN << "SELECCIONAR " << RESET << std::endl;

            if (randomStart) {
                lowestEntropyTilePos = getRandom(0, unCollapseMap.size());
                randomStart = false;
                //std::cout << YELLOW << "Posicion aleatoria: " << lowestEntropyTilePos << RESET << std::endl;
            }
            else {
                lowestEntropyTilePos = selectLowestEntropyTile(unCollapseMap, PosibleTiles.size(), lowestEntropyTilePos, RPP);
                //std::cout << YELLOW << "Posicion con la entropia m�s baja: " << lowestEntropyTilePos << RESET << std::endl;
            }

            //std::cout << GREEN << "COLAPSAR" << RESET << std::endl;
            if (Collapse(unCollapseMap, RPP, reserveRPP, Y, patternArrayLow, lastSelectedPattern, lowestEntropyTilePos, PosibleTiles.size(), true)) {
               //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, false);

                //std::cout << " " << std::endl;
                //std::cout << GREEN << "PROPAGAR" << RESET << std::endl;
                //propagate(unCollapseMap, RPP, patternArrayLow, lowestEntropyTilePos, Y);
                //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, false);
                usedPatternArray.push_back(lastSelectedPattern);

                if (RPP.size() > 0) {
                    BT_cMap.push_back(unCollapseMap);
                    BT_pos.push_back(lowestEntropyTilePos);
                    BT_RPP.push_back(RPP);
                    //std::cout << "Guardado de backtracking completado" << std::endl;
                    step++;

                    /*
                    std::cout << "Posiciones restantes: ";
                    for (int i = 0; i < RPP.size(); i++) {
                        std::cout << RPP[i] << " ";
                    }
                    std::cout << std::endl;
                    */
                    backtrackingRequested = false;
                }
            }
            if (RPP.size() == 0 && !mapCompleted(unCollapseMap)) {
                ControlString("Sin resultado, backtracking apagado, reiniciando");
                serial_it--;
                break;
            }
                /*
                //a�adir un do while que vaya eliminando las posiciones en el backtracking anterior al usado antes de reemplazarlo 
                std::cout << YELLOW << "BACKTRACKING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;

                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
                std::cout << "Tiempo de ejecucion hasta backtracking: " << duration << " ms" << std::endl;

                int posToDelete;

                do {
                    step = BT_cMap.size() - 1;
                    unCollapseMap = BT_cMap.back();
                    RPP = BT_RPP.back();
                    if (!backtrackingRequested) {
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
            /*
            else {
                std::cout << YELLOW << "reintentar con otro punto de propagacion" << RESET << std::endl;
            }

            std::cout << "posibles regiones de propagacion restantes: " << RPP.size() << std::endl;
            std::cout << "usos del backtracking: " << backtrackUses << std::endl;
            std::cout << PURPLE << "punto de iteracion" << RESET << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            */
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }



        std::cout << GREEN << "MAPA COMPLETADO EXITOSAMENTE" << RESET << std::endl;
        printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, false);
        //fin del cronometro del tiempo de ejecucion
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "Tiempo de ejecucion: " << duration << " ms" << std::endl;
        std::cout << "posible pixel color: " << PosibleTiles.size() << std::endl;
        std::cout << "usos del backtracking: " << backtrackUses << std::endl;
        //definicion de metricas
        //KL Divergence
        
        //construccion de una nueva imagen
        reconstructMap(pixelVectorSalida, unCollapseMap, PosibleTiles);
        std::cout << "Mapa reconstruido exitosamente." << std::endl;

        //guardado de la imagen en un nuevo archivo

        // Metodo tamaño (numero de generacion)

        findUniquePattern(usedPatternArray);
        size_t dotPos = exampleMap.find('.');
        std::string nameWithoutExt = "";
        if (dotPos != std::string::npos) {
            // Extraer la parte del nombre de archivo antes del punto
            nameWithoutExt = exampleMap.substr(0, dotPos);
        }
        SaveInfoOnFile(pixelVectorSalida, usedPatternArray, mode + "_" + nameWithoutExt, Y, PosibleTiles);

        //dibujar los patrones en una imagen aparte
        initializePosMap(unCollapseMap, PosibleTiles, Y);
        serial_it++;
        pixelVectorSalida.clear();
        usedPatternArray.clear();
        RPP.clear();
        BT_pos.clear();
        BT_cMap.clear();
        BT_RPP.clear();
        

    }while (serial_it_max > serial_it);

    PerformMetrics();
    

    createPatternDraw(patternArrayLow, patterVectorSalida, Y);
    if (writeImagePPM("patron_Generada.ppm", Y, Y, patterVectorSalida)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    

    return 0;
}