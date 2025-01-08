%{
#include <iostream>
#include <string>
#include <map>

#include "definitions.h"
#include "y.tab.h"

int yyerror(const char* s);

int tokenCounter = 256;
std::map<std::string, int> tokenToYylVal;
std::map<int, std::string> yylValToToken;

int getYylVal(const std::string& token) {
    if (!tokenToYylVal.contains(token)) {
        tokenToYylVal[token] = tokenCounter++;
    }

    return tokenToYylVal[token];
}
%}

%%

[-()<>=+*/;{}.,%]  { return *yytext; }

">="        return GE;
"<="        return LE;
"=="        return EQ;
"!="        return NE;
"while"     return WHILE;
"if"        return IF;
"else"      return ELSE;
"print"     return PRINT; 
"let"       return LET;
"function"  return FUNCTION;

[_a-zA-Z][a-zA-Z0-9_]*  {
                            yylval.sIndex = getYylVal(yytext);
                            yylValToToken[yylval.sIndex] = yytext;
                            return VARIABLE;
                        }

[0-9]+  {
            yylval.iValue = atoi(yytext);
            return INTEGER;
        }


[ \t\n]+    ;

.       { yyerror("invalid character"); }

%%

int yywrap(void) {
    return 1;
}
