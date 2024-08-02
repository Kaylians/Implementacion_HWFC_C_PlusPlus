#include "ReadWrite.h"
#include "Pixel.h"
#include "Pattern.h"
#include "Metrics.h"
#include "DebugUtility.h"
#include <locale>
#include <iomanip> 
namespace fs = std::filesystem;
struct Comma final : std::numpunct<char>
{
    char do_decimal_point() const override { return ','; }
};
std::vector<Pixel> simpleHammingPPM(const std::string& exampleName) {
    std::vector<Pixel> pixeles;
    int w, h;
    std::ifstream archivo(exampleName, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo en Hamming: " << exampleName << std::endl;
    }

    std::string encabezado;
    archivo >> encabezado;

    if (encabezado != "P6") {
        std::cerr << "Formato de archivo PPM no válido en Hamming." << std::endl;
    }

    archivo >> w >> h;
    int maxValor;
    archivo >> maxValor;

    archivo.ignore(); // Ignorar el espacio en blanco después del valor máximo

    pixeles.resize(w * h);
    archivo.read(reinterpret_cast<char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return pixeles;
}

void getPredefineTiles(std::vector<Pixel>& posibleTiles) {

    posibleTiles.emplace_back(0, 255, 0);       // Verde
    //0
    posibleTiles.emplace_back(255, 0, 0);       // Rojo
    posibleTiles.emplace_back(255, 0, 0);       // Rojo
    //2
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    //6
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    //10
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    //14
    posibleTiles.emplace_back(255, 255, 0);     // Amarillo
    //15
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    //17
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    //19
    posibleTiles.emplace_back(255, 165, 0);     // Naranja
    //20
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    //24
    posibleTiles.emplace_back(128, 128, 128);   // Gris
    //25
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    posibleTiles.emplace_back(139, 69, 19);     // Café
    //29
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    //33
}

void findUniquePythonPattern(std::vector<Pattern>& pattArray) {
    std::vector<Pattern> tmpPattArray;
    int weight = 1;
    for (int i = 0; i < pattArray.size(); i++) {
        if (pattArray[i].pattern) {
            for (int j = i + 1; j < pattArray.size(); j++) {
                if (pattArray[j].N == pattArray[i].N) {
                    if (pattArray[j].pattern && pattArray[i].compareCooPattern(pattArray[j].pixelesCoo)) {
                        pattArray[j].pattern = false;
                        weight++;
                    }
                }
            }
            pattArray[i].weight = weight;
        }
        weight = 1;
    }
    for (int i = 0; i < pattArray.size(); i++)
        if (pattArray[i].pattern) {
            tmpPattArray.push_back(pattArray[i]);
            //tmpPattArray.back().id = tmpPattArray.size() - 1;
        }
    pattArray.clear();
    pattArray = tmpPattArray;
}

void makeMirroRotPythonPattern(std::vector<Pattern>& pattArray) {
    std::vector<Pattern> tmpPattArray, finalPattArray;
    std::vector<int> tmpCooVector;


    for (int i = 0; i < pattArray.size(); i++) {
        tmpCooVector = pattArray[i].pixelesCoo;
        finalPattArray.push_back(pattArray[i]);
        for (int j = 0; j < 4; j++) {
            if (j == 0) {
                //añadir espejo del patron inicial
                Pattern newPatternMirror(tmpPattArray.size(), pattArray[i].N);
                newPatternMirror.addPixelCooVector(newPatternMirror.mirrorPatternCoo(tmpCooVector));
                tmpPattArray.push_back(newPatternMirror);
                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();
            }
            else {
                //añdir rotacion del patron base
                Pattern newPatternRot(tmpPattArray.size(), pattArray[i].N);
                tmpCooVector = newPatternRot.rotatePatternCoo(tmpCooVector);
                newPatternRot.addPixelCooVector(tmpCooVector);
                tmpPattArray.push_back(newPatternRot);
                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();
                //rot espejo de la rotacion
                Pattern newPatternRotMirror(tmpPattArray.size(), pattArray[i].N);
                newPatternRotMirror.addPixelCooVector(newPatternRotMirror.mirrorPatternCoo(tmpCooVector));
                tmpPattArray.push_back(newPatternRotMirror);
                finalPattArray.push_back(tmpPattArray.back());
                tmpPattArray.clear();
            }
        }
    }
    pattArray.clear();
    pattArray = finalPattArray;
}

void infoPatternUpdateIDPython(std::vector<std::vector<Pattern>>& H_patternArray) {
    int id = 0;
    for (int i = 0; i < H_patternArray.size(); i++) {
        findUniquePythonPattern(H_patternArray[i]);
        for (int j = 0; j < H_patternArray[i].size(); j++) {
            H_patternArray[i][j].id = j;
            H_patternArray[i][j].hierarchy_Level = i;
        }
    }

}

void definePatterns_PythonExamples(const std::vector<int>& cooPixelPattern,std::vector<std::vector<Pattern>>& H_patternArray, const std::vector<Pixel>& posibleTiles, const std::vector<int>& desire_size, bool rotMirror) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;
    std::vector<Pattern> pattArray;
    //seperacion de la imagen en multiples patrones
    //for (int y = 0; y <= inputImageHeight - N; y++)
    int size = sqrt(cooPixelPattern.size());
    int pos = 0;
    for (int z = 0; z < desire_size.size(); z++) {
        pattArray.clear();
        for (int y = 0; y <= size - desire_size[z]; y++) {
            for (int x = 0; x <= size - desire_size[z]; x++) {
                for (int i = 0; i < desire_size[z]; i++) {
                    for (int j = 0; j < desire_size[z]; j++) {
                        pos = (x + j + y * size + i * size);
                        tmpCooVector.push_back(cooPixelPattern[pos]);
                    }
                }
                Pattern newPattern(0, desire_size[z]);
                newPattern.addPixelCooVector(tmpCooVector);
                pattArray.push_back(newPattern);
                tmpCooVector.clear();
            }
        }
        if (rotMirror) {
            findUniquePythonPattern(pattArray);
            makeMirroRotPythonPattern(pattArray);
        }
        findUniquePythonPattern(pattArray);
        for (int a = 0; a < pattArray.size(); a++)
            H_patternArray[H_patternArray.size()-desire_size.size() + z].push_back(pattArray[a]);
    }

}

//definicion  de las rutas usadas por los ejemplos de python
void generate_File_Paths_Folder(std::vector<std::string>& TOP_HIERARCHIES, std::vector<std::string>& MID_HIERARCHIES, std::vector<std::string>& BASE_EXAMPLES) {


    for (int j = 1; j <= 4; ++j) {
        TOP_HIERARCHIES.push_back("examples/2.5D/top_1/Rot" + std::to_string(j) + ".txt");
        TOP_HIERARCHIES.push_back("examples/2.5D/top_2/Rot" + std::to_string(j) + ".txt");
    }

    for (int i : {1, 2}) {
        MID_HIERARCHIES.push_back("examples/2.5D/mid_" + std::to_string(i) + ".txt");
    }

    for (int i = 3; i <= 4; ++i) {
        for (int j = 1; j <= 4; ++j) {
            MID_HIERARCHIES.push_back("examples/2.5D/mid_" + std::to_string(i) + "/Rot" + std::to_string(j) + ".txt");
        }
    }

    for (int i = 1; i <= 4; ++i) {
        BASE_EXAMPLES.push_back("examples/2.5D/example_1/Rot" + std::to_string(i) + ".txt");
        BASE_EXAMPLES.push_back("examples/2.5D/example_2/Rot" + std::to_string(i) + ".txt");
    }

}

//obetener la información guardada en los .txt
std::vector<std::vector<int>> readFiles(const std::vector<std::string>& filePaths) {
    std::vector<std::vector<int>> allNumbers;

    for (const std::string& filePath : filePaths) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filePath << std::endl;
            exit(0);
        }
        std::vector<int> numbers;
        std::string line;
        while (std::getline(file, line)) {
            
            std::stringstream ss(line);
            std::string number;

            while (std::getline(ss, number, ',')) {
                try {
                    // Eliminar espacios en blanco adicionales
                    number.erase(remove_if(number.begin(), number.end(), ::isspace), number.end());
                    if (!number.empty()) {
                        numbers.push_back(std::stoi(number));
                    }
                }
                catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid number in file " << filePath << ": " << number << std::endl;
                }
                catch (const std::out_of_range& e) {
                    std::cerr << "Number out of range in file " << filePath << ": " << number << std::endl;
                }
            }

            
        }
        allNumbers.push_back(numbers);
        file.close();
    }

    return allNumbers;
}

void load_H_patternArray(std::vector<std::vector<Pattern>>& H_patternArray, std::vector<std::vector<int>>& patterns, bool is_high_pattern, bool rotMirror) {
    std::vector<Pattern> tmp_patternArray;

    for (int i = 0; i < patterns.size(); i++) {
        Pattern newPattern(0, sqrt(patterns[i].size()));
        newPattern.addPixelCooVector(patterns[i]);
        newPattern.need_Predefine_Color = true;
        newPattern.highPattern = is_high_pattern;
        newPattern.id = tmp_patternArray.size();
        newPattern.weight = 1;
        tmp_patternArray.push_back(newPattern);
    }
    if (rotMirror) {
        findUniquePythonPattern(tmp_patternArray);
        makeMirroRotPythonPattern(tmp_patternArray);
    }
    H_patternArray.push_back(tmp_patternArray);
    tmp_patternArray.clear();
}

//funcion para reconstruir una imagen a partir del mapa generado
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        pixelVectorSalida.push_back(tiles[unCollapseMap[i].front()]);
    }
}

// funcion adicional para imprimir los mapas de entrada como archivos .ppm (inutil para el algoritmo, uso de la imagenes para el paper)
void reconstructMap_DataInput(std::vector<std::vector<int>>& Map, const std::vector<Pixel>& tiles) {
    std::vector<Pixel> pixels;
    int ancho, alto;
    std::string rutaArchivo;

    for (int z = 0; z < Map.size(); z++) {
        pixels.clear();

        for (int i = 0; i < Map[z].size(); i++) {
            pixels.push_back(tiles[Map[z][i]]);
        }

        ancho = sqrt(pixels.size());
        alto = ancho;

        ControlPoint(ancho);

        rutaArchivo = "Mapa_" + std::to_string(z) + ".ppm";

        std::ofstream archivo(rutaArchivo, std::ios::binary);
        if (!archivo.is_open()) {
            std::cerr << "Error al abrir el archivo para escritura: " << rutaArchivo << std::endl;
        }

        archivo << "P6\n" << ancho << " " << alto << "\n255\n";
        archivo.write(reinterpret_cast<const char*>(pixels.data()), pixels.size() * sizeof(Pixel));

        // Verificar si ocurrieron errores durante la escritura
        if (archivo.bad()) {
            std::cerr << "Error al escribir en el archivo: " << rutaArchivo << std::endl;
            archivo.close();
        }

        archivo.close();

    }
}

//obtener la información de los patrones de python
void read_Example_Folder(const std::string& mode, std::vector<std::vector<Pattern>>& H_patternArray, const std::vector<Pixel>& posibleTiles,const std::vector<int>& desire_Size) {

    std::vector<std::string> TOP_HIERARCHIES;
    std::vector<std::string> MID_HIERARCHIES;
    std::vector<std::string> BASE_EXAMPLES;

    generate_File_Paths_Folder(TOP_HIERARCHIES, MID_HIERARCHIES, BASE_EXAMPLES);
    // Leer los archivos y extraer los números
    std::vector<std::vector<int>> top_Pattern = readFiles(TOP_HIERARCHIES);
    std::vector<std::vector<int>> mid_Pattern = readFiles(MID_HIERARCHIES);
    std::vector<std::vector<int>> low_Pattern = readFiles(BASE_EXAMPLES);

    //linea para imprimir los mapas de entrada como archivos .ppm
    //reconstructMap_DataInput(low_Pattern, posibleTiles);

    if (mode == "HWFC") {
        H_patternArray.reserve(2 + desire_Size.size());
        load_H_patternArray(H_patternArray, top_Pattern, true, false);
        load_H_patternArray(H_patternArray, mid_Pattern, false, false);
    }
    
    else if (mode == "MWFC") {
        int aux = top_Pattern.front().size();
        std::vector<std::vector<int>> top_Pattern1;
        std::vector<std::vector<int>> top_Pattern2;
        for (int i = 0; i < top_Pattern.size();i++) {
            if (aux == top_Pattern[i].size()) {
                top_Pattern1.push_back(top_Pattern[i]);
            }
            else {
                top_Pattern2.push_back(top_Pattern[i]);
            }
        }

        H_patternArray.reserve(3 + desire_Size.size());

        load_H_patternArray(H_patternArray, top_Pattern1, true, false);
        load_H_patternArray(H_patternArray, top_Pattern2, true, false);
        load_H_patternArray(H_patternArray, mid_Pattern, true, false);
        
    }
    else {
        H_patternArray.reserve(desire_Size.size());
    }
    
    int resize = H_patternArray.size() + desire_Size.size();
    H_patternArray.resize(resize);

    for (int i = 0; i < low_Pattern.size(); i++) {
         definePatterns_PythonExamples(low_Pattern[i], H_patternArray, posibleTiles, desire_Size, false);
    }
}
//funcion para la lectura de la imagen de ejemplo
bool read_Example_PPM(const std::string& exampleName, int& w, int& h, std::vector<Pixel>& pixeles) {
    std::ifstream archivo(exampleName, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << exampleName << std::endl;
        return false;
    }

    std::string encabezado;
    archivo >> encabezado;

    if (encabezado != "P6") {
        std::cerr << "Formato de archivo PPM no válido." << std::endl;
        return false;
    }

    archivo >> w >> h;
    int maxValor;
    archivo >> maxValor;

    archivo.ignore(); // Ignorar el espacio en blanco después del valor máximo

    pixeles.resize(w * h);
    archivo.read(reinterpret_cast<char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));
    std::cout << "Imagen PPM leida exitosamente." << std::endl;
    return true;
}

//funcion para la escritura de una nueva imagen
bool writeImagePPM(const std::string& r, int w, int h, const std::vector<Pixel>& pixeles) {
    std::ofstream archivo(r, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << r << std::endl;
        return false;
    }

    archivo << "P6\n" << w << " " << h << "\n255\n";
    archivo.write(reinterpret_cast<const char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return true;
}



//creación de una imagen con un mosaico de los patrones
void createPatternDraw(const std::vector<Pattern>& pattern, std::vector<Pixel>& pixelVector, int& Y) {
    int lenght = pattern.front().N;
   
    int wAmount = lenght * 10 + 10;

    Pixel pixelNegro = { 0,0,0 };

    int Width = 0;

    do {
        Width++;
    } while (Width * Width < pattern.size());

    int z = 0;
    Y = lenght * Width + Width;
    do {
        for (int x = 0; x < lenght; x++) {
            for (int j = 0; j < Width; j++) {
                for (int i = 0; i < lenght; i++) {
                    if ((z * Width + j) < pattern.size()) {
                        pixelVector.push_back(pattern[z * Width + j].pixeles[x * lenght + i]);
                    }
                    else {
                        pixelVector.push_back(pixelNegro);
                    }
                }
                pixelVector.push_back(pixelNegro);
            }
        }
        for (int i = 0; i < lenght * Width + Width; i++) {
            pixelVector.push_back(pixelNegro);
        }
        z++;
    } while (Width > z);


}

//creación de carpeta con los resultados
bool crearCarpeta(const std::string& ruta) {
    try {
        if (!fs::exists(ruta)) {
            return fs::create_directory(ruta);
        }
        return true; // La carpeta ya existe
    }
    catch (const std::exception& e) {
        std::cerr << "Error al crear la carpeta: " << e.what() << std::endl;
        return false;
    }
}

std::string obtenerNombreUnico(const std::string& carpeta, const std::string& nombreBase, const std::string& fileType) {
    std::string nombre = nombreBase + "_" + "0" + fileType;
    int contador = 1;
    if (!fs::exists(carpeta + "/" + nombre)) {
        return nombre;
    }
    else {
        while (fs::exists(carpeta + "/" + nombre)) {
            std::ostringstream oss;
            oss << contador;
            nombre = nombreBase + "_" + oss.str() + fileType;
            contador++;
        }
    }

    return nombre;
}

std::vector<std::string> ObtenerNombresArchivos(const std::string& carpeta, const std::string& nombreBase, const std::string& fileType) {
    std::string nombre = nombreBase;
    std::vector<std::string> filesName;
    int contador = 0;
    while (fs::exists(carpeta + "/" + nombre + "_" + std::to_string(contador) + fileType)) {
        filesName.push_back(nombre + "_" + std::to_string(contador) + fileType);
        contador++;
    }
    return filesName;
}

std::vector<Pattern> cargarVectorDesdeArchivoCSV(const int N, const std::string& carpetaBase, const std::string format, char delimiter = ';') {

    if (!crearCarpeta(carpetaBase)) {
        ControlString("carpeta no existe");
    }
    std::string rutaArchivo = carpetaBase + "/" + format;
    std::vector<Pattern> vec;
    std::ifstream archivo(rutaArchivo);
    if (archivo.is_open()) {
        std::string linea;
        while (std::getline(archivo, linea)) {
            Pattern pat(0, 0);
            std::istringstream iss(linea);
            std::string token;

            // Leer ID
            if (std::getline(iss, token, delimiter)) {
                pat.id = std::stoi(token);
            }

            // Leer N
            if (std::getline(iss, token, delimiter)) {
                pat.N = std::stoi(token);
            }

            // Leer weight
            if (std::getline(iss, token, delimiter)) {
                pat.weight = std::stoi(token);
            }

            // Leer pixelesCoo
            while (std::getline(iss, token, delimiter)) {
                pat.pixelesCoo.push_back(std::stoi(token));
            }
            if (pat.N == N) {
                vec.push_back(pat);
            }

        }
        archivo.close();
    }
    else {
        std::cerr << "Error al abrir el archivo en txt: " << rutaArchivo << std::endl;
    }

    

    return vec;
}

bool SaveMapFile(const std::string& carpetaBase, const std::string nombreBase, int ancho, int alto, const std::vector<Pixel>& pixels) {
    if (!crearCarpeta(carpetaBase)) {
        return false;
    }
    // Ruta completa del archivo .ppm
    std::string rutaArchivo = carpetaBase + "/" + nombreBase;

    // Escribir los datos en el archivo .ppm
    std::ofstream archivo(rutaArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << rutaArchivo << std::endl;
        return false;
    }

    archivo << "P6\n" << ancho << " " << alto << "\n255\n";
    archivo.write(reinterpret_cast<const char*>(pixels.data()), pixels.size() * sizeof(Pixel));

    // Verificar si ocurrieron errores durante la escritura
    if (archivo.bad()) {
        std::cerr << "Error al escribir en el archivo: " << rutaArchivo << std::endl;
        archivo.close();
        return false;
    }

    archivo.close();
    return true;

}

bool SaveInfo_CSV_PatternsUsed(const std::string& carpetaBase, const std::string& formato, const std::vector<Pattern>& dataPattern, std::string& fileName) {
    if (!crearCarpeta(carpetaBase)) {
        return false;
    }
    std::string nombreUnico = obtenerNombreUnico(carpetaBase, "Map", formato);
    fileName = nombreUnico;
    std::string rutaArchivo = carpetaBase + "/" + nombreUnico;
    std::ofstream archivo(rutaArchivo, std::ios::binary);

    if (archivo.is_open()) {
        for (const Pattern& pat : dataPattern) {
            archivo << pat.id << ";";
            archivo << pat.N << ";";
            archivo << pat.weight << ";";
            for (int i = 0; i < pat.pixelesCoo.size(); i++) {
                archivo << pat.pixelesCoo[i] << ";";
            }
            /*
            std::ostringstream oss;
            for (size_t i = 0; i < pat.pixelesCoo.size(); ++i) {
                oss << std::to_string(pat.pixelesCoo[i]);
                if (i != pat.pixelesCoo.size() - 1) { // Añadir una coma excepto después del último elemento
                    oss << ",";
                }
            }
            std::string result = oss.str();
            archivo << result << ";";
            */
            archivo << "\n";
        }
        archivo.close();
    }

    return true;
}

bool SaveInfo_CSV_Hamming(const std::string& carpetaBase, const std::vector<std::string>& MapNames, const std::vector<std::vector<float>>& similarity) {
    if (!crearCarpeta(carpetaBase)) {
        return false;
    }
    std::string rutaArchivo = carpetaBase + "/" + "Hamming" + ".csv";
    std::ofstream archivo(rutaArchivo, std::ios::binary);
    if (archivo.is_open()) {
        for (int i = 0; i <= MapNames.size(); i++) {
            for (int j = 0; j <= MapNames.size(); j++) {
                if (j == 0 && i == 0) {
                    archivo << "Hamming" << ";";
                }
                else if (i == 0) {
                    archivo << MapNames[j-1] << ";";
                }
                else if (j == 0) {
                    archivo << MapNames[i-1] << "h;";
                }
                else if (similarity[i - 1][j - 1] == -1) {
                    archivo << ";";
                }
                else {
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(6) << similarity[i - 1][j - 1]; // Ajusta la precisión según sea necesario
                    std::string aux_str = oss.str();
                    std::replace(aux_str.begin(), aux_str.end(), '.', ',');

                    archivo << aux_str << ";";
                }
            }
            archivo << "\n";
        }
        archivo.close();
    }
    return true;
}

bool SaveInfo_CSV_KLD(const std::vector<int>& N, const std::string& carpetaBase) {
    if (N.size() <= 0)
        return false;
    if (!crearCarpeta(carpetaBase)) {
        return false;
    }
    for (int z = 0; z < N.size(); z++) {
        std::string aux = std::to_string(N[z]);
        std::string rutaArchivo = carpetaBase + "/" + "KLD_" + aux + ".csv";
        std::vector<Pattern> LoadedFile1, LoadedFile2;
        std::ofstream archivo(rutaArchivo, std::ios::binary);
        archivo.imbue(std::locale(std::locale::classic(), new Comma));
        std::vector<std::string> MapNames = ObtenerNombresArchivos(carpetaBase, "Map", ".csv");
        if (archivo.is_open()) {
            
            for (int i = 0; i <= MapNames.size(); i++) {
                for (int j = 0; j <= MapNames.size(); j++) {
                    if (j == 0 && i == 0) {
                        archivo << "KLD_" << aux << ";";
                    }
                    else if (i == 0) {
                        archivo << MapNames[j - 1] << ";";
                    }
                    else if (j == 0) {
                        archivo << MapNames[i - 1] << ";";
                    }
                    else if (i <= j) {
                        archivo << ";";
                    }
                    else {
                        LoadedFile1 = cargarVectorDesdeArchivoCSV(N[z], carpetaBase, MapNames[i - 1]);
                        LoadedFile2 = cargarVectorDesdeArchivoCSV(N[z], carpetaBase, MapNames[j - 1]);
                        double result1 = KL_Divergence(LoadedFile1, LoadedFile2);
                        double result2 = KL_Divergence(LoadedFile2, LoadedFile1);
                        double result = (result1 + result2) / 2;
                        archivo << result << ";";
                    }
                }
                archivo << "\n";
            }
            archivo.close();
        }
    }
    return true;
}

void PerformMetrics(const std::string& baseFolder, const std::vector<int>& N, const int Map_size) {
    //realizar hamming
    std::vector<std::string> SavePPMNames = ObtenerNombresArchivos(baseFolder, "Map", ".ppm");
    std::vector<Pixel> Map1, Map2; //std::vector<Pixel> Map1 = simpleHammingPPM( + folder +"/" + nombreUnico);
    std::string Map1_Name, Map2_Name;
    std::vector<std::vector<float>> similarity;
    std::vector<float> tmp_simil;
    float aux = 0.0f;
    for (int i = 0; i < SavePPMNames.size(); i++) {
        Map1_Name = baseFolder + "/" + SavePPMNames[i];
        Map1 = simpleHammingPPM(Map1_Name);
        tmp_simil.clear();
        for (int j = 0; j < SavePPMNames.size(); j++) {
            if(i == j)
                tmp_simil.push_back(-1);
            else if (i <= j)
                tmp_simil.push_back(-1);
            else {
                Map2_Name = baseFolder + "/" + SavePPMNames[j];
                Map2 = simpleHammingPPM(Map2_Name);
                int a = hammingMetric(Map1, Map2);
                int b = (Map_size * Map_size);
                aux = static_cast<float>(a)/ static_cast<float>(b);
                tmp_simil.push_back(aux);
            }
        }
        similarity.push_back(tmp_simil);
    }
    SaveInfo_CSV_Hamming(baseFolder, SavePPMNames, similarity);

    //realizar KLD
    SaveInfo_CSV_KLD(N, baseFolder);

}

bool SaveTime(const std::string& carpetaBase, const std::string& nombreBase, const float& duration, const int& backtrackingUse, const int fail_generation) {
    if (!crearCarpeta(carpetaBase)) {
        return false;
    }
    // Ruta completa del archivo .ppm
    
    std::string rutaArchivo = carpetaBase + "/" + "GenerationTime" + ".csv";

    // Escribir los datos en el archivo .ppm
    std::ofstream archivo(rutaArchivo, std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << rutaArchivo << std::endl;
        return false;
    }
    ControlString("tiempo guardado");
    archivo << nombreBase << ";" << duration << ";" << backtrackingUse << ";"<< fail_generation << "\n";

    // Verificar si ocurrieron errores durante la escritura
    if (archivo.bad()) {
        std::cerr << "Error al escribir en el archivo: " << rutaArchivo << std::endl;
        archivo.close();
        return false;
    }

    archivo.close();
    return true;
}

void SaveMapAndTime(const std::string& baseFolder,const std::vector<Pixel>& data, const std::vector<Pattern>& dataPattern, const std::string mode, const int size, const std::vector<Pixel>& posibleTiles, const float& duration, const int& backtrackingUse, const int fail_generation) {
    std::string fileName,
        nombreUnico = obtenerNombreUnico(
            baseFolder, "Map", ".ppm");
    //guardado del mapa actual de la ejecución
    SaveMapFile(baseFolder, nombreUnico, size, size, data);
    SaveInfo_CSV_PatternsUsed(baseFolder, ".csv", dataPattern, fileName);
    SaveTime(baseFolder, nombreUnico, duration, backtrackingUse, fail_generation);
}