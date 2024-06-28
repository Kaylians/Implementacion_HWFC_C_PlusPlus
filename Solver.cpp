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
int getRandomPatternWeighted(const std::vector<int>& total_Weight) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution(total_Weight.begin(), total_Weight.end());
    return distribution(gen);
}
//funcion para elegir la casilla con entropia (posibles colores) disponible en el mapa
int selectLowestEntropyTile(const std::vector<std::vector<int>>& unCollapseMap, int Posibi, int LastLowestEntropyTilePos, const std::vector<int>& RPP) {
    int lowestValue = Posibi + 1, lowestID = -1;
    if (RPP.size() > 0) {
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
        for (int i = 0; i < unCollapseMap.size(); i++) {
            if (unCollapseMap[i].size() > 1)
                if (unCollapseMap[i].size() < lowestValue) {
                    lowestID = i;
                    lowestValue = unCollapseMap[i].size();
                }
        }
    }
    return lowestID;
}
//funciones para confirmar si el patron coincide en cada uno de sus puntos con el espacio alrededor del punto entregado
bool selected_Pattern_Analizer(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, int posibleTilesN, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
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

bool select_Pattern(Pattern& pattern, const std::vector<std::vector<int>>& unCollapseMap, const int N, const int Y, int pos, int posibleTilesN) {

    bool up = false, down = false, right = false, left = false, contains = false;
    //center, norte, sur, este , oeste


    //añadir mejor sistema de localizacion de los patrones aquí

    if (pos / Y >= N - 1)
        up = true;
    if (pos / Y < Y - (N - 1))
        down = true;
    if (pos % Y >= N - 1)
        left = true;
    if (pos % Y < Y - (N - 1))
        right = true;

    //stopExecute(1000, "punto de control seleccion de patron");
    if (down) {
        for (int i = 0; i < N; i++) {
            if (right) {
                for (int j = 0; j < N; j++) {
                    if (selected_Pattern_Analizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, 0, N - 1)) {
                        return true;
                    }
                }
            }
            if (left) {
                for (int j = 0; j < N; j++) {
                    if (selected_Pattern_Analizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, 0, N - 1, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
        }
    }
    if (up) {
        for (int i = 0; i < N; i++) {
            if (right) {
                for (int j = 0; j < N; j++) {
                    if (selected_Pattern_Analizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, 0, N - 1)) {
                        return true;
                    }
                }
            }
            if (left) {
                for (int j = 0; j < N; j++) {
                    if (selected_Pattern_Analizer(pattern, unCollapseMap, posibleTilesN, N, Y, pos, -(N - 1), 0, -(N - 1), 0)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}
// Función que retorna la cantidad de entropia, si todos las posiciones entregadas estan colapsadas retorna 0
int calculate_Entropy_around_window(const std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& tmp_newCoordinates, const int N, const int Y, int pos, int i_min, int i_max, int j_min, int j_max) {
    bool contains = false;
    int count = 0;
    int coo;
    tmp_newCoordinates.clear();
    for (int i = i_min, x = 0; i <= i_max; i++, x++) {
        for (int j = j_min, y = 0; j <= j_max; j++, y++) {
            coo = pos + j + i * Y;
            if (unCollapseMap[coo].size() > 1)
                count += unCollapseMap[coo].size();
            tmp_newCoordinates.push_back(coo);
        }
    }
    return count;
}

//función para definir una ventana alrededor del area con menor entropia en los puntos entregados 
bool define_windowArea_with_lowest_entropy(const std::vector<std::vector<int>>& Map, const std::vector<int>& Propagation_Pos, std::vector<int>& New_Window_Area, const int N_Pattern_Size, const int Y_Map_size) {

    std::vector<int> tmp_new_window, posible_window;

    int current_lowest_entropy = -1, tmp_lowest_entropy = 0, pos;
    bool  up = false, down = false, right = false, left = false;
    //verificación de si se encuentra dentro de los boordes del mapa
    
    for (int i = 0; i < Propagation_Pos.size(); i++) {
        pos = Propagation_Pos[i];

        up = false, down = false, right = false, left = false;

        if (pos / Y_Map_size >= N_Pattern_Size - 1)
            up = true;
        if (pos / Y_Map_size < Y_Map_size - (N_Pattern_Size - 1))
            down = true;
        if (pos % Y_Map_size >= N_Pattern_Size - 1)
            left = true;
        if (pos % Y_Map_size < Y_Map_size - (N_Pattern_Size - 1))
            right = true;
        tmp_lowest_entropy = 0;
        if (up) {
            if (left) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, -(N_Pattern_Size - 1), 0, -(N_Pattern_Size - 1), 0);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                    }
                tmp_lowest_entropy = 0;
            }
            if (right) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, -(N_Pattern_Size - 1), 0, 0, N_Pattern_Size - 1);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                    }
                tmp_lowest_entropy = 0;
            }
        }
        if (down) {
            if (left) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, 0, N_Pattern_Size - 1, -(N_Pattern_Size - 1), 0);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                    }
                tmp_lowest_entropy = 0;
            }
            if (right) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, 0, N_Pattern_Size - 1, 0, N_Pattern_Size - 1);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                    }
                tmp_lowest_entropy = 0;
            }
        }
    }

    if (posible_window.size() > 0) {
        New_Window_Area = posible_window;
        return true;
    }else
        return false;
}
// (!)
void Get_valid_Propagation_Points(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& RPP, const int Y, int posibleTilesN) {

    //propagacion

    bool U = false, S = false, E = false, O = false, extendRPP = false;
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

//función que actualiza las posiciones de propagación revisando cuales están colapsadas y si en una ventana de patron N tiene entropia mayor a 0
bool update_propagation_pos(const std::vector<std::vector<int>>& Map, std::vector<int>& Propagation_Pos, const int N_Pattern_Size, const int Y_Map_size) {
   
    std::vector<int> CollapsePosArray, tmp_new_window;
    int N, pos;
    bool up = false, down = false, right = false, left = false, valid = false;

    for (int i = 0; i < Map.size(); i++) {
        //si la posición esta colapsada
        if (Map[i].size() == 1) {
            pos = i;
            up = false, down = false, right = false, left = false, valid = false;

            if (pos / Y_Map_size >= N_Pattern_Size - 1)
                up = true;
            if (pos / Y_Map_size < Y_Map_size - (N_Pattern_Size - 1))
                down = true;
            if (pos % Y_Map_size >= N_Pattern_Size - 1)
                left = true;
            if (pos % Y_Map_size < Y_Map_size - (N_Pattern_Size - 1))
                right = true;

            if (up) {
                if (left) {
                    if (0 < calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, -(N_Pattern_Size - 1), 0, -(N_Pattern_Size - 1), 0)) {
                        valid = true;
                    }
                }
                if (right && !valid) {
                    if (0 < calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, -(N_Pattern_Size - 1), 0, 0, N_Pattern_Size - 1)) {
                        valid = true;
                    }
                }
            }
            if (down && !valid) {
                if (left) {
                    if (0 < calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, 0, N_Pattern_Size - 1, -(N_Pattern_Size - 1), 0)) {
                        valid = true;
                    }
                }
                if (right && !valid) {
                    if (0 < calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, 0, N_Pattern_Size - 1, 0, N_Pattern_Size - 1)) {
                        valid = true;
                    }
                }
            }

            if(valid)
                CollapsePosArray.push_back(i);
        }
    }

    if (CollapsePosArray.size() > 0) {
        Propagation_Pos = CollapsePosArray;
        return true;
    }
    else
        return false;
}

void Find_Valid_Patterns(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Window_Area, const std::vector<Pattern>& All_Patterns, std::vector<Pattern>& Valid_Patterns) {
    
    bool contains;
    for (int z = 0; z < All_Patterns.size(); z++) {
        contains = true;
        for (int i = 0; i < Window_Area.size(); i++) {
            auto it = std::find(unCollapseMap[Window_Area[i]].begin(), unCollapseMap[Window_Area[i]].end(), All_Patterns[z].pixelesCoo[i]);
            if (it == unCollapseMap[Window_Area[i]].end()) {
                contains = false;
                break;
            }
        }
        if (contains) {
            Valid_Patterns.push_back(All_Patterns[z]);
        }
    }

}
void update_Map_Propagation(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Window_Area, std::vector<int>& Propagation_Pos, std::vector<Pattern>& Valid_Patterns) {
    if (Valid_Patterns.size() > 0) {
        for (int i = 0; i < Window_Area.size(); i++) {
            if (unCollapseMap[Window_Area[i]].size() > 1) {
                unCollapseMap[Window_Area[i]].clear();
                for (int j = 0; j < Valid_Patterns.size(); j++) {
                    unCollapseMap[Window_Area[i]].push_back(Valid_Patterns[j].pixelesCoo[i]);
                }
                std::sort(unCollapseMap[Window_Area[i]].begin(), unCollapseMap[Window_Area[i]].end());
                auto last = std::unique(unCollapseMap[Window_Area[i]].begin(), unCollapseMap[Window_Area[i]].end());
                unCollapseMap[Window_Area[i]].erase(last, unCollapseMap[Window_Area[i]].end());

                // !!!!!!!!!!!!!!!!!!!!!!!!!!! revisar bien la forma de propagacion

               // if (unCollapseMap[Window_Area[i]].size() == 1)
                   // Propagation_Pos.push_back(Window_Area[i]);
            }
        }
    }
    
    
    
}
void propagation(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Propagation_Pos, std::vector<Pattern>& All_Patterns, std::vector<Pattern>& Valid_Patterns, const int Y) {

    int N = All_Patterns.front().N, pos, entropy_value;
    std::vector<int> newCoordinates, tmp_newCoordinates;

    bool up = false, down = false, right = false, left = false;

    for (int i = 0; i < Propagation_Pos.size(); i++) {
        pos = Propagation_Pos[i];
        up = false, down = false, right = false, left = false;

        if (pos / Y >= N - 1)
            up = true;
        if (pos / Y < Y - (N - 1))
            down = true;
        if (pos % Y >= N - 1)
            left = true;
        if (pos % Y < Y - (N - 1))
            right = true;

        Valid_Patterns.clear();

        if (up) {
            if (right) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_newCoordinates, N, Y, pos, -(N - 1), 0, 0, N - 1);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_newCoordinates, All_Patterns, Valid_Patterns);
                    update_Map_Propagation(unCollapseMap,tmp_newCoordinates, Propagation_Pos, Valid_Patterns);
                }
            }
            if (left) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_newCoordinates, N, Y, pos, -(N - 1), 0, -(N - 1), 0);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_newCoordinates, All_Patterns, Valid_Patterns);
                    update_Map_Propagation(unCollapseMap, tmp_newCoordinates, Propagation_Pos, Valid_Patterns);
                }
            }
        }
        if (down) {
            if (right) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_newCoordinates, N, Y, pos, 0, N - 1, 0, N - 1);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_newCoordinates, All_Patterns, Valid_Patterns);
                    update_Map_Propagation(unCollapseMap, tmp_newCoordinates, Propagation_Pos, Valid_Patterns);
                }

            }
            if (left) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_newCoordinates, N, Y, pos, 0, N - 1, -(N - 1), 0);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_newCoordinates, All_Patterns, Valid_Patterns);
                    update_Map_Propagation(unCollapseMap, tmp_newCoordinates, Propagation_Pos, Valid_Patterns);
                }
            }

        }

    }
    
}
std::vector <bool> validDirection(const int N, const int Y, const int pos) {
    
    std::vector <bool> valid_Direction;
    bool up = false, down = false, right = false, left = false;

    for (int i = 0; i < 4; i++)
        valid_Direction.push_back(false);
    
    if (pos / Y >= N - 1)
        up = true;
    if (pos / Y < Y - (N - 1))
        down = true;
    if (pos % Y >= N - 1)
        left = true;
    if (pos % Y < Y - (N - 1))
        right = true;

    if (up && right)
        valid_Direction[0] = true;
    if (up && left)
        valid_Direction[1] = true;
    if (down && right)
        valid_Direction[2] = true;
    if (down && left)
        valid_Direction[3] = true;
    


    return valid_Direction;
}

std::vector <int> defineWindows(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& CollapsedArea, const int Y, const int N, const int posibleTilesN) {
    std::vector <int> new_Window;
    int pos;
    std::vector <bool> valid_Direction;

    for (int x = 0; x < CollapsedArea.size(); x++) {

        valid_Direction = validDirection(N, Y, CollapsedArea[x]);

        if (valid_Direction[0]) {

        }


    }
    return new_Window;
}


void Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Window_Area, std::vector<Pattern>& Valid_Patterns, const int posibleTilesN) {

    auto newPattern = Valid_Patterns.front();
    newPattern = Valid_Patterns[getRandom(0, Valid_Patterns.size())];

    //definicion de casilla sin colapsar para patrones de alta jerarquia
        std::vector<int> unCollapseNode;
    for (int h = 0; h < posibleTilesN; h++)
        unCollapseNode.push_back(h);

    for (int i = 0; i < Window_Area.size(); i++) {
        if (unCollapseMap[Window_Area[i]].size() > 1) {

            unCollapseMap[Window_Area[i]].clear();
            if (newPattern.pixelesCoo[i] == -1)
                unCollapseMap[Window_Area[i]] = unCollapseNode;
            else
                unCollapseMap[Window_Area[i]].push_back(newPattern.pixelesCoo[i]);

        }
    }

}

//verifica si todas las posiciones del mapa estan colapsadas
void Update_Hierchical_Iteration(int& H_Current_iteration, int& H_Count_iteration, const int H_patternArray) {
    if (H_Current_iteration < H_patternArray - 1) {
        H_Current_iteration++;
        stopExecute(1000, "Cambio de jerarquia, numero de iteraciones realizadas: " + std::to_string(H_Count_iteration) + ", Nueva jeraquia: " + std::to_string(H_Current_iteration));
        H_Count_iteration = 0;
    }
    else {
        H_Count_iteration = 0;
    }
}

bool mapCompleted(const std::vector<std::vector<int>>& unCollapseMap) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        if (unCollapseMap[i].size() != 1) {
            return false;
        }
    }
    return true;
}

bool generate_Map(std::string mode,
    std::vector<int>& Desire_Top_Size,
    std::vector<int>& Desire_Size,
    std::vector<Pixel>& Posible_Tiles,
    int Map_Size,
    int Top_Size_i,
    bool printMapBool,
    bool backtrackingActive,
    std::vector<std::vector<Pattern>>& All_Pattern_Array,
    std::string& Base_Folder,
    std::string& Example_Map
) {
    std::vector<Pixel> Pixel_Vector_Out;
    std::vector<Pattern> usedPatternArray, ValidPatterns;
    Pattern lastSelectedPattern(0, 0);
    std::vector<std::vector<int>> UnCollapseMap;
    std::vector<int> Propagation_Pos, Window_Area, reserveRPP, posible_Size;
    Propagation_Pos.resize(0); reserveRPP.resize(0);
    initialize_Map(UnCollapseMap, Posible_Tiles, Map_Size);

    //Variables para backtracking
    std::vector<std::vector<std::vector<int>>> BT_UnCollapseMap; std::vector<std::vector<int>> BT_Propagation_Pos;
    int BT_step = 0, backStep = 0, backtrackUses = 0, totalBacktracking = 0;

    /*
    
    
    */
    std::vector<std::vector<int>> total_Weight;
    std::vector<int> tmp_weight;
    for (int i = 0; i < All_Pattern_Array.size(); i++) {
        for (int j = 0; j < All_Pattern_Array[i].size(); j++) {
            tmp_weight.push_back(All_Pattern_Array[i][j].weight);
        }
        total_Weight.push_back(tmp_weight);
        tmp_weight.clear();
    }

    

    //variables para el conteo de iteraciones realizadas
    std::vector<int> max_iteration_per_hierarchy, Pattern_size_per_hierarchy;
    int current_hierarchy_iteration = 0, current_iteration_count = 0, default_max_iteration_count = 1000;



    

    //impresión de los tamaños disponibles
    for (int i = 0; i < All_Pattern_Array.size(); i++) {
        std::cout << "Patrones de jerarquia: " << i << " tienen tamaño: " << All_Pattern_Array[i].front().N << std::endl;
        //ingreso de un valor maximo de cuantas veces se puede iterar por patron
        max_iteration_per_hierarchy.push_back(default_max_iteration_count);
        //guardado de una variable con información de que tamaño de patron corresponde a cada jerarquia.
        Pattern_size_per_hierarchy.push_back(All_Pattern_Array[i].front().N);
    }

    //Inicio del Cronometro
    bool randomStart = true, engage_BT = false;
    auto start = std::chrono::high_resolution_clock::now(); 
    std::cout << "Iniciando creacion de nuevo mapa." << std::endl;


    int general_testing_stop = 0;

    while (!mapCompleted(UnCollapseMap)) {
        
        /////////// SELECCION
        //seleccion aleatoria inicial
        if (randomStart) {

            if (All_Pattern_Array[current_hierarchy_iteration].front().N > Map_Size) {
                ControlString("ERROR, el patron es superior al tamaño maximo del mapa, abortando generación");
                std::exit(0);
            }

            randomStart = false;
            do {
                //posiciónn inicial aleatoria
                Propagation_Pos.clear();
                Propagation_Pos.push_back(getRandom(0, UnCollapseMap.size()));
                //Definición de una ventana alrededor del punto inicial  
            } while (!define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size));

            ValidPatterns = All_Pattern_Array[current_hierarchy_iteration];
        }
        //sesleccion guiada por la entropia
        else {
            do {
                if (!update_propagation_pos(UnCollapseMap, Propagation_Pos, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size)) {
                    
                    if (current_hierarchy_iteration < All_Pattern_Array.size()-1) {
                        current_hierarchy_iteration++;
                    }
                    else {
                        ControlString("ERROR EN LA SELECCION DE NUEVOS PUNTOS DE PROPAGACION, patron minimo agotado");
                        engage_BT = true;
                    }
                }
                //Definición de una ventana alrededor del punto inicial  
            } while (!define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size));
        }

        ControlString("Lugar de la ventana: ");
        for (int i = 0; i < Window_Area.size(); i++) {

            std::cout << Window_Area[i] << " ";
        }
        std::cout << std::endl;

        /////////// COLAPSO
        if (Window_Area.size() > 0) {
            ControlString("ingreso en el colapso");
            bool colapse_try = true;
            int tmp_current_hierarchy_iteration = current_hierarchy_iteration;
            do {
                ControlString("ingreso ciclooooo en el colapso");
                ValidPatterns.clear();
                Find_Valid_Patterns(UnCollapseMap, Window_Area, All_Pattern_Array[tmp_current_hierarchy_iteration], ValidPatterns);
                if (!ValidPatterns.empty()) {
                    Collapse(UnCollapseMap, Window_Area, ValidPatterns, Posible_Tiles.size());
                    colapse_try = false;
                    //guardado del estado actual del mapa para uso en backtracking
                    if (backtrackingActive) {
                        BT_UnCollapseMap.push_back(UnCollapseMap);
                        BT_Propagation_Pos.push_back(Propagation_Pos);
                        BT_step++;
                    }
                    if (printMapBool) {
                        printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                        stopExecute(general_testing_stop, "Collapse complete");
                    }

                }
                else {
                    if (tmp_current_hierarchy_iteration < All_Pattern_Array.size() - 1) {
                        stopExecute(1000, "reduccion temporal de jerarquia");
                        tmp_current_hierarchy_iteration++;
                        update_propagation_pos(UnCollapseMap, Propagation_Pos, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size);
                        define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size);
                    }
                    else {
                        colapse_try = false;
                        if (backtrackingActive) {
                            if (BT_step - backtrackUses > 0) {
                                BT_step = BT_step - backtrackUses;
                                UnCollapseMap = BT_UnCollapseMap[BT_step - 1];
                                Propagation_Pos = BT_Propagation_Pos[BT_step - 1];
                                BT_UnCollapseMap.resize(BT_step - 1);
                                BT_Propagation_Pos.resize(BT_step - 1);
                                usedPatternArray.resize(BT_step - 1);
                                backtrackUses++;
                            }
                            else {
                                initialize_Map(UnCollapseMap, Posible_Tiles, Map_Size);
                                usedPatternArray.clear();
                                totalBacktracking += backtrackUses;
                                randomStart = true;
                                BT_step = 0;
                                backtrackUses = 0;
                                current_hierarchy_iteration = 0;
                                current_iteration_count = 0;
                            }
                        }
                    }
                }
            } while (colapse_try);
            
            
            
        }

        //ejecución de backtracking
        if(engage_BT)
        if (current_hierarchy_iteration == All_Pattern_Array.size()-1) {
            stopExecute(1000, "iniciando backtracking");
            if (backtrackingActive) {
                if (BT_step - backtrackUses > 0) {
                    BT_step = BT_step - backtrackUses;
                    UnCollapseMap = BT_UnCollapseMap[BT_step - 1];
                    Propagation_Pos = BT_Propagation_Pos[BT_step - 1];
                    BT_UnCollapseMap.resize(BT_step - 1);
                    BT_Propagation_Pos.resize(BT_step - 1);
                    usedPatternArray.resize(BT_step - 1);
                    backtrackUses++;
                }
                else {
                    initialize_Map(UnCollapseMap, Posible_Tiles, Map_Size);
                    usedPatternArray.clear();
                    totalBacktracking += backtrackUses;
                    randomStart = true;
                    BT_step = 0;
                    backtrackUses = 0;
                    current_hierarchy_iteration = 0;
                    current_iteration_count = 0;
                }
            }
            engage_BT = false;
        }
        else {
            current_hierarchy_iteration++;
        }
        

        ///////// PROPAGACION
        //verificación de puntos con entropia mayor a 0 para iterar en la propagacion 
        if (update_propagation_pos(UnCollapseMap, Propagation_Pos, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size)) {

            if (printMapBool) {

                printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                stopExecute(general_testing_stop, "Areas validos para el tamaño de la ventana");
            }


            propagation(UnCollapseMap, Propagation_Pos, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns, Map_Size);
            if (printMapBool) {

                printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                stopExecute(general_testing_stop, "Propagación completada");
            }
        }
        else if (!mapCompleted(UnCollapseMap)) {
            //no hay punto de propagación restantes para este tamaño, forzando salto de jerarquia.
            if (current_hierarchy_iteration < Pattern_size_per_hierarchy.size() - 1) {
                stopExecute(1000, "Propagación de la jerarquia agotada, forzando salto a una jerarquia menor");
                current_hierarchy_iteration++;
            }
            //añadir bien la condicion de que ya no quedan jerarquias, cuando no haya opciones 
            else{

                // añadir salida del backtracking aqui tambien, esto es una salida final

                stopExecute(3000, "Fallo terminal, reiniciando");

                initialize_Map(UnCollapseMap, Posible_Tiles, Map_Size);
                usedPatternArray.clear();
                totalBacktracking += backtrackUses;
                randomStart = true;
                BT_step = 0;
                backtrackUses = 0;
                current_hierarchy_iteration = 0;
                current_iteration_count = 0;
            }
        }


        /*
        //añadido de la nueva area colapsada a los puntos de propagacion
        update_propagation_pos(UnCollapseMap, Propagation_Pos, N_Current_Pattern_Size, Map_Size);
        //Get_valid_Propagation_Points(UnCollapseMap, Propagation_Pos, Map_Size, Posible_Tiles.size());
        printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
        stopExecute(1000, "Propagation Update complete");

        for (int i = 0; i < All_Pattern_Array.size(); i++) {
            propagation(UnCollapseMap, Propagation_Pos, All_Pattern_Array[i], ValidPatterns, Map_Size);
        }
        printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
        stopExecute(1000, "Propagation Complete");
        //Get_valid_Propagation_Points(UnCollapseMap,Propagation_Pos,Map_Size,Posible_Tiles.size());
        update_propagation_pos(UnCollapseMap, Propagation_Pos, N_Current_Pattern_Size, Map_Size);
        printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);

        stopExecute(1000, "Propagation Update Complete");
        
        */

        

        /*
        
        //selección de posición
        if (randomStart) {
            //Inicio del nivel si es que se trata de HWFC
            if (mode == "HWFC") {
                for (int i = 0; i < Top_Size_i; i++) {
                    //busqueda y posicionamiento de patrones de alta jerarquia
                    do {
                        lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
                    } while (!HPattValidTile(lowestEntropyTilePos, Map_Size, Map_Size, HN_max) && Map_Uncollapse[lowestEntropyTilePos].size() > 1);

                    if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, H_patternArray[0], lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), total_Weight[0], true, printMapBool)) {

                        

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
                randomStart = false;
            }
            else {
                lowestEntropyTilePos = getRandom(0, Map_Uncollapse.size());
                randomStart = false;
            }
        }
        else {
            lowestEntropyTilePos = selectLowestEntropyTile(Map_Uncollapse, Posible_Tiles.size(), lowestEntropyTilePos, Propagation_Pos);
        }


        //revision de las iteraciones realizadas en la jerarquia actual
        if (H_Count_iteration < H_Max_iteration[H_Current_iteration]) {
            if (Collapse(Map_Uncollapse, Propagation_Pos, reserveRPP, Map_Size, H_patternArray[H_Current_iteration], lastSelectedPattern, lowestEntropyTilePos, Posible_Tiles.size(), total_Weight[H_Current_iteration], true, printMapBool)) {
                H_Count_iteration++;
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

                if (H_Current_iteration < H_patternArray.size() - 1) {
                    Update_Hierchical_Iteration(H_Current_iteration, H_Count_iteration, H_patternArray.size());
                    Get_All_Valid_Propagation_Pos(Map_Uncollapse, Propagation_Pos, Map_Size, Posible_Tiles.size());
                }
                else if (backtrackingActive) {
                    H_Count_iteration = 0;
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
                        Get_All_Valid_Propagation_Pos(Map_Uncollapse, Propagation_Pos, Map_Size, Posible_Tiles.size());
                    }
                }
                else {
                    ControlString("Sin resultado, backtracking apagado, reiniciando");
                    return false;
                    break;
                }

            }
        }
        else {
            Update_Hierchical_Iteration(H_Current_iteration,H_Count_iteration,H_patternArray.size());
            Get_All_Valid_Propagation_Pos(Map_Uncollapse, Propagation_Pos, Map_Size, Posible_Tiles.size());
        }
        
        */
        
    }


    if (mapCompleted(UnCollapseMap)) {

        //fin del cronometro del tiempo de ejecucion
        auto end = std::chrono::high_resolution_clock::now();
        long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        totalBacktracking += backtrackUses;

        printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, false);

        std::cout << "Tiempo de ejecucion: " << duration << " ms" << std::endl;
        std::cout << "usos del backtracking: " << totalBacktracking << std::endl;

        //definicion de metricas
        //KL Divergence

        //construccion de una nueva imagen
        reconstructMap(Pixel_Vector_Out, UnCollapseMap, Posible_Tiles);
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
        //initialize_Map(Map_Uncollapse, Posible_Tiles, Map_Size);

        Pixel_Vector_Out.clear();
        usedPatternArray.clear();
        Propagation_Pos.clear();
        BT_UnCollapseMap.clear();
        BT_Propagation_Pos.clear();
        backtrackUses = 0;
        totalBacktracking = 0;
        BT_step = 0;

        return true;
    }

    Pixel_Vector_Out.clear();
    usedPatternArray.clear();
    Propagation_Pos.clear();
    BT_UnCollapseMap.clear();
    BT_Propagation_Pos.clear();
    backtrackUses = 0;
    totalBacktracking = 0;
    BT_step = 0;

    return false;
}
