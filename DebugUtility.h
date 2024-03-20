#ifndef DEBUG_H
#define DEBUG_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi, std::vector<int>& RPP, bool MarkRPP);
void printMapWithCollapseMark(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int> lastCollapse, int size, int posibi, std::vector<int>& RPP, bool MarkRPP, bool MarkCollapse);
void ControlPoint(int i);
void ControlPointNoText(int i);
void ControlString(std::string text);

#endif 


/*
   

*/