#include "buffer.h"
#include <ctype.h>

void write_file(Buffer *b, char *output_file) {
    FILE *output;
    output = fopen(output_file, "a");
    fprintf(output, "%s", b->data);
    fclose(output);
    return;
}


Buffer *strip_space(Buffer * b) {
    int ini, final, i;
    Buffer *new_buffer;

    ini = 0;
    final = b->i -1;

    new_buffer = buffer_create();
    while(ini < b->i && isspace(b->data[ini])) ini++;
    while(final >= 0 && isspace(b->data[final])) final--;

    if (final < ini) {
        buffer_destroy(new_buffer);
        buffer_reset(b);
        b->data[0] = '\0';
        return b;
    }

    i = ini;
    while(i <= final) {
        buffer_push_back(new_buffer, b->data[i]);
        i++;
    }
    buffer_push_back(new_buffer, '\n');
    buffer_destroy(b);
    return new_buffer;
}


void center(char *input_file, char *output_file, int c) {
    Buffer *buffer;
    FILE *input;

    input = fopen(input_file, "r");
    buffer = buffer_create();

    while(read_line(input, buffer)) {
        buffer = strip_space(buffer);

        if (isspace(buffer->data[0])) {
            write_file(buffer, output_file);
        }
        else if (buffer->i >= c) {
            write_file(buffer, output_file);
        }
        else {
            write_file(buffer, output_file);
        }

    }
    fclose(input);

    return;
}


int main(int argc, char **argv) {
    char *input_file, *output_file;
    int c;

    input_file = argv[1];
    output_file = argv[2];
    c = atoi(argv[3]);

    if (argc != 4)
        return 1;

    center(input_file, output_file, c);

    return 0;
}
