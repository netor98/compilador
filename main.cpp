// Archivo: main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include "./automatas.cpp"
#include <regex>
#include <vector>

using namespace std;

enum tipoToken {
    numReal,         // 0
    palRes,          // 1
    numEntero,       // 2
    simbolo, // 3
    signo,      // 4
    id
  };

struct Token {
    string tipo;
    int valor;
    int linea;
    int col;
    string lexema;
};

// Nodo de la lista doblemente enlazada
struct Node {
    Token token;
    Node* prev;
    Node* sig;
    Node(const Token& t) : token(t), prev(nullptr), sig(nullptr) {}
};

// Clase para manejar la lista doblemente enlazada
class DoublyLinkedList {
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    // Insertar un nodo al final de la lista
    void insert(const Token& token) {
        Node* newNode = new Node(token);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->sig = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    // Mostrar los tokens en la lista
    void display() const {
        Node* current = head;
        while (current) {
            cout << "Token Type: " << current->token.tipo
                      << ", Valor: " << current->token.valor
                      << ", Posicion: " << current->token.col
                      << ", Linea: " << current->token.linea
                      << ", Lexema: " << current->token.lexema << endl;

            current = current->sig;
        }
    }

private:
    Node* head;
    Node* tail;
};

//TODO: llenar los campos del token dependiendo del automata que acepte la cadena
void insertarNodo(const string& line, DoublyLinkedList &tokenList, int lineNumber) {
    if (automata_NR(line))
        tokenList.insert(Token("numReal", 0, lineNumber, 0, line ));

    else if (automata_NE(line)) {
        tokenList.insert(Token("numEntero", 0, lineNumber, 0, line ));
    }

    else if (automata_IDEN(line)) {
        tokenList.insert(Token("id", 0, lineNumber, 0, line ));
    }

    else if (automata_PALRES(line)) {
        tokenList.insert(Token("palRes", 0, lineNumber, 0, line ));
    }
}

vector<Token> analyzeSymbols(const string& text, int lineNumber) {
    vector<Token> tokens;

    // Expresión regular para detectar los símbolos
    regex symbols_regex(R"((<=|>=|==|!=|<|>|\(|\)|,|;|\.))");
    sregex_iterator iter(text.begin(), text.end(), symbols_regex);
    sregex_iterator end;

    // Agregar cada coincidencia encontrada a la lista de tokens
    while (iter != end) {
        smatch match = *iter;
        tokens.push_back({"Simbolo", 0, lineNumber, static_cast<int>(match.position()), match.str() });
        ++iter;
    }

    return tokens;
}

// Función para detectar "signos" (+, -, *, /)
vector<Token> analyzeSigns(const string& text, int lineNumber) {
    vector<Token> tokens;

    // Expresión regular para detectar los signos
    regex signs_regex(R"([\+\-\*/])");
    sregex_iterator iter(text.begin(), text.end(), signs_regex);
    sregex_iterator end;

    // Agregar cada coincidencia encontrada a la lista de tokens
    while (iter != end) {
        smatch match = *iter;
        tokens.push_back({"Sign", 0, lineNumber, static_cast<int>(match.position()),  match.str() });
        ++iter;
    }

    return tokens;
}


void processLineWithSymbols(const string& line, DoublyLinkedList& tokenList, int lineNumber) {
    // Expresiones regulares para detectar símbolos y signos
    regex symbols_regex(R"((<=|>=|==|=|!=|<|>|\(|\)|,|;|\.|\s))");
    regex signs_regex(R"([\+\-\*/])");

    // Combinamos ambas expresiones en una sola para detectar ambos tipos
    regex combined_regex(R"((<=|>=|==|=|!=|<|>|\(|\)|,|;|\.|\s|[\+\-\*/]))");

    size_t lastPos = 0; // Posición inicial para el análisis de texto
    sregex_iterator iter(line.begin(), line.end(), combined_regex);
    sregex_iterator end;

    while (iter != end) {
        smatch match = *iter;
        size_t symbolPos = match.position();

        // Procesar el texto antes del símbolo o signo
        if (symbolPos > lastPos) {
            string beforeSymbol = line.substr(lastPos, symbolPos - lastPos);

            insertarNodo(beforeSymbol, tokenList, lineNumber);
        }

        // Insertar el símbolo o signo encontrado como token
        string symbol = match.str();
        if (symbol != " ") {
            if (regex_match(symbol, signs_regex)) {
                tokenList.insert({"Sign", 0, lineNumber, static_cast<int>(match.position()),  match.str() });
            } else {
                tokenList.insert({"Simbolo", 0, lineNumber, static_cast<int>(match.position()),  match.str() });
            }
        }

        // Actualizar la posición para procesar el resto de la línea
        lastPos = symbolPos + match.length();
        ++iter;
    }

    // Procesar cualquier texto restante después del último símbolo o signo
    if (lastPos < line.size()) {
        string remainingText = line.substr(lastPos);

        insertarNodo(remainingText, tokenList, lineNumber);
        }
}

// Función que procesa el archivo de texto y genera la lista de tokens
void processFile(const string& filename, DoublyLinkedList& tokenList) {
    ifstream file(filename);
    string line;
    int lineNumber = 1;

    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return;
    }

    while (getline(file, line)) {
        processLineWithSymbols(line, tokenList, lineNumber);
        lineNumber++;
    }

    file.close();
}

int main() {
    string filename = R"(C:\Users\Dell\CLionProjects\Tokenizador\output.txt)"; // Nombre del archivo de entrada

    DoublyLinkedList tokenList;
    processFile(filename, tokenList);

    // Mostrar los tokens encontrados
    tokenList.display();

    return 0;
}

