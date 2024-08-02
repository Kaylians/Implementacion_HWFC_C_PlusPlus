#ifndef RW_H
#define RW_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <unordered_set>

#include "Pixel.h"
#include "Pattern.h"
#include "WFC.h"
#include "DebugUtility.h"

namespace fs = std::filesystem;

void getPredefineTiles(std::vector<Pixel>& posibleTiles);

void loadPatternInfo(const std::string& mode, std::vector<std::vector<Pattern>>& H_patternArray, std::vector<Pattern>& patternArrayHigh, std::vector<Pattern>& patternArrayLow, const std::vector<int>& desire_Size);

//Función para la lectura directa de .txt
void read_Example_Folder(const std::string& mode, std::vector<std::vector<Pattern>>& H_patternArray, const std::vector<Pixel>& posibleTiles, const std::vector<int>& desire_Size);

void infoPatternUpdateIDPython(std::vector<std::vector<Pattern>>& H_patternArray);

// Función para la lectura de la imagen de ejemplo
bool read_Example_PPM(const std::string& r, int& w, int& h, std::vector<Pixel>& pixeles);

// Función para la escritura de una nueva imagen
bool writeImagePPM(const std::string& r, int w, int h, const std::vector<Pixel>& pixeles);

// Función para la reconstrucción del mapa
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles);

//funcion para reconstruir una imagen a partir del mapa generado
void createPatternDraw(const std::vector<Pattern>& pattern, std::vector<Pixel>& pixelVector, int& Y);

void SaveMapAndTime(const std::string& baseFolder, const std::vector<Pixel>& data, const std::vector<Pattern>& dataPattern, const std::string mode, const int size, const std::vector<Pixel>& posibleTiles, const float& duration, const int& backtrackingUse, const int fail_generation);

void PerformMetrics(const std::string& baseFolder, const std::vector<int>& N, const int Map_size);

bool SaveInfo_CSV_KLD(const std::vector<int>& N, const std::string& carpetaBase);

std::vector<std::string> ObtenerNombresArchivos(const std::string& carpeta, const std::string& nombreBase, const std::string& fileType);

#endif 