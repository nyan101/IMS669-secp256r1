test : p256_int.o p256_AF.o p256_ECC.o test.o
	gcc -o test test.o p256_int.o p256_AF.o p256_ECC.o -lgmp

p256_int.o : p256_int.c p256_int.h p256_config.h
	gcc -c p256_int.c -lgmp

p256_AF.o : p256_AF.c p256_AF.h p256_int.h p256_config.h
	gcc -c p256_AF.c -lgmp

p256_ECC.o : p256_ECC.c p256_ECC.h p256_AF.h p256_int.h p256_config.h
	gcc -c p256_ECC.c -lgmp

test.o : test.c p256_int.h p256_AF.h p256_config.h
	gcc -c test.c -lgmp

clean :
	rm *.o