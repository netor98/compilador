#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "headers/automatas.h"

using namespace std;

bool EsPalabraReservada(const string& palabra) {
    return keyword_map.find(palabra) != keyword_map.end();
}



void PreprocesarArchivo(string file) {
    ifstream inputFile(file); //Abre el archivo en modo lectura
    ofstream outputFile("output.txt"); // Crea un archivo

    if (!inputFile.is_open()) { //Si el archivo no existe
        cout << "Archivo no existe" << endl;
        return;
    }

    string content; // Variable para guardar el contenido de cada linea

    while(getline(inputFile, content)) { // Mientras haya lineas en el archivo
        content.erase(remove(content.begin(), content.end(), '\t'), content.end()); // Elimina los tabs

        content.erase(content.begin(), find_if(content.begin(), content.end(), [](unsigned char ch) {
            return !isspace(ch);
        })); // Elimina los espacios al inicio de la linea


        // content.erase(remove_if(content.begin(), content.end(), ::isspace), content.end()); 

        string processedContent;
        string token;
        bool insideKeyword = false;


        for (size_t i = 0; i < content.size(); i++) {
            char ch = content[i];

            // Si el carácter es parte de un identificador o palabra (letra o número)
            if (isalnum(ch) || ch == '_') {
                token += ch; // Construir el token
            } else {
                // Si el token no está vacío, procesarlo
                if (!token.empty()) {
                    // Verificar si es una palabra reservada
                    if (EsPalabraReservada(token)) {
                        processedContent += token + ""; // Añadir palabra reservada con espacio
                    } else {
                        processedContent += token; // Añadir identificador sin espacio extra
                    }
                    token.clear(); // Limpiar el token después de procesarlo
                }

                // Añadir el símbolo o carácter no alfanumérico al resultado procesado
                processedContent += ch;
            }
        }

        // Procesar el último token si queda algo
        if (!token.empty()) {
            if (EsPalabraReservada(token)) {
                processedContent += token + " "; // Añadir palabra reservada con espacio
            } else {
                processedContent += token; // Añadir identificador
            }
        }

        //
        //
        // if (content.find("int ") == 0) {
        //     // Elimina todos los espacios restantes excepto el que sigue a "int"
        //     content.erase(remove_if(content.begin() + 4, content.end(), ::isspace), content.end());
        // } else {
        //     content.erase(remove_if(content.begin(), content.end(), ::isspace), content.end()); 
        //     // Elimina los espacios en blanco
        // }
        //
        if (!(content.empty() || (content[0] == '#' || content[0] == '/'))) {

        outputFile << processedContent << endl;
        }
            //     //
        //     //
        //     auto it = keyword_map.find(content);
        //     if (it != keyword_map.end()) {
        //         // Format output as ID + space + keyword
        //         content = to_string(it->second) + " " + it->first;
        //     }
        //     outputFile << content << endl;
        // }

    }
    // Cierra el archivo
    inputFile.close();
}

void RecorrerArchivo(string file) {
    ifstream inputFile(file);

    if (!inputFile.is_open()) { //Si el archivo no existe
        cout << "Archivo no existe" << endl;
        return;
    }

    char caracter; // Variable para almacenar cada carácter leído

    while (inputFile.get(caracter)) {  // Leer carácter por carácter
        if (caracter != '\n') {
            cout << caracter << endl;  // Imprimir el carácter en la consola
        }
    }

    inputFile.close();
}



// int main() {
    // Llama a la funcion PreprocesarArchivo
    // PreprocesarArchivo("./ejemplos_lenguaje/ejemplo.txt");
    // RecorrerArchivo("./output.txt");
    // if(automata_NR("-201")) cout << "Es número real" << endl;
    // else cout << "No es un número real" << endl;
    // 
    // cout<<"-----------------------------\n"<<endl;
    //
    // if(automata_NE("201")) cout << "Es número entero" << endl;
    // else cout << "No es un número entero" << endl;
    //
    // cout<<"-----------------------------\n"<<endl;
    // 
    // if(automata_PALRES("cambiar")) cout << "Es palabra reservada" << endl;
    // else cout << "No es palabra reservada" << endl;
    //
    // cout<<"-----------------------------\n"<<endl;
    // 
    // if(automata_IDEN("_cambiar")) cout << "Es identificador" << endl;
    // else cout << "No es un identificador válido" << endl;
    // return 0;
// }
