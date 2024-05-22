CC = gcc #clang
CFLAGS = -Wall -Wextra -Wvla -pedantic -g -I include/ -fsanitize=address
LDLIBS = -lSDL2 -lm
LDFLAGS = -fsanitize=address

BIN = agbe
OBJ = src/control.o src/cpu.o src/disassembler.o src/emulation.o \
	  src/jump.o src/load.o src/logic.o src/mbc.o src/ppu.o \
	  src/ppu_utils.o src/prefix.o src/queue.o src/rendering.o src/rotshift.o \
	  src/test.o src/utils.o

all: $(BIN)
	$(CC) -o $(BIN) $(OBJ) $(LDLIBS) $(LDFLAGS)

$(BIN): $(OBJ)

clean:
	$(RM) $(OBJ) $(BIN)


.PHONY: all clean
