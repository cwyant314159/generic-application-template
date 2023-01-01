#include <iostream>

#include "app/cfuncs.h"
#include "app/funcs.hpp"

int main(void)
{
    std::cout << "Hello from the main function\n";
    cfuncs_hello();
    funcs::hello();
    return 0;
}
