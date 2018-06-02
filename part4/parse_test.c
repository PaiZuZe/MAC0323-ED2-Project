/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

#include "parser.h"
#include "buffer.h"
#include "optable.h"
#include "error.h"
#include <string.h>
#include <stdio.h>

#define ERR_PAD 7
static int line_number = 0;

// Prints the str line and a '^'' above the the word indicated by errptr.
void print_error (const char *line, const char *errptr)
{
    char *c = (char *) line;
    int n = snprintf (NULL, 0, "%d", line_number); // Works for C99 standard

    fprintf (stderr, "line %d: %s\n", line_number, line);
    for (int i = 0; i < n + ERR_PAD; i++)
        fprintf (stderr, " ");

    while (c != errptr) {
        if (*c == '\t')
            fprintf (stderr, "\t");
        else
            fprintf (stderr, " ");
        c++;
    }
    fprintf (stderr, "^\n");
    print_error_msg (NULL);
}

void ilist_destroy (Instruction *ilist)
{
    Instruction *next = ilist->next;
printf("call to ilist dest\n");
    if (next) ilist_destroy (next);
    instr_destroy(ilist);
}

// Prints the Operand *opd type and its value.
void print_operand (Operand *opd)
{
    if (opd->type == REGISTER)
        printf ("Register(%u)", (opd->value).reg);
    else if (opd->type == LABEL)
        printf ("Label(%s)", (opd->value).label);
    else if (opd->type == NUMBER_TYPE)
        printf ("Number(%lld)", (opd->value).num);
    else if (opd->type == STRING)
        printf ("String(%s)", (opd->value).str);
    else
        printf ("Operand(?)");
}

/*
 * Handles IS pseudo-operator
 *
 * If alias is valid adds entry in symbol table and returns a positive
 * non-zero integer, if alias is not valid returns zero.
 */
int validate_alias (Instruction *instruction, SymbolTable alias_table)
{
    int valid = 0;
    InsertionResult result = stable_insert (alias_table, instruction->label);

    if (result.new) {
        (result.data)->opd = instruction->opds[0];
        valid = 1;
    }

    return valid;
}

// Prints the parsed line with the specs of its instruction.
void print_parsed_line (const char *line, Instruction *instruction, SymbolTable alias_table)
{
    if (instruction->op == optable_find ("IS")) {
        if (instruction->label) {
            if (!(validate_alias (instruction, alias_table))) {
                set_error_msg ("label \"%s\" already in use", instruction->label);
                print_error (line, (char *) line);
                return;
            }
        }
        else {
            set_error_msg ("IS operator expects a label");
            print_error (line, (char *) line);
            return;
        }
    }
    else if (instruction->label) {
        InsertionResult result = stable_insert (alias_table, instruction->label);
        if (result.new)
            (result.data)->opd = operand_create_label (instruction->label);
    }

    printf ("line     = %s\n", line);
    if (instruction->label)
        printf ("label    = \"%s\"\n", instruction->label);
    else
        printf ("label    = n/a\n");
    printf ("operator = %s\n", (instruction->op)->name);
    printf ("operands = ");
    if (instruction->opds[0])
        print_operand (instruction->opds[0]);
    for (int i = 1; i <= 2; i++)
        if (instruction->opds[i]) {
            printf (", ");
            print_operand (instruction->opds[i]);
        }
    printf ("\n");
}

int main (int argc, char **argv)
{
    char c;
    const char **errptr = emalloc(sizeof(char *));
    FILE *input;
    Buffer *buffer = buffer_create (sizeof(char));
    SymbolTable aliases = stable_create ();
    Instruction **parsed = emalloc (sizeof(Instruction *));
    Instruction *tmp = NULL;

    set_prog_name (argv[0]);
    if (argc - 1 != 1)
        die ("Wrong number of arguments (%d given, 1 expected)\n", argc - 1);

    input = fopen (argv[1], "r");
    if (input == NULL)
        die ("Error reading file %s:", argv[1]);

    printf ("\n");
    while ((c = fgetc (input)) != EOF) {
        ungetc (c, input);
        line_number++;

        read_line (input, buffer);
        if (((char *) buffer->data)[buffer->p - 1] == '\n')
            ((char *) buffer->data)[buffer->p - 1] = '\0';
        else
            buffer_push_char(buffer, '\0');

        c = 0;
        if (parse ((const char *) buffer->data, aliases, parsed, errptr)) {
            if (*parsed) {
                print_parsed_line ((const char *) buffer->data, *parsed, aliases);
                if (!tmp) {
                    tmp = emalloc (sizeof(Instruction));
                    *tmp = **parsed;
                }
                *parsed = (*parsed)->next;
                c = 1;
            }
        }
        else {
            print_error ((const char *) buffer->data, *errptr);
            fprintf (stderr, "\n");
        }
        if (c) printf ("\n");
    }

    ilist_destroy (tmp);
    free (parsed);
    free(errptr);
    stable_destroy (aliases);
    buffer_destroy (buffer);
    fclose (input);

    return 0;
}
