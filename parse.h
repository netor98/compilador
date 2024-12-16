#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

#define MAX_LINE_LEN 256 // Define este macro según tus necesidades

// Redefiniciones de tipos para adaptarlos a C++
typedef char CHAR;
typedef unsigned long DWORD;
typedef char* PZPSTR;

class CParseXML {
public:
    CParseXML();
    DWORD Start(const CHAR* szFile);
    CHAR* GetToken();
    DWORD GetError() { return m_dwError; }

private:
    CHAR m_cCurToken[MAX_LINE_LEN];
    CHAR m_cCurWord[MAX_LINE_LEN];
    CHAR m_cCurBuffer[MAX_LINE_LEN];
    CHAR* m_pCurrentIndex;
    DWORD m_dwError;
    DWORD Read();
    DWORD SetError(DWORD dw) { m_dwError = dw; return m_dwError; }
    CHAR* GetFirstWord(CHAR*);
    CHAR* GetValue(CHAR*);
};

#endif // PARSER_H
