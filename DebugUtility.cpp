#include "Pixel.h"
#include "Pattern.h"
#include "Main.h"
#include "DebugUtility.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

bool SearchRPP(std::vector<int> RPP, int target) {
    for (int i = 0; i < RPP.size(); i++) {
        if (target == RPP[i]) {
            return true;
        }
    }
    return false;
}
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
//funciona para imprimir el mapa que son valores int
void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi, std::vector<int>& RPP, bool MarkRPP) {
    MarkRPP = true;
    for (int i = -1; i < size; i++) {
        if (i == -1) {
            std::cout << BLUE << " X" << RESET << "||";
            for (int z = 0; z < size; z++) {
                if (z < 10) {

                    std::cout << BLUE << " " << z  << RESET << "||";
                }
                else {

                    std::cout << BLUE << z << RESET << "||";
                }
            }
        }
        else {
            for (int j = -1; j < size; j++) {
                if (j == -1) {
                    if (i < 10) {

                        std::cout << " " << BLUE << i << RESET << "||";
                    }
                    else {

                        std::cout << BLUE << i << RESET << "||";
                    }
                }
                else if (unCollapseMap[j + i * size].size() == 1) {
                    if (SearchRPP(RPP, j + i * size) && MarkRPP) {
                        if (unCollapseMap[j + i * size].front() > 9)
                            std::cout << PURPLE << unCollapseMap[j + i * size].front() << RESET << "||";
                        else
                            std::cout << PURPLE << unCollapseMap[j + i * size].front() << "*" << RESET << "||";
                    }
                    else {
                        if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << GREEN  << unCollapseMap[j + i * size].front() << "*" << RESET << "||";
                        else if (unCollapseMap[j + i * size].front() > 9)
                            std::cout << RED  << unCollapseMap[j + i * size].front() << RESET << "||";
                        else
                            std::cout << RED << unCollapseMap[j + i * size].front() << "*" << RESET << "||";
                    }
                }
                else if (unCollapseMap[j + i * size].size() == posibi) {
                    std::cout << "  " << "||";
                }
                else if (unCollapseMap[j + i * size].size() < posibi) {
                    if (unCollapseMap[j + i * size].size() == 0)
                        std::cout << RED << "!X" << RESET << "||";
                    else if(unCollapseMap[j + i * size].size() > 9)
                        std::cout << YELLOW << unCollapseMap[j + i * size].size() << RESET << "||";
                    else 
                        std::cout << YELLOW << unCollapseMap[j + i * size].size() << "*" << RESET << "||";
                }
                else
                    std::cout << "X||";
            }
        }

        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
void ControlPoint(int i) {
    std::cout << PURPLE << "Control point " << YELLOW<< i << RESET << std::endl;
}
void ControlPointNoText(int i) {
    std::cout << RED << i << RESET << std::endl;
}
void ControlString(std::string text) {
    std::cout << YELLOW << text << RESET << std::endl;
}
void PrintPixelCoo(Pattern patt) {
    for (int element : patt.pixelesCoo) {
        std::cout << element;
    }
    std::cout << std::endl;
}