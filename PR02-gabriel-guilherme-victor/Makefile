center : center.o buffer.o    
	gcc -Wall -std=c99 -pedantic -O2 -Wextra buffer.o center.o -o center

center.o : center.c
	gcc -Wall -std=c99 -pedantic -O2 -Wextra center.c -c
    
buffer.o : buffer.c
	gcc -Wall -std=c99 -pedantic -O2 -Wextra buffer.c -c
	
clean :
	rm *.o && rm center	        
