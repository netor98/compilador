#ifndef ASTNODO_H
#define ASTNODO_H

#include <string>
#include <vector>
using namespace std;

struct ASTNodo {
  std::string
      tipo; // Tipo del nodo (e.g., "declaracion", "asignacion", "expresion")
  std::string
      valor; // Valor asociado (e.g., nombre de variable, operador, número)
  std::vector<ASTNodo *> hijos; // Hijos del nodo
  int linea;                    // Línea del token asociado
  int columna;                  // Columna del token asociado

  ASTNodo(std::string t, std::string v, int l, int c)
      : tipo(t), valor(v), linea(l), columna(c) {}
};
#endif

