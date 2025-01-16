%{
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "definitions.h"
#include "vm_definitions.h"

extern FILE* yyin;
std::ostream* outputPtr = &std::cout;
std::string outputFile = "output";

int yylex();
int yyerror(const char* s);

std::map<int, int> sym;
std::vector<std::string> functionParameters;
std::vector<nodeType*> functionArgs;
std::vector<nodeType*> returnList;
std::vector<nodeType*> variableList;
std::vector<nodeType*> exprList;

nodeType* opr(int oper, int nops, ...);
nodeType* id(int i);
nodeType* con(const char* value);
void freeNode(nodeType* p);

extern int ex(nodeType* p, bool push = true);
extern void ProcessImports(const std::string& filename);
extern bool CheckExtension(const std::string& filename);
%}

%union {
    const char* iValue;
    int sIndex;
    nodeType* nPtr;
};

%token <iValue> INTEGER 
%token <sIndex> VARIABLE
%token LENGTH
%token FOR WHILE IF PRINT LET FUNCTION CALL RETURN ARRAY ACCESS MASSIGN
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left BIN_AND BIN_OR
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list expr_list variable_list multiple_assignment

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
                        *outputPtr << yylValToToken[$2] << ":\n";

                        for (const auto& param : functionParameters) {
                            *outputPtr << "\tpop\t" << param << "\n";
                        }

                        ex($7);
                        freeNode($7);
                        functionParameters.clear();
                        *outputPtr << "\treturn\t0\n";
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
    ';'                                                                           { $$ = opr(';', 2, NULL, NULL); }
    | expr ';'                                                                    { $$ = $1; }
    | LET VARIABLE '=' expr ';'                                                   { $$ = opr('=', 2, id($2), $4); }
    | ARRAY VARIABLE '[' expr ']' ';'                                             { $$ = opr(ARRAY, 2, id($2), $4); }
    | PRINT expr ';'                                                              { $$ = opr(PRINT, 1, $2); }
    | VARIABLE '=' expr ';'                                                       { $$ = opr('=', 2, id($1), $3); }
    | VARIABLE '[' expr ']' '=' expr ';'                                          { $$ = opr('=', 3, id($1), $3, $6); }
    | multiple_assignment ';'                                                     { $$ = $1; }
    | RETURN return_list ';'                                                      { $$ = opr(RETURN, 1, returnList); }
    | WHILE '(' expr ')' '{' stmt_list '}'                                        { $$ = opr(WHILE, 2, $3, $6); }

    | FOR '(' multiple_assignment ';' expr ';' multiple_assignment ')' '{' stmt_list '}' {
                                                        $$ = opr(FOR, 4, $3, $5, $7, $10);
                                                    }

    | IF '(' expr ')' '{' stmt_list '}' %prec IFX                                 { $$ = opr(IF, 2, $3, $6); }
    | IF '(' expr ')' '{' stmt_list '}' ELSE '{' stmt_list '}'                    { $$ = opr(IF, 3, $3, $6, $10); }
    ;

multiple_assignment:
                   variable_list '=' expr_list { $$ = opr(MASSIGN, 2, $1, $3); }
                   ;

variable_list:
             VARIABLE                     { $$ = id($1); }
             | variable_list ',' VARIABLE { $$ = opr(',', 2, $1, id($3)); }
             ;

expr_list:
         expr                 { $$ = $1; }
         | expr_list ',' expr { $$ = opr('#', 2, $1, $3); }
         ;

stmt_list:
         stmt               { $$ = $1; }
         | stmt_list stmt   { $$ = opr(';', 2, $1, $2); }
         ;

expr:
    INTEGER                                { $$ = con($1); }
    | VARIABLE                             { $$ = id($1); }
    | LENGTH '(' VARIABLE ')'              { $$ = opr(LENGTH, 1, id($3)); }
    | VARIABLE '(' arg_list ')'            { $$ = opr(CALL, 2, id($1), functionArgs); }             
    | VARIABLE '[' expr ']'                { $$ = opr(ACCESS, 2, id($1), $3); }
    | '-' expr %prec UMINUS                { $$ = opr(UMINUS, 1, $2); }
    | expr '+' expr                        { $$ = opr('+', 2, $1, $3); }
    | expr '-' expr                        { $$ = opr('-', 2, $1, $3); }
    | expr '*' expr                        { $$ = opr('*', 2, $1, $3); }
    | expr '/' expr                        { $$ = opr('/', 2, $1, $3); }
    | expr '%' expr                        { $$ = opr('%', 2, $1, $3); }
    | expr '<' expr                        { $$ = opr('<', 2, $1, $3); }
    | expr '>' expr                        { $$ = opr('>', 2, $1, $3); }
    | expr GE expr                         { $$ = opr(GE, 2, $1, $3); }
    | expr LE expr                         { $$ = opr(LE, 2, $1, $3); }
    | expr NE expr                         { $$ = opr(NE, 2, $1, $3); }
    | expr EQ expr                         { $$ = opr(EQ, 2, $1, $3); }
    | expr BIN_AND expr                    { $$ = opr(BIN_AND, 2, $1, $3); }
    | expr BIN_OR expr                     { $$ = opr(BIN_OR, 2, $1, $3); }
    | '(' expr ')'                         { $$ = $2; }
    ;

arg_list:
        arg_list ',' expr { functionArgs.push_back($3); }
        | expr            { functionArgs = std::vector<nodeType*>{$1}; }
        |                 { functionArgs.clear(); }
        ;

return_list:
           return_list ',' expr { returnList.push_back($3); }
           | expr               { returnList = std::vector<nodeType*>{$1}; }
           |                    { returnList.clear(); }
           ;

%%

nodeType* con(const char* value) {
    nodeType* p = new nodeType;

    p->type = typeCon;
    p->value = new conNodeType(std::move(std::string(value)));

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
    } else if (oper == RETURN) {
        const auto& returns = va_arg(ap, std::vector<nodeType*>);
        opr->op = returns;
        opr->nops = returns.size();
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
    if (argc == 1) {
        throw std::runtime_error("provide an input file");
    }

    std::string converted = std::string(argv[1]);

    if (!std::filesystem::exists(converted)) {
        throw std::runtime_error("input file does not exist");
    }

    std::string mergedFile = std::string(argv[1]) + "_processed";
    assert(yyin == NULL);
    
    ProcessImports(converted);
    yyin = fopen(mergedFile.c_str(), "r");

    if (argc >= 3) {
        outputFile = std::string(argv[2]);
        outputPtr = new std::ofstream(outputFile);
    }

    try {
        yyparse();
        closeStreams();

        VirtualMachine vm;
        vm.Run();
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    std::filesystem::remove(mergedFile);

    return 0;
}
