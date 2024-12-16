#include "parser.h"
#include "token.h"
// Constructor
Parser::Parser(const vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

// Obtener el token actual
Token Parser::currentToken() const {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    return {"", "", -1, -1, ""}; // Token vacío
}

// Avanzar al siguiente token
void Parser::advance() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

// Manejo de errores
void Parser::error(const string& message) {
    Token token = currentToken();
    cerr << "Error sintactico en la linea " << token.linea
         << ", columna " << token.col << ": " << message
         << " (token: " << token.lexema << ")" << endl;
    exit(EXIT_FAILURE);
}

// Producción principal: program
shared_ptr<ASTNode> Parser::program() {
    auto node = make_shared<ASTNode>("program", "");
    while (currentToken().tipo != "") {
        node->hijos.push_back(declaracion());
    }
    return node;
}

// Producción: declaración
shared_ptr<ASTNode> Parser::declaracion() {
    Token token = currentToken();
    if (token.tipo == "PalRes" && token.lexema == "var") {
        auto node = make_shared<ASTNode>("declaracion", token.lexema);
        advance();
        if (currentToken().tipo == "id") {
            node->hijos.push_back(make_shared<ASTNode>("id", currentToken().lexema));
            advance();
            if (currentToken().lexema == "=") {
                node->hijos.push_back(make_shared<ASTNode>("operador", "="));
                advance();
                node->hijos.push_back(expresion());
            } else {
                error("Se esperaba '=' en la declaración.");
            }
        } else {
            error("Se esperaba un identificador después de 'var'.");
        }
        return node;
    }
    error("Se esperaba una declaración con 'var'.");
    return nullptr; // Nunca llega aquí
}

// Producción: expresión
shared_ptr<ASTNode> Parser::expresion() {
    auto node = make_shared<ASTNode>("expresion", currentToken().lexema);
    if (currentToken().tipo == "numEntero" || currentToken().tipo == "numReal") {
        advance();
        return node;
    }
    error("Se esperaba un número en la expresión.");
    return nullptr; // Nunca llega aquí
}

// Método principal para analizar
shared_ptr<ASTNode> Parser::parse() {
    return program();
}
