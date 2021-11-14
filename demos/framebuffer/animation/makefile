EXE = test_animation

CROSS_PREFIX	?= 

CC = $(CROSS_PREFIX)gcc

SRC = $(wildcard *.c)

CFLAGS := -g -Wall -fPIC
CFLAGS := -I ../include
LDFLAGS := -L ../lib -ljpeg -lpthread

$(EXE):$(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY : clean

clean:
	rm -f $(EXE)