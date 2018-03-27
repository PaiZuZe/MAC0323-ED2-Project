#include <string.h>

typedef struct {
    char **key;
    EntryData *data;
    int last, max;
} stable_s;


SymbolTable stable_create() {
    SymbolTable sym_table = malloc(sizeof(stable_s));
    sym_table.max = 128;
    sym_table.last = 0;
    sym_table.key = malloc(sizeof(char *));
    sym_table.data = malloc(sizeof(EntryData));
    return sym_table;
}

void stable_destroy(SymbolTable table) {
    int i;
    for (i = 0; i < table.last; i++)
        free(table.key[i]);
    free(table.key);
    free(table.data);
    free(table);
    return;
}

InsertionResult stable_insert(SymbolTable table, const char *key) {
    InsertionResult result;
    int i = 0;
    for (i = 0; i < table.last; i++) {
        if (!strcmp(key, table.key[i])) {
            result.new = 0;
            result.data = table.data[i];
            return result;
        }
    }

    if (table.last == table.max) {
        int new_max = 2 * table.max;
        table.key = realloc(table.key, new_max);
        table.data = realloc(table.data, new_max);
        table.max = new_max;
    }

    table.key[table.last] = strcpy(malloc(strlen(key)*sizeof(char)), key);
    result.new = 1;
    result.data = &table.data[i];
    return result;
}

EntryData *stable_find(SymbolTable table, const char *key) {
    int i;
    for (i = 0; i < table.last; i++)
        if (!strcmp(key, table.key[i]))
            return &table.data[i];
    return NULL;
}

int stable_visit(SymbolTable table,
                 int (*visit)(const char *key, EntryData *data)) {

}
