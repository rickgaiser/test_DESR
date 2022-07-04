#include <fcntl.h>
#include <kernel.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#define MiB (1024*1024)

extern void __start(void);
extern void* _end;
extern void* _heap_size;

void print_machine_state()
{
    int stack;
    printf("Program: [%p, %p], program size %i, heap size %p\n", &__start, &_end, (int)&_end - (int)&__start, &_heap_size);
    printf("EndOfHeap %iKiB, memorySize %iKiB, machineType %i\n", (int)EndOfHeap() / 1024, GetMemorySize() / 1024, MachineType());
    printf("Stack start at %iKiB\n", (int)&stack / 1024);
}

int IsDESRMachine(void)
{
    int fd;

    fd = open("rom0:PSXVER", O_RDONLY);
    if (fd > 0) {
        close(fd);
        return 1;
    }

    return 0;
}

void _ps2sdk_memory_init()
{
    if (GetMemorySize() == 32*MiB && IsDESRMachine() == 1) {
        // Switch to 64MiB mode
        SetMemoryMode(0);
        _InitTLB();
        // Restart application
        __start();
    }
}

int main(int argc, char** argv)
{
    printf("argc = %d\n", argc);
    for (int i=0; i<argc; i++)
        printf("argv[%d] = %s\n", i, argv[i]);

    print_machine_state();

    return 0;
}
