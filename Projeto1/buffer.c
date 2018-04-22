/* Guilherme Costa Vieira               Nº USP: 9790930
   Gabriel Kazuyuki Isomura             Nº USP: 9793673
   Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
*/
#include "buffer.h"
#include <unistd.h>

Buffer *buffer_resize(Buffer *B) {
    B->buffer_size *= 2;
    B->data = realloc(B->data, B->buffer_size * B->member_size);
    return B;
}

Buffer *buffer_create(size_t member_size) {
    Buffer *buffer = malloc(sizeof(Buffer));
    buffer->buffer_size = 1;
    buffer->member_size = member_size;
    buffer->p = 0;
    buffer->data = malloc(member_size);
    return buffer;
}

void buffer_destroy(Buffer *B) {
    free(B->data);
    free(B);
    return;
}

void buffer_reset(Buffer *B) {
    free(B->data);
    B->data = malloc((B->member_size));
    B->p = 0;
    B->buffer_size = 1;
    return;
}

void *buffer_push_back(Buffer *B){
    if (B->buffer_size == B->p)
        B = buffer_resize(B);
    return &((char *) B->data)[B->p];
}

int read_line(FILE *input, Buffer *B) {
    char char_read;
    int n_chars_read = 0;

    buffer_reset(B);
    char_read = fgetc(input);


    if (char_read == EOF)
        return 0;

    while (char_read != EOF && char_read != '\n') {
        buffer_push_char(B, char_read);
        B->p += 1;
        n_chars_read++;
        char_read = fgetc(input);
    }

    if (char_read == '\n') {
        buffer_push_char(B, char_read);
        B->p += 1;
        n_chars_read++;
    }

    return n_chars_read;
}
