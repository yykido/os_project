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

No Lincese