# Template ATmega328P Application

This project serves as a template for Makefle based projects targeting the
ATmega328P microcontroller. The template implements the ubiquitous `blinky`
application. The best way to use the executable from this template is to flash
the `blinky.hex` file to an Arduino UNO with an ICSP.

At this time, there is no GDB debug support. AVR microcontrollers use debugWire
for single-step debugging. This requires programming fuses into a configuration
that disables the ICSP. I have not found a way to automate the set/reset of 
these fuse settings in a reliable way. If you have a solution or some thoughts,
please submit a pull request with an example.

## Repo Layout

The root of the repository contains this README, the project `Makefile`, the
project `.gitignore`, and five directories:

- `app`
- `scripts`

The `app` directory contains the main application source code, startup files,
and linker script. Any source code that cannot be used for other projects
should be placed in this directory. For a well structured application, this
directory would ideally only contain the `main.cpp`, startup code, and any
library/dependency specializations. Source code for reusable modules and
libraries should go in the `dependencies` directory.

Utility scripts for programming and debugging can be found in the `scripts`
directory. Each script has its own section later in the README.

After building the application, the final binary can be found in `bin`.
Compiled object files are located in the `obj` directory. Both of these
directories have `.gitignore` filters.

## Make Targets

The provided Makefile includes targets for compiling C, C++, and assembly
( `.s` ) source files as well as a link target for the elf file. The default
target generates the application's elf file.

Other targets include the `clean`, binary `flash`, and `dump` targets. The
`flash` target uses a helper script in the `scripts` directory to flash the
application elf to the board. The `dump` target pretty prints various
Makefile variables that are helpful when debugging Makefile issues.

## Hardware

Besides the ATmega328P and supporting circuitry, this project utilizes an
[Atmel-ICE](https://www.microchip.com/en-us/development-tool/ATATMEL-ICE)
programmer. These can be purchased from various vendors on the internet for.

It is not required, but an Arduino UNO can be used as a target. The
[Arduino UNO](https://store-usa.arduino.cc/products/arduino-uno-rev3)
has the ATmega328P with supporting circuitry.

## Software

This project uses `Make` and the `avr-gcc` cross compiler tools provided by the
host system package manager. If a particular install of the cross compiler
tools is required, the Make variables at the top of the Makefile can be changed
to use the appropriate tool.

Below is a table of other tools required to fully utilize the Makefile and
scripts.

| Tool    | Usage           | Download        |
|---------|-----------------|-----------------|
| avrdude | flash utility   | package manager |
| bear    | intellisense DB | package manager |

## Scripts

### **flash.sh**

`flash.sh` is a wrapper around the `avrdude` programmer application. The script
requires an Intel hex file as its only parameter.

### **gen_compile_commands.sh**

For VSCode users, the `gen_compile_commands.sh` script can generate a
`compile_commands.json` file for C++ intellisense. The script depends on the
`bear` utility which can be downloaded using the host system's package manager.
The script must do a full rebuild (i.e `make clean all`) when generating the
JSON file.
