MF=			makefile
CC=			g++
EXE=    	extractor
SRC=		main.c
OBJ= 		$(SRC:.c=.o)
HD=			
CFLAGS=		-Wall
LDFLAGS=	-Wall -lm

# RULES, NO NEED TO EDIT BELOW THIS LINE

all:    $(EXE)

$(EXE): $(OBJ)
	$(CC)  $^ -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $< $(CFLAGS) 

$(OBJ): $(MF) $(HD)

.PHONY: clean clean-all

clean:
	rm -f $(OBJ)

clean-all: clean
	rm -f $(EXE)
