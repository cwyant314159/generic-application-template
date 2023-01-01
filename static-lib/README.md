# Template Static Library

The template static library is a valid C/C++ archive library and can be built
by simply running `make`. The template library currently targets a desktop
environment, but with minimal tweaking, it can target embedded devices as well.
The remainder of this README is a detailed discussion of the Makefile.

The Makefile is roughly split into two parts. The first part contains the
variable assignments and source file discovery. Part two contains the Make
targets.

## TL;DR Limitations and Quirks

- The Makefile uses the `MKDIR` variable assuming that the `-p` option or some
equivalent is present. Heavy modifications will be required if this Makefile
needs to be used on a platform without this feature.

- Include directories are not recursivley added to `INC_FLAGS`.

## Part 1

The first part of the Makefile contains all the variable assignments for tools,
source files, compiler flags, and any other variables required by Make targets.
Most variables are assigned using the colon equals ( `:=` ) which forces
variable expansion to happen at assignment. If a is assigned using equals
( `=` ) for deferred variable expansion, an explantion will be provided. The
following sections will go over the logical groupings of variables in the
Makefile.

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
files ( `lib.a` ). Although not used in the compilation of an application it is
defined here so that it could potentially be passed to sub-Makefiles.

Both `RM` and `MKDIR` define the shell commands to remove files and create
directories. The `RM` variable has the `-r` and `-f` flags given since those
flags are almost always used when removing files in a Makefile. `MKDIR` is
given the `-p` option to simplify the creation of directories during
compilation. This Makefile heavily relies on this flag or something equivalent.
For platforms without this feature, heavy modifications would be required to
get this exact Makefile to work as intended. A possible solution would be to
make a script that replicates the functionality of `mkdir -p`.

### Library Variables

The variables `LIB_DIR`, `LIB_NAME`, and `LIB` are simply used to define where
to put the compiled static archive and what to call it. The `LIB_NAME` variable
is the name of the library without the standard GCC static library prefix `lib`
and extension `.a`. The `LIB` variable contains the full name of the archive
file ( `libfoo.a` ).

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
$(foreach dir, $(INC_DIRS), <prepend action>)
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
( `+=` ).

The `SRC_FILES` variable is a space separated list of all the C and C++ source
files in the directories of `SRC_DIRS`. The list is generated using a
combination of Make macros and shell commands. Unlike the generation of
`INC_FLAGS`, the source file list is made by recursively searching the
directories of `SRC_DIRS`. To achieve this, the `foreach` macro is used to
iterate over the source directories, and the `find` command is used on each
directory to look for source files.

```
$(foreach <loop counter>, <iterable>, <action>)
$(foreach dir, $(SRC_DIRS), <find source files>)
```

To call the `find` command, the Make macro `shell` is used.

```
$(shell <command>)
$(shell find $(dir) -type f -name '*.cpp")
```

The arguments to the `find` command specify that the object types to search for
are files (as opposed to files and directories). The name argument specifies a
wildcard search for anything with a `.cpp` extension. This particular command
recursively searches `$(dir)` are returns all C++ files relative to `$(dir)`.
To find C files, the `-name '\*.cpp'` argument is changed to `-name '\*.c'`.

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
separated list of all the compiled object files and is generated in
a two step process.

The first step creates the list of object files from the source files by simply
appending the `.o` file extension to each item of the `SRC_FILES` variable. The
second step is a `foreach` macro that prepends the `OBJ_DIR` to each object
file generated from the first step. This is accomplished using the same idiom
as the `INC_FLAGS` variable.

```
$(foreach <loop counter>, <iterable>, <action>)
$(foreach obj, $(OBJS), <prepend action>)
$(addprefix <prefix>, <thing to prepend to>)
$(addprefix $(OBJ_DIR)/, $(obj))
```

The `obj` variable is surrounded by `$()` since it is a Make variable in use.
Also note the `/` added to the `OBJ_DIR` variable. If this slash is left off
the substition would look like this: `objsrc/module/foo.c`.

### Compiler Flag Variables

There are several compiler flag variables that are used in both the compile
targets and linker target. Several build upon each other, but a couple are
standalone. The list of compiler flag variables is as follows:

- `WARN_FLAGS`
- `COMMON_FLAGS`
- `CFLAGS`
- `CXXFLAGS`
- `ARFLAGS`

The `WARN_FLAGS` variable is a space separated list of all the warning flags to
pass to both the C and C++ compiler. Similar to other variables in this Makefile
the first assignment is empty so that following assignments can use the addition
assignment operator ( `+=` ).

`COMMON_FLAGS` is a space separated list of all the compiler flags that are
passed to both the C and C++ compiler. Unlike other variables in this Makefile
the first assignment is not empty. The first assignment to `COMMON_FLAGS` is
the `WARN_FLAGS` variable described above. This is not done for any technical
reason. It is just done to save an assignment operation. Since these compiler
flags are given to both compilers the `INC_FLAGS` variable is also used here.

The `CFLAGS` variable is the C compiler specific flags. To save on typing
in the Makefile targets, `CFLAGS` is assigned the value of `COMMON_FLAGS`, and
the addition assignment operator ( `+=` ) is used to add the C compiler specific
flags.

The `CXXFLAGS` variable is the C++ compiler specific flags. To save on typing
in the Makefile targets, `CXXFLAGS` is assigned the value of `COMMON_FLAGS`,
and the addition assignment operator ( `+=` ) is used to add the C++ compiler
specific flags.

`ARFLAGS` contains all the flags that are passed to the GCC archiver utility.

### Other Variables

This section describes any other variables that do not fit into the sections
above.

The `DUMP_FMT` variable specifies the shell command to call on space separated
variables in the `dump` target. There are two main steps to this shell command.
Step one is replacing all the spaces with new line characters using the `tr`
command. Step two uses a two part `sed` command to (1) add four spaces in front
of all alphanumeric character or dashes '-' and (2) remove all empty lines. The
dump (debug) target is described in more detail in part two.

```
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'
```

## Part Two

The second part of the Makefile contains the targets and recipes to do the
actual build. The following sections will describe each target in detail.

### All Target

The first target (default target) is called `all`. When running Make without
any arguments this is the target that is executed. The target name `all` has no
real meaning. It is chosen to be in alignment with the conventions of other
Makefiles. For this Makefile, there are no actions specified and the dependent
is the compiled library file generated by the archive target.

### Archive Target

The archive target produces the final binary `$(LIB_DIR)/$(LIB)`. The archive
target depends on the compiled object files specified in the `OBJS` variable.
The first action of the archive target is creating the directory of the final
binary. This is accomplished using the `MKDIR` variable (that relies heavily on
the `-p` option) and the `dirname` shell command. The `BIN_DIR` variable could
have been used directly.

### Compile Target

The compile targets produce compiled object files from the source files
specified in the `SRC_FILES` variable. There are two compile targets in this
Makefile. There is one for C source compiles, and one for C++ source compiles.
Both targets use wildcard notation for matching object files to source files
with the `%` character. The first action in of both compile targets is creating
the directory of the compiled object file. This is accomplished using the
`MKDIR` variable and the `dirname` shell command. It is not possible to simply
use the `OBJ_DIR` variable here since the source file might have multiple parent
directories that must be accounted for under the `OBJ_DIR`. These potential
parent directories are the reason why this Makefile heavily relies on the `-p`
option for the `MKDIR` variable.

### Clean Target

The `clean` target removes all build generated files. It uses the `RM` variable,
and it assumes that the recursive ( `-r` ) and force ( `-f` ) flags are
already specified in the variable. This target has no dependencies.

### Dump Target

The `dump` target is mostly used for debugging the Makefile. It is a `.PHONY`
target (does not produce an output file). It's purpose is to output the
contents of the variables in the Makefile. This target utilizes the `DUMP_FMT`
variable described in part one.

## Dep (.d) Files

The final few lines of the Makefile deal with what are called dep files. Since
make targets are re-ran based on changes to dependent files, modifications to
header files are usually ignored. No targets in the Makefile have stated
dependencies on header files. Dep files are created after compilation and
describe the header file dependencies of an object file. The contents of dep
files are Make targets that show the header file dependencies of a given object
file. Dep files are generated in this Makefile with the `-MMD` compiler option. This
option will produce a `.d` with the same file name as the `.o` output file.
