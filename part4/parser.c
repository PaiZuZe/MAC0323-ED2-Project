#include "parser.h"
#include "buffer.h"
#include "optable.h"

char **split_line(const char *og_string) {
    int count = 0;
    int is_comment = 0;
    char **words = emalloc(sizeof(char *) * 5);
    Buffer *b = buffer_create(sizeof(char));

    for (int i = 0; i < 5; i++) words[i] = '\0';

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
            buffer_push_char(b, '\0');
            b->p += 1;
            words[count++] = estrdup((char *) b->data);
            buffer_reset(b);
        }
    }
    if (!is_comment && count != 0) {
        buffer_push_char(b, '\0');
        b->p += 1;
        words[count++] = estrdup((char *) b->data);
    }
    buffer_destroy(b);
    return words;
}

int right_args(const Operator *operat, OperandType *types, const char **errptr) {
    for (int i = 0; i < 3; i++) {
        if (operat->opd_types[i] != types[i]) {
            //Errado isso, aprende a ler antes.
            //&errptr = estrdup("ERROR: Wrong operator type\n");
            printf("Deu um cara errado aqui\n");
            return 0;
        }
    }
    printf("Deu tudo bom\n");
    return 1;
}

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    /*
    char **words = split_line(s);
    char *label = NULL;
    const Operator *operat = NULL;
    OperandType arg_types[3] = {OP_NONE, OP_NONE, OP_NONE};
    Operand *opds[3];
    */

    //Linha vazia.
    /*If(words[0] == NULL) {
        return 1;
    }

    //operat = optable_find(words[0]);

    //Linha com label ou o operando não existe (como diferenciar ???) :(
    //Se tem label, pode ser um IS, ou está colocando uma label em uma linha.
    //De qualquer jeito precisa colocar na alias_table.
    else if(operat == NULL) {
        //imagina que é um label.
        label = estrdup(words[0]);
    }
    //Linha sem label, e com um operando que existe, precisa ver se tem os args certos.
    else {
        for (int i = 1; i < 4; i++) {
            //Não tem esse arg, manda um OP_NONE.
            if (words[i] == NULL) arg_types[i - 1] = OP_NONE;

            //se tem um $ é um REGISTER,
            else if (words[i][0] == '$') arg_types[i - 1] = REGISTER;

            //ver se é um IMMEDIATE, tudo digitos;
            else {
                for (int j = 0; j < strlen(words[i]); i++) {
                    //entrou aqui o manolo me mandou uma merda gigante.
                    if (!isdigit(words[i][j])) {
                    }
                }
                arg_types[i - 1] = IMMEDIATE;
            }
        }
    }
    //veio até aqui então ta tudo certo.
    &instr = instr_create(label, operat, opds);
    &errptr = NULL;
    return 1;
    */

    char **words = split_line("start  ADD   a , a , 1 *Ola meu amigo");
    if (words[0] != NULL && optable_find(words[0]) == NULL)
        printf("Tem não meu irmão0\n");
    else if (words[0] != NULL) {
        OperandType bob[3] = {REGISTER, REGISTER, IMMEDIATE};
        right_args(optable_find(words[0]), bob, errptr);
    }
    char **words1 = split_line("MUL  a, $2,$3");
    if (words1[0] != NULL && optable_find(words1[0]) == NULL)
        printf("Tem não meu irmão1\n");
    else if (words1[0] != NULL) {
        OperandType bob1[3] = {REGISTER, REGISTER, IMMEDIATE};
        right_args(optable_find(words1[0]), bob1, errptr);
    }
    char **words2 = split_line("a      IS      $0");
    if (words2[0] != NULL && optable_find(words2[0]) == NULL)
        printf("Tem não meu irmão2\n");
    else if (words2[0] != NULL) {
        OperandType bob2[1] = {REGISTER};
        right_args(optable_find(words2[0]), bob2, errptr);
    }
    char **words3 = split_line("");
    if (words3[0] != NULL && optable_find(words3[0]) == NULL)
        printf("Tem não meu irmão3\n");
    else if (words3[0] != NULL) {
        right_args(optable_find(words3[0]), NULL, errptr);
    }
    char **words4 = split_line("      ");
    if (words4[0] != NULL && optable_find(words4[0]) == NULL)
        printf("Tem não meu irmão4\n");
    else if (words4[0] != NULL) {
        right_args(optable_find(words4[0]), NULL, errptr);
    }
    char **words5 = split_line("*OLA MEU CARO");
    if (words5[0] != NULL && optable_find(words5[0]) == NULL)
        printf("Tem não meu irmão5\n");
    else if (words5[0] != NULL) {
        right_args(optable_find(words5[0]), NULL, errptr);
    }
    char **words6 = split_line("MUL  a, $2,$3");
    if (words6[0] != NULL && optable_find(words6[0]) == NULL)
        printf("Tem não meu irmão6\n");
    else if (words6[0] != NULL) {
        OperandType bob6[3] = {REGISTER, IMMEDIATE, IMMEDIATE};
        right_args(optable_find(words6[0]), bob6, errptr);
    }

    return 1;
}


int main() {
    parse(NULL, NULL, NULL, NULL);


    return 0;
}
