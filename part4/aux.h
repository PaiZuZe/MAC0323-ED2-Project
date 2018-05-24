/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

/* Handy functions */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    Allocates a memory block, returning a pointer to it or exiting the program
    if the allocation failed.
*/
void *emalloc (size_t size);

/*
    Allocates memory for dest and copies string src to it, returning dest
*/
char *strclone (char *dest, const char *src);

/*
    Allocates memory for dest and copies the first n characters of string src to
    it, returning dest
*/
char *strnclone (char *dest, const char *src, size_t n);

