
#include "LitBoardDriver.h"
#include <iostream>


int main(int argc, char* argv[])
{
    // Check for another instance
    HANDLE hHandle = CreateMutex( nullptr, TRUE, "litboard" );
    if( ERROR_ALREADY_EXISTS == GetLastError() )
    {
        // Another instance is running
        std::cout << "[ERROR] Another instance of the driver is already running. Exiting application...";
        return(1); // Exit program
    }

    auto& driver = lbd::LitBoardDriver::getInstance();
    driver.run();

    ReleaseMutex( hHandle );
    CloseHandle( hHandle );
    return( 1 );
}
