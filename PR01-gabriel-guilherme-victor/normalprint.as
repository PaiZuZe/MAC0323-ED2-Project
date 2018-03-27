* Guilherme Costa Vieira               Nº USP: 9790930
* Gabriel Kazuyuki Isomura             Nº USP: 9793673
* Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
        	EXTERN		normalprint
a_char  	IS      	$0
l_char  	IS      	$1 
char    	IS      	$2 

*Sub-rotina imprime uma linha de caracteres sem justificar

normalprint SUBU		a_char, rSP, 16
			LDOU   	    a_char, a_char, 0
			SUBU    	l_char, rSP, 24
			LDOU        l_char, l_char, 0
			SETW        rX, 2

loop   		CMPU        rZ, a_char, l_char
        	JZ          rZ, end
        	LDBU        rY, a_char, 0
        	JZ          rY, pspc
        	INT         #80
        	ADDU        a_char, a_char, 1
        	JMP         loop

pspc    	SETW		rY, 32
			INT         #80
			ADDU        a_char, a_char, 1	
			JMP         loop

end			RET		2