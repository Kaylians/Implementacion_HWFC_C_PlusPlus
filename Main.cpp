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

// gdb ./Main
// r (argumento)
// valgrind ./WFC
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./WFC
// valgrind --tool=cachegrind ./WFC
*/


// g++ -O3 Main.cpp Solver.cpp pattern.cpp HWFC.cpp WFC.cpp DebugUtility.cpp ReadWrite.cpp Metrics.cpp -o Main -lboost_program_options
//
// ./Main --mode "HWFC" --example "example.ppm" --desire_size 2 --top_size 8 --top_amount 1 --size 20 --amount 2
// 
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

#include <boost/program_options.hpp>

namespace po = boost::program_options;

//funcion para inicializar la generaci�n de numeros aleatorios
void initializeRandomSeed() {
    std::srand(std::time(0));
}

//definir cuantas posibles estados puede tomar un pixel y cuantas veces aparece
void define_Posible_Tiles(const std::vector<Pixel>& pixelVector, std::vector<Pixel>& posibleTiles) {
    posibleTiles.push_back(pixelVector.front());
    for (const auto& pixel : pixelVector)
        if (!Pixel::contienePixel(posibleTiles, pixel))
            posibleTiles.push_back(pixel);
}

std::string CutStringBeforeChar(std::string& string, const char pointer, const bool Cut_Before) {
    size_t dotPos = string.find(pointer);
    std::string nameWithoutExt = "";
    if (Cut_Before) {
        if (dotPos != std::string::npos) {
            // Extraer la parte del nombre de archivo antes del punto
            nameWithoutExt = string.substr(0, dotPos);
        }
    }
    else {
        if (dotPos != std::string::npos) {
            // Extraer la parte del nombre de archivo antes del punto
            nameWithoutExt = string.substr(dotPos, string.length());
        }
    }
    
    return nameWithoutExt;
}

void input_Boolean(bool& boolean, const std::string& Display) {
    ControlString(Display);
    int inputValue = -1;
    do {
        try {
            std::cout << "[1 Si / 2 No]";
            std::cin >> inputValue;
        }
        catch (const std::exception& e) {
            std::cerr << "Error, valor de ingreso no valido" << e.what() << std::endl;
        }

    } while (inputValue != 1 && inputValue != 2);

    if (inputValue == 1) 
        boolean = true;
    else 
        boolean = false;
}

int main(int argc, char* argv[]) {
    std::vector<int> Desire_Size;
    std::vector<int> Desire_Top_Size;
    std::string Example_Map;
    std::string mode;
    int Map_Size = 20, Top_Size_i = 10, Map_Requested_i = 1, Map_Requested_completed_i = 0, Image_Width, Image_Height;

    //variables de almacenamiento de pixeles
    std::vector<Pixel> Pixel_Vector, Pixel_Vector_Out, Pattern_Vector_Out, Posible_Tiles;

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
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    initializeRandomSeed();

    bool backtrackingActive = true, printMapBool;

    input_Boolean(printMapBool, "¿Mostrar el proceso de generación del mapa? ");
    std::string Output_Folder_base = "generatedLevels/";
    std::string Output_Folder = Output_Folder_base + mode + "_size_" + std::to_string(Map_Size) + "_" + Example_Map;

    //crear carpeta base "generatedLevels para guardar todos los resultados"
    try {
        if (std::filesystem::create_directory(Output_Folder_base)) {
            std::cout << "Carpeta creada exitosamente." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error al crear la carpeta: " << e.what() << std::endl;
    }

    std::vector<Pattern> patternArrayLow, patternArrayHigh;
    std::vector<std::vector<Pattern>> H_patternArray;
    std::sort(Desire_Size.begin(), Desire_Size.end(), std::greater<int>());

    if (mode == "WFC") {
        std::vector<int> fix_desire_size;
        fix_desire_size.push_back(Desire_Size[0]);
        Desire_Size = fix_desire_size;
        fix_desire_size.clear();
    }

    //separación del nombre reservado "Python" para efectuar lectura directa de los .txt usados en Python
    if (Example_Map == "Python") {
        getPredefineTiles(Posible_Tiles);
        read_Example_Folder(mode, H_patternArray, Posible_Tiles, Desire_Size);
        infoPatternUpdateIDPython(H_patternArray);
        //inicio del algoritmo y generación de la cantidad de mapas solicitados

        long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        do {
            if (generate_Map(mode,Desire_Top_Size, Desire_Size, Posible_Tiles, Map_Size, Top_Size_i, printMapBool, backtrackingActive, H_patternArray, Output_Folder, Example_Map, duration)) {
                Map_Requested_completed_i++;
            }
        } while (Map_Requested_i > Map_Requested_completed_i);
    }
    //uso de imagen .ppm como ejemplo
    else { 
        std::string folder = "examples/" + Example_Map;
        if (!read_Example_PPM(folder, Image_Width, Image_Height, Pixel_Vector))
            exit(0);
        //defincion de las casillas
        define_Posible_Tiles(Pixel_Vector, Posible_Tiles);

        //DEFINICION DE LOS PATRONES
        
        if (mode == "HWFC") {
            //patrones de multiples tama�os para uso alto
            std::vector<Pattern> H_patt = definePatternsHWFC(Pixel_Vector, Posible_Tiles, Image_Height, Image_Width, Desire_Top_Size);
            H_patternArray.push_back(H_patt);
        }
        std::vector<std::vector<Pattern>> patt;
        patt = definePatternsWFC(Pixel_Vector, Posible_Tiles, Image_Height, Image_Width, Desire_Size);

        for (int i = 0; i < patt.size(); i++) {
            H_patternArray.push_back(patt[i]);
        }
        infoPatternUpdateIDPython(H_patternArray);
        //inicio del algoritmo y generación de la cantidad de mapas solicitados
        long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        do {
            if (generate_Map(mode, Desire_Top_Size, Desire_Size, Posible_Tiles, Map_Size, Top_Size_i, printMapBool, backtrackingActive, H_patternArray, Output_Folder, Example_Map, duration)) {
                Map_Requested_completed_i++;
            }
        } while (Map_Requested_i > Map_Requested_completed_i);
    }

    PerformMetrics(Output_Folder, Desire_Size, Map_Size);

    return 0;
}