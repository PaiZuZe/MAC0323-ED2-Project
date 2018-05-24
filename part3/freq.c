/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

#include "stable.h"
#include "buffer.h"
#include "aux.h"
#include <string.h>
#include <ctype.h>

typedef struct earray_s
{
    char *key;
    int val;
} *EntryArray;

EntryArray entries;
int entries_top;

EntryArray earray_create (int n, int *count)
{
    EntryArray earr = emalloc (n*sizeof(struct earray_s));

    *(count) = 0;
    return earr;
}

/*
    Stores entry in array
*/
int get_entry (const char *key, EntryData *data)
{
    entries[entries_top].key = strclone (entries[entries_top].key, key);
    entries[entries_top++].val = data->i;
    return 1;
}

/*
    Compares entries keys
*/
int entry_compare (const void *a, const void *b)
{
    return strcmp (((EntryArray) a)->key, ((EntryArray) b)->key);
}

/*
    Inserts buffer content in symbol table
*/
InsertionResult safe_st_insert (SymbolTable table, Buffer *buffer)
{
    char *key = buffer->data;

    key = strnclone (key, (char *) buffer->data, buffer->p);
    return stable_insert (table, key);
}

/*
    Gets ST keys, visiting it with get_entry, and sorts them. Prints the sorted
    keys specified
*/
void print_keys (SymbolTable table, int max_word)
{
    int spaces, n = entries_top, i;

    entries = earray_create (n, &entries_top);
    i = stable_visit (table, &get_entry);
    qsort (entries, n, sizeof(struct earray_s), &entry_compare);

    for (i = 0; i < entries_top; i++) {
        spaces = max_word - strlen (entries[i].key);
        printf ("%s ", entries[i].key);
        while (spaces > 0) {
            putc (' ', stdout);
            spaces--;
        }
        printf ("%d\n", entries[i].val);
    }
}

/*
    Reads file and stores words on both symbol table and BST, keeping track of
    the length of the longest word.
*/
void store_words (FILE *file, SymbolTable *table, int *max_word)
{
    Buffer *b;
    char c, in_word = 0;
    InsertionResult insertion;

    b = buffer_create (sizeof(char));
    c = fgetc (file);
    while (c != EOF) {
        if (!isspace(c)) {
            if (!in_word) in_word = 1;
            buffer_push_char (b, c);
            b->p++;
        }
        else if (in_word) {
            in_word = 0;
            buffer_push_char (b, '\0');
            b->p++;
            insertion = safe_st_insert (*(table), b);
            if (insertion.new) {
                insertion.data->i = 1;
                entries_top++;
            }
            else insertion.data->i++;

            if ((int) strlen ((char *) b->data) > *(max_word))
                *(max_word) = (int) strlen ((char *) b->data);
            buffer_reset (b);
        }
        c = fgetc (file);
    }
    if (in_word) {
        buffer_push_char (b, '\0');
        b->p++;
        insertion = safe_st_insert (*(table), b);
        if (insertion.new) {
            insertion.data->i = 1;
            entries_top++;
        }
        else insertion.data->i++;

        if ((int) strlen ((char *) b->data) > *(max_word))
           *(max_word) = (int) strlen ((char *) b->data);
        buffer_reset (b);
    }
    buffer_destroy (b);
}

int main (int argc, char **argv) {
    SymbolTable st;
    FILE *input;
    int max_word = -1;
    entries_top = 0;

    if (argc != 2) {
        fprintf (stderr, "ERROR: Expected 1 arguments.\n");
        exit (1);
    }

    input = fopen (argv[1], "r");
    if (input == NULL) {
        fprintf (stderr, "ERROR: File %s does not exist.\n", argv[1]);
        exit (1);
    }

    st = stable_create ();
    store_words (input, &st, &max_word);
    print_keys (st, max_word);

    stable_destroy (st);
    return 0;
}
