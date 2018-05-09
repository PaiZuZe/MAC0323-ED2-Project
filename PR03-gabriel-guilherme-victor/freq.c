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

typedef struct bst_s
{
    char *key;
    struct bst_s *left;
    struct bst_s *right;
} *Node;

Node bst_insert (Node root, Buffer *buffer)
{
    if (root == NULL) {
        root = emalloc (sizeof (struct bst_s));
        root->key = strnclone (root->key, (char *) buffer->data, buffer->p + 1);
        printf ("Inserting in tree: %s\n", root->key);
        root->left = NULL;
        root->right = NULL;
    }
    else if (strcmp (buffer->data, root->key) < 0)
        root->left = bst_insert (root->left, buffer);
    else if (strcmp (buffer->data, root->key) > 0)
        root->right = bst_insert (root->right, buffer);

    return root;
}

InsertionResult safe_st_insert (SymbolTable table, Buffer *buffer)
{
    char *key = buffer->data;

    key = strnclone (key, (char *) buffer->data, buffer->p + 1);
    return stable_insert (table, key);
}

void print_keys (Node root, SymbolTable table, int max_word)
{
    int count, spaces;

    if (root->left != NULL) print_keys (root->left, table, max_word);

    spaces = max_word - strlen (root->key);
    count = (stable_find (table, root->key))->i;
    printf ("%s ", root->key);
    while (spaces > 0) {
        putc (' ', stdout);
        spaces--;
    }
    printf ("%d\n", count);

    if (root->right != NULL) print_keys (root->right, table, max_word);
}

void store_words (FILE *file, Node *root, SymbolTable *table, int *max_word)
{
    Buffer *b;
    char c, in_word = 0;
    InsertionResult insertion;

    b = buffer_create (sizeof(char));
    c = fgetc (file);
    while (c != EOF) {
        if (!isblank (c) && c != '\n') { /* \n Nao e blank e buga a saida */
            if (!in_word) in_word = 1;
            buffer_push_char (b, c);
            b->p++;
        }
        else if (in_word) {
            in_word = 0;
            insertion = safe_st_insert (*(table), b);
            if (insertion.new) {
                insertion.data->i = 1;
                *(root) = bst_insert (*(root), b);
            }
            else insertion.data->i++;

            if ((int) strlen ((char *) b->data) > *(max_word))
                *(max_word) = (int) strlen ((char *) b->data);
            buffer_reset (b);
        }
        c = fgetc (file);
    }
    if (in_word) {
        insertion = safe_st_insert (*(table), b);
        if (insertion.new) {
            insertion.data->i = 1;
            *(root) = bst_insert (*(root), b);
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
    Node tree = NULL;
    int max_word;
    FILE *input;

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
    store_words (input, &tree, &st, &max_word);
    print_keys (tree, st, max_word);

    stable_destroy (st);
    return 0;
}

