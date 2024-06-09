/*funcion o lo que sea para ver cuanta distancia minima puede tener de grosor un color.
si por ejemplo un pixel se ve siempre solo, nunca esta solo, quizas si es lineal o en que formas se usa
valor que guarde cuantos pixeles de cada tipo tiene un patron, ver como usar esa informacion
ver distancia entre patrones, quizas con el punto inicial de donde estaban en el mapa original

//siguiente tarea: 
// puntero, revisar que RPP se esta eliminando
// A�adir que no se toquen colores que no deben
// A�adir un RPP para colapsados y no colapsados, primero se debe usar el de colapsados
// g++ -g -Wall -Wextra -o WFC WFC.cpp
//medir tiempo
// Ejecuci�n: 
// ./Main --mode "WFC" --LP 2 3 --example "example2.ppm" --size 10
// ./Main --mode "HWFC" --LP 2 3 --MP 5 --HP 8 HP_i --example "example.ppm" --size 10 --metric --serial_i 100
// gdb ./Main
// r (argumento)
// valgrind ./WFC
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./WFC
// valgrind --tool=cachegrind ./WFC
*/

// g++ - O3 Main.cpp pattern.cpp HWFC.cpp MWFC.cpp WFC.cpp DebugUtility.cpp ReadWrite.cpp Metrics.cpp - o Main - lboost_program_options


// Ruta: cd /mnt/d/Memoria\ HWFC/Code/test2/src


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
#include <stdexcept>

#include "Pattern.h"
#include "Main.h"
#include "Pixel.h"
#include "WFC.h"
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
    std::this_thread::sleep_for(std::chrono::milliseconds(times));
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
void define_Posible_Tiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
}
//funcion para inicializar el mapa
void initialize_Map(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& posibleTiles, int Y) {
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
bool selectPatternAnalizer(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, int posibleTilesN, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
    bool contains = false;
    std::vector<int> mapCoo;
    int count = 0;
    for (int i = i_min, x = 0; i <= i_max; i++, x++) {
        for (int j = j_min, y = 0; j <= j_max; j++, y++) {
            for (int z = 0; z < unCollapseMap[pos + j + i * Y].size(); z++) {
                //si la casilla esta con todas las posibilidades disponibles
                
                if (unCollapseMap[pos + j + i * Y].size() == posibleTilesN) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }
                
                
                //Si todas las casillas ya estan colapsadas, return false 
                if (unCollapseMap[pos + j + i * Y].size() == 1) {
                    count++;
                    if (count == N * N) {
                        return false;
                    }
                }
                //si la casilla corresponde al espacio vacio de un patron de alta jerarquia
                if (pattern.pixelesCoo[y + x * N] == -1) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }
                //si la casilla contiene la posibilidad de la casilla corespondiente al patron
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
bool selectPattern(Pattern& pattern, std::vector<Pattern>& pattArray, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos, int posibleTilesN, bool forceCenter) {
    bool U = false, S = false, E = false, W = false, C = false, contains = false;
    //center, norte, sur, este , oeste
    if (N % 2 == 1)
        if (pos % Y >= (N / 2) && pos % Y < Y - (N / 2) && pos / Y >= (N / 2) && pos / Y < Y - (N / 2)) {
            if (forceCenter) {
                for (int i = 0; i < pattArray.size(); i++) {
                    if (selectPatternAnalizer(pattArray[i], unCollapseMap, posibleTilesN, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                        pattern = pattArray[i];
                        return true;
                    }
                }
            }
            else if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                return true;
            }
        }

    //añadir mejor sistema de localizacion de los patrones aquí

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
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, 0, N - 1)) {
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
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
//funcion para colapsar una posicion a una patron en concreto que coincida por cada pixel con los valores adyacentes al punto
bool Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, std::vector<int>& reserveRPP, const int Y, std::vector<Pattern>& pattern, Pattern& selectedPatt, int pos, int posibleTilesN, bool extendRPP, bool printMapBool) {
    auto newPattern = pattern.front();
    int stuck_Counter = 0;
    std::vector<int> usedPatterns;
    std::vector<int> uncollapseNode;
    for (int h = 0; h < posibleTilesN; h++)
        uncollapseNode.push_back(h);
    auto iterador = usedPatterns.end();
    bool finded;
    do {
        do {
            if (usedPatterns.size() == pattern.size()) {
                auto i = std::find(RPP.begin(), RPP.end(), pos);
                if (i != RPP.end()) {
                    int posicion = std::distance(RPP.begin(), i);
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
            newPattern = pattern[getRandomPatternWeighted(pattern, 3)];
            iterador = std::find(usedPatterns.begin(), usedPatterns.end(), newPattern.id);
        } while (iterador != usedPatterns.end());
        usedPatterns.push_back(newPattern.id);

        if (selectPattern(newPattern, pattern, unCollapseMap, newPattern.N, Y, pos, posibleTilesN,false)) {
            finded = true;
        }
        else {
            finded = false;
        }
        stuck_Counter++;
    } while (!finded);
        stuck_Counter = 0;

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

    //propagacion

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
    if (printMapBool)
        printMap(unCollapseMap, Y, posibleTilesN, RPP, false);
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
            //if (selectPattern(patternArray[j], patternArray, unCollapseMap, patternArray[j].N, Y, posiblePos[i], false)) 
               // propagationPattern.push_back(patternArray[j]);
            
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

// g++ -O3 Main.cpp pattern.cpp HWFC.cpp WFC.cpp DebugUtility.cpp ReadWrite.cpp Metrics.cpp -o Main -lboost_program_options
//./Main --mode "HWFC" --example "example.ppm" --desire_size 2 --top_size 8 --top_amount 1 --size 20 --amount 2

int main(int argc, char* argv[]) {
    std::vector<int> Desire_Size;
    std::vector<int> Desire_Top_Size;
    std::string Example_Map;
    std::string mode;
    int Map_Size, X, Top_Size_i, Map_Requested_i, Map_Requested_completed_i = 0;

    po::options_description desc("Opciones permitidas");
    desc.add_options()
        ("help", "Mostrar informaci�n de ayuda")
        ("mode", po::value<std::string>(&mode), "Ingresar modo del algoritmo: WFC, MWFC, HWFC")
        ("example", po::value<std::string>(&Example_Map), "Ingresar nombre del archivo en formato .PPM")
        ("desire_size", po::value<std::vector<int>>(&Desire_Size)->multitoken(), "Ingresar tamano de patrones a usar como valores enteros")
        ("top_size", po::value<std::vector<int>>(&Desire_Top_Size)->multitoken(), "Ingresar tamano de patrones de jerarquia a usar como valores enteros")
        ("top_amount", po::value<int>(&Top_Size_i), "Ingresar iteraciones para patron alto")
        ("size", po::value<int>(&Map_Size), "Ingresar tamano de la imagen de salida")
        ("amount", po::value<int>(&Map_Requested_i), "Ingresar iteraciones para patron alto");
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

        if (vm.count("example")) {
            std::cout << "Valor de cadena ingresado: " << Example_Map << std::endl;
        }
        else {
            std::cout << "Valor de cadena no ingresado." << std::endl;
        }

        if (vm.count("desire_size")) {
            std::cout << "Usar patrones de baja jerarquia: ";
            for (int val : Desire_Size) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Valores de patrones no ingresados." << std::endl;
        }

        if (vm.count("top_size")) {
            std::cout << "Usar patrones de alta jerarquia: ";
            for (int val : Desire_Top_Size) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "Valores de patrones altos no ingresados." << std::endl;
        }

        if (vm.count("top_amount")) {
            std::cout << "iteraciones de alta jerarquia: " << Top_Size_i << std::endl;
        }
        else {
            std::cout << "Valores de patrones altos no ingresados." << std::endl;
        }

        if (vm.count("size")) {
            std::cout << "tamano imagen de salida: " << Map_Size << std::endl;
        }
        else {
            std::cout << "Valor de tamano no ingresado." << std::endl;
        }

        if (vm.count("amount")) {
            std::cout << "iteracion serial: " << Map_Requested_i << std::endl;
        }
        else {
            std::cout << " " << std::endl;
        }

    }
    catch (const boost::program_options::error& e) {
        std::cerr << "Error al analizar las opciones: " << e.what() << std::endl;
        return 1;
    }
    int inputValue = -1;
    do {
        try {
            std::cout << "Usar backtracking? [1 Si / 0 No]";
            std::cin >> inputValue;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        
    } while (inputValue != 1 && inputValue != 0);
    bool backtrackingActive;
    if (inputValue == 1) {
        backtrackingActive = true;
    }
    else {
        backtrackingActive = false;
    }
    int inputValueMap = -1;
    do {
        try {
            std::cout << "Imprimir mapa? [1 Si / 0 No]";
            std::cin >> inputValueMap;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

    } while (inputValueMap != 1 && inputValueMap != 0);
    bool printMapBool;
    if (inputValueMap == 1) {
        printMapBool = true;
    }
    else {
        printMapBool = false;
    }

    initializeRandomSeed();

    //transformar en función
    size_t dotPos = Example_Map.find('.');
    std::string nameWithoutExt = "";
    if (dotPos != std::string::npos) {
        // Extraer la parte del nombre de archivo antes del punto
        nameWithoutExt = Example_Map.substr(0, dotPos);
    }

    std::string File_Name = "./" + Example_Map, PPM_Identifier; std::ifstream file(File_Name, std::ios::binary); int inputImageWidth, inputImageHeight;
    std::string Base_Folder = "generatedLevels/" + mode + "_" + nameWithoutExt + "_size_" + std::to_string(Map_Size);


    //variables para almacenar
    std::vector<Pixel> Pixel_Vector, Pixel_Vector_Out, Pattern_Vector_Out, Posible_Tiles;
    std::vector<std::vector<int>> Map_Uncollapse; std::vector<int> Propagation_Pos, reserveRPP; Propagation_Pos.resize(0); reserveRPP.resize(0);





    //lectura de la imagen de entrada
    if (!readImagePPM(Example_Map, inputImageWidth, inputImageHeight, Pixel_Vector))
        exit(0);
    



    //defincion de las casillas
    define_Posible_Tiles(Pixel_Vector, Posible_Tiles);

    initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);

    int lowestEntropyTilePos;
    //inicio del algoritmo WFC
    bool randomStart = true;

    //almacen de informaci�n para backtracking
    std::vector<std::vector<std::vector<int>>> BT_Map_Uncollapse; std::vector<std::vector<int>> BT_Propagation_Pos;
    int BT_step = 0, backStep = 0, backtrackUses = 0, totalBacktracking = 0;
    Pattern lastSelectedPattern(0,0);

    std::vector<Pattern> patternArrayBase, patternArrayLow, patternArrayHigh, usedPatternArray;

    //DEFINICION DE LOS PATRONES
    if (mode == "WFC") {
        ControlString("Obtener patrones WFC");
        definePatternsWFC(patternArrayLow, Pixel_Vector, Posible_Tiles, inputImageHeight, inputImageWidth, Desire_Size);
    }
    else {
        //Patrones de multiples tama�os
        ControlString("Obtener patrones MWFC");
        definePatternsWFC(patternArrayLow, Pixel_Vector, Posible_Tiles, inputImageHeight, inputImageWidth, Desire_Size);
    }
    if (mode == "HWFC") {
        //patrones de multiples tama�os para uso alto
        ControlString("Obtener patrones altos HWFC");
        definePatternsHWFC(patternArrayHigh, Pixel_Vector, Posible_Tiles, inputImageHeight, inputImageWidth, Desire_Top_Size);
    }
    infoPatternUpdateID(patternArrayBase,patternArrayLow,patternArrayHigh);
    
    //inicio del algoritmo y generación de la cantidad de mapas solicitados
    do {

        //inicio de cronometro
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Iniciando creacion de nuevo mapa." << std::endl;

        //EJECUCION DE JERARQUIA ALTA EN HWFC
        if (mode == "HWFC") {
            //definir patron de mayor tamaño sobre el cual iterar
            int HN_max = 0;
            for (int i = 0; i < Desire_Top_Size.size(); i++)
                if (HN_max < Desire_Top_Size[i])
                    HN_max = Desire_Top_Size[i];

            for (int i = 0; i < Top_Size_i; i++) {
                //busqueda y posicionamiento de patrones de alta jerarquia
                do {
                    lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
                } while (!HPattValidTile(lowestEntropyTilePos, Map_Size, Map_Size, HN_max) && Map_Uncollapse[lowestEntropyTilePos].size() > 1);

                if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, patternArrayHigh, lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), true, printMapBool)) {
                    //printMapWithCollapseMark(unCollapseMap, lastSelectedPattern.coordinate, Y, PosibleTiles.size(), RPP, false, false);
                    
                    //Guardado de Backtracking
                    if (backtrackingActive) {
                        if (Propagation_Pos.size() > 0) {
                            BT_Map_Uncollapse.push_back(Map_Uncollapse);
                            BT_Propagation_Pos.push_back(Propagation_Pos);
                            BT_step++;
                        }
                    }
                }
                else i--;
            }
            //stopExecute(2000, "patrones de media jerarquia...");
            bool condition = true;
            //colapso de patrones de tama�o mediano
            do {
                lowestEntropyTilePos = selectLowestEntropyTile(Map_Uncollapse, Posible_Tiles.size(), lowestEntropyTilePos, Propagation_Pos);
                //std::cout << YELLOW << "Posicion con la entropia m�s baja: " << lowestEntropyTilePos << RESET << std::endl;
                //std::cout << GREEN << "COLAPSAR mediano" << RESET << std::endl;
                if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, patternArrayLow, lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), false, printMapBool))
                {
                    //Guardado de Backtracking
                    if (backtrackingActive) {
                        if (Propagation_Pos.size() > 0) {
                            BT_Map_Uncollapse.push_back(Map_Uncollapse);
                            BT_Propagation_Pos.push_back(Propagation_Pos);
                            usedPatternArray.push_back(lastSelectedPattern);
                            BT_step++;
                        }
                    }
                    else {
                        usedPatternArray.push_back(lastSelectedPattern);
                    }
                    //printMapWithCollapseMark(unCollapseMap, lastSelectedPattern.coordinate, Y, PosibleTiles.size(), RPP, true, false);
                }
            } while (Propagation_Pos.size() > 1);

        }
        for (int i = 0; i < reserveRPP.size(); i++) {
            Propagation_Pos.push_back(reserveRPP[i]);
        }
        //searchForRPP(unCollapseMap, RPP);
        std::sort(Propagation_Pos.begin(), Propagation_Pos.end());
        Propagation_Pos.erase(std::unique(Propagation_Pos.begin(), Propagation_Pos.end()), Propagation_Pos.end());
        //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, true);
        //ControlPoint(0);
        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        //std::exit(0);
        int controlPointN = 0;

        while (!mapCompleted(Map_Uncollapse)) {
            //std::cout << PURPLE << "step: " << step << RESET << std::endl;
            //seleccionar una casilla para colpasar
            //std::cout << GREEN << "SELECCIONAR " << RESET << std::endl;
            if (randomStart) {
                lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
                randomStart = false;
                //std::cout << YELLOW << "Posicion aleatoria: " << lowestEntropyTilePos << RESET << std::endl;
            }
            else {
                lowestEntropyTilePos = selectLowestEntropyTile(Map_Uncollapse, Posible_Tiles.size(), lowestEntropyTilePos, Propagation_Pos);
                //std::cout << YELLOW << "Posicion con la entropia m�s baja: " << lowestEntropyTilePos << RESET << std::endl;
            }

            //std::cout << GREEN << "COLAPSAR" << RESET << std::endl;
            if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, patternArrayLow, lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), true, printMapBool)) {
               //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, false);
                //std::cout << " " << std::endl;
                //std::cout << GREEN << "PROPAGAR" << RESET << std::endl;
                //propagate(unCollapseMap, RPP, patternArrayLow, lowestEntropyTilePos, Y);
                //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, false);
                
                //Guardado de Backtracking
                if (backtrackingActive) {
                    if (Propagation_Pos.size() > 0) {
                        BT_Map_Uncollapse.push_back(Map_Uncollapse);
                        BT_Propagation_Pos.push_back(Propagation_Pos);
                        usedPatternArray.push_back(lastSelectedPattern);
                        BT_step++;
                    }
                }
                else {
                    usedPatternArray.push_back(lastSelectedPattern);
                }
            }
            if (Propagation_Pos.size() == 0 && !mapCompleted(Map_Uncollapse)) {
                if (backtrackingActive) {
                    if (BT_step - backtrackUses > 0) {
                        BT_step = BT_step - backtrackUses;
                        Map_Uncollapse = BT_Map_Uncollapse[BT_step - 1];
                        Propagation_Pos = BT_Propagation_Pos[BT_step - 1];
                        BT_Map_Uncollapse.resize(BT_step - 1);
                        BT_Propagation_Pos.resize(BT_step - 1);
                        usedPatternArray.resize(BT_step - 1);
                        backtrackUses++;
                    }
                    else {
                        initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);
                        usedPatternArray.clear();
                        totalBacktracking += backtrackUses;
                        BT_step = 0;
                        backtrackUses = 0;
                    }
                    if (Propagation_Pos.size() == 0) {
                        searchForRPP(Map_Uncollapse,Propagation_Pos);
                    }
                }            
                else {
                    ControlString("Sin resultado, backtracking apagado, reiniciando");
                    break;
                }
                
            }
        }


        if (mapCompleted(Map_Uncollapse)) {
            std::cout << GREEN << "MAPA COMPLETADO EXITOSAMENTE" << RESET << std::endl;
        
            //fin del cronometro del tiempo de ejecucion
            auto end = std::chrono::high_resolution_clock::now();
            long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            totalBacktracking += backtrackUses;

            printMap(Map_Uncollapse, Map_Size, Posible_Tiles.size(), Propagation_Pos, false);

            std::cout << "Tiempo de ejecucion: " << duration << " ms" << std::endl;
            std::cout << "usos del backtracking: " << totalBacktracking << std::endl;

            //definicion de metricas
            //KL Divergence

            //construccion de una nueva imagen
            reconstructMap(Pixel_Vector_Out, Map_Uncollapse, Posible_Tiles);
            std::cout << "Mapa reconstruido exitosamente." << std::endl;

            //guardado de la imagen en un nuevo archivo

            // Metodo tamaño (numero de generacion)
            
            findUniquePattern(usedPatternArray);
            size_t dotPos = Example_Map.find('.');
            std::string nameWithoutExt = "";
            if (dotPos != std::string::npos) {
                // Extraer la parte del nombre de archivo antes del punto
                nameWithoutExt = Example_Map.substr(0, dotPos);
            }
            std::cout << "Guardando y procesando la informacion en archivos..." << std::endl;

            SaveMapAndTime(Base_Folder, Pixel_Vector_Out, usedPatternArray, mode + "_" + nameWithoutExt, Map_Size, Posible_Tiles, duration, backtrackUses);
            std::cout << "Guardando completado." << std::endl;
            //dibujar los patrones en una imagen aparte
            initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);
            Map_Requested_completed_i++;
        }
        
        Pixel_Vector_Out.clear();
        usedPatternArray.clear();
        Propagation_Pos.clear();
        BT_Map_Uncollapse.clear();
        BT_Propagation_Pos.clear();
        backtrackUses = 0;
        totalBacktracking = 0;
        BT_step = 0;
        

    }while (Map_Requested_i > Map_Requested_completed_i);

    PerformMetrics(Base_Folder, Desire_Size);
    createPatternDraw(patternArrayLow, Pattern_Vector_Out, Map_Size);
    if (writeImagePPM("patron_Generada.ppm", Map_Size, Map_Size, Pattern_Vector_Out)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    

    return 0;
}