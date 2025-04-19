# Custom Operating System

This is a simple operating system project that demonstrates basic OS concepts.

## Prerequisites

Before building this OS, you need to install the following dependencies:

### For macOS (using Homebrew):
```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew install i386-elf-gcc
brew install i386-elf-binutils
brew install nasm
brew install qemu
```

### For Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential nasm qemu-system-x86
# Install i386-elf toolchain
sudo apt-get install gcc-multilib
```

### For Arch Linux:
```bash
sudo pacman -S nasm qemu
# Install i386-elf toolchain
sudo pacman -S gcc-multilib
```

## Building the OS

1. Clone this repository:
```bash
git clone https://github.com/yourusername/os_project.git
cd os_project
```

2. Build the OS:
```bash
make
```

3. Run the OS in QEMU:
```bash
make run
```

4. For debugging:
```bash
make debug
```

## Project Structure

- `boot/` - Bootloader and kernel entry code
- `kernel/` - Kernel source code
- `drivers/` - Device drivers
- `cpu/` - CPU-specific code
- `libc/` - Basic C library implementation

## Makefile Targets

- `make` - Build the OS
- `make run` - Run the OS in QEMU
- `make debug` - Run the OS in QEMU with GDB debugging
- `make clean` - Clean build artifacts

## Troubleshooting

If you encounter the error "i386-elf-gcc: No such file or directory", make sure you have installed the i386-elf toolchain as described in the Prerequisites section.

## License

[Your chosen license]

*Concepts you may want to Google beforehand: freestanding, uint32_t, size_t*

**Goal: Fix miscellaneous issues with our code**

The OSDev wiki has a section [which describes some issues with
JamesM's tutorial](http://wiki.osdev.org/James_Molloy%27s_Tutorial_Known_Bugs).
Since we followed his tutorial for lessons 18-22 (interrupts through malloc), we'll
need to make sure we fix any of the issues before moving on.

1. Wrong CFLAGS
---------------

We add  `-ffreestanding` when compiling `.o` files, which includes `kernel_entry.o` and thus
`kernel.bin` and `os-image.bin`.

Before, we disabled libgcc (not libc) through the use of `-nostdlib` and we didn't re-enable
it for linking. Since this is tricky, we'll delete `-nostdlib`

`-nostdinc` was also passed to gcc, but we will need it for step 3, so let's delete it.


2. kernel.c `main()` function
-----------------------------

Modify `kernel/kernel.c` and change `main()` to `kernel_main()` since gcc recognizes "main" as 
a special keyword and we don't want to mess with that.

Change `boot/kernel_entry.asm` to point to the new name accordingly.

To fix the `i386-elf-ld: warning: cannot find entry symbol _start; defaulting to 0000000000001000`
warning message, add a `global _start;` and define the `_start:` label in `boot/kernel_entry.asm`.


3. Reinvented datatypes
-----------------------

It looks like it was a bad idea to define non-standard data types like `u32` and such, since
C99 introduces standard fixed-width data types like `uint32_t`

We need to include `<stdint.h>` which works even in `-ffreestanding` (but requires stdlibs)
and use those data types instead of our own, then delete them on `type.h`

We also delete the underscores around `__asm__` and `__volatile__` since they aren't needed.


4. Improperly aligned `kmalloc`
-------------------------------

First, since `kmalloc` uses a size parameter, we'll use the correct data type `size_t` instead
of `u32int_t`. `size_t` should be used for all parameters which "count" stuff and cannot be
negative. Include `<stddef.h>`. 

We will fix our `kmalloc` in the future, making it a proper memory manager and aligning data types.
For now, it will always return a new page-aligned memory block.


5. Missing functions
--------------------

We will implement the missing `mem*` functions in following lessons


6. Interrupt handlers
---------------------
`cli` is redundant, since we already established on the IDT entries if interrupts
are enabled within a handler using the `idt_gate_t` flags.

`sti` is also redundant, as `iret` loads the eflags value from the stack, which contains a 
bit telling whether interrupts are on or off.
In other words the interrupt handler automatically restores interrupts whether or not 
interrupts were enabled before this interrupt

On `cpu/isr.h`, `struct registers_t` has a couple issues. 
First, the alleged `esp` is renamed to `useless`.
The value is useless because it has to do with the current stack context, not what was interrupted.
Then, we rename `useresp` to `esp`

We add `cld` just before `call isr_handler` on `cpu/interrupt.asm` as suggested
by the osdev wiki.

There is a final, important issue with `cpu/interrupt.asm`. The common stubs create an instance
of `struct registers` on the stack and then call the C handler. But that breaks the ABI, since
the stack belongs to the called function and they may change them as they please. It is needed
to pass the struct as a pointer.

To achieve this, edit `cpu/isr.h` and `cpu/isr.c` and change `registers_t r` into `registers_t *t`,
then, instead of accessing the fields of the struct via `.`, access the fields of the pointer via `->`.
Finally, in `cpu/interrupt.asm`, and add a `push esp` before calling both `isr_handler` and
`irq_handler` -- remember to also `pop eax` to clear the pointer afterwards.

Both current callbacks, the timer and the keyboard, also need to be changed to use a pointer to
`registers_t`.
