#include <cstdio>

#include "definitions.h"
#include "y.tab.h"

static int lbl;

int ex(nodeType* p) {
    std::ostream& output = *outputPtr;

    int lbl1, lbl2;

    if (!p) return 0;

    switch (p->type) {
        case typeCon:
            output << "\tpush\t" << std::get<conNodeType*>(p->value)->value
                   << "\n";
            break;
        case typeId:
            output << "\tpush\t"
                   << yylValToToken[std::get<idNodeType*>(p->value)->i] << "\n";
            break;
        case typeOpr:
            oprNodeType* node = std::get<oprNodeType*>(p->value);

            switch (node->oper) {
                case WHILE:
                    output << "L" << (lbl1 = lbl++) << ":\n";
                    ex(node->op[0]);
                    output << "\tjz\tL" << (lbl2 = lbl++) << "\n";
                    ex(node->op[1]);
                    output << "\tjmp\tL" << lbl1 << "\n";
                    output << "L" << lbl2 << ":\n";
                    break;
                case IF:
                    ex(node->op[0]);
                    if (node->nops > 2) {
                        output << "\tjz\tL" << (lbl1 = lbl++) << "\n";
                        ex(node->op[1]);
                        output << "\tjmp\tL" << (lbl2 = lbl++) << "\n";
                        output << "L" << lbl1 << ":\n";
                        ex(node->op[2]);
                        output << "L" << lbl2 << ":\n";
                    } else {
                        output << "\tjz\tL" << (lbl1 = lbl++) << "\n";
                        ex(node->op[1]);
                        output << "L" << lbl1 << ":\n";
                    }
                    break;
                case PRINT:
                    ex(node->op[0]);
                    output << "\tprint\n";
                    break;
                case '=':
                    ex(node->op[1]);
                    output << "\tpop\t"
                           << yylValToToken
                                  [std::get<idNodeType*>(node->op[0]->value)->i]
                           << "\n";
                    break;
                case UMINUS:
                    ex(node->op[0]);
                    output << "\tneg\n";
                    break;
                case CALL: {
                    for (int i = node->nops - 2; i >= 0; --i) {
                        ex(node->op[i]);
                    }

                    idNodeType* id =
                        std::get<idNodeType*>(node->op.back()->value);

                    output << "\tcall\t" << yylValToToken[id->i] << "\n";
                    break;
                }
                case RETURN: {
                    for (int i = returnList.size() - 1; i >= 0; --i) {
                        ex(returnList[i]);
                    }

                    output << "\treturn\n";
                    break;
                }
                default:
                    ex(node->op[0]);
                    ex(node->op[1]);
                    switch (node->oper) {
                        case '+':
                            output << "\tadd\n";
                            break;
                        case '-':
                            output << "\tsub\n";
                            break;
                        case '*':
                            output << "\tmul\n";
                            break;
                        case '/':
                            output << "\tdiv\n";
                            break;
                        case '%':
                            output << "\tmod\n";
                            break;
                        case '<':
                            output << "\tcompLT\n";
                            break;
                        case '>':
                            output << "\tcompGT\n";
                            break;
                        case GE:
                            output << "\tcompGE\n";
                            break;
                        case LE:
                            output << "\tcompLE\n";
                            break;
                        case NE:
                            output << "\tcompNE\n";
                            break;
                        case EQ:
                            output << "\tcompEQ\n";
                            break;
                    }
            }
    }

    output.flush();

    return 1;
}
