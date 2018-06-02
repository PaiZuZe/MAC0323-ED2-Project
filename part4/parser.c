/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

#include "parser.h"
#include "error.h"
#include "buffer.h"
#include "optable.h"
#include <ctype.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

#define MAX_NUM_WORDS 5 // A macal instruction will have 5 words at maximum.
#define MAX_NUM_OPERANDS 3 // A macal operator will have 3 operands at maximum.

/*
 * Receives a const string str and a char **words_ptrs, which is a vector of
 * pointers to the first letter of every word in the string str. Returns a
 * vector of each word in str (str is not modified).
 */
char **split_line(const char *str, const char **words_ptrs)
{
    int count = 0;
    bool new_word = true;
    bool is_string = false;
    char **words = emalloc(sizeof(char *) * MAX_NUM_WORDS);
    Buffer *b = buffer_create(sizeof(char));

    // Adding '\0' to every word.
    for (int i = 0; i < MAX_NUM_WORDS; i++) words[i] = '\0';

    // Breaking str into words and storing them in the words vector.
    for (unsigned int i = 0; i < strlen(str); i++) {
        // The rest of the str is a comment.
        if (str[i] == '*')
            break;

        // Checks if we are adding a string.
        if (new_word && str[i] == '"')
            is_string = true;
        else if (is_string && str[i] == '"')
            is_string = false;

        // str[i] belongs to a word
        if ((!isspace(str[i]) && str[i] != ',') || is_string) {
            if (new_word) {
                new_word = false;
                words_ptrs[count] = &str[i];
            }
            buffer_push_char(b, str[i]);
            b->p += 1;
        }
        // It is the end of a word if the buffer is not empty.
        else if (b->p != 0) {
            buffer_push_char(b, '\0');
            b->p += 1;
            words[count++] = estrdup((char *) b->data);
            new_word = true;
            buffer_reset(b);
        }
    }
    // Add the last word in the words vector.
    if (count != 0) {
        buffer_push_char(b, '\0');
        b->p += 1;
        words[count++] = estrdup((char *) b->data);
    }
    buffer_destroy(b);

    return words;
}

/*
 * Returns 1 if the the types in types matches with the Operator *op types,
 * returns 0 otherwise. *errptr is set is set before returning 0.
 */
int right_args(const char *s, const Operator *op, OperandType *types,
               const char **errptr, int init, char const **words_ptrs)
{
    for (int i = 0; i < MAX_NUM_OPERANDS; i++) {
        if (types[i] == OP_NONE && op->opd_types[i] != OP_NONE) {
            *errptr = &s[strlen(s)];
            set_error_msg("expected operand");
            return 0;
        }
        else if ((op->opd_types[i] & types[i]) != types[i]) {
            *errptr = words_ptrs[i + init];
            set_error_msg("wrong operand type");
            return 0;
        }
    }
    return 1;
}

/*
 * Returns 1 if it could get the types of the words in the instruction and 0
 * otherwise.
 */
int get_arg_types(char **words, SymbolTable alias_table, OperandType *arg_types,
                  int init, const char **errptr, char const **words_ptrs)
{
    EntryData *data;

    // Goes through every word that is an operand and get their types.
    for (int i = init; i < MAX_NUM_OPERANDS + init; i++) {
        /*
         * One operand has already appeared and the label has the same
         * name of a operand.
         */
        if (words[i] != NULL && optable_find(words[i]) != NULL) {
            *errptr = words_ptrs[i];
            set_error_msg("label has the same name as an operand");
            return 0;
        }
        // Argument does not exist, so we set its type to OP_NONE.
        if (words[i] == NULL)
            arg_types[i - init] = OP_NONE;

        // If it has a '$', then it is a register.
        else if (words[i][0] == '$') {
            if (strlen(words[i]) == 1) {
                *errptr = words_ptrs[i];
                set_error_msg("Not valid register");
                return 0;
            }
            for (unsigned int j = 1; j < strlen(words[i]); j++)
                if (!isdigit(words[i][j])) {
                    *errptr = words_ptrs[i];
                    set_error_msg("not a valid register");
                    return 0;
                }
            if (strtoll(&words[i][1], NULL, 10) > 255) {
                *errptr = words_ptrs[i];
                set_error_msg("not a valid register");
                return 0;
            }
            arg_types[i - init] = REGISTER;
        }

        // If it has at the start '"' and at the end, then it is a string.
        else if (words[i][0] == '"')
            if (words[i][strlen(words[i]) - 1] == '"' && strlen(words[i]) > 1)
                arg_types[i - init] = STRING;
            else {
                *errptr = words_ptrs[i];
                set_error_msg("expected \", at the end of string");
                return 0;
            }

        // The label is in the s_table.
        else if ((data = stable_find(alias_table, words[i])) != NULL)
            arg_types[i - init] = data->opd->type;

        // Is a negative number?
        else if (words[i][0] == '-') {
            for (unsigned int j = 1; j < strlen(words[i]); j++)
                if (!isdigit(words[i][j])) {
                    *errptr = words_ptrs[i];
                    set_error_msg("expected label, register or number");
                    return 0;
                }
            // HEXADECIMAL NEGATIVO? VERIFICAR O TAMANHO DO NÚMERO
            arg_types[i - init] = NEG_NUMBER;
        }

        //It's a Hexa decimal number?
        else if (words[i][0] == '#') {
            if (strlen(words[i]) == 1) {
                *errptr = words_ptrs[i];
                set_error_msg("Not valid hexadecimal");
                return 0;
            }
            for (unsigned int j = 1; j < strlen(words[i]); j++) {
                if (!isxdigit(words[i][j])) {
                    *errptr = words_ptrs[i];
                    set_error_msg("expected label, register or number");
                    return 0;
                }
            }
            long long int num = strtoll(&words[i][1], NULL, 16);
            if (num < 255)
                arg_types[i - init] = BYTE1;
            else if (num < 65535)
                arg_types[i - init] = BYTE2;
            else if (num < 16777215)
                arg_types[i - init] = BYTE3;
            else if (num < 4294967295)
                arg_types[i - init] = TETRABYTE;
            else {
                *errptr = words_ptrs[i];
                set_error_msg("number is too big");
                return 0;
            }
        }

        // It must be a decimal number.
        else {
            /*
             * Iterates through a word to see if it is a number. If it is not
             * then we could not find a type for that word.
             */
            for (unsigned int j = 0; j < strlen(words[i]); j++) {
                if (!isdigit(words[i][j])) {
                    *errptr = words_ptrs[i];
                    set_error_msg("expected label, register or number");
                    return 0;
                }
            }
            long long int num = strtoll(words[i], NULL, 10);
            if (num < 255)
                arg_types[i - init] = BYTE1;
            else if (num < 65535)
                arg_types[i - init] = BYTE2;
            else if (num < 16777215)
                arg_types[i - init] = BYTE3;
            else if (num < 4294967295)
                arg_types[i - init] = TETRABYTE;
            else {
                *errptr = words_ptrs[i];
                set_error_msg("number is too big");
                return 0;
            }
        }
    }
    return 1;
}

int is_num_type(OperandType arg_type) {
    OperandType type = arg_type & NUMBER_TYPE;
    if (type == BYTE1 || type == BYTE2 || type == BYTE3 || type == TETRABYTE
        || type == NEG_NUMBER)
        return 1;
    return 0;
}

// Creates the operands in opds.
void operands_create(Operand **opds, OperandType *arg_types, char **words, int init)
{
    int j = 0;
    for (int i = init; i < MAX_NUM_OPERANDS + init; i++) {
        if (arg_types[j] == OP_NONE)
            return;

        else if ((arg_types[j] & REGISTER) == REGISTER)
            opds[j] = operand_create_register(atoi(&words[i][1]));

        else if (is_num_type(arg_types[j]))
            if (words[i][0] == '#')
                opds[j] = operand_create_number(strtoll(words[i], NULL, 16));
            else
                opds[j] = operand_create_number(strtoll(words[i], NULL, 10));

        else if ((arg_types[j] & LABEL) == LABEL)
            opds[j] = operand_create_label(words[i]);

        else if ((arg_types[j] & STRING) == STRING)
            opds[j] = operand_create_string(words[i]);
        j++;
    }
}

/*
 * Breaks the instruction of assembler language in its parts: label operator
 * and operands. Returns 1 if it succeds or 0 otherwise.
 */
int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr)
{
    int init = 1;
    char *label = NULL;
    const Operator *op = NULL;
    const char **words_ptrs = emalloc(sizeof(char *) * MAX_NUM_WORDS);
    char **words = split_line(s, words_ptrs);
    OperandType arg_types[MAX_NUM_OPERANDS] = {OP_NONE, OP_NONE, OP_NONE};
    Operand *opds[MAX_NUM_OPERANDS] = {NULL, NULL, NULL};

    // Empty line.
    if (words[0] == NULL)
        return 1;

    op = optable_find(words[0]);
    // There is a label.
    if (op == NULL) {
        // The next one must be an operator in order to succed the parsing.
        if (words[1] != NULL && optable_find(words[1]) == NULL) {
            if (words_ptrs[1] == NULL)
                *errptr = &s[strlen(s)];
            set_error_msg("expected operator");
            return 0;
        }
        // Verifies if the label is valid.
        if (words[0][0] != '_' && !isalpha(words[0][0])) {
            set_error_msg("invalid label");
            *errptr = words_ptrs[0];
            return 0;
        }
        else
            for (unsigned int i = 1; i < strlen(words[0]); i++)
                if (words[0][i] != '_' && !isalnum(words[0][i])) {
                    set_error_msg("invalid label");
                    *errptr = words_ptrs[0];
                    return 0;
                }
        init = 2;
        label = estrdup(words[0]);
        if(!get_arg_types(words, alias_table, arg_types, init, errptr, words_ptrs))
            return 0;
        operands_create(opds, arg_types, words, init);
        op = optable_find(words[1]);
    }
    // There is no label but there is an operator.
    else {
        if(!get_arg_types(words, alias_table, arg_types, init, errptr, words_ptrs))
            return 0;
        operands_create(opds, arg_types, words, init);
    }
    // The number of arguments or the types are wrong.
    if (!right_args(s, op, arg_types, errptr, init, words_ptrs))
        return 0;
    // Creates the instruction.
    *instr = instr_create(label, op, opds);
    *errptr = NULL;

    return 1;
}
