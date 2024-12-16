// Archivo: main.cpp
#include "./AnalizadorSintactico/Analizador.h"
#include "./AnalizadorSintactico/AstNodo.h"
#include "./headers/automatas.h"
#include "./automatas.cpp"
#include "./prePreocesarArchivo.cpp"
#include "./bprinter/include/bprinter/table_printer.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
//ola montalvo
using namespace std;


enum tipoToken {
  numReal,   // 0
  palRes,    // 1
  numEntero, // 2
  simbolo,   // 3
  signo,     // 4
  id
};

// struct Token {
//   string tipo;
//   string valor;
//   int linea;
//   int col;
//   string lexema;
// };

// Nodo de la lista doblemente enlazada
struct Node {
  Token token;
  Node *prev;
  Node *sig;
  Node(const Token &t) : token(t), prev(nullptr), sig(nullptr) {}
};

// Clase para manejar la lista doblemente enlazada
class DoublyLinkedList {
public:
  DoublyLinkedList() : head(nullptr), tail(nullptr) {}

  // Insertar un nodo al final de la lista
  void insert(const Token &token) {
    Node *newNode = new Node(token);
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
    Node *current = head;
    bprinter::TablePrinter tp(&std::cout);
    tp.AddColumn("Token Tipo", 15);
    tp.AddColumn("Valor", 12);
    tp.AddColumn("Linea", 12);
    tp.AddColumn("Posición", 12);
    tp.AddColumn("Lexema", 12);
    tp.PrintHeader();

    while (current) {
      tp << current->token.tipo << current->token.valor
         << current->token.linea <<current->token.col + 1
         << current->token.lexema;
      // cout << "Token Type: " << current->token.tipo
      //      << ", Valor: " << current->token.valor
      //      << ", Posicion: " << current->token.col
      //      << ", Linea: " << current->token.linea
      //      << ", Lexema: " << current->token.lexema << endl;

      current = current->sig;
    }
    tp.PrintFooter();
  }

  Node* getHead() const {
        return head;
    }

private:
  Node *head;
  Node *tail;
};

// TODO: llenar los campos del token dependiendo del automata que acepte la
// cadena
void insertarNodo(const string &line, DoublyLinkedList &tokenList,
                  int lineNumber, int position) {

  Token newToken;

  if (line.empty()) {
    return;
  }

  if (automata_NR(line)) {
    newToken = {"numReal", line, lineNumber, position, line};
    // tokenList.insert(Token{"numReal", to_string(real), lineNumber, 0, line});
  } else if (automata_NE(line)) {
    newToken = {"numEntero", line, lineNumber, position, line};
    // tokenList.insert(Token{"numEntero", to_string(entero), lineNumber, 0,
    // line});
  }

  else if (automata_IDEN(line)) {
    newToken = {"id", "-", lineNumber, position, line};
    // tokenList.insert(Token{"id", 0, lineNumber, 0, line});
  }

  else if (automata_PALRES(line)) {
    newToken = {"PalRes", "-", lineNumber, position, line};
    // tokenList.insert(Token{"palRes", 0, lineNumber, 0, line});
  } else {
      cout << "Token invalido: " << line << endl;
      return;
  }
  // cout << "Inserting Token: "
  //      << "Type: " << newToken.tipo << ", "
  //      << "Value: " << newToken.valor << ", "
  //      << "Line: " << newToken.linea << ", "
  //      << "Col: " << newToken.col << ", "
  //      << "Lexema: " << newToken.lexema << endl;

  tokenList.insert(newToken);

}

void processLineWithSymbols(const string &line, DoublyLinkedList &tokenList,
                            int lineNumber) {
  // Expresiones regulares para detectar símbolos y signos
  regex symbols_regex(R"((<=|>=|==|=|!=|<|>|\(|\)|,|;|:|\s|\{|\}))");
  regex signs_regex(R"([\+\-\*/])");

  // Combinamos ambas expresiones en una sola para detectar ambos tipos
  regex combined_regex(R"((<=|>=|==|=|!=|<|>|\(|\)|,|;|\s|:|\{|\}|[\+\-\*/]))");

  size_t lastPos = 0; // Posición inicial para el análisis de texto
  sregex_iterator iter(line.begin(), line.end(), combined_regex);
  sregex_iterator end;

  while (iter != end) {
    smatch match = *iter;
    size_t symbolPos = match.position();

    // Procesar el texto antes del símbolo o signo
    if (symbolPos > lastPos) {
      string beforeSymbol = line.substr(lastPos, symbolPos - lastPos);

      insertarNodo(beforeSymbol, tokenList, lineNumber, lastPos);
    }

    // Insertar el símbolo o signo encontrado como token
    string symbol = match.str();
    if (symbol != " ") {
      // cout<<symbol<<endl;
      if (regex_match(symbol, signs_regex)) {
        tokenList.insert({"Simbolo", "-", lineNumber,
                          static_cast<int>(match.position()), match.str()});
      } else {
        tokenList.insert({"Simbolo", "-", lineNumber,
                          static_cast<int>(match.position()), match.str()});
      }
    }

    // Actualizar la posición para procesar el resto de la línea
    lastPos = symbolPos + match.length();
    ++iter;
  }

  // Procesar cualquier texto restante después del último símbolo o signo
  if (lastPos < line.size()) {
    string remainingText = line.substr(lastPos);
    // cout<<remainingText<<endl;
    insertarNodo(remainingText, tokenList, lineNumber, lastPos);
  }
}

// Función que procesa el archivo de texto y genera la lista de tokens
void processFile(const string &filename, DoublyLinkedList &tokenList) {
  ifstream file(filename);
  // cout << "Leyendo archivo" << endl;

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



std::vector<Token> convertToVector(const DoublyLinkedList& list) {
    std::vector<Token> tokens; // Vector que contendrá los tokens
    Node* current = list.getHead(); // Obtiene el primer nodo de la lista

    while (current != nullptr) {
        tokens.push_back(current->token); // Copia el token al vector
        current = current->sig;          // Avanza al siguiente nodo
    }

    return tokens;
}





void printAST(ASTNodo* node, int depth = 0) {
    if (!node) return;

    for (int i = 0; i < depth; i++) cout << "  ";
    cout << node->tipo<< " (" << node->valor<< ") [Linea: " << node->linea<< ", Columna: " << node->columna + 1 << "]" << endl;

    for (ASTNodo* child : node->hijos) {
        printAST(child, depth + 1);
    }
}















int main() {
  PreprocesarArchivo("input.txt");
  string filename = R"(./output.txt)"; // Nombre del
                                       // archivo de

  DoublyLinkedList tokenList;
  processFile(filename, tokenList);

  tokenList.display();

 std::vector<Token> tokens = convertToVector(tokenList);

  Analizador analizador(tokens);
  cout << "Iniciando el análisis sintáctico..." << endl;
 ASTNodo* ast = analizador.parse();
  printAST(ast);
  return 0;
}







