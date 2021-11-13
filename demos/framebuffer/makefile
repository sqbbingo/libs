EXE = 

CROSS_PREFIX	?= 

CC = $(CROSS_PREFIX)gcc

SRC = $(wildcard *.c)
EXE = $(basename $(SRC))

CFLAGS := -g -Wall -fPIC
CFLAGS := -I ./include
LDFLAGS := -L ./lib -ljpeg

$(EXE):$(SRC)
	$(foreach aim,$(EXE),$(CC) $(CFLAGS) $(aim).c -o $(aim) $(LDFLAGS);)

.PHONY : clean

clean:
	rm -f $(EXE)