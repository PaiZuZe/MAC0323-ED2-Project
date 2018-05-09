/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

#include "aux.h"

void *emalloc (size_t size)
{
    void *ptr = malloc (size);

    if (ptr == NULL) {
        fprintf (stderr, "Memory allocation error!\n");
        exit (1);
    }
    return ptr;
}

char *strclone (char *dest, const char *src)
{
    size_t length;

    length = strlen (src);
    dest = emalloc (length*sizeof(char));
    dest = strcpy (dest, src);
    if (length > 0) dest[length] = '\0';

    return dest;
}

char *strnclone (char *dest, const char *src, size_t n)
{
    dest = emalloc (n*sizeof(char));
    dest = strncpy (dest, src, n);
    if (n > 0) dest[n - 1] = '\0';

    return dest;
}

