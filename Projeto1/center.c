/* Guilherme Costa Vieira               Nº USP: 9790930
   Gabriel Kazuyuki Isomura             Nº USP: 9793673
   Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
*/
#include "buffer.h"
#include <ctype.h>

/* Receives a pointer to Buffer b and string output_file. Prints the characters
 * stored in the buffer in the file named output_file. */
void write_file(Buffer *b, char *output_file, int line, int col) {
    FILE *output;
    int spaces = (col - b->p)/2;
    output = fopen(output_file, "a");
    /* fprintf(stderr, "b->p = %d and spaces = %d\n", b->p, spaces); */
    /* Checks if the line is too long. */
    if (spaces >= 0) {
        while (spaces >= 0) {
            fprintf(output, " ");
            spaces--;
        }
    }
    else
        fprintf(stderr, "center: %s: line %d is too long.\n",
                output_file, line);
    fprintf(output, "%s", (char *) b->data);
    fclose(output);
    return;
}

/* Receives and pointer to Buffer b. Removes the spaces before the first
 * non-space character and removes the spaces after the last non-space
 * character. Returns a pointer to Buffer containing the string with the
 * spaces removed. */
Buffer *strip_space(Buffer * b) {
    int ini, final, i;
    Buffer *new_buffer;
    new_buffer = buffer_create(b->member_size);

    ini = 0; /* Position of the first non-space character. */
    final = (int) b->p -1; /* Position of the last non-space character. */
    /* Find ini and final. */
    while(ini < final + 1 && isspace(((char *) b->data)[ini])) ini++;
    while(final >= 0 && isspace(((char *) b->data)[final])) final--;

    /* Checks if is a blank line. */
    if (final < ini) {
        buffer_destroy(new_buffer);
        buffer_reset(b);
        /* Adds null terminator in the first position in order to not print
         * garbage (indicating an empty line). */
        ((char *)b->data)[0] = '\0';
        return b;
    }

    /* Pushes the characters between the first and the last non-space
     * characters. */
    i = ini;
    while(i <= final) {
        buffer_push_char(new_buffer, ((char *) b->data)[i]);
        new_buffer->p += 1;
        i++;
    }
    buffer_push_char(new_buffer, '\n');
    new_buffer->p += 1;
    buffer_push_char(new_buffer, '\0');
    new_buffer->p += 1;
    buffer_destroy(b);
    return new_buffer;
}

void center(char *input_file, char *output_file, int col) {
    int line;
    Buffer *buffer;
    FILE *input;
    buffer = buffer_create(sizeof(char));

    /* Opening the input_file. */
    input = fopen(input_file, "r");
    if (input == NULL) {
        fprintf(stderr, "ERROR: File %s does not exist.\n", input_file);
        exit(1);
    }

    /* Reads every line from input_file, centralize and print them to the
     * output_file. */
    line = 1;
    while(read_line(input, buffer)) {
        buffer = strip_space(buffer);

        if (iscntrl(((char *) buffer->data)[0]))
            continue;
        else
            write_file(buffer, output_file, line, col);
        line++;
    }
    fclose(input);

    return;
}

int main(int argc, char **argv) {
    int col;
    char *input_file, *output_file;

    /* Read arguments. */
    input_file = argv[1];
    output_file = argv[2];
    col = atoi(argv[3]);

    if (argc != 4) {
        fprintf(stderr, "ERROR: Expected 3 arguments.\n");
        return 1;
    }

    center(input_file, output_file, col); /* Centralize the text. */

    return 0;
}
