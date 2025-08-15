# KernelLab

A **bare-metal kernel development project** exploring low-level OS concepts including bootloader, VGA text mode, keyboard input, and mouse handling. Built from scratch using **C** and **NASM** for educational purposes.

---

## Features

- **Bootloader** in NASM to load kernel
- **32-bit kernel** written in C
- **VGA text mode** printing
- **Keyboard input** (polling mode)
- **PS/2 mouse handling** (planned / basic)
- Minimalistic, bare-metal OS for learning purposes

---

## Prerequisites

- **Ubuntu / Linux** system
- **QEMU** (emulator)
- **NASM** (Netwide Assembler)
- **i686 cross-compiler** for 32-bit kernel

Install dependencies:

```bash
sudo apt update
sudo apt install qemu qemu-system-i386 nasm gcc-multilib
