SymbolTable stable_create() {

}

void stable_destroy(SymbolTable table) {

}

InsertionResult stable_insert(SymbolTable table, const char *key) {

}

EntryData *stable_find(SymbolTable table, const char *key) {

}

int stable_visit(SymbolTable table,
                 int (*visit)(const char *key, EntryData *data)) {

}
