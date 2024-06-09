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
#include "Solver.h"
#include <thread>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

//funcion para inicializar la generaci�n de numeros aleatorios
void initializeRandomSeed() {
    std::srand(std::time(0));
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

//definir cuantas posibles estados puede tomar un pixel y cuantas veces aparece
void define_Posible_Tiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
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
    





    //lectura de la imagen de entrada
    if (!readImagePPM(Example_Map, inputImageWidth, inputImageHeight, Pixel_Vector))
        exit(0);
    



    //defincion de las casillas
    define_Posible_Tiles(Pixel_Vector, Posible_Tiles);
    
    //inicio del algoritmo WFC




    std::vector<Pattern> patternArrayBase, patternArrayLow, patternArrayHigh;

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
        

        generate_Map(mode, Desire_Top_Size,Posible_Tiles,Map_Size,Top_Size_i,printMapBool,backtrackingActive,patternArrayBase,patternArrayLow,patternArrayHigh,Base_Folder,Example_Map);

        Map_Requested_completed_i++;
       
    }while (Map_Requested_i > Map_Requested_completed_i);

    PerformMetrics(Base_Folder, Desire_Size);
    createPatternDraw(patternArrayLow, Pattern_Vector_Out, Map_Size);
    if (writeImagePPM("patron_Generada.ppm", Map_Size, Map_Size, Pattern_Vector_Out)) {
        std::cout << "Imagen PPM escrita exitosamente." << std::endl;
    }
    

    return 0;
}