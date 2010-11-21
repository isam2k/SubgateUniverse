#	File:		makefile
#	Project:	SubgateUniverse
#	Author:		Samuel Aeberhard

# *** substitutions
CC = gcc
OBJSU = src/su.o src/suFileHandling.o src/suGameRoutines.o src/suNetworking.o
OBJSUD = src/sud.o src/sud_nw_utils.o src/sud_pm_utils.o
OBJTOOLS = tools/packet.o
BIN = subgate_universe sud packet
CFLAGS	= -lm -lGL -lGLU -lSDL -lGLEW

# *** build all
all:						$(BIN)

# *** build rules
subgate_universe:			$(OBJSU)
							$(CC) -o $@ $^ $(CFLAGS)

sud:						$(OBJSUD)
							$(CC) -o $@ $^
							
packet:						$(OBJTOOLS)
							$(CC) -o $@ $^

%.o:						%.c
							$(CC) -Wall -c -o $@ $<

# *** clean rule
clean:						
							rm -f -r $(BIN) $(OBJSU) $(OBJSUD) $(OBJTOOLS)
