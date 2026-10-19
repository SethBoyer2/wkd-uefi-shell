
CC = x86_64-w64-mingw32-gcc

INCLUDES = -I $(HOME)/gnu-efi/inc \
           -I $(HOME)/gnu-efi/inc/x86_64 \
           -I $(HOME)/gnu-efi/inc/protocol

CFLAGS = -ffreestanding $(INCLUDES)

LDFLAGS = -nostdlib -Wl,-dll -shared \
          -Wl,--subsystem,10 -e efi_main

all: BOOTX64.EFI

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

data.o: data.c
	$(CC) $(CFLAGS) -c -o data.o data.c

BOOTX64.EFI: main.o data.o
	$(CC) $(LDFLAGS) -o BOOTX64.EFI main.o data.o

clean:
	rm -f main.o data.o BOOTX64.EFI

.PHONY: all clean
