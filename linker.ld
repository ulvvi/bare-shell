ENTRY(_start)

SECTIONS
{
    . = 0x40000000;
    
    .text : { 
        *(.text) 
    }

    .rodata : {
        *(.rodata*)
    }
    
    .data : { 
        *(.data) 
    }
    
    . = ALIGN(8);
    
    __bss_start = .;

    .bss : { 
        *(.bss*) 
    }

    __bss_end = .;
}