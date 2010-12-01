#	File:		makefile
#	Project:	SubgateUniverse
#	Author:		Samuel Aeberhard

# *** substitutions
CC = gcc
OBJSU = src/su_main.o src/su_fh_utils.o src/su_gr_utils.o src/su_nw_utils.o
OBJSUD = src/sud_main.o src/sud_nw_utils.o src/sud_pm_utils.o
OBJTOOLS = tools/packet.o
BIN = subgate sud packet
CFLAGS	= -lm -lGL -lGLU -lSDL -lGLEW
BINDIR = /usr/games
RESDIR = ~/.subgateuniverse

# *** build all
all:			$(BIN)

# *** build rules
subgate:		$(OBJSU)
			$(CC) -o $@ $^ $(CFLAGS)

sud:			$(OBJSUD)
			$(CC) -o $@ $^

packet:			$(OBJTOOLS)
			$(CC) -o $@ $^

%.o:			%.c
			$(CC) -Wall -c -o $@ $<

# *** clean rule
clean:			
			rm -f -r $(BIN) $(OBJSU) $(OBJSUD) $(OBJTOOLS)
