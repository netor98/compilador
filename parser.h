#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include "token.h" // Asegúrate de tener el struct Token en este archivo

using namespace std;

// Nodo del Árbol Sintáctico
struct ASTNode {
    string tipo;
    string valor;
    vector<shared_ptr<ASTNode>> hijos;

    ASTNode(const string& tipo, const string& valor) : tipo(tipo), valor(valor) {}
};

// Clase del Analizador Sintáctico
class Parser {
private:
    vector<Token> tokens;
    size_t currentTokenIndex = 0;

    Token currentToken() const;      // Obtener el token actual
    void advance();                  // Avanzar al siguiente token
    void error(const string& message); // Manejo de errores

    // Métodos para las producciones de la gramática
    shared_ptr<ASTNode> program();
    shared_ptr<ASTNode> declaracion();
    shared_ptr<ASTNode> expresion();

public:
    Parser(const vector<Token>& tokens); // Constructor
    shared_ptr<ASTNode> parse();         // Método principal para analizar
};

#endif
