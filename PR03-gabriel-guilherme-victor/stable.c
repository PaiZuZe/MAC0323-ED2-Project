/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

/*
    TODO
    Arrumar o redimensionamento
    Tirar lixo das strings
*/

#include "stable.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* Remover antes de entregar */

#define INIT_SIZE 191
#define HASH_FACTOR 61

typedef struct bucket_s
{
    char *key;
    EntryData *val;
    struct bucket_s *next;
} *Bucket;

struct stable_s
{
    Bucket *data;
    long int size, items;
};

Bucket bucket_insert (Bucket head, const char *key)
{
    Bucket new_bucket;

    new_bucket = emalloc (sizeof(struct bucket_s));
    new_bucket->key = strclone (new_bucket->key, key);
    new_bucket->val = emalloc (sizeof(EntryData));
    new_bucket->next = head;

    return new_bucket;
}

void bucket_destroy (Bucket head)
{
    Bucket bkt;

    if (head != NULL) {
        bkt = head->next;
        free (head);
        bucket_destroy (bkt);
    }
}

Bucket bucket_get (Bucket head, const char *key)
{
    Bucket bkt = NULL;

    if (head != NULL) {
        if (strcmp (head->key, key) == 0) bkt = head;
        else bkt = bucket_get (head->next, key);
    }

    return bkt;
}

int bucket_visit (Bucket head, int (*visit)(const char *key, EntryData *data))
{
    int result = 1;

    if (head != NULL) {
        if (visit (head->key, head->val) == 0) result = 0;
        else if (head->next != NULL) result = bucket_visit (head, visit);
    }

    return result;
}

/* Hashing modular */
long int hash_it (const char *key, long int size)
{
    long int h = 0, i;

    for (i = 0; key[i] != '\0'; i++) h = (HASH_FACTOR*i + key[i]) % size;
    return h;
}

SymbolTable stable_resize (SymbolTable table)
{
    SymbolTable new_st = table;
    InsertionResult insertion;
    Bucket bkt;
    long int h = 2*table->size - 1, i;

    new_st = emalloc (sizeof(struct stable_s));
    new_st->size = h;
    new_st->data = emalloc (sizeof(Bucket)*new_st->size);
    new_st->items = table->items;
    for (i = 0; i < new_st->size; i++) new_st->data[i] = NULL;

    for (i = 0; i < table->size; i++) {
        bkt = table->data[i];
        while (bkt != NULL) {
            insertion = stable_insert (new_st, bkt->key);
            *(insertion.data) = *(bkt->val);

            bkt = bkt->next;
            free (table->data[i]);
        }
    }
    free (table);

    return new_st;
}

SymbolTable stable_create (void)
{
    SymbolTable st;
    int i;

    st = emalloc (sizeof(struct stable_s));
    st->size = INIT_SIZE;
    st->data = emalloc (sizeof(Bucket)*st->size);
    st->items = 0;
    for (i = 0; i < st->size; i++) st->data[i] = NULL;

    return st;
}

void stable_destroy (SymbolTable table)
{
    long int i;

    if (table != NULL) {
        for (i = 0; i < table->size; i++) bucket_destroy (table->data[i]);
        free (table);
    }
}

InsertionResult stable_insert (SymbolTable table, const char *key)
{
    InsertionResult result;
    Bucket *bkt;

    if (table != NULL) {
        /*if (table->items >= 10*table->size)
            table = stable_resize (table);*/

        bkt = &table->data[hash_it (key, table->size)];
        if (bucket_get (*(bkt), key) == NULL) {
            *(bkt) = bucket_insert (*(bkt), key);
            result.new = 1;
            result.data = (*(bkt))->val;
            table->items++;
        }
        else {
            result.new = 0;
            result.data = (*(bkt))->val;
        }
    }
    else {
        result.new = 0;
        result.data = NULL;
    }

    return result;
}

EntryData *stable_find (SymbolTable table, const char *key)
{
    EntryData *entry = NULL;
    Bucket bkt;

    if (table != NULL) {
        bkt = bucket_get (table->data[hash_it (key, table->size)], key);
        if (bkt != NULL) entry = bkt->val;
    }

    return entry;
}

int stable_visit (SymbolTable table,
                  int (*visit)(const char *key, EntryData *data))
{
    int i, result = 1;

    if (table != NULL)
        for (i = 0; i < table->size && result != 0; i++)
            result = bucket_visit (table->data[i], visit);

    return result;
}

