#include "buffer.h"

Buffer *buffer_resize(Buffer *B) {
    int new_n, i = B->n*2;
    Buffer *new_buffer = malloc(sizeof(Buffer));
    new_buffer->n = new_n;
    new_buffer->i = B->i;
    new_buffer->data = malloc(new_n * sizeof(char));
    for (i = 0; i < B->n; i++)
        new_buffer->data[i] = B->data[i];
    buffer_destroy(B);
    return new_buffer;
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
    B->i = 0;
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
    buffer_push_back(B, char_read);
    /*Provavelmente podia ser um do while*/
    if (char_read == EOF)
        return 0;
    n_chars_read++;
    while (char_read != EOF || char_read != '\n') {
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
