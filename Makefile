EE_BIN = test.elf
EE_OBJS = crt0.o crt0c.o main.o

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

run: $(EE_BIN)
	ps2client -h 192.168.1.10 execee host:$(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
