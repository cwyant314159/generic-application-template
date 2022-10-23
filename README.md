# Template Project 

This repos is used as the basis for my C/C++ projects. It currently targets a
desktop environment, but with minimal tweaking, it can target embedded devices
as well.

This is a Makefile based template. It recursively searches the `src` directory
for both `*.c` and `*.cpp` files. Object files have the same path as source
but are rooted under the directory contained in the Make variable `$(OBJ_DIR)`.

For the local include path, the only `-I` option is the `include` directory
(i.e. `-Iinclude` is the only option passed to gcc). This is done to force the
convention of fully specifying paths to `#includes`. For example, if you had a
header called `my_funcs.h` in the directory `include/app/modules/`, the 
`#include` statement would look like this:

```c
#include "app/modules/my_funcs.h
```

This is my prefered style, and I think it is a good convention to follow. It
allows you to have header files that have the same name but reside in different
directories. This is very useful for embedded development where multiple driver
modules might have a `hardware_regs.h` header.

One of the drawbacks to this project template is that it cannot support having
a C file named the same as a C++ file __in the same directory__. This is a
limitation caused by how the object files are named.
