CROSS_COMPILE = arm-fsl-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc

CFLAGS += -Isdl/include/SDL
CFLAGS += -Iffmpeg/include
# CFLAGS += -Iffmpeg/include/libavcodec
# CFLAGS += -Iffmpeg/include/libavfilter
# CFLAGS += -Iffmpeg/include/libavformat
# CFLAGS += -Iffmpeg/include/libavutil
# CFLAGS += -Iffmpeg/include/libswresample
# CFLAGS += -Iffmpeg/include/libswscale

LDFLAGS += -Lsdl/lib
LDFLAGS += -Lffmpeg/lib
LDFLAGS += -lavdevice
LDFLAGS += -lavfilter
LDFLAGS += -lavformat
LDFLAGS += -lavcodec
LDFLAGS += -lswscale
LDFLAGS += -lswresample
LDFLAGS += -lavutil
LDFLAGS += -lrt
# LDFLAGS += -lm
LDFLAGS += -lSDL
# LDFLAGS += -lz

media_player:media_player.c
	$(CC) $(CFLAGS) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) 
