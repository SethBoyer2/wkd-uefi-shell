# Overview
A freestanding x86-64 UEFI application written in C, designed to run without an operating system.

## Currently implemented functionality
Console input & output, command handling and custom string utilities

## Development Environment
This project was created using MinGW for compilation and linking, OVMF and QEMU to allow UEFI booting and certain GNU-EFI components for active development. Most of my development efforts are done through Ubuntu using WSL, 
along with a native Fedora installation on a separate device.

## Building & Running 
I generally just followed OSDev's UEFI App bare bones tutorial (wiki.osdev.org/UEFI_App_Bare_Bones) for the current build process, with some modifications,
including a MakeFile that runs the compilation and linking commands to save time during testing. I intend on providing further information 
& alternate tools and build processes when I'm more well-versed in the toolchain.

Overall though, emulation is done using QEMU and OVMF, GNU-EFI provides headers and a solid library, Mtools is required to create and manipulate the FAT 
image used in the build, and MinGW-w64 is used for compilation and linking.

It's also worth mentioning that I'm working with a modified data.c file, in which I made all of the members of the LibStubUnicodeInterface structure NULL 
to avoid bloat and remove dependencies on unused Unicode helper functions, as per the OSdev documentation (You will need to re-enable these if you intend to use them)

## Planned features
This project is intended to be used in a repurposed vintage serial data transfer switch, so I eventually intend on mapping the physical 4-way rotary switch inputs through a 
microcontroller to interact with portions of the application. I'm planning on mapping each switch position to different keys and have each 'keypress' swap the UEFI shell to different application modes, 
like the default shell, a system information screen, and a collection of mini-games that will touch on context-dependent input mapping, with the rotary inputs being used as controls for each game.

I also intend on expanding the shell itself to include file I/O and a simple text editor.
