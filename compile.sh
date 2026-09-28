aarch64-linux-gnu-gcc -ffreestanding -nostdlib -c boot.s -o boot.o
aarch64-linux-gnu-gcc -ffreestanding -nostdlib -fno-pie -fno-stack-protector -c main.c -o main.o
aarch64-linux-gnu-ld -T linker.ld -o kernel.elf boot.o main.o
qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -semihosting -kernel kernel.elf