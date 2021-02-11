
#include "LitBoardDriver.h"

int main(int argc, char* argv[])
{
    auto driver = lbd::LitBoardDriver::getInstance();
    driver->run();
}


