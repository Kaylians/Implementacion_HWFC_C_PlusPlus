#include "ReadWrite.h"
#include "Pixel.h"
#include "Pattern.h"
#include "Metrics.h"
#include "DebugUtility.h"
#include <locale>
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
        std::cerr << "Formato de archivo PPM no v�lido en Hamming." << std::endl;
    }

    archivo >> w >> h;
    int maxValor;
    archivo >> maxValor;

    archivo.ignore(); // Ignorar el espacio en blanco despu�s del valor m�ximo

    pixeles.resize(w * h);
    archivo.read(reinterpret_cast<char*>(pixeles.data()), pixeles.size() * sizeof(Pixel));

    return pixeles;
}

void getPredefineTiles(std::vector<Pixel>& posibleTiles) {

    posibleTiles.emplace_back(0, 255, 0);       // Verde

    posibleTiles.emplace_back(255, 0, 0);       // Rojo
    posibleTiles.emplace_back(255, 0, 0);       // Rojo

    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro

    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�

    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco
    posibleTiles.emplace_back(255, 255, 255);   // Blanco

    posibleTiles.emplace_back(255, 255, 0);     // Amarillo

    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul

    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�

    posibleTiles.emplace_back(255, 165, 0);     // Naranja

    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro
    posibleTiles.emplace_back(0, 0, 0);         // Negro

    posibleTiles.emplace_back(128, 128, 128);   // Gris

    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�
    posibleTiles.emplace_back(139, 69, 19);     // Caf�

    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
    posibleTiles.emplace_back(0, 0, 255);       // Azul
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
            tmpPattArray[tmpPattArray.size() - 1].id = tmpPattArray.size() - 1;
        }
    pattArray.clear();
    pattArray = tmpPattArray;
}

void infoPatternUpdateIDPython(std::vector<Pattern>& pLow, std::vector<Pattern>& pHigh) {
    std::vector<Pattern> pBase;
    pBase.reserve(pLow.size() + pHigh.size());
    pBase.insert(pBase.end(), pLow.begin(), pLow.end());

    if (pHigh.size() > 0)
        pBase.insert(pBase.end(), pHigh.begin(), pHigh.end());
    findUniquePythonPattern(pBase);

    for (int i = 0; i < pHigh.size(); i++) {
        for (int j = 0; j < pBase.size(); j++) {
            if (pHigh[i].compareCooPattern(pBase[j].pixelesCoo)) {
                pHigh[i].id = pBase[j].id;
            }
        }
    }
    for (int i = 0; i < pLow.size(); i++) {
        for (int j = 0; j < pBase.size(); j++) {
            if (pLow[i].compareCooPattern(pBase[j].pixelesCoo)) {
                pLow[i].id = pBase[j].id;
            }
        }
    }
}

void definePatterns_PythonExamples(const std::vector<int>& cooPixelPattern, std::vector<Pattern>& pattArray, const std::vector<Pixel>& posibleTiles, const int size, const std::vector<int>& desire_size) {
    std::vector<Pixel> tmpVector;
    std::vector<int> tmpCooVector;

    //seperacion de la imagen en multiples patrones
    //for (int y = 0; y <= inputImageHeight - N; y++)
    int pos = 0;
    for (int z = 0; z < desire_size.size(); z++) {
        for (int y = 0; y <= size - desire_size[z]; y++) {
            for (int x = 0; x <= size - desire_size[z]; x++) {
                for (int i = 0; i < desire_size[z]; i++) {
                    for (int j = 0; j < desire_size[z]; j++) {
                        pos = (x + j + y * size + i * size);
                        tmpCooVector.push_back(cooPixelPattern[pos]);
                    }
                }
                Pattern newPattern(pattArray.size(), desire_size[z]);
                newPattern.addPixelCooVector(tmpCooVector);
                pattArray.push_back(newPattern);
                tmpCooVector.clear();
            }
        }
        //findUniquePattern(pattArray);
        //makeMirroRotPattern(pattArray);
        findUniquePythonPattern(pattArray);
    }
    std::cout << "Patrones base obtenidos de la imagen: " << pattArray.size() << std::endl;
    std::sort(pattArray.begin(), pattArray.end(), comparePatternWFC);

    for (int i = 0; i < pattArray.size(); i++) {
        pattArray[i].id = i;
    }

    std::cout << "Patrones obtenidos de la imagen: " << pattArray.size() << std::endl;
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

//obetener la informaci�n guardada en los .txt
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

//obtener la informaci�n de los patrones de python
void read_Example_Folder(std::vector<Pattern>& patternArrayLow, std::vector<Pattern>& patternArrayHigh, std::vector<Pixel>& posibleTiles, const std::vector<int>& desire_Size) {

    std::vector<std::string> TOP_HIERARCHIES;
    std::vector<std::string> MID_HIERARCHIES;
    std::vector<std::string> BASE_EXAMPLES;
    generate_File_Paths_Folder(TOP_HIERARCHIES, MID_HIERARCHIES, BASE_EXAMPLES);

    // Leer los archivos y extraer los n�meros
    std::vector<std::vector<int>> top_Pattern = readFiles(TOP_HIERARCHIES);
    std::vector<std::vector<int>> mid_Pattern = readFiles(MID_HIERARCHIES);
    std::vector<std::vector<int>> low_Pattern = readFiles(BASE_EXAMPLES);

    for (int i = 0; i < top_Pattern.size(); i++) {
        Pattern newPattern(0, sqrt(top_Pattern[i].size()));
        newPattern.addPixelCooVector(top_Pattern[i]);
        newPattern.need_Predefine_Color = true;
        patternArrayHigh.push_back(newPattern);

    }
    for (int i = 0; i < low_Pattern.size(); i++) {
        definePatterns_PythonExamples(low_Pattern[i], patternArrayLow, posibleTiles, sqrt(low_Pattern[i].size()), desire_Size);
    }
    for (int i = 0; i < mid_Pattern.size(); i++) {
        Pattern newPattern(0, sqrt(mid_Pattern[i].size()));
        newPattern.addPixelCooVector(mid_Pattern[i]);
        newPattern.need_Predefine_Color = true;
        patternArrayLow.push_back(newPattern);

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
        std::cerr << "Formato de archivo PPM no v�lido." << std::endl;
        return false;
    }

    archivo >> w >> h;
    int maxValor;
    archivo >> maxValor;

    archivo.ignore(); // Ignorar el espacio en blanco despu�s del valor m�ximo

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
//funcion para reconstruir una imagen a partir del mapa generado
void reconstructMap(std::vector<Pixel>& pixelVectorSalida, std::vector<std::vector<int>>& unCollapseMap, const std::vector<Pixel>& tiles) {
    for (int i = 0; i < unCollapseMap.size(); i++) {
        pixelVectorSalida.push_back(tiles[unCollapseMap[i].front()]);
    }
}
//creaci�n de una imagen con un mosaico de los patrones
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
//creaci�n de carpeta con los resultados
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
            archivo << pat.N << ";";
            archivo << pat.weight << ";";
            for (int i = 0; i < pat.pixelesCoo.size(); i++) {
                archivo << pat.pixelesCoo[i] << ";";
            }
            archivo << "\n";
        }
        archivo.close();
    }

    return true;
}
bool SaveInfo_CSV_Hamming(const std::string& carpetaBase, const std::vector<std::string>& MapNames, const std::vector<std::vector<int>>& similarity) {
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
                else {
                    archivo << similarity[i-1][j-1] << ";";
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
void PerformMetrics(const std::string& baseFolder, const std::vector<int>& N) {
    //realizar hamming
    std::vector<std::string> SavePPMNames = ObtenerNombresArchivos(baseFolder, "Map", ".ppm");
    std::vector<Pixel> Map1, Map2; //std::vector<Pixel> Map1 = simpleHammingPPM( + folder +"/" + nombreUnico);
    std::string Map1_Name, Map2_Name;
    std::vector<std::vector<int>> similarity;
    std::vector<int> tmp_simil;

    for (int i = 0; i < SavePPMNames.size(); i++) {
        Map1_Name = baseFolder + "/" + SavePPMNames[i];
        Map1 = simpleHammingPPM(Map1_Name);
        tmp_simil.clear();
        for (int j = 0; j < SavePPMNames.size(); j++) {
            Map2_Name = baseFolder + "/" + SavePPMNames[j];
            Map2 = simpleHammingPPM(Map2_Name);
            int aux = hammingMetric(Map1, Map2);
            tmp_simil.push_back(aux);
        }
        similarity.push_back(tmp_simil);
    }
    SaveInfo_CSV_Hamming(baseFolder, SavePPMNames, similarity);

    //realizar KLD
    SaveInfo_CSV_KLD(N, baseFolder);

}
bool SaveTime(const std::string& carpetaBase, const std::string& nombreBase, const float& duration, const int& backtrackingUse) {
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
    archivo << nombreBase << ";" << duration << ";" << backtrackingUse << "\n";

    // Verificar si ocurrieron errores durante la escritura
    if (archivo.bad()) {
        std::cerr << "Error al escribir en el archivo: " << rutaArchivo << std::endl;
        archivo.close();
        return false;
    }

    archivo.close();
    return true;
}
void SaveMapAndTime(const std::string& baseFolder,const std::vector<Pixel>& data, const std::vector<Pattern>& dataPattern, const std::string mode, const int size, const std::vector<Pixel>& posibleTiles, const float& duration, const int& backtrackingUse) {
    std::string fileName,
        nombreUnico = obtenerNombreUnico(
            baseFolder, "Map", ".ppm");

    //guardado del mapa actual de la ejecuci�n
    SaveMapFile(baseFolder, nombreUnico, size, size, data);
    SaveInfo_CSV_PatternsUsed(baseFolder, ".csv", dataPattern, fileName);
    SaveTime(baseFolder, nombreUnico, duration, backtrackingUse);
    
}