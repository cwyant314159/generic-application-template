# Template Project 

This repos is used as the basis for my C/C++ projects. It currently targets a
desktop environment, but with minimal tweaking, it can target embedded devices
as well.

The Makefile is roughly split into two parts. The first part contains the
variable assignments and source file discovery. All Make targets can be found
in the second part. The remainder of this README will describe each part in
more detail and discuss any quirks or limitations with this template.

## TL;DR Limitations and Quirks

- The Makefile uses the `MKDIR` variable assuming that the `-p` option or some
equivalent is present. Heavy modifications will be required if this Makefile 
needs to be used on Windows.


## Part 1

The first part of the Makefile contains all the variable assignments for tools,
source files, compiler flags, and any other variables required by Make targets.
Most variables are assigned using the colon equals (`:=`) which forces variable
expansion to happen at assignment. There are a few places where variables are
assigned using equals (`=`) for deferred variable expansion. These instants
are discuessed in later sections. The following sections will go over the 
logical groupings of variables in the Makefile.

### Tool Variables

The tool variables are defined first since these will most likely be the thing
to change when trying to target a different platform. The list of tools
variables is as follows:

- `CC`
- `CXX`
- `LD`
- `AR`
- `RM`
- `MKDIR`

The `CC` and `CXX` variables define the C and C++ compiler (think of X as a
slanty plus). Both variables are given the `-c` option to signify a compile
only (instead of a compiler and link) step. The `-x` option specifies the
language of the compile. These options could have been added to the `CFLAGS`
and `CXXFLAGS` variables, but I like having them here since they kind of define
the compiler.

The `LD` variable defines the linker tool used to make the final binary.

The `AR` variable defines the archiver tool that is used to make static library
files (lib\*.a). Although not used in the compilation of an application it is
defined here so that it could potentially be passed to sub-Makefiles.

Both `RM` and `MKDIR` define the bash commands to remove files and make
directories. The `RM` variable has the `-r` and `-f` flags given since those
flags are almost always used when removing files in a Makefile. `MKDIR` is
given the `-p` option to simplify the creation of directories during
compilation. This Makefile heavily relies on this flag or something equivalent.
For platforms like Windows, heavy modifications would be required to get this
exact Makefile to work as intended. A possible solution would be to make a
script that replicates the functionality of `mkdir -p`.

### Binary Variables

This is a Makefile based template. It recursively searches the `src` directory
for both `*.c` and `*.cpp` files. Object files have the same path as source
but are rooted under the directory contained in the Make variable `$(OBJ_DIR)`.

For the local include path, the only `-I` option is the `include` directory
(i.e. `-Iinclude` is the only option passed to gcc). This is done to force the
convention of fully specifying paths to `#includes`. For example, if you had a
header called `my_funcs.h` in the directory `include/app/modules/`, the 
`#include` statement would look like this:

```c
#include "app/modules/my_funcs.h"
```

This is my prefered style, and I think it is a good convention to follow. It
allows you to have header files that have the same name but reside in different
directories. This is very useful for embedded development where multiple driver
modules might have a `hardware_regs.h` header.

One of the drawbacks to this project template is that it cannot support having
a C file named the same as a C++ file __in the same directory__. This is a
limitation caused by how the object files are named.
