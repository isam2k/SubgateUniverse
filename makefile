#	File:		makefile
#	Project:	SubgateUniverse
#	Author:		Samuel Aeberhard

# *** substitutions
CC = gcc
OBJ = src/SubgateUniverse.o src/suFileHandling.o src/suGameRoutines.o src/suNetworking.o
BIN = su sud
CFLAGS	= -lm -lGL -lGLU -lSDL -lGLEW

# *** build rules
su:							$(OBJ)
							$(CC) -o $@ $^ $(CFLAGS)

sud:						src/sud.o
							$(CC) -o $@ $^

%.o:						%.c
							$(CC) -Wall -c -o $@ $<

# *** clean rule
clean:						
							rm -f -r $(BIN) $(OBJ) src/sud.o
