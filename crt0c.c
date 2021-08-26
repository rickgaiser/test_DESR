#include <kernel.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>

//extern void* _end;
//extern int _heap_size;
//extern void* _fbss;
//extern void* _stack;
//extern int _stack_size;

__attribute__((weak)) void _ps2sdk_args_parse(int argc, char** argv);
__attribute__((weak)) void _ps2sdk_libc_init();
__attribute__((weak)) void _ps2sdk_libc_deinit();
__attribute__((weak)) void _ps2sdk_memory_init();
__attribute__((weak)) void _init();
__attribute__((weak)) void _fini();
int main(int argc, char** argv);

struct sargs {
    int32_t argc;
    char* argv[16];
    char payload[256];
};
extern struct sargs _args;

noreturn void _start()
{
    // clear bss area
    //memset(_fbss, 0, _end - _fbss);

    // setup current thread
    //SetupThread(_gp, _stack, _stack_size, &args, ExitThread);

    // initialize heap
    //SetupHeap(_end, _heap_size);

    // writeback data cache
    FlushCache(0);

    // Capability to override 32MiB on DESR/DVR models
    // NOTE: this call can restart the application
    if (_ps2sdk_memory_init)
        _ps2sdk_memory_init();

    // call ps2sdk argument parsing (weak)
    if (_ps2sdk_args_parse)
        _ps2sdk_args_parse(_args.argc, _args.argv);

    // initialize ps2sdk libc (weak)
    if (_ps2sdk_libc_init)
        _ps2sdk_libc_init();

    // call global constructors (weak)
    if (_init)
        _init();

    // Initialize the kernel (Apply necessary patches).
    _InitSys();

    // call main
    EI();
    _exit(main(_args.argc, _args.argv));
}

noreturn void _exit(int retval)
{
    // call global destructors (weak)
    if (_fini)
        _fini();

    // uninitialize ps2sdk libc (weak)
    if (_ps2sdk_libc_deinit)
        _ps2sdk_libc_deinit();

    // Exit
    Exit(retval);
}
