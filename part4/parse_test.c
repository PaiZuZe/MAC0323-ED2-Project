#include "parser.h"
#include "buffer.h"
#include "error.h"
#include "optable.h"

static int line_number = 0;

void print_error (const char *line, char *errptr)
{
    char *c = (char *) line;

    printf ("line %d: %s\n", line_number, c);
    /*while (c != errptr) {
        printf (" a");
        c++;
    }*/
    print_error_msg ("^\nError:");
}

void print_operand (Operand *opd)
{
    if (opd->type == REGISTER)
        printf ("Register(%u)", (opd->value).reg);
    else if (opd->type == REGISTER)
        printf ("Label(%s)", (opd->value).label);
    else if (opd->type == REGISTER)
        printf ("Number(%lld)", (opd->value).num);
    else if (opd->type == REGISTER)
        printf ("String(%s)", (opd->value).str);
    else
        printf ("Operand(?)");
}

/*
    Handles IS pseudo-operator

    If alias is valid adds entry in symbol table and returns a positive non-zero
    integer, if alias is not valid returns zero.
*/
int validate_alias (Instruction *instruction, SymbolTable alias_table)
{
    int valid = 0;
    InsertionResult result = stable_insert (alias_table, instruction->label);

    if (result.new != 0) {
        (result.data)->opd = instruction->opds[0];
        valid = 1;
    }

    return valid;
}

void print_parsed_line (const char *line, Instruction *instruction, SymbolTable alias_table)
{
    if (instruction->op == optable_find ("IS")) {
        if (instruction->label) {
            if (!(validate_alias (instruction, alias_table))) {
                set_error_msg ("label %s already in use", instruction->label);
                print_error (line, (char *) line);
                return;
            }
        }
        else {
            set_error_msg ("label missing");
            print_error (line, (char *) line);
            return;
        }
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
    FILE *input;
    Buffer *buffer = buffer_create (sizeof(char));
    SymbolTable aliases = stable_create ();
    Instruction *parsed = emalloc (sizeof(Instruction));
    char *error, c;

    set_prog_name(argv[0]);
    if (argc - 1 != 1)
        die ("Wrong number of arguments (%d given, 1 expected)\n", argc - 1);

    input = fopen (argv[1], "r");
    if (input == NULL)
        die ("Error reading file %s:", argv[1]);

    while ((c = fgetc (input)) != EOF) {
        ungetc (c, input);
        line_number++;

        read_line (input, buffer);
        if (((char *) buffer->data)[buffer->p - 1] == '\n')
            ((char *) buffer->data)[buffer->p - 1] = '\0';
        else
            buffer_push_char(buffer, '\0');

        free (parsed);
        parsed = NULL;
        if (parse ((const char *) buffer->data, aliases, &parsed, (const char **) &error)) {
            if (parsed) print_parsed_line ((const char *) buffer->data, parsed, aliases);
        }
        else
            print_error ((const char *) buffer->data, error);
        printf ("\n");
    }

    stable_destroy (aliases);
    buffer_destroy (buffer);
    fclose (input);
    //return 0;

    printf ("E agora os testes do parser!\n");

    set_prog_name(argv[0]);

    Instruction **instr = emalloc(sizeof(Instruction *));
    const char **errptr = emalloc(sizeof(char *));
    *errptr = NULL;

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


    char *words8 = "     DIV     a,$0";


    SymbolTable ST = stable_create();
    InsertionResult bob;

    printf("====================================================================================\n");
    printf("Testando o 0\n");
    if (parse(words0, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 0\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");


    printf("====================================================================================\n");
    printf("Testando o 1\n");
    if (parse(words1, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 1\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

    printf("====================================================================================\n");
    bob = stable_insert(ST, "a");
    if (bob.new) bob.data->opd = operand_create_register('2');
    printf("Testando o 2\n");
    if (parse(words2, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 2\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

    printf("====================================================================================\n");
    bob = stable_insert(ST, "start");
    if (bob.new) bob.data->opd = operand_create_label("blah");
    printf("Testando o 3\n");
    if (parse(words3, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 3\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

    printf("====================================================================================\n");

    printf("Testando o 4\n");
    if (parse(words4, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 4\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

printf("====================================================================================\n");

    printf("Testando o 5\n");
    if (parse(words5, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 5\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

    printf("====================================================================================\n");

    printf("Testando o 6\n");
    if (parse(words6, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 6\n");

    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    *errptr = NULL;
    printf("\n\n");

    printf("====================================================================================\n");

    printf("Testando o 7\n");
    if (parse(words7, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 7\n");
    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");

    printf("====================================================================================\n");

    printf("Testando o 8\n");

    if (parse(words8, ST, instr, errptr) == 0)
        print_error_msg(NULL);
    else
        printf("Deu bom para o 8\n");
    if (*errptr != NULL)
        printf("%s\n", *errptr);
    else
        printf("ERRPT É NULL!\n");


    *errptr = NULL;
    printf("\n");

    return 0;
}
