test : ECC.o test.o
	gcc -o test test.o ECC.o -lgmp

ECC.o : ECC.c ECC.h
	gcc -c ECC.c -lgmp

test.o : test.c
	gcc -c test.c -lgmp

clean :
	rm *.o