#include "ReadWrite.h"
#include "Pixel.h"
#include "Pattern.h"
namespace fs = std::filesystem;

//funcion para la lectura de la imagen de ejemplo
bool readImagePPM(const std::string& r, int& w, int& h, std::vector<Pixel>& pixeles) {
    std::ifstream archivo(r, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << r << std::endl;
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
bool SaveMapFile(const std::string& carpetaBase, const std::string& formato, int ancho, int alto, const std::vector<Pixel>& pixels) {
    std::string carpeta = "generatedLevels/"+carpetaBase;

    if (!crearCarpeta(carpeta)) {
        return false;
    }

    // Obtener un nombre único para el archivo .ppm
    std::string nombreBase = "Map";
    std::string nombreUnico = obtenerNombreUnico(carpeta, nombreBase, formato);

    // Ruta completa del archivo .ppm
    std::string rutaArchivo = carpeta + "/" + nombreUnico;

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
    std::cout << "Archivo guardado correctamente en: " << rutaArchivo << std::endl;
    return true;

}
bool SavePatternInfoFile(const std::string& carpetaBase, const std::string& formato, const std::vector<Pattern>& vec, std::string& name) {
    std::string carpeta = "generatedLevels/" + carpetaBase;

    if (!crearCarpeta(carpeta)) {
        return false;
    }

    std::string nombreBase = "Map";
    std::string nombreUnico = obtenerNombreUnico(carpeta, nombreBase, formato);
    name = nombreUnico;
    std::string rutaArchivo = carpeta + "/" + nombreUnico;
    std::ofstream archivo(rutaArchivo, std::ios::binary);
    
    if (archivo.is_open()) {
        for (const Pattern& pat : vec) {
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
std::vector<Pattern> cargarVectorDesdeArchivoCSV(const std::string& carpetaBase, const std::string format, char delimiter = ';') {
    std::string carpeta = "generatedLevels/" + carpetaBase;

    if (!crearCarpeta(carpeta)) {
        ControlString("carpeta no existe");
    }
    std::string rutaArchivo = carpeta + "/" + format;
    std::vector<Pattern> vec;
    std::cerr << "la ruta del archivo es: " << rutaArchivo << std::endl;
    std::ifstream archivo(rutaArchivo);
    if (archivo.is_open()) {
        std::string linea;
        while (std::getline(archivo, linea)) {
            Pattern pat (0,0);
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

            vec.push_back(pat);
            
        }
        archivo.close();
        std::cout << "Vector cargado desde archivo de texto: " << rutaArchivo << std::endl;
    }
    else {
        std::cerr << "Error al abrir el archivo en txt: " << rutaArchivo << std::endl;
    }
    return vec;
}
void SaveInfoOnFile(const std::vector<Pixel>& data, const std::vector<Pattern>& dataPattern, const std::string mode,const int size, const std::vector<Pixel>& posibleTiles) {
    std::string folder = mode +"_size_" + std::to_string(size);
    std::string fileName;

    SaveMapFile(folder, ".ppm", size, size, data);
    SavePatternInfoFile(folder, ".csv", dataPattern, fileName);
    std::vector<std::string> SaveCSVNames = ObtenerNombresArchivos("generatedLevels/"+folder,"Map", ".csv");
    for (int i = 0; i < SaveCSVNames.size(); i++) {
        ControlString(SaveCSVNames[i]);
    }
    std::vector<Pattern> patternsCargados = cargarVectorDesdeArchivoCSV(folder, fileName);
}
