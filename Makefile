EE_BIN = test.elf
EE_OBJS = crt0.o main.o
EE_CFLAGS = -fno-builtin
EE_LDFLAGS = -nostartfiles

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

run: $(EE_BIN)
	ps2client -h 192.168.1.10 execee host:$(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
