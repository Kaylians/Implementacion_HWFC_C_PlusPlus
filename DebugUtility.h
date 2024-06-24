#ifndef DEBUG_H
#define DEBUG_H

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define WHITE    "\033[37m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"

#define DGRAY    "\033[90m" 
#define GRAY "\033[97m"
#define BROWN "\033[38;5;94m"  // Approximated brown (often used as a replacement)
#define ORANGE "\033[38;5;214m"

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

void stopExecute(int times, std::string texto);
void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi, std::vector<int>& RPP, bool MarkRPP);
void printMapWithCollapseMark(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int> lastCollapse, int size, int posibi, std::vector<int>& RPP, bool MarkRPP, bool MarkCollapse);
void ControlPoint(int i);
void ControlPointNoText(int i);
void ControlString(std::string text);
void PrintPixelCoo(Pattern patt);
#endif 
