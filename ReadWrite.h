#ifndef RW_H
#define RW_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Pixel.h"
#include "Pattern.h"

// Función para la lectura de la imagen de ejemplo
bool readImagePPM(const std::string& r, int& w, int& h, std::vector<Pixel>& pixeles);

// Función para la escritura de una nueva imagen
bool writeImagePPM(const std::string& r, int w, int h, const std::vector<Pixel>& pixeles);

// Función para la reconstrucción del mapa
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles);

void createPatternDraw(const std::vector<Pattern>& pattern, std::vector<Pixel>& pixelVector, int& Y);

#endif 