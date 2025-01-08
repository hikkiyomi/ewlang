%{
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>

#include "definitions.h"

extern FILE* yyin;
std::ostream* outputPtr = &std::cout;

int yylex();
int yyerror(const char* s);

std::map<int, int> sym;
std::vector<std::string> functionParameters;
std::vector<nodeType*> functionArgs;

nodeType* opr(int oper, int nops, ...);
nodeType* id(int i);
nodeType* con(int value);
void freeNode(nodeType* p);

extern int ex(nodeType* p);
%}

%union {
    int iValue;
    int sIndex;
    nodeType* nPtr;
};

%token <iValue> INTEGER 
%token <sIndex> VARIABLE
%token WHILE IF PRINT LET FUNCTION CALL
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
       function 
       ;

function:
        // | function stmt                   { ex($2); freeNode($2); }
        | function function_declaration
        ;

function_declaration:
                    FUNCTION VARIABLE '(' parameter_list ')' '{' stmt_list '}'
                    {
                        *outputPtr << yylValToToken[$2] << ":\t";

                        for (const auto& param : functionParameters) {
                            *outputPtr << param << "\t";
                        }

                        *outputPtr << "\n";

                        ex($7);
                        freeNode($7);
                        functionParameters.clear();
                    }
                    ;

parameter_list:
              parameter_list ',' VARIABLE {
                  functionParameters.push_back(yylValToToken[$3]);
              }
              | VARIABLE {
                  functionParameters = std::vector<std::string>{yylValToToken[$1]};
              }
              | { functionParameters.clear(); }
              ;

stmt:
    ';'                                 { $$ = opr(';', 2, NULL, NULL); }
    | expr ';'                          { $$ = $1; }
    | LET VARIABLE '=' expr ';'         { $$ = opr('=', 2, id($2), $4); }
    | PRINT expr ';'                    { $$ = opr(PRINT, 1, $2); }
    | VARIABLE '=' expr ';'             { $$ = opr('=', 2, id($1), $3); }
    | WHILE '(' expr ')' stmt           { $$ = opr(WHILE, 2, $3, $5); }
    | IF '(' expr ')' stmt %prec IFX    { $$ = opr(IF, 2, $3, $5); }
    | IF '(' expr ')' stmt ELSE stmt    { $$ = opr(IF, 3, $3, $5, $7); }
    | '{' stmt_list '}'                 { $$ = $2; }
    ;

stmt_list:
         stmt               { $$ = $1; }
         | stmt_list stmt   { $$ = opr(';', 2, $1, $2); }
         ;

expr:
    INTEGER                     { $$ = con($1); }
    | VARIABLE                  { $$ = id($1); }
    | VARIABLE '(' arg_list ')' { $$ = opr(CALL, 2, id($1), functionArgs); }             
    | '-' expr %prec UMINUS     { $$ = opr(UMINUS, 1, $2); }
    | expr '+' expr             { $$ = opr('+', 2, $1, $3); }
    | expr '-' expr             { $$ = opr('-', 2, $1, $3); }
    | expr '*' expr             { $$ = opr('*', 2, $1, $3); }
    | expr '/' expr             { $$ = opr('/', 2, $1, $3); }
    | expr '%' expr             { $$ = opr('%', 2, $1, $3); }
    | expr '<' expr             { $$ = opr('<', 2, $1, $3); }
    | expr '>' expr             { $$ = opr('>', 2, $1, $3); }
    | expr GE expr              { $$ = opr(GE, 2, $1, $3); }
    | expr LE expr              { $$ = opr(LE, 2, $1, $3); }
    | expr NE expr              { $$ = opr(NE, 2, $1, $3); }
    | expr EQ expr              { $$ = opr(EQ, 2, $1, $3); }
    | '(' expr ')'              { $$ = $2; }
    ;

arg_list:
        arg_list ',' expr { functionArgs.push_back($3); }
        | expr            { functionArgs = std::vector<nodeType*>{$1}; }
        |                 { functionArgs.clear(); }
        ;

%%

nodeType* con(int value) {
    nodeType* p = new nodeType;

    p->type = typeCon;
    p->value = new conNodeType(value);

    return p;
}

nodeType* id(int i) {
    nodeType* p = new nodeType;

    p->type = typeId;
    p->value = new idNodeType(i);

    return p;
}

nodeType* opr(int oper, int nops, ...) {
    va_list ap;
    nodeType* p = new nodeType;
    oprNodeType* opr = new oprNodeType;

    p->type = typeOpr;
    opr->oper = oper;
    opr->nops = nops;

    va_start(ap, nops);

    if (oper == CALL) {
        nodeType* identifier = va_arg(ap, nodeType*);
        const auto& args = va_arg(ap, std::vector<nodeType*>);

        opr->op = args;
        opr->op.push_back(identifier);
        opr->nops = args.size() + 1;
    } else {
        opr->op.assign(nops, nullptr);

        for (int i = 0; i < nops; ++i) {
            opr->op[i] = va_arg(ap, nodeType*);
        }
    }

    va_end(ap);

    p->value = opr;

    return p;
}

void freeNode(nodeType* p) {
    if (!p) {
        return;
    }

    if (p->type == typeOpr) {
        oprNodeType* opr = std::get<oprNodeType*>(p->value);

        for (int i = 0; i < opr->nops; ++i) {
            delete opr->op[i];
        }
    }

    delete p;
}

void closeStreams() {
    if (yyin != NULL) {
        fflush(yyin);
        fclose(yyin);
        yyin = NULL;
    }

    outputPtr->flush();

    if (outputPtr != &std::cout) {
        delete outputPtr;
    }
}

int yyerror(const char* s) {
    std::cerr << s << "\n";
    closeStreams();

    return 0;
}

int main(int argc, char** argv) {
    if (argc >= 2) {
        assert(yyin == NULL);
        yyin = fopen(argv[1], "r");
    }

    if (argc >= 3) {
        outputPtr = new std::ofstream(argv[2]);
    }

    yyparse();
    closeStreams();

    return 0;
}
