# Template Project 

This repos is used as the basis for my C/C++ projects. The template application
is a valid C/C++ program and can be build by simply running `make`. The template
application currently targets a desktop environment, but with minimal tweaking,
it can target embedded devices as well.

The Makefile is roughly split into two parts. The first part contains the
variable assignments and source file discovery. All Make targets can be found
in the second part. The remainder of this README will describe each part in
more detail and discuss any quirks or limitations with this template.

## TL;DR Limitations and Quirks

- The Makefile uses the `MKDIR` variable assuming that the `-p` option or some
equivalent is present. Heavy modifications will be required if this Makefile 
needs to be used on Windows.

- When compiling on Windows, the `BIN` variable must include the `.exe`
extension to prevent the link step from unnecessarily running.

- Include directories are not recursivley added to `INC_FLAGS`. 

## Part 1

The first part of the Makefile contains all the variable assignments for tools,
source files, compiler flags, and any other variables required by Make targets.
Most variables are assigned using the colon equals ( `:=` ) which forces
variable expansion to happen at assignment. There are a few places where
variables are assigned using equals ( `=` ) for deferred variable expansion.
These instants are discuessed in later sections. The following sections will go
over the logical groupings of variables in the Makefile.

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

Both `RM` and `MKDIR` define the shell commands to remove files and make
directories. The `RM` variable has the `-r` and `-f` flags given since those
flags are almost always used when removing files in a Makefile. `MKDIR` is
given the `-p` option to simplify the creation of directories during
compilation. This Makefile heavily relies on this flag or something equivalent.
For platforms like Windows, heavy modifications would be required to get this
exact Makefile to work as intended. A possible solution would be to make a
script that replicates the functionality of `mkdir -p`.

### Binary Variables

The variables `BIN_DIR` and `BIN` are simply used to define where to put the
compiled binary and what to call it. When using this Makefile on Windows, the
`.exe` __MUST__ be added to `BIN` when compiling for desktop applications. If
the extention is left off, the linking step will always run since compiles on
Windows will have `.exe` added if it is not present.

### Include Variables

The `INC_DIRS` variable is a space separated list of all the include directories
for the project. The first instance of `INC_DIRS` is an empty assignment. This
is done so that subsequent lines can use the addition assignment ( `+=` ) to add
to the list. There is nothing preventing the first include directory from being
assigned on the first instance of `INC_DIRS`. I just always make copy/paste
errors when adding a new include directory because I will copy that line instead
of a line that was the addition assignment ( `+=` ) operator.

The `INC_FLAGS` variable is a space separated list of the include directories
from `INC_DIRS` prepended with the compiler flag `-I`. This is accomplised by
using Make macros. The `foreach` macro takes three arguments: the loop counter
(think the `int i` in a C for loop), the thing to iterate over, and the action
to perfrom on each item of the iterable thing.

```
$(foreach <loop counter>, <iterable>, <action>)
$(foreach dir, $(INC_DIRS), <prepend -I action>)
```
To prepend the `-I` argument, the `addprefix` macro is used. The `addprefix`
macro takes two parameters: a prefix and a thing to add the prefix too.

```
$(addprefix <prefix>, <thing to prepend to>)
$(addprefix -I, $(dir))
```

The `dir` variable is surrounded with `$()` since it is a Make variable in use.
One quirk (feature) of this implementation is that include paths are not
recursively added (i.e. `-Iinclude` is the only option passed to gcc when
`INC_DIRS := include`). This is done to force the convention of fully specifying
paths to `#includes`. For example, if you had a header called `my_funcs.h` in
the directory `include/app/modules/`, the `#include` statement would look like
this:

```c
#include "app/modules/my_funcs.h"
```

This is my prefered style, and I think it is a good convention to follow. It
allows you to have header files that have the same name but reside in different
directories. This is very useful for embedded development where multiple driver
modules might have a `hardware_regs.h` header.

### Source Variables

`SRC_DIRS` is a space separated list of directories that contain the source
code of an application. Just like the `INC_DIRS` variable the first assignment
is blank and all subsequent assignments use the addition assignment operator
( `+=` ). The `SRC_FILES` variable is a generated spaces separated list of all
the C and C++ source files in the directories of `SRC_DIRS`. The list is
generated using a combination of Make macros and shell commands. Unlike the
generation of `INC_VARS`, the source file list is made by recursively searching
the directories of `SRC_DIRS`. To achieve this, the `foreach` macro is used to
iterate over the source directories, and the `find` command is used on each
directory to look for source files.

```
$(foreach <loop counter>, <iterable>, <action>)
$(foreach dir, $(SRC_DIRS), <find source files>)
```

To call the `find` command, the Make macro shell is used. 

```
$(shell <command>)
$(shell find $(dir) -type f -name '*.cpp")
```

The arguments to the `find` command specify that the object types to search for
are files (as opposed to files and directories). The name argument specifies a
wildcard search for anything with a .cpp extension. This particular command
recursively searches `$(dir)` are returns all C++ files relative to $(dir). To
find C files, the `-name '\*.cpp'` argument is changed to `-name '\*.c'`.

To better illustrate the output of the `find` command (and by extension the
`SRC_FILES`), assume the following directory heirarchy:

```
project
    include
    src
        module1
            foo.cpp
            bar.c
        module2
            fugazi.c
    Makefile
```

The `SRC_DIRS` variable would have the directory `src` assigned to it. After
running the macros and shell commands, the contents of `SRC_FILES` would be
equivalent to the following:

```
SRC_FILES := src/module1/foo.cpp src/module1/bar.c src/module2/fugazi.c 
```


### Object Variables

The `OBJ_DIR` variable is simply the directory where all compiled object files
will be placed during compilation. The `OBJS` variable contains a space
separated list of all the compiled object files. The variable is generated in
two steps.

The first step creates the list of object files from the source files by simply
appending the `.o` file extension to each item of the `SRC_FILES` variable. The
second step of the `OBJS` variable generation is simply a `foreach` macro that
prepends the `OBJ_DIR` to each object file generated from the first step. This
is accomplished using the same idiom as the `INC_FLAGS` variable.

```
$(foreach <loop counter>, <iterable>, <action>)
$(foreach obj, $(OBJS), <prepend -I action>)
$(addprefix <prefix>, <thing to prepend to>)
$(addprefix $(OBJ_DIR)/, $(obj))
```

The `obj` variable is surrounded by `$()` since it is a Make variable in use.
Also note the `/` added to the `OBJ_DIR` variable. If this slash is left off
the substition would look like this: `objsrc/module/foo.c`.

### Compiler Flag Variables

<!-- TODO -->

### Other Variables

<!-- TODO -->

## Part Two

The second part of the Makefile contains the targets and recipes to do the
actual build. The following sections will describe each target in detail.

### All Target

<!-- TODO -->

### Linker Target

<!-- TODO -->

### C Compile Target

<!-- TODO -->

### C++ Compile Target

<!-- TODO -->

### Clean Target

<!-- TODO -->

### Dump Target

<!-- TODO -->


## Dep (.d) Files

<!-- TODO -->
