#include "module/module.hpp"
#include "module/module_cbindings.h"

#include <iostream>

void module::hello(void)
{
    std::cout << "Hello from the module\n";
}

void module_cbindings_hello(void)
{
    std::cout << "Hello from the module C binding\n";
}
