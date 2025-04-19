#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "shell.h"
#include <stdint.h>

void kernel_main() {
    isr_install();
    irq_install();

    asm("int $2");
    asm("int $3");

    kprint("Welcome to OS Shell!\n");
    kprint("Available commands:\n");
    kprint("ls - List directory contents\n");
    kprint("mkdir <dirname> - Create a new directory\n");
    kprint("mv <source> <dest> - Move or rename a file/directory\n");
    kprint("rename <oldname> <newname> - Rename a file/directory\n");
    kprint("touch <filename> - Create a new file\n");
    kprint("rm <name> - Remove a file or directory\n");
    kprint("clear - Clear the screen\n");
    kprint("help - Show this help message\n");
    kprint("quit - Exit the shell\n");
    kprint("> ");
}

void user_input(char *input) {
    if (strcmp(input, "quit") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else {
        shell_execute(input);
    }
    kprint("> ");
}
