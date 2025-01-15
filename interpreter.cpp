#include <cstdio>

#include "definitions.h"
#include "y.tab.h"

static int lbl;

// If push is true, then in case of typeId it will push.
// Otherwise it will pop.
int ex(nodeType* p, bool push = true) {
    std::ostream& output = *outputPtr;

    int lbl1, lbl2;

    if (!p) return 0;

    switch (p->type) {
        case typeCon:
            output << "\tpush\t" << std::get<conNodeType*>(p->value)->value
                   << "\n";
            break;
        case typeId:
            output << (push ? "\tpush\t" : "\tpop\t")
                   << yylValToToken[std::get<idNodeType*>(p->value)->i] << "\n";
            break;
        case typeOpr:
            oprNodeType* node = std::get<oprNodeType*>(p->value);

            switch (node->oper) {
                case ';': {
                    ex(node->op[0]);
                    ex(node->op[1]);
                    break;
                }
                case ',': {
                    ex(node->op[1], push);
                    ex(node->op[0], push);
                    break;
                }
                case '#': {
                    ex(node->op[0]);
                    ex(node->op[1]);
                    break;
                }
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
                    for (int i = node->nops - 1; i >= 1; --i) {
                        ex(node->op[i]);
                    }

                    output << "\tpop\t" << (node->nops == 3 ? "arr\t" : "")
                           << yylValToToken[std::get<idNodeType*>(
                                                node->op[0]->value)
                                                ->i];

                    output << "\n";

                    break;
                case MASSIGN: {
                    ex(node->op[1]);
                    ex(node->op[0], false);

                    /*std::vector<idNodeType*> ids;*/
                    /*std::vector<nodeType*> exprs;*/
                    /**/
                    /*for (int i = 0; i < node->nops; ++i) {*/
                    /*    if (node->op[i]->type == typeId) {*/
                    /*        ids.push_back(*/
                    /*            std::get<idNodeType*>(node->op[i]->value));*/
                    /*    } else {*/
                    /*        exprs.push_back(node->op[i]);*/
                    /*    }*/
                    /*}*/
                    /**/
                    /*for (int i = exprs.size() - 1; i >= 0; --i) {*/
                    /*    ex(exprs[i]);*/
                    /*}*/
                    /**/
                    /*for (int i = 0; i < ids.size(); ++i) {*/
                    /*    output << "\tpop\t" << yylValToToken[ids[i]->i] <<
                     * "\n";*/
                    /*}*/

                    break;
                }
                case FOR: {
                    // Initialize for-loop variables.
                    ex(node->op[0]);

                    // Evaluate the expression and put it on top of the
                    // stack.
                    output << "L" << (lbl1 = lbl++) << ":\n";
                    ex(node->op[1]);
                    output << "\tjz\tL" << (lbl2 = lbl++) << "\n";

                    // Evaluate statements inside braces.
                    ex(node->op[3]);

                    // Evaluate the for-loop step;
                    ex(node->op[2], true);

                    output << "\tjmp\tL" << lbl1 << "\n";
                    output << "L" << lbl2 << ":\n";

                    /*std::vector<idNodeType*> ids;*/
                    /*std::vector<nodeType*> exprs;*/
                    /**/
                    /*for (int i = 0; i < node->nops - 4; ++i) {*/
                    /*    if (node->op[i]->type == typeId) {*/
                    /*        ids.push_back(*/
                    /*            std::get<idNodeType*>(node->op[i]->value));*/
                    /*    } else {*/
                    /*        exprs.push_back(node->op[i]);*/
                    /*    }*/
                    /*}*/
                    /**/
                    /*for (int i = exprs.size() - 1; i >= 0; --i) {*/
                    /*    ex(exprs[i]);*/
                    /*}*/
                    /**/
                    /*for (int i = 0; i < ids.size(); ++i) {*/
                    /*    output << "\tpop\t" << yylValToToken[ids[i]->i] <<
                     * "\n";*/
                    /*}*/
                    /**/
                    /*int checkExpressionIndex = node->nops - 4;*/
                    /**/
                    /*output << "L" << (lbl1 = lbl++) << ":\n";*/
                    /**/
                    /*// Evaluate the expression and put it on top of the
                     * stack.*/
                    /*ex(node->op[checkExpressionIndex]);*/
                    /*output << "\tjz\tL" << (lbl2 = lbl++) << "\n";*/
                    /**/
                    /*// Evaluate statements inside braces.*/
                    /*ex(node->op[checkExpressionIndex + 3]);*/
                    /**/
                    /*// Evaluate the for-loop step;*/
                    /*ex(node->op[checkExpressionIndex + 2]);*/
                    /**/
                    /*// Push the value of the step to variable.*/
                    /*output*/
                    /*    << "\tpop\t"*/
                    /*    << yylValToToken[std::get<idNodeType*>(*/
                    /*                         node->op[checkExpressionIndex
                     * + 1]*/
                    /*                             ->value)*/
                    /*                         ->i];*/
                    /**/
                    /*output << "\n";*/
                    /**/
                    /*output << "\tjmp\tL" << lbl1 << "\n";*/
                    /*output << "L" << lbl2 << ":\n";*/
                    /**/
                    /*for (int i = node->nops - 1; i >= 1; --i) {*/
                    /*    ex(node->op[i]);*/
                    /*}*/

                    break;
                }
                case UMINUS: {
                    ex(node->op[0]);
                    output << "\tneg\n";
                    break;
                }
                case LENGTH: {
                    output << "\tlength\t"
                           << yylValToToken
                                  [std::get<idNodeType*>(node->op[0]->value)->i]
                           << "\n";

                    break;
                }
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
                    for (int i = node->nops - 1; i >= 0; --i) {
                        ex(node->op[i]);
                    }

                    output << "\treturn\t" << node->nops << "\n";
                    break;
                }
                case ARRAY: {
                    ex(node->op.back());

                    idNodeType* id = std::get<idNodeType*>(node->op[0]->value);

                    output << "\tarray\t" << yylValToToken[id->i] << "\n";
                    break;
                }
                case ACCESS: {
                    ex(node->op.back());

                    idNodeType* id = std::get<idNodeType*>(node->op[0]->value);

                    output << "\taccess\t" << yylValToToken[id->i] << "\n";
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
