#include "parser.h"
#include "buffer.h"
#include "optable.h"

char **split_line(const char *og_string) {
    int count = 0;
    int is_comment = 0;
    char **words = emalloc(sizeof(char *) * 5);
    Buffer *b = buffer_create(sizeof(char));

    for (unsigned int i = 0; i < strlen(og_string); i++) {
        if (og_string[i] == '*') {
            is_comment = 1;
            break;
        }
        if (!isspace(og_string[i]) && og_string[i] != ',') {
            buffer_push_char(b, og_string[i]);
            b->p += 1;
        }
        else if (b->p != 0){ //b->p != 0, buffer não está vazio
            words[count++] = estrdup((char *) b->data);
            buffer_reset(b);
        }
    }
    if (!is_comment && count != 0) words[count++] = estrdup((char *) b->data);
    //buffer_destroy(b)
    return words;
}

int right_args(const Operator *operat, OperandType *types, int n_operands, const char **errptr) {
    int count = 3;
    //vê se tem o num certo.
    for (int i = 0; i < 3; i++) {
        if (operat->opd_types[i] != types[i]) {
            &errptr = estrdup("ERROR: Wrong operator type\n");
            return 0;
        }
        if (operat->opd_types[i] == OP_NONE) count--;
    }
    if (n_operands != count) {
        &errptr = estrdup("ERROR: Expected operand\n");
        return 0;
    }
    return 1;
}

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    /*char **words = split_line(s);
    const Operator operat;
    */

    //Linha vazia.
    /*If(words[0] == NULL) {
        return 1;
    }
    //operat = optable_find(words[0]);

    //Linha com label ou o operando não existe :(.
    else if(operat == NULL) {

    }
    //Linha sem label.
    else {

    }
    */

    char **words = split_line("start  ADD   a , a , 1 *Ola meu amigo");
    if (words[0] != NULL && optable_find(words[0]) == NULL)
        printf("Tem não meu irmão0\n");
    else if (words[0] != NULL) {
        right_args(optable_find(words[0]), 3, errptr);
    }
    char **words1 = split_line("MUL  a, $2,$3");
    if (words1[0] != NULL && optable_find(words1[0]) == NULL)
        printf("Tem não meu irmão1\n");
    else if (words1[0] != NULL) {
        right_args(optable_find(words1[0]), 3, errptr);
    }
    char **words2 = split_line("a      IS      $0");
    if (words2[0] != NULL && optable_find(words2[0]) == NULL)
        printf("Tem não meu irmão2\n");
    else if (words2[0] != NULL) {
        right_args(optable_find(words2[0]), 1, errptr);
    }
    char **words3 = split_line("");
    if (words3[0] != NULL && optable_find(words3[0]) == NULL)
        printf("Tem não meu irmão3\n");
    else if (words3[0] != NULL) {
        right_args(optable_find(words3[0]), 0, errptr);
    }
    char **words4 = split_line("      ");
    if (words4[0] != NULL && optable_find(words4[0]) == NULL)
        printf("Tem não meu irmão4\n");
    else if (words4[0] != NULL) {
        right_args(optable_find(words4[0]), 0, errptr);
    }
    char **words5 = split_line("*OLA MEU CARO");
    if (words5[0] != NULL && optable_find(words5[0]) == NULL)
        printf("Tem não meu irmão5\n");
    else if (words5[0] != NULL) {
        right_args(optable_find(words5[0]), 0, errptr);
    }

    return 1;
}


int main() {
    const char bob[] = "start  ADD   a , a , 1 *Ola meu amigo";
    char **bobs;
    bobs = split_line(bob);
    for (int i = 0; i < 5; i++) printf("%s ", bobs[i]);
    printf("\n");

    const char bob1[] = "MUL  a, $2,$3";
    char **bobs1;
    bobs1 = split_line(bob1);
    for (int i = 0; i < 5; i++) printf("%s ", bobs1[i]);
    printf("\n");

    const char bob2[] = "a      IS      $0";
    char **bobs2;
    bobs2 = split_line(bob2);
    for (int i = 0; i < 5; i++) printf("%s ", bobs2[i]);
    printf("\n");

    const char bob3[] = "";
    char **bobs3;
    bobs3 = split_line(bob3);
    for (int i = 0; i < 5; i++) printf("%s ", bobs3[i]);
    printf("\n");

    const char bob4[] = "       ";
    char **bobs4;
    bobs4 = split_line(bob4);
    for (int i = 0; i < 5; i++) printf("%s ", bobs4[i]);
    printf("\n");

    const char bob5[] = "*OLA MEU CARO";
    char **bobs5;
    bobs5 = split_line(bob5);
    for (int i = 0; i < 5; i++) printf("%s ", bobs5[i]);
    printf("\n");

    parse(NULL, NULL, NULL, NULL);


    return 0;
}
