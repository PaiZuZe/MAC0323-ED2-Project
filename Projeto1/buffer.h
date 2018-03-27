// Guilherme Costa Vieira               Nº USP: 9790930
// Gabriel Kazuyuki Isomura             Nº USP: 9793673
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
/*
  buffer.h

  A character buffer.
*/

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h>
#include <stdlib.h>


// Buffer struct.
typedef struct buffer_s {
  char *data;

  // Buffer max. size and first free position.
  int n, i;
} Buffer;

/*
  Create and return a new and empty buffer.
*/
Buffer *buffer_create();

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B);

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B);

/*
  Add a character c to the end of the buffer.
*/
void buffer_push_back(Buffer *B, char c);

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.

  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B);

#endif
