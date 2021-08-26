   .extern   _heap_size
   .extern   _stack
   .extern   _stack_size

   .extern _start

   .set   noat
   .set   noreorder

   .text
   .align   2

#
# __start function
# sets up the minimum needed for going to 'c' code
#
   .globl  __start
   .ent    __start
__start:

zerobss:
   # clear bss area

   la   $2, _fbss
   la   $3, _end

1:
   sltu   $1, $2, $3
   beq   $1, $0, 2f
   nop
   sq   $0, ($2)
   addiu   $2, $2, 16
   j   1b
   nop
2:

setupthread:
   # setup current thread

   la   $4, _gp
   la   $5, _stack
   la   $6, _stack_size
   la   $7, _args
   la   $8, _root
   move   $gp, $4
   addiu   $3, $0, 60
   syscall         # SetupThread(_gp, _stack, _stack_size, _args, _root)
   move   $sp, $2

   # initialize heap

   la   $4, _end
   la   $5, _heap_size
   addiu   $3, $0, 61
   syscall         # SetupHeap(_end, _heap_size)

   j _start
   nop
   .end   __start

#
# _root function
# wrapper for calling ExitThread
#
   .ent   _root
_root:
   addiu   $3, $0, 35
   syscall         # ExitThread() (noreturn)
   .end   _root

#
# Data
#
   .bss
   .align   6
   .globl _args
_args:
   .space   4+16*4+256   # argc, 16 arguments, 256 bytes payload