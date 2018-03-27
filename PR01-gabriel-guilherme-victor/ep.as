* Guilherme Costa Vieira               Nº USP: 9790930
* Gabriel Kazuyuki Isomura             Nº USP: 9793673
* Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756
 		    EXTERN 	   main
c           IS         $0                           * Colunas lidas na entrada padrão
spc         IS         $1                           * Se um caractere é ou não espaço
i           IS         $2                           * Índice do vetor de palavras no heap (marca o endereço do começo de uma nova palavra)
n           IS         $3                           * Total de palavras 
words       IS         $4                           * Primeiro endereço livre do stack
nAdd        IS         $5
j           IS         $8
cplusi      IS         $9
totalchar   IS         $10
aux         IS         $11
endof       IS         $12
k           IS         $13
m           IS         $14
comp        IS         $15
ultc        IS         $16

* Carrega c da linha de comando
main        SUBU	    rX, rSP, 16				
		LDOU 	    rX, rX, 0
		PUSH 	    rX
	      CALL        atoi
		OR          c, rA, 0
            OR          words, rSP, 0
            SETW        rX, 1
            XOR         i, i, i
            XOR         n, n, n
            XOR         m, m, m
            XOR         endof, endof, endof
            XOR         rA, rA, rA

*Lê as palavras e guarda na memória

start       INT         #80                           

limbo       SETW        rZ, 10
		JN          rA, eof          
		CMPU        aux, rA, rZ        
		JZ          aux, looppar       
            CALL        isspace                      
            JNZ         spc, start   
            PUSH        i                         
            STBU        rA, i, 0                        
            ADDU        i, i, 1                         
            ADDU        n, n, 1
            JMP         readword                         

*Verifica quando um parágrafo é quebrado

looppar     INT         #80
		JN          rA, eof
		CMPU        aux, rA, rZ
		JZ          aux, breakpar
		CALL        isspace
		JZ          spc, limbo
		JMP         looppar

breakpar    JMP         justify  
breakpar2   SETW        rX, 2
            SETW        rA, 10
            INT         #80
            JN          ultc, end
            SETW        rX, 1 
            JP          endof, end
            XOR         i, i, i
            JMP         start
         
readword    INT         #80                          
            JN          rA, finishword                 
            CALL        isspace                       
            JNZ         spc, finishword
            SETW        rZ, 10
            CMPU        comp, rA, rZ
            JZ          comp, finishword   
            STBU        rA, i, 0                        
            ADDU        i, i, 1                         
            JMP         readword                      
            
finishword  XOR         rY, rY, rY                      
            STBU        rY, i, 0                         
            ADDU        i, i, 1
            JZ          comp, looppar                         
            JMP         limbo  

eof         JP          endof, end
		ADDU        endof, endof, 1
			
*Início do processo de justificação                     
            
justify   	OR          ultc, i, 0                                      
		SL          n, n, 3                        
            ADDU        nAdd, n, words
            SUBU        totalchar, i, 1
            JN          totalchar, start
            ADDU        m, m, i
            XOR         i, i, i

*Coloca os caracteres em uma linha

putinline   JMP         largeword                  *Verifica se uma palavra é maior que as colunas
pilloop     CMPU        aux, i, totalchar
		JP          aux, breakpar2
		OR          j, i, 0
            ADDU        cplusi, i, c 

lineLoop    CMPU        rY, j, cplusi
            JZ          rY, decide
            ADDU        j, j, 1 
            JMP         lineLoop

* Decide os casos de justificação

decide      ADDU        rY, j, 1
            CMPU        rY, rY, totalchar
            JP          rY, plast
            LDBU        rY, j, 0
            JZ          rY, nprint    
            JMP         justifyline

*Caso 1: ser última ou única linha
plast       SAVE        rSP, c, i
            PUSH        totalchar
            PUSH        i
            CALL        normalprint
            REST        rSP, c, i
            SETW        rY, 10
            SETW        rX, 2
            INT         #80
            JMP         breakpar2

*Caso 2: a linha cabe exatamente nas colunas

nprint      SAVE        rSP, c, totalchar
            PUSH        j
            PUSH        i
            CALL        normalprint
            REST        rSP, c, totalchar
            SETW        rY, 10
            SETW        rX, 2
            INT         #80
            ADDU        rX, j, 1
            OR          i, rX, 0
            JMP         putinline

*Caso 3: a linha precisa ser justificada

justifyline OR          rZ, i, 0

keepjus     LDBU        rZ, j, 0
            JZ          rZ, calljust      
            SUBU        j, j, 1
            JMP         keepjus

calljust    SAVE        rSP, c, totalchar
            PUSH        c
            PUSH        j
            PUSH        i
            CALL        alignjust
            REST        rSP, c, totalchar
            SETW        rY, 10
            SETW        rX, 2
            INT         #80
            ADDU        rX, j, 1
            OR          i, rX, 0
            JMP         putinline

*Largeword verifica se uma palavra é grande demais para caber em c colunas

largeword   OR          j, i, 0
            ADDU        rZ, j, c
            CMPU        rY, rZ, totalchar 
            JP          rY, pilloop
            XOR         rX, rX, rX
            OR          aux, j, 0

looplrg     CMPU        rY, aux, rZ
            JZ          rY, scases
            LDBU        rA, aux, 0
            JNZ         rA, nospace
            ADDU        rX, rX, 1
nospace     ADDU        aux, aux, 1
            JMP         looplrg

*Verifica os casos possíveis

scases      SETW        rY, 1
            CMPU        aux, rX, rY
            JN          aux, oneword
            JZ          aux, twowords
            JMP         pilloop

*Uma palavra grande demais para a linha

oneword     LDBU        rA, j, 0
            SAVE        rSP, c, rA
            PUSH        j
            CALL        puts
            REST        rSP, c, rA
owloop      LDBU        rA, j, 0
            ADDU        j, j, 1
            JZ          rA, endowl
            JMP         owloop    

endowl      SETW        rX, 2
            SETW        rY, 10
            INT         #80
            OR          i, j, 0
            JMP         putinline

*Uma palavra pequena e uma grande demais

twowords    LDBU        rA, j, c
            JZ          rA, pilloop
            SAVE        rSP, c, rA
            PUSH        j
            CALL        puts
            REST        rSP, c, rA
twloop      LDBU        rA, j, 0
            ADDU        j, j, 1
            JZ          rA, endtwl
            JMP         twloop    

endtwl      SETW        rX, 2
            SETW        rY, 10
            INT         #80
            OR          i, j, 0
            JMP         putinline          

end         INT         0

*Sub-rotina isspace checa se um caractere guardado em rA é um espaço
isspace     CMPU        rY, rA, 9                
            JZ          rY, space                
            CMPU        rY, rA, 13                 
            JZ          rY, space
            CMPU        rY, rA, 32
            JZ          rY, space
            XOR         spc, spc, spc             
            RET         0                       
space       SETW        spc, 1                   
            RET         0