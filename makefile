MF=			makefile
CC=			g++
EXE=    	extractor
SRC=		$(SRC:.o=.cpp)
OBJ= 		main.o functions.o
HD=			functions.hpp
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
