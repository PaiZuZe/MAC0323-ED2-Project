/* Guilherme Costa Vieira               Nº USP: 9790930
   Gabriel Kazuyuki Isomura             Nº USP: 9793673
   Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
*/
#include "buffer.h"

#include <unistd.h>

Buffer *buffer_resize(Buffer *B) {
    B->n *= 2;
    B->data = (char *) realloc(B->data, B->n * sizeof(char));
    return B;
}

Buffer *buffer_create() {
    Buffer *buffer = malloc(sizeof(Buffer));
    buffer->n = 1;
    buffer->i = 0;
    buffer->data = malloc(sizeof(char));
    return buffer;
}

void buffer_destroy(Buffer *B) {
    free(B->data);
    free(B);
    return;
}

void buffer_reset(Buffer *B) {
    free(B->data);
    B->data = malloc(sizeof(char));
    B->i = 0;
    B->n = 1;
    return;
}

void buffer_push_back(Buffer *B, char c) {
    if (B->n == B->i)
        B = buffer_resize(B);
    B->data[B->i] = c;
    B->i++;
    return;
}

int read_line(FILE *input, Buffer *B) {
    char char_read;
    int n_chars_read = 0;

    buffer_reset(B);
    char_read = fgetc(input);

    if (char_read == EOF)
        return 0;

    while (char_read != EOF && char_read != '\n') {
        buffer_push_back(B, char_read);
        n_chars_read++;
        char_read = fgetc(input);
    }

    if (char_read == '\n') {
        buffer_push_back(B, char_read);
        n_chars_read++;
    }

    return n_chars_read;
}
