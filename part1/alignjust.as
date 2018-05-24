* Guilherme Costa Vieira               Nº USP: 9790930
* Gabriel Kazuyuki Isomura             Nº USP: 9793673
* Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
        	EXTERN		alignjust
a_char  	IS      	$0       *endereço do primeiro caractere da linha
l_char  	IS      	$1       *endereço do ultimo caractere da linha
c           IS          $2       *numero de colunas
spcs        IS          $3       *numero de espaços
words       IS          $4       *numero de palavras
resspc      IS          $5       *espaços para serem distribuídos
curspc      IS          $6       *grupo de espaço atual
quodiv      IS          $7       *quociente da divisão
aux         IS          $8       *variável auxiliar
linech      IS          $9       *

*Sub-rotina que justifica uma linha

alignjust   SUBU		a_char, rSP, 16
			LDOU   	    a_char, a_char, 0
			SUBU    	l_char, rSP, 24
			LDOU        l_char, l_char, 0
			SUBU    	c, rSP, 32
			LDOU        c, c, 0
			SETW        rX, 2

			XOR         spcs, spcs, spcs
			XOR         curspc, curspc, curspc

*Ver quantos espaços tem na linha
			OR			rZ, a_char, 0
spcloop		CMPU        rY, rZ, l_char
			JZ          rY, counts
			LDBU        aux, rZ, 0
			JZ          aux, spcplus
			ADDU        rZ, rZ, 1
			JMP         spcloop

spcplus     ADDU        spcs, spcs, 1
			ADDU        rZ, rZ, 1
			JMP         spcloop

counts      ADDU        words, spcs, 1
			SUBU        aux, l_char, a_char
			SUBU        resspc, c, aux
			DIVU        quodiv, resspc, spcs

loop   		CMPU        rZ, a_char, l_char
        	JZ          rZ, end
        	LDBU        rY, a_char, 0
        	JZ          rY, pspc
        	INT         #80
        	ADDU        a_char, a_char, 1
        	JMP         loop

pspc    	XOR         aux, aux, aux 
			SETW		rY, 32
			INT         #80
looppspc    CMPU        rZ, aux, quodiv
            JZ          rZ, endprspc
            INT         #80
            ADDU        aux, aux, 1
            JMP         looppspc

endprspc	ADDU        curspc, curspc, 1
			SUBU        aux, words, rR
			CMPU        rZ, curspc, aux
			JN          rZ, bcktoloop
			INT         #80
bcktoloop	ADDU        a_char, a_char, 1	
			JMP         loop

end			RET		3