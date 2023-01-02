# Template STM32F BluePill Application

This project serves as a template for Makefle based projects targeting the
STM32 BluePill board. With Makefile (and potentially source code) modifications,
this template can be used to target any ARM Cortex-M microcontroller.

The template implements the ubiquitous `blinky` application that can be
compiled with or without the STM32 HAL library. By default the Makefile
compiles the application without the HAL. To use the HAL, the `USE_HAL`
Make variable must be set to 1.

```bash
USE_HAL=1 make all
```

The non-HAL version of the application uses C++ code with questionable choices
of language constructs. Design decisions were made with the goal of utilizing
as many potentially useful C++ constructs as possible.

## Repo Layout

The root of the repository contains this README, the project `Makefile`, the
project `.gitignore`, and five directories:

- `.vscode`
- `app`
- `dependencies`
- `doc`
- `scripts`

`.vscode` is provided for VSCode users to allow editor configuration tracking
in Git. This folder is also where the `gen_compile_commands.sh` script will
place the `compile_commands.json` file.

The `app` directory contains the main application source code, startup files,
and linker script. Any source code that cannot be used for other projects
should be placed in this directory. For a well structured application, this
directory would ideally only contain the `main.cpp`, startup code, and any
library/dependency specializations. Source code for reusable modules and
libraries should go in the `dependencies` directory.

`doc` contains copies of datasheets and reference manuals used for development.
The directory currently contains reference manuals and errata for the
BluePill's microcontroller.

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

## Dependencies

This project depends on a stripped down copy of the
[STM32CubeF1](https://github.com/STMicroelectronics/STM32CubeF1) repository.
The STM32CubeF1 dependency directory contains the complete STM32CubeIDE HAL and
a copy of the CMSIS specialized for the STM32F1xx processors. Templates for
various features of the HAL can be found in the `hal-templates` directory of
the dependency.

## Hardware

Besides the STM32 BluePill, this project utilizes an
[STLinkV2](https://www.amazon.com/HiLetgo-Emulator-Downloader-Programmer-STM32F103C8T6/dp/B07SQV6VLZ/ref=sr_1_3?keywords=st-link+v2&qid=1668131513&sr=8-3)
programmer. These can be purchased from various vendors on the internet for.
The average cost of the programmer is about $10USD. Use caution when purchasing
the programmer. There are scams (even on Amazon) attempting to sell multiple
programmers for less than the average cost. Other programmers can also be used,
but the utility scripts will need to be modified for the new programmer.

It is not required, but a decent sized breadboard makes circuit prototyping and
troubleshooting substantially easier. Brand and size don't really matter as
long as the breadboard has plenty of space for the application. Jumpers or wire
will be required to make connections. AWG 22 works best for the breadboards
listed below.

- [small](https://www.amazon.com/DEYUE-breadboard-Set-Prototype-Board/dp/B07LFD4LT6/ref=sr_1_5?keywords=breadboard&qid=1668135739&sr=8-5)
- [medium](https://www.amazon.com/EL-CP-003-Breadboard-Solderless-Distribution-Connecting/dp/B01EV6LJ7G/ref=sr_1_12?keywords=breadboard&qid=1668135739&sr=8-12)
- [large](https://www.amazon.com/Makeronics-3220-Point-Solderless-Breadboard/dp/B07D5VN89C/ref=sr_1_10?keywords=breadboard&qid=1668135739&sr=8-10)

## Software

This project uses `Make` and the `arm-none-eabi-gcc` cross compiler tools
provided by the host system package manager. If a particular install of the
cross compiler tools is required, the Make variables at the top of the
Makefile can be changed to use the appropriate tool. It is assumed that
`arm-none-eabi-gdb` or similar debugger is provided with the cross compiler
tools.

Below is a table of other tools required to fully utilize the Makefile and
scripts.

| Tool                 | Usage           | Download        |
|----------------------|-----------------|-----------------|
| openocd              | GDB server      | package manager |
| STM32_Programmer_CLI | flash utility   | [ST Website](https://www.st.com/en/development-tools/stm32cubeprog.html) |
| bear                 | intellisense DB | package manager |

## Scripts

### **flash.sh**

`flash.sh` is a wrapper around the `STM32_Programmer_CLI` programmer
application provided as part of the STM32CubeProgrammer package from ST. The
script references the utility via the environment variable
`STM32CUBEPROGRAMMERROOT`. A binary file (either `.elf` or `.hex`) is the only
required argument of the script.

### **gen_compile_commands.sh**

For VSCode users, the `gen_compile_commands.sh` script can generate a
`compile_commands.json` file for C++ intellisense. The script depends on the
`bear` utility which can be downloaded using the host system's package manager.
By default, the script generates a JSON file for the non-HAL based project. To
generate a JSON file for HAL based projects, the `--hal` flag can be passed to
the script. The script must do a full rebuild (i.e `make clean all`) when
generating the JSON file.

### **gdb_debug.sh / gdb_server.sh**

Both `gdb_debug.sh` and `gdb_server.sh` are used to debug an application
(assuming all the necessary hardware is present). To debug an application,
connect the BluePill board to the host system via the STLink. Open a terminal
window and run:

```bash
./scripts/gdb_server.sh
```

Open another terminal window and run:
```
./scripts/gdb_debug.sh <binary file>
```

The `gdb_server.sh` script calls `openocd` with the command file `openocd.cfg`.
The command file sources the configuration for the STLink programmer and
STM32F1x chips. These configuration files are provided with the install of
`openocd`. Any arguments passed to `gdb_server.sh` are forwarded to `openocd`
after the command file.

The `gdb_debug.sh` script is a wrapper around `arm-none-eabi-gdb`. The wrapper
calls `arm-none-eabi-gdb` with the GDB command file `debug.gdb`. The command
file turns off confirmation requests and output pagination. The remote target
is assumed to be on port 3333 (`openocd` default). The remote monitor resets
and halts the processor. The final step of the file is to load the processor
with a binary file given on the command line. All arguments passed to
`gdb_debug.sh` are forwarded to `arm-none-eabi-gdb`.
