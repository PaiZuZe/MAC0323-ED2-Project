#include "parser.h"
#include "buffer.h"


char **split_line(char *og_string) {
    int count = 0;
    int flag = 0;
    char **words = emalloc(sizeof(char *) * 3);
    Buffer *b = buffer_create(sizeof(char));

    for (unsigned int i = 0; i < strlen(og_string); i++) {
        if (og_string[i] == '*') {
            flag = 1;
            break;
        }
        if (!isspace(og_string[i])) {
            buffer_push_char(b, og_string[i]);
            b->p += 1;
        }
        else if (b->p != 0){ //b->p != 0, buffer não está vazio
            words[count++] = estrdup((char *) b->data);
            buffer_reset(b);
        }
    }
    if (!flag) words[count++] = estrdup((char *) b->data);
    buffer_destroy(b);
    return words;
}
/*
int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    return 0;
}
*/

int main() {
    char bob[] = "Ola meu amigo!!! *Elementar meu caro Watson";
    char **bobs;
    printf("%s\n", bob);
    bobs = split_line(bob);
    for (int i = 0; i < 3; i++) printf("%s\n", bobs[i]);
    return 0;
}
