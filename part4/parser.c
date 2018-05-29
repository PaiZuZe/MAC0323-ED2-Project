#include "parser.h"
#include "buffer.h"
#include "optable.h"

char **split_line(const char *og_string) {
    int count = 0;
    int is_comment = 0;
    char **words = emalloc(sizeof(char *) * 5);
    Buffer *b = buffer_create(sizeof(char));

    // colocando \0 no começo de todas as palavras
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
        if (types[i] == OP_NONE && operat->opd_types[i] != OP_NONE) {
            printf("Arg de tipo invalido e um %d era para ser um %d, e foi o %d\n", types[i], operat->opd_types[i], i);
            return 0;
        }
        else if ((operat->opd_types[i] & types[i]) != types[i]) {
            //Errado isso, aprende a ler antes.
            //&errptr = estrdup("ERROR: Wrong operator type\n");
            printf("Arg de tipo invalido e um %d era para ser um %d, e foi o %d\n", types[i], operat->opd_types[i], i);
            return 0;
        }
    }
    printf("Deu tudo bom\n");
    return 1;
}

int get_arg_types(char **words, SymbolTable alias_table, OperandType *arg_types,
                  int init, const char **errptr) {
    EntryData *data;

    for (int i = init; i < 3 + init; i++) {
        //Não tem esse arg, manda um OP_NONE.
        if (words[i] == NULL) arg_types[i - init] = OP_NONE;

        //se tem um $ é um REGISTER.
        else if (words[i][0] == '$') arg_types[i - init] = REGISTER;

        //Esta na s_table
        // SUPONDO QUE O RESULTADO DA FIND É SEMPRE UM operador
        else if ((data = stable_find(alias_table, words[i])) != NULL)
            arg_types[i - init] = data->opd->type;

        //Ou é um IMMEDIATE (bagulho de um byte ou um REGISTER) ou é um treco muito errado.
        else {
            for (unsigned int j = 0; j < strlen(words[i]); j++) {
                //entrou aqui o manolo me mandou uma merda gigante.
                if (!isdigit(words[i][j])) {
                    return 0;
                }
            }
            //arg_types[i - init] = NUMBER_TYPE;
            arg_types[i - init] = IMMEDIATE;
        }
    }
    return 1;
}

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    char **words = split_line(s);
    char *label = NULL;
    const Operator *operat = NULL;
    OperandType arg_types[3] = {OP_NONE, OP_NONE, OP_NONE};
    Operand *opds[3] = {NULL, NULL, NULL};

    //Linha vazia.
    if(words[0] == NULL) {
        return 1;
    }
    operat = optable_find(words[0]);
    //Se tem label, pode ser um IS, ou está colocando uma label em uma linha.
    if(operat == NULL) {
        //se o prox cara não for um operador, deu ruim.
        if (words[1] != NULL && optable_find(words[1]) == NULL) {
            return 0;
        }
        //imagina que é um label.
        label = estrdup(words[0]);
        get_arg_types(words, alias_table, arg_types, 2, errptr);
        operat = optable_find(words[1]);
    }

    //Linha sem label, e com um operando que existe, precisa ver se tem os args certos.
    else {
        get_arg_types(words, alias_table, arg_types, 1, errptr);
    }
    //Entrou aqui então ou o nArgs ou os tipos estão errados.
    if (!right_args(operat, arg_types, errptr)) {
        return 0;
    }
    //veio até aqui então ta tudo certo.
    *instr = instr_create(label, operat, opds);
    //*errptr = NULL;
    return 1;
}


int main() {
    Instruction **instr = emalloc(sizeof(Instruction *));
    //test only comment
    char *words0 = "* Teste";
    //test adding label
    char *words1 = "a      IS      $0";
    //test working with two label
    char *words2 = "start  ADD     a,a,1";
    //test one label and comment
    char *words3 = "       MUL     a,$2,$3     * Multiplica.";
    //just label
    char *words4 = "      JMP     start";
    //missing args
    char *words5 = "     DIV     a,2";
    char *words6 = "MUL  a, $2,$3";

    //wrong kind of arg.
    char *words7 = "CALL start, 2,$3";

    SymbolTable ST = stable_create();
    InsertionResult bob;

    printf("Testando o 0\n");
    parse(words0, ST, instr, NULL);
    printf("Deu bom para o 0\n");
    printf("Testando o 1\n");
    parse(words1, ST, instr, NULL);
    printf("Deu bom para o 1\n");
    bob = stable_insert(ST, "a");
    if (bob.new) bob.data->opd = operand_create_register('2');
    printf("Testando o 2\n");
    parse(words2, ST, instr, NULL);
    printf("Deu bom para o 2\n");
    bob = stable_insert(ST, "start");
    if (bob.new) bob.data->opd = operand_create_label("blah");
    printf("Testando o 3\n");
    parse(words3, ST, instr, NULL);
    printf("Deu bom para o 3\n");
    printf("Testando o 4\n");
    parse(words4, ST, instr, NULL);
    printf("Deu bom para o 4\n");
    printf("Testando o 5\n");
    parse(words5, ST, instr, NULL);
    printf("Deu bom para o 5\n");
    printf("Testando o 6\n");
    parse(words6, ST, instr, NULL);
    printf("Deu bom para o 6\n");
    printf("Testando o 7\n");
    parse(words7, ST, instr, NULL);
    printf("Deu bom para o 7\n");

    return 0;
}
