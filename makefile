#	File:		makefile
#	Project:	SubgateUniverse
#	Author:		Samuel Aeberhard

# *** substitutions
OBJ = SubgateUniverse.o suFileHandling.o suGameRoutines.o suNetworking.o
BIN = SubgateUniverse
CFLAGS	= -lm -lGL -lGLU -lSDL -lGLEW

# *** build rules
SubgateUniverse:			$(OBJ)
							gcc -o $@ $^ $(CFLAGS)

%.o:						%.c
							gcc -Wall -c -o $@ $<

# *** clean rule
clean:						
							rm -f -r $(BIN) $(OBJ)
