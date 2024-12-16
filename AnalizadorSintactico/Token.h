#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

struct Token {
  string tipo;
  string valor;
  int linea;
  int col;
  string lexema;
};

#endif

