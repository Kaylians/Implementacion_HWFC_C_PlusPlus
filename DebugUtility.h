#ifndef DEBUG_H
#define DEBUG_H

#include "Pattern.h"
#include "Pixel.h"

#include <iostream>
#include <vector>
#include <string>

void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi);
void ControlPoint(int i);
void ControlString(std::string text);

#endif 