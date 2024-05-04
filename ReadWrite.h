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

#include <algorithm>
#include <unordered_set>

#include "Pixel.h"
#include "Pattern.h"
#include "DebugUtility.h"

namespace fs = std::filesystem;

// Función para la lectura de la imagen de ejemplo
bool readImagePPM(const std::string& r, int& w, int& h, std::vector<Pixel>& pixeles);

// Función para la escritura de una nueva imagen
bool writeImagePPM(const std::string& r, int w, int h, const std::vector<Pixel>& pixeles);

// Función para la reconstrucción del mapa
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles);

//funcion para reconstruir una imagen a partir del mapa generado
void createPatternDraw(const std::vector<Pattern>& pattern, std::vector<Pixel>& pixelVector, int& Y);

void SaveInfoOnFileAndMetrics(const std::vector<Pixel>& pixeles, const std::vector<Pattern>& usedPattern, const std::string mode, const int size, const std::vector<Pixel>& posibleTiles);

std::vector<std::string> ObtenerNombresArchivos(const std::string& carpeta, const std::string& nombreBase, const std::string& fileType);

#endif 