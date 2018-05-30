#include "parser.h"
#include "buffer.h"
#include "optable.h"

char **split_line(const char *og_string, const char**str_ptrs) {
    int count = 0;
    int str_ptrs_counter = 0;
    int is_comment = 0;
    int new_word = 1;
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
            if (new_word) {
                new_word = 0;
                str_ptrs[str_ptrs_counter++] = &og_string[i];
            }
            buffer_push_char(b, og_string[i]);
            b->p += 1;
        }
        else if (b->p != 0){ //b->p != 0, buffer não está vazio
            buffer_push_char(b, '\0');
            b->p += 1;
            words[count++] = estrdup((char *) b->data);
            buffer_reset(b);
            new_word = 1;
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

int right_args(const Operator *operat, OperandType *types, const char **errptr, int init, char const **str_ptrs) {
    for (int i = 0; i < 3; i++) {
        if (types[i] == OP_NONE && operat->opd_types[i] != OP_NONE) {
            *errptr = str_ptrs[i+init];
            set_error_msg("expected operand");
            return 0;
        }
        else if ((operat->opd_types[i] & types[i]) != types[i]) {
            //Errado isso, aprende a ler antes.
            //&errptr = estrdup("ERROR: Wrong operator type\n");
            *errptr = str_ptrs[i+init];
            set_error_msg("wrong operand type");
            return 0;
        }
    }
    return 1;
}

int get_arg_types(char **words, SymbolTable alias_table, OperandType *arg_types,
                  int init, const char **errptr, char const **str_ptrs) {
    EntryData *data;

    for (int i = init; i < 3 + init; i++) {
        // se entrou nesse if, já apareceu um operador e o label tem o msm nome de operando
        if (words[i] != NULL && optable_find(words[i]) != NULL) {
            *errptr = str_ptrs[i];
            set_error_msg("label has the same name as an operand");
            return 0;
        }
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
                    *errptr = str_ptrs[i];
                    set_error_msg("expected label, register or number");
                    return 0;
                }
            }
            arg_types[i - init] = BYTE1;
        }
    }
    return 1;
}

void operands_create(Operand **opds, OperandType *arg_types, char **words, int init) {
    for (int i = init; i < 3 + init; i++) {
        if (arg_types[i] == OP_NONE) return;
        else if ((arg_types[i] & REGISTER) == REGISTER) opds[i] = operand_create_register(words[i][1]);
        //AQUI PODE DAR RUIM
        else if ((arg_types[i] & NUMBER_TYPE) == BYTE1) opds[i] = operand_create_number((octa) words[i]);
        else if ((arg_types[i] & LABEL) == LABEL) opds[i] = operand_create_label(words[i]);
        else if ((arg_types[i] & STRING) == STRING) opds[i] = operand_create_string(words[i]);
    }
}

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    int init = 1;
    const char **str_ptrs = emalloc(sizeof(char *) * 5);
    char **words = split_line(s, str_ptrs);
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
            *errptr = str_ptrs[0];
            set_error_msg("expected operator");
            return 0;
        }
        //Vê se o rotulo e valido.
        if (words[0][0] != '_' && !isalpha(words[0][0])) {
            set_error_msg("invalid label");
            *errptr = str_ptrs[0];
            return 0;
        }
        else
            for (unsigned int i = 1; i < strlen(words[0]); i++)
                if (words[0][i] != '_' && !isalnum(words[0][i])) {
                    set_error_msg("invalid label");
                    *errptr = str_ptrs[0];
                    return 0;
                }
        init = 2;
        label = estrdup(words[0]);
        get_arg_types(words, alias_table, arg_types, init, errptr, str_ptrs);
        operands_create(opds, arg_types, words, init);
        operat = optable_find(words[1]);
    }

    //Linha sem label, e com um operando que existe, precisa ver se tem os args certos.
    else {
        get_arg_types(words, alias_table, arg_types, 1, errptr, str_ptrs);
        operands_create(opds, arg_types, words, 1);
    }
    //Entrou aqui então ou o nArgs ou os tipos estão errados.
    if (!right_args(operat, arg_types, errptr, init, str_ptrs))
        return 0;

    //veio até aqui então ta tudo certo.
    *instr = instr_create(label, operat, opds);
    *errptr = NULL;

    return 1;
}

/*
int main(int argc, char *argv[]) {
    set_prog_name(argv[0]);

    Instruction **instr = emalloc(sizeof(Instruction *));
    const char **errptr = emalloc(sizeof(char *));
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
    if (parse(words0, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 0\n");
    printf("Testando o 1\n");
    if (parse(words1, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 1\n");
    bob = stable_insert(ST, "a");
    if (bob.new) bob.data->opd = operand_create_register('2');
    printf("Testando o 2\n");
    if (parse(words2, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 2\n");
    bob = stable_insert(ST, "start");
    if (bob.new) bob.data->opd = operand_create_label("blah");
    printf("Testando o 3\n");
    if (parse(words3, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 3\n");
    printf("Testando o 4\n");
    if (parse(words4, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 4\n");
    printf("Testando o 5\n");
    if (parse(words5, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 5\n");
    printf("Testando o 6\n");
    if (parse(words6, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 6\n");
    printf("Testando o 7\n");
    if (parse(words7, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 7\n");

    return 0;
} */
