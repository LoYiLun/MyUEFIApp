## Overview
    This is a basic application displays the infomation of the pci device. (lspci Command Simulation Practice)
## Prerequisites

- Clone the TianoCore EDK2 repository.
- Required dependencies such as NASM, ACPI Source Language (ASL), Python, etc.
## Building

1. **Initialize Environment:**
   - Open the command prompt (CMD) and navigate to the root directory of the EDK2 project.
   - Execute the command to set up the environment for the first time:
     ```
     edksetup.bat rebuild
     ```

2. **Build AlePpkg:**
   - Once the environment setup is complete, use the command to build the AlenPkg:
     ```
     build -a X64 -p AlenPkg\AlenPkg.dsc
     ```

## Simulation

**QEMU Simulation:**
   - Utilize the QEMU emulator to simulate the EFI file from the build directory and observe the results.
     ```
     cd qemu dir
     qemu-system-x86_64.exe -bios "OVMF.fd" -hda fat:rw:C:\"yourDir" -net none
     ```
## Result
    Input ListPciDev will show the result.
![image](https://github.com/LoYiLun/MyUEFIApp/blob/main/result.png?raw=true)

