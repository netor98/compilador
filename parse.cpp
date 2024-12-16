#include <parser.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>

using namespace std;

class ASTNode {
public:
    string type;
    string value;
    vector<shared_ptr<ASTNode>> children;

    ASTNode(string t, string v) : type(t), value(v) {}
};

class Parser {
public:
    Parser(const vector<string>& tokens) : tokens(tokens), currentTokenIndex(0) {}

    shared_ptr<ASTNode> parse();
    void error(const string& message);

private:
    vector<string> tokens;
    size_t currentTokenIndex;

    string currentToken() const;
    void advance();
};

string Parser::currentToken() const {
    return tokens[currentTokenIndex];
}

void Parser::advance() {
    if (currentTokenIndex < tokens.size()) currentTokenIndex++;
}

void Parser::error(const string& message) {
    cerr << "Error sintáctico: " << message << endl;
    exit(EXIT_FAILURE);
}

shared_ptr<ASTNode> Parser::parse() {
    // Implementa un análisis de prueba
    cout << "Iniciando análisis sintáctico..." << endl;
    return make_shared<ASTNode>("root", "program");
}

int main() {
    vector<string> tokens = {"token1", "token2", "token3"};
    Parser parser(tokens);

    shared_ptr<ASTNode> ast = parser.parse();
    cout << "Árbol sintáctico generado exitosamente." << endl;

    return 0;
}
