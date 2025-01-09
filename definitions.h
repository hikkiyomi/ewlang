#pragma once

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum nodeEnum { typeCon, typeId, typeOpr };

struct conNodeType;
struct idNodeType;
struct oprNodeType;

struct nodeType {
    nodeEnum type;
    std::variant<conNodeType*, idNodeType*, oprNodeType*> value;
};

struct conNodeType {
    int value;
};

struct idNodeType {
    int i;
};

struct oprNodeType {
    int oper;
    int nops;
    std::vector<nodeType*> op;
};

extern std::map<int, int> sym;
extern std::map<std::string, int> tokenToYylVal;
extern std::map<int, std::string> yylValToToken;
extern std::vector<std::string> returnList;

extern std::string outputFile;
extern std::ostream* outputPtr;
