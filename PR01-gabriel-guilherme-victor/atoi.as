* Guilherme Costa Vieira               Nº USP: 9790930
* Gabriel Kazuyuki Isomura             Nº USP: 9793673
* Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
        EXTERN  atoi
a       IS      $0 
seq     IS      $1 
char    IS      $2 
num     IS      $3 

*Sub-rotina recebe um endereço para uma sequência de chars terminados em 0 
*e transforma em números decimais

atoi    SUBU    a, rSP, 16      
        XOR     num, num, num
        LDOU    seq, a, 0

loop    LDBU    char, seq, 0        
        JZ      char, save
        MULU    num, num, 10
        SUBU    char, char, 48
        ADDU    num, num, char
        ADDU    seq, seq, 1
        JMP     loop

save    OR      rA, num, 0

end     RET     1