#ifndef AUTOMATAS_H
#define AUTOMATAS_H

#include <string>
#include <unordered_map>

bool automata_NR(std::string cadena);
bool automata_NE(std::string cadena);
bool automata_PALRES(std::string cadena);
bool automata_IDEN(std::string cadena);

void PreprocesarArchivo(std::string file);

enum Keyword {
  Var,
  Const,
  Verdadero,
  Falso,
  Si,
  Sino,
  Entonces,
  Hacer,
  Mientras,
  Para,
  Romper,
  Caso,
  Cambiar,
  Booleano,
  Flotante,
  Entero,
  Cadena,
  Largo,
  Corto,
  Vacio,
  Funcion,
  Devolver,
  Escribir,
  Leer
};

std::unordered_map<std::string, Keyword> keyword_map = {
    {"var", Var},
    {"const", Const},
    {"verdadero", Verdadero},
    {"falso", Falso},
    {"si", Si},
    {"sino", Sino},
    {"entonces", Entonces},
    {"hacer", Hacer},
    {"mientras", Mientras},
    {"para", Para},
    {"romper", Romper},
    {"caso", Caso},
    {"cambiar", Cambiar},
    {"booleano", Booleano},
    {"Escribir", Escribir},
    {"Leer", Leer},
    {"flotante", Flotante},
    {"entero", Entero},
    {"cadena", Cadena},
    {"largo", Largo},
    {"corto", Corto},
    {"vacio", Vacio},
    {"funcion", Funcion},
    {"devolver", Devolver}};
#endif
