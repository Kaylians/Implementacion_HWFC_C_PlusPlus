#include "Solver.h"

//funcion para inicializar el mapa
void initialize_Map(std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& posibleTiles, int Y) {
    std::vector<int> tmp;
    unCollapseMap.clear();
    for (int i = 0; i < Y * Y; i++) {
        for (int j = 0; j < posibleTiles.size(); j++) {
            tmp.push_back(j);
        }
        unCollapseMap.push_back(tmp);
        tmp.clear();
    }
}
//funcion para obtener numeros aleatorios con min y max
int getRandom(int min, int max) {
    int rnd = 0;
    rnd = std::rand() % max;
    return rnd;
}
//funcion random para elegir el patron que m�s se repite
int getRandomPatternWeighted(const std::vector<Pattern> pattern, int weightMultiplier) {

    std::vector<double> pesos(pattern.size());
    for (size_t i = 0; i < pattern.size(); i++) {
        pesos[i] = pattern[i].weight * weightMultiplier;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution(pesos.begin(), pesos.end());
    return distribution(gen);
}
//funcion para elegir la casilla con entropia (posibles colores) disponible en el mapa
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int Posibi, int LastLowestEntropyTilePos, const std::vector<int>& RPP) {
    int lowestValue = Posibi + 1, lowestID = -1;
    //std::cout << "cantidad de regiones de propagacion: " << RPP.size() << std::endl;
    if (RPP.size() > 0) {
        //std::cout << RED << "seleccionar entre regiones colapsadas" << RESET << std::endl;
        lowestID = RPP[getRandom(0, RPP.size())];
        lowestValue = unCollapseMap[lowestID].size();
        for (int i = 0; i < RPP.size(); i++) {
            if (lowestValue > unCollapseMap[RPP[i]].size()) {
                lowestID = RPP[i];
                lowestValue = unCollapseMap[RPP[i]].size();
            }
        }
    }
    else {
        //std::cout << RED << "Buscar entre posiciones de baja entropia" << RESET << std::endl;
        for (int i = 0; i < unCollapseMap.size(); i++) {
            //si es que hay m�s de una posibilidad para la casilla
            if (unCollapseMap[i].size() > 1)
                if (unCollapseMap[i].size() < lowestValue) {
                    lowestID = i;
                    lowestValue = unCollapseMap[i].size();
                }
        }
    }
    return lowestID;
}
//función simple para buscar por espacios de propagacion
void searchForRPP(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() > 1) {
            RPP.push_back(i);
        }
    }
}
//funcion para colapsar una posicion a una patron en concreto que coincida por cada pixel con los valores adyacentes al punto

//funciones para confirmar si el patron coincide en cada uno de sus puntos con el espacio alrededor del punto entregado
bool selectPatternAnalizer(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, int posibleTilesN, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
    bool contains = false;
    std::vector<int> mapCoo;
    int count = 0;
    for (int i = i_min, x = 0; i <= i_max; i++, x++) {
        for (int j = j_min, y = 0; j <= j_max; j++, y++) {
            for (int z = 0; z < unCollapseMap[pos + j + i * Y].size(); z++) {
                //si la casilla esta con todas las posibilidades disponibles

                if (unCollapseMap[pos + j + i * Y].size() == posibleTilesN) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }


                //Si todas las casillas ya estan colapsadas, return false 
                if (unCollapseMap[pos + j + i * Y].size() == 1) {
                    count++;
                    if (count == N * N) {
                        return false;
                    }
                }
                //si la casilla corresponde al espacio vacio de un patron de alta jerarquia
                if (pattern.pixelesCoo[y + x * N] == -1) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }
                //si la casilla contiene la posibilidad de la casilla corespondiente al patron
                if (pattern.pixelesCoo[y + x * N] == unCollapseMap[pos + j + i * Y][z]) {
                    mapCoo.push_back(pos + j + i * Y);
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                mapCoo.clear();
                return false;
            }
            else {
                contains = false;
            }

        }
    }
    pattern.coordinate = mapCoo;
    return true;
}
bool selectPattern(Pattern& pattern, std::vector<Pattern>& pattArray, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos, int posibleTilesN, bool forceCenter) {
    bool U = false, S = false, E = false, W = false, C = false, contains = false;
    //center, norte, sur, este , oeste
    if (N % 2 == 1)
        if (pos % Y >= (N / 2) && pos % Y < Y - (N / 2) && pos / Y >= (N / 2) && pos / Y < Y - (N / 2)) {
            if (forceCenter) {
                for (int i = 0; i < pattArray.size(); i++) {
                    if (selectPatternAnalizer(pattArray[i], unCollapseMap, posibleTilesN, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                        pattern = pattArray[i];
                        return true;
                    }
                }
            }
            else if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N / 2), (N / 2), -(N / 2), (N / 2))) {
                return true;
            }
        }

    //añadir mejor sistema de localizacion de los patrones aquí

    if (pos / Y >= N - 1)
        U = true;
    if (pos / Y < Y - (N - 1))
        S = true;
    if (pos % Y >= N - 1)
        W = true;
    if (pos % Y < Y - (N - 1))
        E = true;

    if (U) {
        for (int i = 0; i < N; i++) {
            if (W) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    else if (S) {
        for (int i = 0; i < N; i++) {
            if (W) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
            else if (E) {
                for (int j = 0; j < N; j++) {
                    if (selectPatternAnalizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, 0, N - 1)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Propagate(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, const int Y, int posibleTilesN) {

    //propagacion

    bool U = false, S = false, E = false, O = false, extendRPP = true;
    int colide;
    std::vector<int> tmpRPP;
    std::vector<int> tmpEraseRPP;
    for (int i = 0; i < RPP.size(); i++) {
        //std::cout << RPP[i];
        if (unCollapseMap[RPP[i]].size() < posibleTilesN) {
            U = false, S = false, E = false, O = false, colide = 0;
            if (RPP[i] / Y > 0) {
                if (unCollapseMap[RPP[i] - Y].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] - Y);
                    tmpRPP.push_back(RPP[i]);
                    U = true;
                }
            }
            if (RPP[i] / Y < Y - 1) {
                if (unCollapseMap[RPP[i] + Y].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] + Y);
                    tmpRPP.push_back(RPP[i]);
                    S = true;
                }
            }
            if (RPP[i] % Y < Y - 1) {
                if (unCollapseMap[RPP[i] + 1].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] + 1);
                    tmpRPP.push_back(RPP[i]);
                    E = true;
                }
            }
            if (RPP[i] % Y < Y && RPP[i] % Y > 0) {
                if (unCollapseMap[RPP[i] - 1].size() > 1) {
                    if (extendRPP)
                        tmpRPP.push_back(RPP[i] - 1);
                    tmpRPP.push_back(RPP[i]);
                    O = true;
                }
            }

            if (!U && !S && !E && !O && unCollapseMap[RPP[i]].size() == 1) {
                tmpEraseRPP.push_back(RPP[i]);
            }
        }
    }
    for (int i = 0; i < tmpEraseRPP.size(); i++) {
        auto it = std::find(RPP.begin(), RPP.end(), tmpEraseRPP[i]);

        if (it != RPP.end())
            RPP.erase(it);
    }
    for (int i = 0; i < tmpRPP.size(); i++) {
        RPP.push_back(tmpRPP[i]);
    }
    tmpRPP.clear();

    std::sort(RPP.begin(), RPP.end());
    RPP.erase(std::unique(RPP.begin(), RPP.end()), RPP.end());
}

bool Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, std::vector<int>& reserveRPP, const int Y, std::vector<Pattern>& pattern, Pattern& selectedPatt, int pos, const int posibleTilesN, bool printMapBool) {
    auto newPattern = pattern.front();
    int stuck_Counter = 0;
    std::vector<int> usedPatterns;
    std::vector<int> uncollapseNode;
    for (int h = 0; h < posibleTilesN; h++)
        uncollapseNode.push_back(h);
    auto iterador = usedPatterns.end();
    bool finded;
    do {
        do {
            if (usedPatterns.size() == pattern.size()) {
                auto i = std::find(RPP.begin(), RPP.end(), pos);
                if (i != RPP.end()) {
                    int posicion = std::distance(RPP.begin(), i);
                    if (unCollapseMap[posicion].size() > 1)
                        reserveRPP.push_back(posicion);
                    RPP.erase(i);
                }
                return false;
            }
            newPattern = pattern[getRandomPatternWeighted(pattern, 3)];
            iterador = std::find(usedPatterns.begin(), usedPatterns.end(), newPattern.id);
        } while (iterador != usedPatterns.end());
        usedPatterns.push_back(newPattern.id);

        if (selectPattern(newPattern, pattern, unCollapseMap, newPattern.N, Y, pos, posibleTilesN, false)) {
            finded = true;
        }
        else {
            finded = false;
        }
        stuck_Counter++;
    } while (!finded);
    stuck_Counter = 0;

    for (int i = 0; i < newPattern.N; i++) {
        for (int j = 0; j < newPattern.N; j++) {
            if (unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].size() > 1) {
                unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].clear();
                //convierte cada pixel del arreglo a su respectivo valor numerico que lo representa dentro de un collapseMap
                if (newPattern.pixelesCoo[j + newPattern.N * i] == -1) {
                    unCollapseMap[newPattern.coordinate[j + newPattern.N * i]] = uncollapseNode;
                }
                else {
                    unCollapseMap[newPattern.coordinate[j + newPattern.N * i]].push_back(newPattern.pixelesCoo[j + newPattern.N * i]);
                    RPP.push_back(newPattern.coordinate[j + newPattern.N * i]);
                }
            }
        }
    }

    selectedPatt = newPattern;

    Propagate(unCollapseMap, RPP, Y, posibleTilesN);
    printMap(unCollapseMap,Y,posibleTilesN,RPP,true);

    return true;
}
//verifica si todas las posiciones del mapa estan colapsadas
bool mapCompleted(const std::vector<std::vector<int>>& unCollapseMap) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}

void generate_Map( std::string mode,
    std::vector<int>& Desire_Top_Size,
    std::vector<Pixel>& Posible_Tiles,
    int Map_Size,
    int Top_Size_i,
    bool printMapBool,
    bool backtrackingActive,
    std::vector<Pattern>& patternArrayBase,
    std::vector<Pattern>& patternArrayLow,
    std::vector<Pattern>& patternArrayHigh,
    std::string& Base_Folder,
    std::string& Example_Map
) {
    std::vector<Pixel> Pixel_Vector_Out;
    std::vector<Pattern> usedPatternArray;
    std::vector<std::vector<int>> Map_Uncollapse;
    std::vector<int> Propagation_Pos, reserveRPP;
    Propagation_Pos.resize(0); reserveRPP.resize(0);

    initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);

    //almacen de informaci�n para backtracking
    std::vector<std::vector<std::vector<int>>> BT_Map_Uncollapse; std::vector<std::vector<int>> BT_Propagation_Pos;
    int BT_step = 0, backStep = 0, backtrackUses = 0, totalBacktracking = 0;
    Pattern lastSelectedPattern(0, 0);

    int lowestEntropyTilePos;
    bool randomStart = true;

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Iniciando creacion de nuevo mapa." << std::endl;

    //EJECUCION DE JERARQUIA ALTA EN HWFC
    if (mode == "HWFC") {
        //definir patron de mayor tamaño sobre el cual iterar
        int HN_max = 0;
        for (int i = 0; i < Desire_Top_Size.size(); i++)
            if (HN_max < Desire_Top_Size[i])
                HN_max = Desire_Top_Size[i];

        for (int i = 0; i < Top_Size_i; i++) {
            //busqueda y posicionamiento de patrones de alta jerarquia
            do {
                lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
            } while (!HPattValidTile(lowestEntropyTilePos, Map_Size, Map_Size, HN_max) && Map_Uncollapse[lowestEntropyTilePos].size() > 1);
            ControlString("Activo 1");
            if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, patternArrayHigh, lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), printMapBool)) {
                ControlString("Activo 2");
                //Guardado de Backtracking
                if (backtrackingActive) {
                    if (Propagation_Pos.size() > 0) {
                        BT_Map_Uncollapse.push_back(Map_Uncollapse);
                        BT_Propagation_Pos.push_back(Propagation_Pos);
                        BT_step++;
                    }
                }
            }
            else i--;
        }
        //stopExecute(2000, "patrones de media jerarquia...");
        bool condition = true;

    }
    for (int i = 0; i < reserveRPP.size(); i++) {
        Propagation_Pos.push_back(reserveRPP[i]);
    }
    //searchForRPP(unCollapseMap, RPP);
    std::sort(Propagation_Pos.begin(), Propagation_Pos.end());
    Propagation_Pos.erase(std::unique(Propagation_Pos.begin(), Propagation_Pos.end()), Propagation_Pos.end());
    //printMap(unCollapseMap, Y, PosibleTiles.size(), RPP, true);
    //ControlPoint(0);
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //std::exit(0);
    int controlPointN = 0;

    while (!mapCompleted(Map_Uncollapse)) {
        if (randomStart) {
            lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
            randomStart = false;
        }
        else {
            lowestEntropyTilePos = selectLowestEntropyTile(Map_Uncollapse, Posible_Tiles.size(), lowestEntropyTilePos, Propagation_Pos);
        }

        if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, patternArrayLow, lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), printMapBool)) {
            Propagate(Map_Uncollapse, Propagation_Pos, Map_Size, Posible_Tiles.size());
            
            //Guardado de Backtracking
            if (backtrackingActive) {
                if (Propagation_Pos.size() > 0) {
                    BT_Map_Uncollapse.push_back(Map_Uncollapse);
                    BT_Propagation_Pos.push_back(Propagation_Pos);
                    usedPatternArray.push_back(lastSelectedPattern);
                    BT_step++;
                }
            }
            else {
                usedPatternArray.push_back(lastSelectedPattern);
            }
        }
        if (Propagation_Pos.size() == 0 && !mapCompleted(Map_Uncollapse)) {
            if (backtrackingActive) {
                if (BT_step - backtrackUses > 0) {
                    BT_step = BT_step - backtrackUses;
                    Map_Uncollapse = BT_Map_Uncollapse[BT_step - 1];
                    Propagation_Pos = BT_Propagation_Pos[BT_step - 1];
                    BT_Map_Uncollapse.resize(BT_step - 1);
                    BT_Propagation_Pos.resize(BT_step - 1);
                    usedPatternArray.resize(BT_step - 1);
                    backtrackUses++;
                }
                else {
                    initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);
                    usedPatternArray.clear();
                    totalBacktracking += backtrackUses;
                    BT_step = 0;
                    backtrackUses = 0;
                }
                if (Propagation_Pos.size() == 0) {
                    searchForRPP(Map_Uncollapse, Propagation_Pos);
                }
            }
            else {
                ControlString("Sin resultado, backtracking apagado, reiniciando");
                break;
            }

        }
    }


    if (mapCompleted(Map_Uncollapse)) {

        //fin del cronometro del tiempo de ejecucion
        auto end = std::chrono::high_resolution_clock::now();
        long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        totalBacktracking += backtrackUses;

        printMap(Map_Uncollapse, Map_Size, Posible_Tiles.size(), Propagation_Pos, false);

        std::cout << "Tiempo de ejecucion: " << duration << " ms" << std::endl;
        std::cout << "usos del backtracking: " << totalBacktracking << std::endl;

        //definicion de metricas
        //KL Divergence

        //construccion de una nueva imagen
        reconstructMap(Pixel_Vector_Out, Map_Uncollapse, Posible_Tiles);
        std::cout << "Mapa reconstruido exitosamente." << std::endl;

        //guardado de la imagen en un nuevo archivo

        // Metodo tamaño (numero de generacion)

        findUniquePattern(usedPatternArray);
        size_t dotPos = Example_Map.find('.');
        std::string nameWithoutExt = "";
        if (dotPos != std::string::npos) {
            // Extraer la parte del nombre de archivo antes del punto
            nameWithoutExt = Example_Map.substr(0, dotPos);
        }
        std::cout << "Guardando y procesando la informacion en archivos..." << std::endl;

        SaveMapAndTime(Base_Folder, Pixel_Vector_Out, usedPatternArray, mode + "_" + nameWithoutExt, Map_Size, Posible_Tiles, duration, backtrackUses);
        std::cout << "Guardando completado." << std::endl;
        //dibujar los patrones en una imagen aparte
        initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);
        
    }

    Pixel_Vector_Out.clear();
    usedPatternArray.clear();
    Propagation_Pos.clear();
    BT_Map_Uncollapse.clear();
    BT_Propagation_Pos.clear();
    backtrackUses = 0;
    totalBacktracking = 0;
    BT_step = 0;
}
