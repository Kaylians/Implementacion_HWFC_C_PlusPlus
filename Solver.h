#ifndef SOLVER_H
#define SOLVER_H

#include "Pixel.h"
#include "WFC.h"
#include "HWFC.h"
#include "Pattern.h"
#include "ReadWrite.h"
#include "DebugUtility.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>

bool generate_Map( std::string mode,
     std::vector<int>& Desire_Top_Size,
    std::vector<int>& Desire_Size,
     std::vector<Pixel>& Posible_Tiles,
     int Map_Size,
     int Top_Size_i,
     bool printMapBool,
     bool backtrackingActive,
     std::vector<std::vector<Pattern>>& H_patternArray,
     std::string& Base_Folder,
     std::string& Example_Map,
    long pattern_time
);

#endif 