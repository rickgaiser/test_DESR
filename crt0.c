#include <kernel.h>
#include <stdint.h>
#include <stdnoreturn.h>  // noreturn
#include <string.h>

extern char* _end;
extern char* _heap_size;
extern char* _fbss;
extern char* _stack;
extern char* _stack_size;

__attribute__((weak)) void _ps2sdk_args_parse(int argc, char** argv);
__attribute__((weak)) void _ps2sdk_libc_init();
__attribute__((weak)) void _ps2sdk_libc_deinit();
__attribute__((weak)) void _ps2sdk_memory_init();
__attribute__((weak)) void _init();
__attribute__((weak)) void _fini();
noreturn void _start2();
int main(int argc, char** argv);

// System provided arguments when loaded normally
struct sargs {
    int32_t argc;
    char* argv[16];
    char payload[256];
};
struct sargs _args;

// ps2link provided arguments
struct sargs_ps2link {
    int32_t pid;
    struct sargs args;
};
struct sargs_ps2link *pargs_ps2link;

/*
 * First function to be called by the loader
 * This function sets up the stack and heap.
 * DO NOT USE THE STACK IN THIS FUNCTION!
 */
noreturn void _start(struct sargs_ps2link *pargs)
{
    asm volatile(
        "# Save first argument  \n"
        "la     $2, pargs_ps2link \n"
        "sw     $4, ($2)        \n"
        "                       \n"
        "# SetupThread          \n"
        "la     $4, _gp         \n"
        "la     $5, _stack      \n"
        "la     $6, _stack_size \n"
        "la     $7, _args       \n"
        "la     $8, ExitThread  \n"
        "move   $gp, $4         \n"
        "addiu  $3, $0, 60      \n"
        "syscall                \n"
        "move   $sp, $2         \n"
        "                       \n"
        "# Jump to _start2      \n"
        "j      _start2         \n");
    
    // Prevent gcc noreturn warning
    while(1){}
}

/*
 * Second start function, stack can be used as normal.
 */
noreturn void _start2()
{
    int retval;
    struct sargs *pa;

    // clear bss area
    memset(&_fbss, 0, &_end - &_fbss);

    // initialize heap
    SetupHeap(&_end, (int)&_heap_size);

    // writeback data cache
    FlushCache(0);

    // Capability to override 32MiB on DESR/DVR models
    // NOTE: this call can restart the application
    if (_ps2sdk_memory_init)
        _ps2sdk_memory_init();
    
    // Use ps2link arguments if loaded by ps2link
    pa = &_args;
    if (_args.argc == 0 && pargs_ps2link != NULL && pargs_ps2link->args.argc != 0)
        pa = &pargs_ps2link->args;

    // call ps2sdk argument parsing (weak)
    if (_ps2sdk_args_parse)
        _ps2sdk_args_parse(pa->argc, pa->argv);

    // initialize ps2sdk libc (weak)
    if (_ps2sdk_libc_init)
        _ps2sdk_libc_init();

    // call global constructors (weak)
    if (_init)
        _init();

    // Initialize the kernel (Apply necessary patches).
    _InitSys();

    // Enable interruts
    EI();

    // call main
    retval = main(pa->argc, pa->argv);

    // call global destructors (weak)
    if (_fini)
        _fini();

    // uninitialize ps2sdk libc (weak)
    if (_ps2sdk_libc_deinit)
        _ps2sdk_libc_deinit();

    // Exit
    Exit(retval);
}
