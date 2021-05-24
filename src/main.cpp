#include "scope_printer.hpp"

#include <stdlib.h>
#include <thread>

void std_async();
void await_future();
void await_boost_future();

int main()
{
    scope_printer scope(__FUNCTION__);
    // std_async();
    // await_future();
    await_boost_future();
    return EXIT_SUCCESS;
}
