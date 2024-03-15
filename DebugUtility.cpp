#include "Pixel.h"
#include "Pattern.h"
#include "Main.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

bool SearchRPP(std::vector<int> RPP, int target) {
    for (int i = 0; i < RPP.size(); i++) {
        if (target == RPP[i]) {
            return true;
        }
    }
    return false;
}
//funciona para imprimir el mapa que son valores int
void printMap(const std::vector<std::vector<int>>& unCollapseMap, int size, int posibi, std::vector<int>& RPP, bool MarkRPP) {
    MarkRPP = true;
    for (int i = -1; i < size; i++) {
        if (i == -1) {
            std::cout << BLUE << " X" << " " << RESET << "|| ";
            for (int z = 0; z < size; z++) {
                if (z < 10) {

                    std::cout << BLUE << " " << z << " " << RESET << "|| ";
                }
                else {

                    std::cout << BLUE << z << " " << RESET << "|| ";
                }
            }
        }
        else {
            for (int j = -1; j < size; j++) {
                if (j == -1) {
                    if (i < 10) {

                        std::cout << " " << BLUE << i << RESET << " || ";
                    }
                    else {

                        std::cout << BLUE << i << RESET << " || ";
                    }
                }
                
                else if (unCollapseMap[j + i * size].size() == 1) {
                    if (unCollapseMap[j + i * size].front() < 10 && SearchRPP(RPP, j + i * size) && MarkRPP) {
                        if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 1)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 2)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 3)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 4)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                    else if (unCollapseMap[j + i * size].front() < 10) {
                        if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << GREEN << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 1)
                            std::cout << RED << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 2)
                            std::cout << BLUE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 3)
                            std::cout << WHITE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 4)
                            std::cout << YELLOW << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                    else {
                        std::cout << GREEN << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                }
                else if (SearchRPP(RPP, j + i * size) && MarkRPP)
                    std::cout << PURPLE << " " << "." << " " << RESET << "|| ";

                else if (unCollapseMap[j + i * size].size() == 0) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() == posibi) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() < posibi)
                    std::cout << PURPLE << " " << unCollapseMap[j + i * size].size() << RESET << " || ";
                else
                    std::cout << "X ||";
            }
        }

        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
void printMapWithCollapseMark(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int> lastCollapse,int size, int posibi, std::vector<int>& RPP, bool MarkRPP) {
    MarkRPP = true;
    bool collapseMark = false;
    for (int i = -1; i < size; i++) {
        if (i == -1) {
            std::cout << BLUE << " X" << " " << RESET << "|| ";
            for (int z = 0; z < size; z++) {
                if (z < 10) {

                    std::cout << BLUE << " " << z << " " << RESET << "|| ";
                }
                else {

                    std::cout << BLUE << z << " " << RESET << "|| ";
                }
            }
        }
        else {
            for (int j = -1; j < size; j++) {
                if (j == -1) {
                    if (i < 10) {
                        std::cout << " " << BLUE << i << RESET << " || ";
                    }
                    else {

                        std::cout << BLUE << i << RESET << " || ";
                    }
                }
                else if (unCollapseMap[j + i * size].size() == 1) {

                    for (int h = 0; h < lastCollapse.size(); h++) {
                        collapseMark = false;
                        if (j + i * size == lastCollapse[h]) {
                            collapseMark = true;
                            break;
                        }
                    }
                    if (unCollapseMap[j + i * size].front() < 10 && SearchRPP(RPP, j + i * size) && MarkRPP) {
                        if (collapseMark)
                            std::cout << CYAN << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 1)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 2)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 3)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 4)
                            std::cout << PURPLE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                    else if (unCollapseMap[j + i * size].front() < 10) {
                        if (collapseMark)
                            std::cout << CYAN << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 0)
                            std::cout << GREEN << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 1)
                            std::cout << RED << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 2)
                            std::cout << BLUE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 3)
                            std::cout << WHITE << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                        else if (unCollapseMap[j + i * size].front() == 4)
                            std::cout << YELLOW << " " << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                    else {
                        std::cout << GREEN << unCollapseMap[j + i * size].front() << "*" << RESET << "|| ";
                    }
                }
                else if (SearchRPP(RPP, j + i * size) && MarkRPP)
                    std::cout << PURPLE << " " << "." << " " << RESET << "|| ";

                else if (unCollapseMap[j + i * size].size() == 0) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() == posibi) {
                    std::cout << "  " << " || ";
                }
                else if (unCollapseMap[j + i * size].size() < posibi)
                    std::cout << PURPLE << " " << unCollapseMap[j + i * size].size() << RESET << " || ";
                else
                    std::cout << "X ||";
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