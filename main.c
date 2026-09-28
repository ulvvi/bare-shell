#include <stdint.h>

#define UART0_BASE 0x09000000
#define UART0_DR (*(volatile unsigned int *)(UART0_BASE + 0x00)) //data register
#define UART0_FR (*(volatile unsigned int *)(UART0_BASE + 0x18)) //flag register

//flag register bits
#define UART_FR_TXFF (1 << 5) //transmit FIFO full
#define UART_FR_RXFE (1 << 4) //transmit FIFO empty


char uart_getc(void){

    while(UART0_FR & UART_FR_RXFE){
        //wait, stack has nothing
    }
    //bitmask to return lowest bits
    return (char)(UART0_DR & 0xFF);
}


void uart_putc(char c) {
    while (UART0_FR & UART_FR_TXFF) {
        //do nothing, just wait
    }
    //write the character to the data register
    UART0_DR = (uint32_t)c;
}

void uart_puts(const char *s) {
    while(*s != '\0'){
        if(*s == '\n'){
            uart_putc('\r');
        }
        uart_putc(*s);
        s++;
    }
}

void uart_print_hex(uint64_t val){
    const char hex_chars[] = "0123456789ABCDEF";

    uart_puts("0x");

    for(int i = 60; i >= 0; i -=4){
        uint8_t digit = (val >> i) & 0xF;
        uart_putc(hex_chars[digit]);
    }

}

void check_dtb_adress(uint64_t dtb_address){
    uint32_t magic = *(volatile uint32_t *)dtb_address;

    uart_puts("Raw Magic Read: ");
    uart_print_hex(magic);
    uart_puts("\n");

    if (magic == 0xEDFE0DD0) {
        uart_puts("DTB Magic: VALID (0xD00DFEED)\n");
    } else {
        uart_puts("DTB Magic: INVALID\n");
    }
}

extern uint64_t getStackPointer(void);

static inline uint64_t get_sp(void) {
    return getStackPointer();
}

void print_stack(void){
    uart_puts("Stack pointer at address: ");
    uart_print_hex(get_sp());
    uart_puts("\n");
}

int str_equals(const char *s1, const char *s2){
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2) {
            return 0;
        }
        s1++;
        s2++;
    }
    return (*s1 == *s2);
}

//random variable uninitialized
int boot_count;

extern void cpu_hang(void);
extern void shutdown(void);

char cmd_buffer[64];
int cmd_index;

void main(uint64_t dtb_address){

    uart_puts("hello world\n");

    //bss clearing works
    if(boot_count == 0){
        uart_puts("BSS OK\n");
    }

    uart_puts("> ");
    cmd_index = 0;
    while(1){
        char c = uart_getc();

        switch(c){
            case '\r':
                uart_puts("\r\n");
                cmd_buffer[cmd_index] = '\0';
                
                if(str_equals(cmd_buffer, "help")) {
                    uart_puts("Avaliable commands:\n");
                    uart_puts("  help  - Show this menu\n");
                    uart_puts("  dtb   - Inspect Device Tree\n");
                    uart_puts("  stack - Print current stack pointer\n");
                    uart_puts("  exit - turns off machine\n");
                }
                else if (str_equals(cmd_buffer, "dtb")) {
                    uart_puts("DTB address is: ");
                    uart_print_hex(dtb_address);
                    uart_puts("\n");
                    check_dtb_adress(dtb_address);
                }
                 
                else if (str_equals(cmd_buffer, "stack")) {
                    print_stack();
                }
                else if(str_equals (cmd_buffer, "exit")) {
                    uart_puts("Shutting down...\n");
                    shutdown();
                }

                else if (cmd_index > 0) {
                    uart_puts("Unknown command: ");
                    uart_puts(cmd_buffer);
                    uart_puts("\n");
                }

                cmd_index = 0;
                uart_puts("> ");

                break;

            case 127:
                if(cmd_index > 0){
                    cmd_index--;
                    uart_puts("\b \b");
                }
                break;

            default:
                if(cmd_index < 63){
                    cmd_buffer[cmd_index] = c; 
                    cmd_index++;
                    uart_putc(c);
                }
                break;
        }
    }

    cpu_hang();
}