# Yet Another BootLoader, OS Kernel and Other stuff

## Forked from
* https://github.com/hse-cs-ami/yabloko-public

## Updates
* Add functions to read files from encrypted disk to RAM 'on-the-fly'

### How does it work?
* I use [ECB](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_codebook_(ECB)) mode and [XOR cipher](https://en.wikipedia.org/wiki/XOR_cipher) to encrypt the drive
* You can find the `IV` and `ENC_KEY` constants on the beggining of the Makefile
* When user boot OS, he enters the key which is stored in `cr3` register
* When user wants to run a program, the code of program decrypts using the key entered by the user. If the key right, the program runs, otherwise it's not.
* NOTE: Only ELF files are encripted - the directory structure and kernel code aren't ecnrypted

## Quickstart:
```
$ ./setup.sh
$ make
```
## How to run using llvm

You can use this way even if you have windows. You need to install llvm and qemu. 
Check that executables `clang`, `ld.lld`, `qemu-system-i386` available from your terminal/console.

```
make LLVM=on
```

## How to debug in my favorite IDE

Start debug server using command `make debug-server` or `make debug-server-nox` if you don't want to see gui, and 
then connect using remote gdb option to localhost:1234 (symbols file is kernel.bin)

## Includes code from:
* https://github.com/mit-pdos/xv6-public
* https://github.com/FRosner/FrOS
* https://github.com/dhavalhirdhav/LearnOS
* https://wiki.osdev.org
