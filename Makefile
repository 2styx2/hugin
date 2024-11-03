gcc = ~/opt/cross/bin/i686-elf-gcc
ld = ~/opt/cross/bin/i686-elf-gcc
flags = -c -ffreestanding -Wall -Wextra

all: kernel boot assemble

kernel:
	$(gcc) $(flags) src/kernel.c -o build/kernel.o

 
boot:
	nasm -felf32 src/boot.asm -o build/boot.o

assemble:
	$(ld) -T linker.ld -o hugin.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc
	mv hugin.bin src/isodir/boot
	grub-mkrescue -o build/hugin.iso src/isodir
	qemu-system-i386 -cdrom build/hugin.iso

