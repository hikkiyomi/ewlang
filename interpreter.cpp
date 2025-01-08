#include <cstdio>

#include "definitions.h"
#include "y.tab.h"

static int lbl;

int ex(nodeType* p) {
    int lbl1, lbl2;

    if (!p) return 0;

    switch (p->type) {
        case typeCon:
            printf("\tpush\t%d\n", std::get<conNodeType*>(p->value)->value);
            break;
        case typeId:
            printf("\tpush\t%s\n",
                   yylValToToken[std::get<idNodeType*>(p->value)->i].c_str());
            break;
        case typeOpr:
            oprNodeType* node = std::get<oprNodeType*>(p->value);
            switch (node->oper) {
                case WHILE:
                    printf("L%03d:\n", lbl1 = lbl++);
                    ex(node->op[0]);
                    printf("\tjz\tL%03d\n", lbl2 = lbl++);
                    ex(node->op[1]);
                    printf("\tjmp\tL%03d\n", lbl1);
                    printf("L%03d:\n", lbl2);
                    break;
                case IF:
                    ex(node->op[0]);
                    if (node->nops > 2) {
                        printf("\tjz\tL%03d\n", lbl1 = lbl++);
                        ex(node->op[1]);
                        printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                        printf("L%03d:\n", lbl1);
                        ex(node->op[2]);
                        printf("L%03d:\n", lbl2);
                    } else {
                        printf("\tjz\tL%03d\n", lbl1 = lbl++);
                        ex(node->op[1]);
                        printf("L%03d:\n", lbl1);
                    }
                    break;
                case PRINT:
                    ex(node->op[0]);
                    printf("\tprint\n");
                    break;
                case '=':
                    ex(node->op[1]);
                    printf(
                        "\tpop\t%s\n",
                        yylValToToken[std::get<idNodeType*>(node->op[0]->value)
                                          ->i]
                            .c_str());
                    break;
                case UMINUS:
                    ex(node->op[0]);
                    printf("\tneg\n");
                    break;
                case CALL:
                    for (int i = 0; i < node->nops; i++) {
                        ex(node->op[i]);
                    }

                    printf("\tcall\n");
                    break;
                default:
                    ex(node->op[0]);
                    ex(node->op[1]);
                    switch (node->oper) {
                        case '+':
                            printf("\tadd\n");
                            break;
                        case '-':
                            printf("\tsub\n");
                            break;
                        case '*':
                            printf("\tmul\n");
                            break;
                        case '/':
                            printf("\tdiv\n");
                            break;
                        case '%':
                            printf("\tmod\n");
                            break;
                        case '<':
                            printf("\tcompLT\n");
                            break;
                        case '>':
                            printf("\tcompGT\n");
                            break;
                        case GE:
                            printf("\tcompGE\n");
                            break;
                        case LE:
                            printf("\tcompLE\n");
                            break;
                        case NE:
                            printf("\tcompNE\n");
                            break;
                        case EQ:
                            printf("\tcompEQ\n");
                            break;
                    }
            }
    }

    return 1;
}
