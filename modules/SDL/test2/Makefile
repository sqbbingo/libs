SRC = $(wildcard *.c)
ELF = $(SRC:%.c=%)

CROSS_PREFIX ?=

CC = $(CROSS_PREFIX)gcc
RM = rm -f

CFLAGS += -Isdl/include/SDL
CFLAGS += -Its/include

LIBS += -lSDL -lSDLmain
LIBS += -lts

LDFLAGS += -Lsdl/lib
LDFLAGS +=-Lts/lib
LDFLAGS += -g -Wall -pthread

all:$(ELF)

$(ELF):%:%.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@ $(LIBS)

.PHONY:clean
clean:
	-$(RM) $(ELF) 