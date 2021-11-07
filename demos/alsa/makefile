EXE = alsa_test
CROSS_PREFIX 	?=
OUTPUT			?= ./build

CC	=$(CROSS_PREFIX)gcc
CXX = $(CROSS_PREFIX)g++
LD 	= $(CROSS_PREFIX)ld
AR 	= $(CROSS_PREFIX)ar
CP 	= cp
RM 	= rm
MKDIR 	= mkdir

CFLAGS := -g -Wall -fPIC
CFLAGS += $($(ARCH)_CFLAGS)

SHARED := -shared

LDFLAGS := -lasound

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

$(EXE):$(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	$(RM) -f $(EXE) $(OBJ)