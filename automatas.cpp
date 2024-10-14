#include "headers/automatas.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

using namespace std;

bool automata_NR(std::string input) {

  enum State {
    START,         // 0
    SIGN,          // 1
    INTEGER,       // 2
    DECIMAL_POINT, // 3
    FRACTION,      // 4
    ACCEPT,        // 5
    ERROR
  };

  State state = START;
  int i = 0;
  // cout << "Estado" << "-" << "Caracter" << endl;
  while (i < input.size()) {
    char currentChar = input[i];
    // cout << state << "  ---  " << currentChar << endl;
    // cout << currentChar << endl;
    switch (state) {
    case START:
      if (currentChar == '+' || currentChar == '-') {
        state = SIGN;
      }

      else if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else
        state = ERROR;
      break;

    case SIGN:
      if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else
        state = ERROR;
      break;

    case INTEGER:
      if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else if (currentChar == '.') {
        state = DECIMAL_POINT;
      }

      else
        state = ERROR;
      break;

    case DECIMAL_POINT:
      if (isdigit(currentChar)) {
        state = FRACTION;
      }

      else
        state = ERROR;
      break;

    case FRACTION:
      if (isdigit(currentChar)) {
        state = FRACTION;
      }

      else
        state = ERROR;
      break;

    default:
      state = ERROR;
      break;
    }

    if (state == ERROR)
      return false;

    i++;
  }

  // cout << input << " ";
  return state == FRACTION;
}

bool automata_NE(std::string input) {
  enum State {
    START,   // 0
    SIGN,    // 1
    INTEGER, // 3
    CERO,    // 4
    ACCEPT,
    ERROR
  };

  
  State state = START;
  int i = 0;
  // cout << "Estado" << "-" << "Caracter" << endl;
  while (i < input.size()) {
    char currentChar = input[i];
    // cout << state << "  ---  " << currentChar << endl;
    // cout << currentChar << endl;
    switch (state) {
    case START:
      if (currentChar == '+' || currentChar == '-') {
        state = SIGN;
      }

      else if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else
        state = ERROR;
      break;

    case SIGN:
      if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else
        state = ERROR;
      break;

    case INTEGER:
      if (isdigit(currentChar)) {
        state = INTEGER;
      }

      else
        state = ERROR;
      break;

    default:
      state = ERROR;
      break;
    }

    if (state == ERROR)
      return false;

    i++;
  }

  // cout << input << " ";
  return state == INTEGER;
}

bool automata_PALRES(std::string input) {
  enum State {
    START,   // 0
    KEYWORD, // 1
    ERROR,
    ACCEPT
  };

  State state = START;
  int i = 0;
  // cout << "Estado" << "-" << "Caracter" << endl;
  while (i < input.size()) {
    char currentChar = input[i];
    // cout << state << "  ---  " << currentChar << endl;
    // cout << currentChar << endl;
    switch (state) {
    case START:
      if (isalpha(currentChar)) {
        state = KEYWORD;
      } else
        state = ERROR;
      break;
    case KEYWORD:
      if (isalpha(currentChar)) {
        state = KEYWORD;
      } else
        state = ERROR;
      break;
    default:
      break;
    }

    if (state == ERROR)
      return false;

    i++;
  }

  // cout << input << " ";
  if (keyword_map.find(input) != keyword_map.end()) {
    return true;
  } else
    return false;
}

bool automata_IDEN(std::string input) {
  enum State {
    START,        // 0
    ALPHANUMERIC, // 1
    ERROR,
    ACCEPT
  };

  State state = START;
  int i = 0;
  // cout << "Estado" << "-" << "Caracter" << endl;
  while (i < input.size()) {
    char currentChar = input[i];
    // cout << state << "  ---  " << currentChar << endl;
    // cout << currentChar << endl;
    switch (state) {
    case START:
      if (isalpha(currentChar) || currentChar == '_') {
        state = ALPHANUMERIC;
      } else
        state = ERROR;
      break;

    case ALPHANUMERIC:
      if (isalpha(currentChar) || isdigit(currentChar) || currentChar == '_') {
        state = ALPHANUMERIC;
      } else
        state = ERROR;
      break;

    default:
      break;
    }

    if (state == ERROR)
      return false;

    i++;
  }

  // cout << input << " ";
  if (keyword_map.find(input) != keyword_map.end()) {
    // cout << "Es palabra reservada" << endl;
    return false;
  }
  return state == ALPHANUMERIC;
}
