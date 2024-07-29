CC = gcc #clang
CFLAGS = -Wall -Wextra -Wvla -pedantic -g -I include/ -O0
LDLIBS = -lSDL2 -lm

BIN = agbe
OBJ = src/control.o			\
	  src/cpu.o 			\
	  src/interrupts.o 		\
	  src/timers.o 			\
	  src/disassembler.o 	\
	  src/emulation.o 		\
	  src/jump.o 			\
	  src/load.o 			\
	  src/logic.o 			\
	  src/mbc.o 			\
	  src/ppu.o 			\
	  src/ppu_utils.o		\
	  src/prefix.o			\
	  src/queue.o 			\
	  src/rendering.o 		\
	  src/rotshift.o 		\
	  src/main.o 			\
	  src/save.o 			\
	  src/serial.o 			\
	  src/apu.o				\

all: $(BIN)

debug: CFLAGS += -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: all

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDLIBS) $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(BIN)


.PHONY: all clean
