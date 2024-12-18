#include "Analizador.h"
#include "AstNodo.h"
#include <iostream>
#include <set>
#include <string>

const std::set<std::string> tiposValidos = {"entero", "flotante", "booleano",
                                            "cadena", "largo",    "corto",
                                            "vacio",  "var",      "const"};

bool esValidoTipo(const std::string &tipo) {
  return tiposValidos.find(tipo) != tiposValidos.end();
}

// Constructor
Analizador::Analizador(const std::vector<Token> &t) : tokens(t), index(0) {}

ASTNodo *Analizador::parse() {
  ASTNodo *root = new ASTNodo("programa", "", 0, 0);

  // Procesar todas las instrucciones en la lista de tokens
  while (TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion != nullptr) {
      root->hijos.push_back(instruccion);
    } else {
      // Si la instrucción no es válida, avanza para evitar bucles infinitos
      avanzar();
    }
  }

  return root;
}

// Devuelve el token actual
Token Analizador::TokenActual() {
  return index < tokens.size() ? tokens[index] : Token{"EOF", "", -1, -1};
}

// Avanza al siguiente token
void Analizador::avanzar() {
  if (index < tokens.size())
    index++;
}

// Reporta un error sintáctico
void Analizador::reportarError(const std::string &mensaje, int linea, int col) {
  std::cerr << "Error sintáctico en línea " << linea << ", col " << col << ": "
            << mensaje << std::endl;
}

ASTNodo *Analizador::parsePrograma() {
  ASTNodo *root = new ASTNodo("programa", "", 0, 0);

  while (TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion != nullptr) {
      root->hijos.push_back(instruccion);
    } else {
      avanzar(); // Saltar tokens inválidos
    }
  }

  return root;
}

ASTNodo *Analizador::parseInstruccion() {
  Token current = TokenActual();

  // Ignorar la llave de cierre '}' sin generar error
  if (current.lexema == "}") {
    return nullptr; // Finaliza el bloque actual
  }

  ASTNodo *instruccion = nullptr;

  if (current.lexema == "si") {
    instruccion = parseSi();
  } else if (current.lexema == "mientras") {
    instruccion = parseMientras();
  } else if (current.lexema == "para") {
    instruccion = parsePara();
  } else if (current.lexema == "devolver") {
    instruccion = parseDevolver();
  } else if (current.lexema == "funcion") {
    instruccion = parseFuncion();
  } else if (current.tipo == "PalRes") {
    instruccion = parseDeclaracion();
  } else if (current.tipo == "id") {
    instruccion = parseAsignacion();
  } else {
    reportarError("Instruccion desconocida", current.linea, current.col);
    return nullptr;
  }

  // Consumir punto y coma ';' después de una instrucción válida
  if (TokenActual().lexema == ";") {
    avanzar(); // Consumir ';'
  }

  return instruccion;
}

ASTNodo *Analizador::parseDeclaracion() {
  Token current = TokenActual();

  // Verificar si el token es una palabra reservada válida
  if (current.tipo == "PalRes" && esValidoTipo(current.lexema)) {
    std::string tipo = current.lexema;
    avanzar(); // Consumir el tipo

    // Verificar si el siguiente token es un identificador
    Token identificador = TokenActual();
    if (identificador.tipo != "id") {
      reportarError("Se esperaba un identificador en la declaracion",
                    identificador.linea, identificador.col);
      return nullptr;
    }
    avanzar(); // Consumir el identificador

    ASTNodo *nodoDeclaracion =
        new ASTNodo("declaracion", tipo + " " + identificador.lexema,
                    current.linea, current.col);

    // Verificar si hay una asignación opcional
    if (TokenActual().lexema == "=") {
      avanzar(); // Consumir '='
      ASTNodo *expresion = parseExpresion();
      if (!expresion) {
        reportarError("Expresion invalida en la declaracion",
                      TokenActual().linea, TokenActual().col);
        return nullptr;
      }
      nodoDeclaracion->hijos.push_back(expresion);
    }

    // Verificar y consumir explícitamente el punto y coma ';'
    if (TokenActual().lexema != ";") {
      reportarError("Se esperaba ';' al final de la declaracion",
                    TokenActual().linea, TokenActual().col);
      return nullptr;
    }
    avanzar(); // Consumir ';'

    return nodoDeclaracion;
  }

  reportarError("Tipo de declaracion invalida", current.linea, current.col);
  return nullptr;
}

ASTNodo *Analizador::parseAsignacion() {
  Token identificador = TokenActual();

  // Verificar que el token sea un identificador
  if (identificador.tipo != "id") {
    reportarError("Se esperaba un identificador en la asignacion",
                  identificador.linea, identificador.col);
    return nullptr;
  }
  avanzar(); // Consumir el identificador

  // Verificar el símbolo '='
  if (TokenActual().lexema != "=") {
    reportarError("Se esperaba '=' en la asignacion", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '='

  // Procesar la expresión después del '='
  ASTNodo *expresion = parseExpresion();
  if (!expresion) {
    reportarError("Expresion invalida en la asignacion", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  // Crear y retornar el nodo de asignación (sin consumir el ';')
  ASTNodo *nodoAsignacion = new ASTNodo("asignacion", identificador.lexema,
                                        identificador.linea, identificador.col);
  nodoAsignacion->hijos.push_back(expresion);

  return nodoAsignacion;
}

ASTNodo *Analizador::parseExpresion() {
  ASTNodo *termino = parseTermino();

  while (TokenActual().tipo == "Simbolo" &&
         (TokenActual().lexema == "+" || TokenActual().lexema == "-" ||
          TokenActual().lexema == "||" || TokenActual().lexema == "&&" ||
          TokenActual().lexema == "<" || TokenActual().lexema == ">" ||
          TokenActual().lexema == "<=" || TokenActual().lexema == ">=" ||
          TokenActual().lexema == "==" || TokenActual().lexema == "!=")) {

    Token operador = TokenActual();
    avanzar();

    ASTNodo *siguienteTermino = parseTermino();
    ASTNodo *nuevoNodo =
        new ASTNodo("operacion", operador.lexema, operador.linea, operador.col);
    nuevoNodo->hijos.push_back(termino);
    nuevoNodo->hijos.push_back(siguienteTermino);
    termino = nuevoNodo;
  }
  return termino;
}

ASTNodo *Analizador::parseTermino() {
  ASTNodo *factor = parseFactor();

  while (TokenActual().tipo == "Simbolo" &&
         (TokenActual().lexema == "*" || TokenActual().lexema == "/")) {
    Token operador = TokenActual();
    avanzar();

    ASTNodo *siguienteFactor = parseFactor();
    ASTNodo *nuevoNodo =
        new ASTNodo("operacion", operador.lexema, operador.linea, operador.col);
    nuevoNodo->hijos.push_back(factor);
    nuevoNodo->hijos.push_back(siguienteFactor);
    factor = nuevoNodo;
  }

  return factor;
}

ASTNodo *Analizador::parseFactor() {
  Token current = TokenActual();

  // Si es un número
  if (current.tipo == "numEntero" || current.tipo == "numReal") {
    ASTNodo *nodo =
        new ASTNodo("numero", current.lexema, current.linea, current.col);
    avanzar(); // Consumir el número
    return nodo;
  }

  // Si es un identificador
  if (current.tipo == "id") {
    ASTNodo *nodo = new ASTNodo("identificador", current.lexema, current.linea,
                                current.col);
    avanzar(); // Consumir el identificador
    return nodo;
  }

  // Si es una expresión entre paréntesis
  if (current.lexema == "(") {
    avanzar(); // Consumir "("
    ASTNodo *nodoExpresion = parseExpresion();
    if (TokenActual().lexema != ")") {
      reportarError("Se esperaba ')'", TokenActual().linea, TokenActual().col);
      return nullptr;
    }
    avanzar(); // Consumir ")"
    return nodoExpresion;
  }

  // Si no coincide con ninguna regla
  reportarError("Factor invalido", current.linea, current.col);
  return nullptr;
}

ASTNodo *Analizador::parseSi() {
  Token current = TokenActual();

  // Verificar la palabra reservada 'si'
  if (current.lexema != "si") {
    reportarError("Se esperaba la palabra reservada 'si'", current.linea,
                  current.col);
    return nullptr;
  }
  avanzar(); // Consumir 'si'

  // Verificar el paréntesis de apertura '('
  if (TokenActual().lexema != "(") {
    reportarError("Se esperaba '(' después de 'si'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '('

  // Parsear la condición dentro de los paréntesis
  ASTNodo *condicion = parseExpresion();
  if (!condicion) {
    reportarError("Condicion invalida en 'si'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  // Verificar el paréntesis de cierre ')'
  if (TokenActual().lexema != ")") {
    reportarError("Se esperaba ')' despues de la condicion",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ')'

  // Verificar la palabra reservada 'entonces'
  if (TokenActual().lexema != "entonces") {
    reportarError("Se esperaba 'entonces' despues de 'si'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir 'entonces'

  // Verificar la llave de apertura '{'
  if (TokenActual().lexema != "{") {
    reportarError("Se esperaba '{' para el bloque del 'si'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '{'

  // Crear nodo para el bloque 'si'
  ASTNodo *nodoSi = new ASTNodo("si", "", current.linea, current.col);
  nodoSi->hijos.push_back(condicion); // Agregar la condición como hijo

  // Procesar instrucciones dentro del bloque 'si'
  while (TokenActual().lexema != "}" && TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion)
      nodoSi->hijos.push_back(instruccion);
  }

  // Verificar la llave de cierre '}'
  if (TokenActual().lexema != "}") {
    reportarError("Se esperaba '}' para cerrar el bloque del 'si'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '}'

  // Manejar el bloque 'sino' opcional
  if (TokenActual().lexema == "sino") {
    avanzar(); // Consumir 'sino'

    // Verificar la llave de apertura '{'
    if (TokenActual().lexema != "{") {
      reportarError("Se esperaba '{' para el bloque del 'sino'",
                    TokenActual().linea, TokenActual().col);
      return nullptr;
    }
    avanzar(); // Consumir '{'

    ASTNodo *nodoSino =
        new ASTNodo("sino", "", TokenActual().linea, TokenActual().col);
    while (TokenActual().lexema != "}" && TokenActual().tipo != "EOF") {
      ASTNodo *instruccion = parseInstruccion();
      if (instruccion)
        nodoSino->hijos.push_back(instruccion);
    }

    // Verificar la llave de cierre '}'
    if (TokenActual().lexema != "}") {
      reportarError("Se esperaba '}' para cerrar el bloque del 'sino'",
                    TokenActual().linea, TokenActual().col);
      return nullptr;
    }
    avanzar(); // Consumir '}'

    nodoSi->hijos.push_back(nodoSino);
  }

  return nodoSi;
}

ASTNodo *Analizador::parseMientras() {
  Token current = TokenActual();
  if (current.lexema != "mientras")
    return nullptr;

  avanzar(); // Consumir "mientras"

  if (TokenActual().lexema != "(") {
    reportarError("Se esperaba '(' después de 'mientras'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "("

  ASTNodo *condicion = parseExpresion(); // Procesar la condición
  if (!condicion) {
    reportarError("Condicion invalida en 'mientras'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  if (TokenActual().lexema != ")") {
    reportarError("Se esperaba ')' después de la condicion",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ")"

  if (TokenActual().lexema != "{") {
    reportarError("Se esperaba '{' para el bloque del 'mientras'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "{"

  ASTNodo *nodoMientras =
      new ASTNodo("mientras", "", current.linea, current.col);
  while (TokenActual().lexema != "}" && TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion)
      nodoMientras->hijos.push_back(instruccion);
  }

  if (TokenActual().lexema != "}") {
    reportarError("Se esperaba '}' para cerrar el bloque del 'mientras'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "}"

  nodoMientras->hijos.insert(nodoMientras->hijos.begin(), condicion);
  return nodoMientras;
}

ASTNodo *Analizador::parseFuncion() {
  Token current = TokenActual();

  // Verificar la palabra reservada "funcion"
  if (current.lexema != "funcion") {
    reportarError("Se esperaba la palabra reservada 'funcion'", current.linea,
                  current.col);
    return nullptr;
  }
  avanzar(); // Consumir "funcion"

  // Verificar el nombre de la función (identificador)
  Token nombreFuncion = TokenActual();
  if (nombreFuncion.tipo != "id") {
    reportarError("Se esperaba un nombre de funcion (identificador)",
                  nombreFuncion.linea, nombreFuncion.col);
    return nullptr;
  }
  avanzar(); // Consumir el nombre de la función

  // Crear el nodo de la función
  ASTNodo *nodoFuncion = new ASTNodo("funcion", nombreFuncion.lexema,
                                     nombreFuncion.linea, nombreFuncion.col);

  // Verificar el paréntesis de apertura "("
  if (TokenActual().lexema != "(") {
    reportarError("Se esperaba '(' despues del nombre de la funcion",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "("

  // Procesar parámetros
  std::vector<ASTNodo *> parametros;
  while (TokenActual().lexema != ")" && TokenActual().tipo != "EOF") {
    Token parametro = TokenActual();

    // Cada parámetro debe ser un identificador
    if (parametro.tipo != "id") {
      reportarError("Se esperaba un identificador como parametro",
                    parametro.linea, parametro.col);
      return nullptr;
    }
    parametros.push_back(new ASTNodo("parametro", parametro.lexema,
                                     parametro.linea, parametro.col));
    avanzar(); // Consumir el parámetro

    // Verificar si hay una coma ","
    if (TokenActual().lexema == ",") {
      avanzar(); // Consumir ","
    } else if (TokenActual().lexema != ")") {
      reportarError("Se esperaba ',' o ')'", TokenActual().linea,
                    TokenActual().col);
      return nullptr;
    }
  }

  // Verificar el paréntesis de cierre ")"
  if (TokenActual().lexema != ")") {
    reportarError("Se esperaba ')' despues de los parametros",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ")"

  // Agregar parámetros como hijos del nodo de la función
  for (ASTNodo *parametro : parametros) {
    nodoFuncion->hijos.push_back(parametro);
  }

  // Verificar la llave de apertura "{"
  if (TokenActual().lexema != "{") {
    reportarError("Se esperaba '{' para el inicio del cuerpo de la funcion",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "{"

  // Procesar el cuerpo de la función
  while (TokenActual().lexema != "}" && TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion) {
      nodoFuncion->hijos.push_back(instruccion);
    } else {
      reportarError("Instrucción invalida en el cuerpo de la funcion",
                    TokenActual().linea, TokenActual().col);
      return nullptr;
    }
  }

  // Verificar la llave de cierre "}"
  if (TokenActual().lexema != "}") {
    reportarError("Se esperaba '}' para cerrar el cuerpo de la funcion",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir "}"

  return nodoFuncion;
}

ASTNodo *Analizador::parseDevolver() {
  Token current = TokenActual();

  // Verificar la palabra reservada 'devolver'
  if (current.lexema != "devolver") {
    reportarError("Se esperaba la palabra reservada 'devolver'", current.linea,
                  current.col);
    return nullptr;
  }
  avanzar(); // Consumir 'devolver'

  // Procesar la expresión a devolver
  ASTNodo *expresion = parseExpresion();
  if (!expresion) {
    reportarError("Expresion invalida despues de 'devolver'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }

  // Verificar el punto y coma ";"
  if (TokenActual().lexema != ";") {
    reportarError("Se esperaba ';' despues de la expresion en 'devolver'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ";"

  // Crear y retornar el nodo 'devolver'
  ASTNodo *nodoDevolver =
      new ASTNodo("devolver", "", current.linea, current.col);
  nodoDevolver->hijos.push_back(expresion);
  return nodoDevolver;
}

ASTNodo *Analizador::parsePara() {
  Token current = TokenActual();

  // Verificar la palabra reservada 'para'
  if (current.lexema != "para") {
    reportarError("Se esperaba la palabra reservada 'para'", current.linea,
                  current.col);
    return nullptr;
  }
  avanzar(); // Consumir 'para'

  // Verificar el paréntesis de apertura '('
  if (TokenActual().lexema != "(") {
    reportarError("Se esperaba '(' despues de 'para'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '('

  // Procesar la inicialización
  ASTNodo *inicializacion = nullptr;
  if (TokenActual().tipo == "PalRes") { // Declaración
    inicializacion = parseDeclaracion();
  } else if (TokenActual().tipo == "id") { // Asignación
    inicializacion = parseAsignacion();
  } else {
    reportarError("Inicializacion invalida en 'para'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  // Procesar la condición
  ASTNodo *condicion = parseExpresion();
  if (!condicion) {
    reportarError("Condicion invalida en 'para'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  // Consumir explícitamente el ';' después de la condición
  if (TokenActual().lexema != ";") {
    reportarError("Se esperaba ';' despues de la condicion en 'para'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ';'

  // Procesar la actualización
  ASTNodo *actualizacion = parseAsignacion();
  if (!actualizacion) {
    reportarError("Actualizacion invalida en 'para'", TokenActual().linea,
                  TokenActual().col);
    return nullptr;
  }

  // Verificar paréntesis de cierre ')'
  if (TokenActual().lexema != ")") {
    reportarError("Se esperaba ')' despues de la actualizacion en 'para'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir ')'

  // Verificar la llave de apertura '{'
  if (TokenActual().lexema != "{") {
    reportarError("Se esperaba '{' para el cuerpo del 'para'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '{'

  // Crear nodo 'para'
  ASTNodo *nodoPara = new ASTNodo("para", "", current.linea, current.col);
  nodoPara->hijos.push_back(inicializacion);
  nodoPara->hijos.push_back(condicion);
  nodoPara->hijos.push_back(actualizacion);

  // Procesar el cuerpo del ciclo
  while (TokenActual().lexema != "}" && TokenActual().tipo != "EOF") {
    ASTNodo *instruccion = parseInstruccion();
    if (instruccion) {
      nodoPara->hijos.push_back(instruccion);
    } else {
      if (TokenActual().lexema == "}") {
        break; // Fin del bloque, salir del bucle
      } else {
        reportarError("Instruccion invalida dentro del cuerpo de 'para'",
                      TokenActual().linea, TokenActual().col);
        avanzar(); // Avanzar para evitar bucles infinitos
      }
    }
  }

  // Verificar la llave de cierre '}'
  if (TokenActual().lexema != "}") {
    reportarError("Se esperaba '}' para cerrar el cuerpo del 'para'",
                  TokenActual().linea, TokenActual().col);
    return nullptr;
  }
  avanzar(); // Consumir '}'

  return nodoPara;
}
