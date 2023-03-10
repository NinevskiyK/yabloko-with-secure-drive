asm(".asciz \"kernel start\\n\"");

#include "console.h"
#include "cpu/isr.h"
#include "cpu/gdt.h"
#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "drivers/ata.h"
#include "drivers/misc.h"
#include "drivers/uart.h"
#include "fs/fs.h"
#include "lib/string.h"
#include "proc.h"
#include "lib/errno.h"

void init_encryption_key() {
    printk("Please, enter the encryption key, and then press enter: ");
    while (1) {
        if (kbd_buf_size > 0 && kbd_buf[kbd_buf_size-1] == '\n') {
            kbd_buf[kbd_buf_size-1] = '\0';
            m_errno = 0;
            volatile long n = strtoi(kbd_buf); // volatile - to sync every time 'n' changes
            memset(kbd_buf, 0, kbd_buf_size);
            if (m_errno) {
                n = 0; // clear key
                if (m_errno == NAN_ERR) {
                    panic("Not a number!");
                } else {
                    panic("Key is too big!");
                }
            }
            kbd_buf_size = 0;
            asm ("mov %%eax, %%cr3\n\t"
                "xor %%eax, %%eax" : : "a"(n));
            n = 0; // clear key
            return;
        }
        asm("hlt");
    }
}

void _start() {
    load_gdt();
    init_keyboard();
    uartinit();
    load_idt();
    sti();

    vga_clear_screen();
    init_encryption_key();
    printk("YABLOKO\n");

    printk("\n> ");
    while (1) {
        if (kbd_buf_size > 0 && kbd_buf[kbd_buf_size-1] == '\n') {
            if (!strncmp("halt\n", kbd_buf, kbd_buf_size)) {
                qemu_shutdown();
            } else if (!strncmp("run ", kbd_buf, 4)) {
                kbd_buf[kbd_buf_size-1] = '\0';
                const char* cmd = kbd_buf + 4;
                run_elf(cmd);
            } else {
                printk("unknown command, try: halt | run CMD");
            }
            kbd_buf_size = 0;
            printk("\n> ");
        }
        asm("hlt");
    }
}
