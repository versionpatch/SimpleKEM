CC = g++
CFLAGS = -O2 -Wall -Wextra -Wshadow -Wpedantic -Werror
LIBS = -lgmpxx -lgmp


all : decaps.o encaps.o keygen.o x25519.o shake128.o hash_elgamal.o
	$(CC) keygen.o x25519.o shake128.o hash_elgamal.o $(LIBS) -o keygen
	$(CC) encaps.o x25519.o shake128.o hash_elgamal.o $(LIBS) -o encaps
	$(CC) decaps.o x25519.o shake128.o hash_elgamal.o $(LIBS) -o decaps

x25519 : x25519.cpp
	$(CC) $(CFLAGS) -c x25519.cpp
shake128 : shake128.cpp
	$(CC) $(CFLAGS) -c shake128.cpp
elgamal : hash_elgamal.cpp
	$(CC) $(CFLAGS) -c hash_elgamal.cpp
keygen : keygen.cpp 
	$(CC) $(CFLAGS) -c keygen.cpp
encaps : encaps.cpp 
	$(CC) $(CFLAGS) -c encaps.cpp
decaps : decaps.cpp
	$(CC) $(CFLAGS) -c decaps.cpp

clean : 
	rm *.o keygen encaps decaps