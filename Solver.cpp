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
bool define_windowArea_with_lowest_entropy(const std::vector<std::vector<int>>& Map, const std::vector<int>& Propagation_Pos, std::vector<int>& New_Window_Area, const int N_Pattern_Size, const int Y_Map_size, int& window_origin_pos) {

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
                        window_origin_pos = pos;
                    }
                tmp_lowest_entropy = 0;
            }
            if (right) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, -(N_Pattern_Size - 1), 0, 0, N_Pattern_Size - 1);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                        window_origin_pos = pos;
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
                        window_origin_pos = pos;
                    }
                tmp_lowest_entropy = 0;
            }
            if (right) {
                tmp_lowest_entropy = calculate_Entropy_around_window(Map, tmp_new_window, N_Pattern_Size, Y_Map_size, pos, 0, N_Pattern_Size - 1, 0, N_Pattern_Size - 1);
                if (tmp_lowest_entropy != 0)
                    if ((current_lowest_entropy > tmp_lowest_entropy) || current_lowest_entropy == -1) {
                        current_lowest_entropy = tmp_lowest_entropy;
                        posible_window = tmp_new_window;
                        window_origin_pos = pos;
                    }
                tmp_lowest_entropy = 0;
            }
        }
    }

    if (posible_window.size() > 0) {
        New_Window_Area = posible_window;
        return true;
    }
    else {
        New_Window_Area.clear();
        return false;
    }
}

bool is_point_Banned(const int value, const std::vector<int>& Banned_Propagation_Pos) {
    auto it = std::find(Banned_Propagation_Pos.begin(), Banned_Propagation_Pos.end(), value);
    if (it != Banned_Propagation_Pos.end())
        return true;
    else 
        return false;
}
bool Remove_Point_from_Propagation(const int value, std::vector<int>& Propagation_Pos) {
    auto it = std::find(Propagation_Pos.begin(), Propagation_Pos.end(), value);
    // Si el valor fue encontrado, eliminarlo
    if (it != Propagation_Pos.end()) {
        Propagation_Pos.erase(it);
        return true;
    }
    else {
        return false;
    }
}
//función que actualiza las posiciones de propagación revisando cuales están colapsadas y si en una ventana de patron N tiene entropia mayor a 0
bool update_all_valid_propagation_pos(const std::vector<std::vector<int>>& Map, std::vector<int>& Propagation_Pos, const std::vector<int>& Banned_Propagation_Pos, const int N_Pattern_Size, const int Y_Map_size) {
   
    std::vector<int> CollapsePosArray, tmp_new_window;
    int N, pos;
    bool up = false, down = false, right = false, left = false, valid = false;

    for (int i = 0; i < Map.size(); i++) {
        //si la posición esta colapsada
        if (Map[i].size() == 1 && !is_point_Banned(i,Banned_Propagation_Pos)) {
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
            if (All_Patterns[z].pixelesCoo[i] == -1) {
                break;
            }
            auto it = std::find(unCollapseMap[Window_Area[i]].begin(), unCollapseMap[Window_Area[i]].end(), All_Patterns[z].pixelesCoo[i]);
            if (it == unCollapseMap[Window_Area[i]].end()) {
                contains = false;
                break;
            }
        }
        if (contains) {
            //puede tener patrones repetidos
            Valid_Patterns.push_back(All_Patterns[z]);
        }
    }

}
void update_Propagation_Around_Window(std::vector<std::vector<int>>& Map, std::vector<int>& Window_Area, std::vector<int>& Propagation_Pos, std::vector<Pattern>& Valid_Patterns) {
    if (Valid_Patterns.size() > 0) {
        for (int i = 0; i < Window_Area.size(); i++) {
            for (int j = 0; j < Valid_Patterns.size(); j++) {
                Map[Window_Area[i]].push_back(Valid_Patterns[j].pixelesCoo[i]);
            }
            if (Map[Window_Area[i]].size() > 1) {
                //Map[Window_Area[i]].clear();
                
                /*
                std::sort(Map[Window_Area[i]].begin(), Map[Window_Area[i]].end());
                auto last = std::unique(Map[Window_Area[i]].begin(), Map[Window_Area[i]].end());
                Map[Window_Area[i]].erase(last, Map[Window_Area[i]].end());
                */
            }
        }
    }
    
    
    
}
void Append_Pattern_Array(std::vector<Pattern>& Valid_Patterns, std::vector<Pattern>& tmp_Valid_Patterns) {
    for (int i = 0; i < tmp_Valid_Patterns.size(); i++) {
        Valid_Patterns.push_back(tmp_Valid_Patterns[i]);
    }
}
void propagation(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Pos_Array, std::vector<Pattern>& All_Patterns, std::vector<Pattern>& Valid_Patterns, const int Y, const int uncollapse_Size) {

    int N = All_Patterns.front().N, pos, entropy_value;
    std::vector<int> new_Window, tmp_Window;

    std::vector<std::vector<int>> tmp_unCollapseMap;
    std::vector<Pattern> tmp_Valid_Patterns;

    tmp_unCollapseMap.resize(unCollapseMap.size());

    bool up = false, down = false, right = false, left = false;

    Valid_Patterns.clear();


    for (int i = 0; i < Pos_Array.size(); i++) {
        pos = Pos_Array[i];

        up = false, down = false, right = false, left = false;

        if (pos / Y >= N - 1)
            up = true;
        if (pos / Y < Y - (N - 1))
            down = true;
        if (pos % Y >= N - 1)
            left = true;
        if (pos % Y < Y - (N - 1))
            right = true;

        if (up) {
            if (right) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_Window, N, Y, pos, -(N - 1), 0, 0, N - 1);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_Window, All_Patterns, tmp_Valid_Patterns);
                    update_Propagation_Around_Window(tmp_unCollapseMap,tmp_Window, Pos_Array, tmp_Valid_Patterns);
                    Append_Pattern_Array(Valid_Patterns,tmp_Valid_Patterns);
                    tmp_Valid_Patterns.clear();
                }
            }
            if (left) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_Window, N, Y, pos, -(N - 1), 0, -(N - 1), 0);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_Window, All_Patterns, tmp_Valid_Patterns);
                    update_Propagation_Around_Window(tmp_unCollapseMap, tmp_Window, Pos_Array, tmp_Valid_Patterns);
                    Append_Pattern_Array(Valid_Patterns, tmp_Valid_Patterns);
                    tmp_Valid_Patterns.clear();
                }
            }
        }
        if (down) {
            if (right) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_Window, N, Y, pos, 0, N - 1, 0, N - 1);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_Window, All_Patterns, tmp_Valid_Patterns);
                    update_Propagation_Around_Window(tmp_unCollapseMap, tmp_Window, Pos_Array, tmp_Valid_Patterns);
                    Append_Pattern_Array(Valid_Patterns, tmp_Valid_Patterns);
                    tmp_Valid_Patterns.clear();
                }
            }
            if (left) {
                entropy_value = calculate_Entropy_around_window(unCollapseMap, tmp_Window, N, Y, pos, 0, N - 1, -(N - 1), 0);
                if (entropy_value > 0) {
                    Find_Valid_Patterns(unCollapseMap, tmp_Window, All_Patterns, tmp_Valid_Patterns);
                    update_Propagation_Around_Window(tmp_unCollapseMap, tmp_Window, Pos_Array, tmp_Valid_Patterns);
                    Append_Pattern_Array(Valid_Patterns, tmp_Valid_Patterns);
                    tmp_Valid_Patterns.clear();
                }
            }
        }

    }

    for (int i = 0; i < tmp_unCollapseMap.size(); i++) {
        if (tmp_unCollapseMap[i].size() > 0 && unCollapseMap[i].size() > 1) {
            std::sort(tmp_unCollapseMap[i].begin(), tmp_unCollapseMap[i].end());
            auto last = std::unique(tmp_unCollapseMap[i].begin(), tmp_unCollapseMap[i].end());
            tmp_unCollapseMap[i].erase(last, tmp_unCollapseMap[i].end());
            unCollapseMap[i] = tmp_unCollapseMap[i];
        }
    }
}

bool Tile_and_Pattern_are_Compatible(const std::vector<int>& Tile_Map, const int Tile_Pattern) {
    for (int i = 0; i < Tile_Map.size(); i++)
        if (Tile_Pattern == Tile_Map[i])
            return true;
    return false;
}

bool Collapse(std::vector<std::vector<int>>& unCollapseMap, std::vector<int>& Window_Area, const std::vector<Pattern>& Valid_Patterns, const int posibleTilesN) {

    //definicion de casilla sin colapsar para patrones de alta jerarquia
    std::vector<int> unCollapseNode;
    for (int h = 0; h < posibleTilesN; h++)
        unCollapseNode.push_back(h);

    std::vector<int> New_Window_Area;
    auto newPattern = Valid_Patterns.front();
    //newPattern = Valid_Patterns[getRandom(0, Valid_Patterns.size())];
    bool valid_Pattern;
    for (int z = 0; z < Valid_Patterns.size(); z++) {
        newPattern = Valid_Patterns[z];
        valid_Pattern = true;
        for (int i = 0; i < Window_Area.size(); i++) {
            if (newPattern.pixelesCoo[i] != -1) {
                if (!Tile_and_Pattern_are_Compatible(unCollapseMap[Window_Area[i]], newPattern.pixelesCoo[i])) {
                    valid_Pattern = false;
                    break;
                }
            }
        }
        if (valid_Pattern) {
            for (int i = 0; i < Window_Area.size(); i++) {
                if (unCollapseMap[Window_Area[i]].size() > 1) {
                    unCollapseMap[Window_Area[i]].clear();
                    if (newPattern.pixelesCoo[i] == -1) {
                        unCollapseMap[Window_Area[i]] = unCollapseNode;
                    }
                    else {
                        New_Window_Area.push_back(Window_Area[i]);
                        unCollapseMap[Window_Area[i]].push_back(newPattern.pixelesCoo[i]);
                    }
                }
            }
            Window_Area = New_Window_Area;
            return true;
        }
    }
    return false;
}

void loadPositionOnArray(std::vector<int>& Base_Vector, std::vector<int>& New_Vector) {
    for (int i = 0; i < New_Vector.size(); i++) {
        Base_Vector.push_back(New_Vector[i]);
    }
    std::sort(Base_Vector.begin(), Base_Vector.end());
    auto last = std::unique(Base_Vector.begin(), Base_Vector.end());
    Base_Vector.erase(last, Base_Vector.end());
}

bool Request_Backtracking(
    std::vector<std::vector<int>>& UnCollapseMap, std::vector<std::vector<std::vector<int>>>& BT_UnCollapseMap,
    std::vector<int>& Propagation_Pos, std::vector<std::vector<int>>& BT_Propagation_Pos, std::vector<int>& Banned_Propagation_Pos,
    const std::vector<Pixel>& Posible_Tiles, std::vector<int>& count_iteration_per_hierarchy,
    const int Map_Size, int& BT_step, int& backtrackUses, int& current_hierarchy_iteration,
    bool& randomStart, const bool backtrackingActive
    ) {

    if (backtrackingActive) {
        if(backtrackUses < 200)
            
            if (BT_step - backtrackUses > 0) {
                stopExecute(0, "backtracking requested");
                BT_step = BT_step - backtrackUses;
                UnCollapseMap = BT_UnCollapseMap[BT_step - 1];
                Propagation_Pos = BT_Propagation_Pos[BT_step - 1];
                BT_UnCollapseMap.resize(BT_step - 1);
                BT_Propagation_Pos.resize(BT_step - 1);
                backtrackUses++;
                Banned_Propagation_Pos.clear();
                return true;
            }
    }

    stopExecute(1000, "Geración no viable, reiniciando proceso 1");
    initialize_Map(UnCollapseMap, Posible_Tiles, Map_Size);
    randomStart = true;
    BT_step = 0;
    backtrackUses = 0;
    current_hierarchy_iteration = 0;
    Propagation_Pos.clear();
    Banned_Propagation_Pos.clear();

    for (int i = 0; i < count_iteration_per_hierarchy.size(); i++) {
        count_iteration_per_hierarchy[i] = 0;
    }

    return true;
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
    Pattern selected_pattern_to_collapse(0, 0);
    std::vector<std::vector<int>> UnCollapseMap;
    std::vector<int> Propagation_Pos, Window_Area, Banned_Propagation_Pos, posible_Size;
    Propagation_Pos.resize(0); Banned_Propagation_Pos.resize(0);
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
    std::vector<int> max_iteration_per_hierarchy, count_iteration_per_hierarchy, Pattern_size_per_hierarchy;
    int current_hierarchy_iteration = 0, current_iteration_count = 0, default_max_iteration_count = 1000;




    int window_origin_pos;

    //impresión de los tamaños disponibles
    for (int i = 0; i < All_Pattern_Array.size(); i++) {
        std::cout << "Patrones de jerarquia: " << i << " tienen tamaño: " << All_Pattern_Array[i].front().N <<" con: "<< All_Pattern_Array[i].size() << " Patrones disponibles" << std::endl;
        //ingreso de un valor maximo de cuantas veces se puede iterar por patron
        max_iteration_per_hierarchy.push_back(default_max_iteration_count);

        count_iteration_per_hierarchy.push_back(0);
        //guardado de una variable con información de que tamaño de patron corresponde a cada jerarquia.
        Pattern_size_per_hierarchy.push_back(All_Pattern_Array[i].front().N);
    }

    //Inicio del Cronometro
    
    auto start = std::chrono::high_resolution_clock::now(); 
    std::cout << "Iniciando creacion de nuevo mapa." << std::endl;

    bool randomStart = true, engage_BT = false, try_skip_next_hierarchy = false, use_ordered_hierarchy = true;
    int general_testing_stop_time = 100;

    max_iteration_per_hierarchy[1] = 2;

    // Ruta: cd /mnt/d/Memoria\ HWFC/Code/test2/src
    
    //Ciclo iterativo principal
    while (!mapCompleted(UnCollapseMap)) {

        if (max_iteration_per_hierarchy[current_hierarchy_iteration] <= count_iteration_per_hierarchy[current_hierarchy_iteration]) {
            if (!use_ordered_hierarchy ) {
                current_hierarchy_iteration = getRandom(1, All_Pattern_Array.size() - 1);
            }
            else if (use_ordered_hierarchy && current_hierarchy_iteration < All_Pattern_Array.size() - 1) {
                stopExecute(2500, "muchos intentos en la jerarquia: "+ std::to_string(current_hierarchy_iteration) +", bajemos de jerarquiaa");
                current_hierarchy_iteration++;
                Banned_Propagation_Pos.clear();
            }
            else {
                stopExecute(1000,"testing");
                Request_Backtracking(UnCollapseMap, BT_UnCollapseMap, Propagation_Pos, BT_Propagation_Pos, Banned_Propagation_Pos, Posible_Tiles, count_iteration_per_hierarchy, Map_Size, BT_step, backtrackUses, current_hierarchy_iteration, randomStart, backtrackingActive);
            }
        }
        ControlPoint(current_hierarchy_iteration);
        ControlPoint(count_iteration_per_hierarchy[current_hierarchy_iteration]);
        //SELECCION
        if (randomStart) {

            if (All_Pattern_Array[current_hierarchy_iteration].front().N > Map_Size) {
                ControlString("ERROR, el patron es superior al tamaño maximo del mapa, abortando generación");
                std::exit(0);
            }
            int initial_Pattern_size_Helper;
            randomStart = false;
            do {
                //posiciónn inicial aleatoria
                Propagation_Pos.clear();
                Propagation_Pos.push_back(getRandom(0, UnCollapseMap.size()));

                Find_Valid_Patterns(UnCollapseMap, Window_Area, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns);
                selected_pattern_to_collapse = ValidPatterns[getRandom(0, ValidPatterns.size())];
                initial_Pattern_size_Helper = selected_pattern_to_collapse.N;
                //guardado de un unico patron incial
                ValidPatterns.clear();
                ValidPatterns.push_back(selected_pattern_to_collapse);
                //Definición de una ventana alrededor del punto inicial  
            } while (!define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, initial_Pattern_size_Helper, Map_Size, window_origin_pos));
        }
        //seleccion guiada por la entropia
        else {
            bool window_defined = false;
            do {
                stopExecute(0, "Seleccionar una posicion");
                if (!define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size, window_origin_pos)) {
                    Propagation_Pos.clear();
                }
                else {
                    window_defined = true;
                }
            
                if (Propagation_Pos.size() == 0) {
                    if (!use_ordered_hierarchy) {
                        current_hierarchy_iteration = getRandom(1, All_Pattern_Array.size() - 1);
                        update_all_valid_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size);

                    }
                    else if (use_ordered_hierarchy && current_hierarchy_iteration < All_Pattern_Array.size() - 1) {
                        current_hierarchy_iteration++;
                        Banned_Propagation_Pos.clear();
                        Propagation_Pos.clear();
                        update_all_valid_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size);
                    }
                    else {
                        Request_Backtracking(UnCollapseMap, BT_UnCollapseMap, Propagation_Pos, BT_Propagation_Pos, Banned_Propagation_Pos, Posible_Tiles, count_iteration_per_hierarchy, Map_Size, BT_step, backtrackUses, current_hierarchy_iteration, randomStart, backtrackingActive);
                    }
                    /*
                    * /if (!update_all_valid_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[current_hierarchy_iteration], Map_Size)) {
                        
                    }
                     */
                }
                //Definición de una ventana alrededor del punto inicial  
            } while (!window_defined);
        }

        std::cout << "ventana de menor entropia: ";
        for (int i = 0; i < Window_Area.size(); i++) {
            std::cout << " " << Window_Area[i] / Map_Size << "," << Window_Area[i] % Map_Size << " / ";
        }
        std::cout << std::endl;
        
        /////////// COLAPSO
        if (Window_Area.size() > 0) {
            bool colapse_try = true;
            int tmp_current_hierarchy_iteration = current_hierarchy_iteration;

            do {
                Find_Valid_Patterns(UnCollapseMap, Window_Area, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns);
                if (!ValidPatterns.empty()) {
                    if (Collapse(UnCollapseMap, Window_Area, ValidPatterns, Posible_Tiles.size())) {
                        count_iteration_per_hierarchy[current_hierarchy_iteration]++;
                        colapse_try = false;
                        //guardado del estado actual del mapa para uso en backtracking
                        if (backtrackingActive) {
                            BT_UnCollapseMap.push_back(UnCollapseMap);
                            BT_Propagation_Pos.push_back(Propagation_Pos);
                            BT_step++;
                        }
                        if (printMapBool) {
                            printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                            stopExecute(general_testing_stop_time, "Collapse complete");
                        }
                        if (ValidPatterns.front().highPattern && current_hierarchy_iteration == 0) {
                            current_hierarchy_iteration++;
                            Banned_Propagation_Pos.clear();
                        }
                    }
                    else {

                        Remove_Point_from_Propagation(window_origin_pos, Propagation_Pos);
                        colapse_try = false;
                        Window_Area.clear();
                        if (printMapBool) {
                            printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                            stopExecute(general_testing_stop_time, "Colapse have failed");
                        }
                    }
                    

                }
                else {
                    if(!use_ordered_hierarchy){
                        tmp_current_hierarchy_iteration = getRandom(1, All_Pattern_Array.size() - 1);
                        update_all_valid_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size);
                        define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size, window_origin_pos);
                        Find_Valid_Patterns(UnCollapseMap, Window_Area, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns);
                    }
                    else if (try_skip_next_hierarchy) {
                        if (tmp_current_hierarchy_iteration < All_Pattern_Array.size() - 1) {
                            tmp_current_hierarchy_iteration++;
                            update_all_valid_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size);
                            define_windowArea_with_lowest_entropy(UnCollapseMap, Propagation_Pos, Window_Area, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size, window_origin_pos);
                            Find_Valid_Patterns(UnCollapseMap, Window_Area, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns);
                        }
                        else {
                            colapse_try = false;
                            Banned_Propagation_Pos.push_back(window_origin_pos);
                            Window_Area.clear();
                            ValidPatterns.clear();
                        }
                    }
                    //Backtracking output
                    else {
                        colapse_try = false;
                        Remove_Point_from_Propagation(window_origin_pos, Propagation_Pos);
                        Banned_Propagation_Pos.push_back(window_origin_pos);
                        //update_propagation_pos(UnCollapseMap, Propagation_Pos, Banned_Propagation_Pos, Pattern_size_per_hierarchy[tmp_current_hierarchy_iteration], Map_Size);
                        if (printMapBool) {
                            printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                            stopExecute(general_testing_stop_time, "!! un punto de propagación fue banneado: " + std::to_string(window_origin_pos/Map_Size)+ " " + std::to_string(window_origin_pos%Map_Size));
                        }
                        /*
                        esto debe ir al inicio de la iteracion
                        if (Propagation_Pos.empty()){
                            if (current_hierarchy_iteration < All_Pattern_Array.size() - 1) {
                                stopExecute(1000, "no quedan punto de propagación validos, bajando de jerarquia");
                                current_hierarchy_iteration++;
                                Banned_Propagation_Pos.clear();
                            }
                            else {
                                Request_Backtracking(UnCollapseMap, BT_UnCollapseMap, Propagation_Pos, BT_Propagation_Pos, Banned_Propagation_Pos, Posible_Tiles, count_iteration_per_hierarchy, Map_Size, BT_step, backtrackUses, current_hierarchy_iteration, randomStart, backtrackingActive);
                            }
                        }
                        */
                        
                        Window_Area.clear();
                    }
                }
            } while (colapse_try);  
        }

        ///////// PROPAGACION  //verificación de puntos con entropia mayor a 0 para iterar en la propagacion 
        if (Window_Area.size() > 0) {
            propagation(UnCollapseMap, Window_Area, All_Pattern_Array[current_hierarchy_iteration], ValidPatterns, Map_Size, Posible_Tiles.size());
            loadPositionOnArray(Propagation_Pos, Window_Area);
            if (printMapBool) {
                printMap(UnCollapseMap, Map_Size, Posible_Tiles.size(), Propagation_Pos, true);
                stopExecute(general_testing_stop_time, "Propagación completada");
            }

            /*/ imprimir posibilidades de las posiciones despues de propagar
            
            for (int i = 0; i < UnCollapseMap.size(); i++) {
                if (UnCollapseMap[i].size() > 1 && UnCollapseMap[i].size() < Posible_Tiles.size()) {
                    std::cout << "posicion: " << i / Map_Size << "," << i % Map_Size << " :";
                    for (int j = 0; j < UnCollapseMap[i].size(); j++) {
                        std::cout << UnCollapseMap[i][j] << " ";
                    }
                    std::cout << std::endl;
                }
            }
            */
            
        }
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
        current_hierarchy_iteration = 0;
        current_iteration_count = 0;

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
