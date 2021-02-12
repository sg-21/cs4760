BT	= bt
BT_OBJ	= bt.o
OUTPUT	= $(BT)

.SUFFIXES: .c .o

all: $(OUTPUT)

$(BT): $(BT_OBJ)
	gcc -Wall -g -o $@ $(BT_OBJ)

.c.o:
	gcc -Wall -g -c $<

.PHONY: clean
clean:
	/bin/rm -f $(OUTPUT) *.o
