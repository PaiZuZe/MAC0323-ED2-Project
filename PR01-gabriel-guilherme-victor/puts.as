* Guilherme Costa Vieira               Nº USP: 9790930
* Gabriel Kazuyuki Isomura             Nº USP: 9793673
* Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
			EXTERN		puts
s           IS          $0

*Sub-rotina recebe um endereço e imprime uma sequência de caracteres terminadas em 0

puts        SUBU        s, rSP, 16
            LDOU        s, s, 0
            SETW        rX, 2
write       LDB         rY, s, 0
            JZ          rY, end
            INT         #80
            ADDU        s, s, 1
            JMP         write
end         RET         1