.global _start
.section .text
_start:
    //stores the extra info about the bootloader in x19, which is passed as an argument to the kernel
    mov x19, x0

    //setting up the stack pointer
    //QEMU RAM starts at 0x40000000, and the stack pointer is set to the end of RAM
    ldr x0, =0x48000000
    mov sp, x0

    ldr x0, =__bss_start
    ldr x1, =__bss_end

clear_bss_loop:
    cmp x0, x1
    b.ge bss_cleared
    str xzr, [x0], #8
    b clear_bss_loop

bss_cleared:
    mov x0, x19
    bl main

//fallback hang
hang:
    wfi    //waits till interrupt occurs   
    b .



//globals
.global cpu_hang
cpu_hang:
    wfi
    b cpu_hang

.global getStackPointer
getStackPointer:
    mov x0, sp
    ret

.global shutdown
shutdown:
    ldr     x2, =0x20026       
    sub     sp, sp, #16        
    str     w2, [sp, #0]       
    str     wzr, [sp, #4]      

    mov     x0, #0x18          
    mov     x1, sp             
    hlt     #0xf000           
