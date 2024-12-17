#ifndef ANALIZADOR_H
#define ANALIZADOR_H

#include <string>
#include <vector>
#include "./AstNodo.h"
#include "./Token.h"
using namespace std;

class Analizador{
private:
  std::vector<Token> tokens; // Lista de tokens como entrada
  int index;                 // Índice del token actual

  Token TokenActual(); // Devuelve el token actual
  void avanzar();       // Avanza al siguiente token
  void reportarError(const std::string &mensaje, int linea, int columna);

  // Funciones para reglas de la gramática
  ASTNodo *parseDeclaracion();
  ASTNodo *parseAsignacion();
  ASTNodo *parseExpresion();
  ASTNodo *parseTermino();
  ASTNodo *parseFactor();
  ASTNodo *parseInstruccion();
  ASTNodo *parseSi();
  ASTNodo *parseMientras();
  ASTNodo *parseFuncion();
  ASTNodo *parsePara();
  ASTNodo *parseDevolver();
  ASTNodo *parsePrograma();

public:
  Analizador(const std::vector<Token> &t);
  ASTNodo *parse();
};

#endif
