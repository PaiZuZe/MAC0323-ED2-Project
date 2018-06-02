/*
    Guilherme Costa Vieira               9790930
    Gabriel Kazuyuki Isomura             9793673
    Victor Chiaradia Gramuglia Araujo    9793756
*/

#include "stable.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 191
#define HASH_FACTOR 61

long int hash_it (const char *key, long int size);

/*
    A linked list data type, stores elements with the same hash code
*/
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

/*
    Inserts an element in the bucket (at the front of the list)
*/
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
        if (head->key != NULL) free(head->key);
        if (head->val != NULL) free(head->val);
        bkt = head->next;
        free (head);
        bucket_destroy (bkt);
    }
}

/*
    Searches a bucket for the node associated with a given key. Returns NULL if
    such node isn't found.
*/
Bucket bucket_get (Bucket head, const char *key)
{
    Bucket bkt = NULL;

    if (head != NULL) {
        if (strcmp (head->key, key) == 0) bkt = head;
        else bkt = bucket_get (head->next, key);
    }

    return bkt;
}

/*
    Visits each element in the bucket. If the visit function returns zero, then
    the iteration stops. Returns zero if the iteration was stopped by the visit
    function, nonzero otherwise.
*/
int bucket_visit (Bucket head, int (*visit)(const char *key, EntryData *data))
{
    int result = 1;

    if (head != NULL) {
        if ((*visit)(head->key, head->val) == 0) result = 0;
        else if (head->next != NULL) result = bucket_visit (head->next, visit);
    }

    return result;
}

/*
    Modular hashing function
*/
long int hash_it (const char *key, long int size)
{
    long int h = 0, i;

    for (i = 0; key[i] != '\0'; i++) h = (HASH_FACTOR*i + key[i]) % size;
    return h;
}

/*
    Resizes Bucket array
*/
SymbolTable stable_resize (SymbolTable table)
{
    Bucket *old = table->data;
    Bucket bkt;
    long int old_size = table->size, h, i;

    table->size = 2*table->size - 1;
    table->data = emalloc (sizeof(Bucket)*table->size);
    for (i = 0; i < table->size; i++) table->data[i] = NULL;

    for (i = 0; i < old_size; i++) {
        for (bkt = old[i]; bkt != NULL; bkt = bkt->next) {
            h = hash_it (bkt->key, table->size);
            table->data[h] = bucket_insert (table->data[h], bkt->key);
            *(table->data[h]->val) = *(bkt->val);
        }
        bucket_destroy (old[i]);
    }

    return table;
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
        free (table->data);
        free (table);
    }
}

InsertionResult stable_insert (SymbolTable table, const char *key)
{
    InsertionResult result;
    Bucket *bkt;

    if (table != NULL) {
        if (table->items >= 10*table->size)
            table = stable_resize (table);

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
