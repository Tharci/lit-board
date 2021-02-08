/*
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include <iostream>

#define MAX_STR 255

int main(int argc, char* argv[])
{
    int res;
    unsigned char buf[65];
    wchar_t wstr[MAX_STR];
    hid_device *handle;
    int i;

    // Initialize the hidapi library
    res = hid_init();

    // Open the device using the VID, PID,
    // and optionally the Serial number.
    handle = hid_open(0x4d9, 0xa291, NULL);

    // Read the Manufacturer String
    res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    std::cout << "Manufacturer String: " << (int)wstr[1] << std::endl;

    // Read the Product String
    res = hid_get_product_string(handle, wstr, MAX_STR);
    std::cout << "Product String: " << (const char*)wstr << std::endl;

    // Read the Serial Number String
    res = hid_get_serial_number_string(handle, wstr, MAX_STR);
    std::cout << "Serial Number String: " << (int)wstr[0] << " " << (const char*)wstr << std::endl;

    // Read Indexed String 1
    res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
    std::cout << "Indexed String 1: " << wstr << std::endl;

    buf[0] = 1;
    buf[1] = 1;
    res = hid_write(handle, buf, 64);
    std::cout << res << std::endl;

    res = hid_read(handle, buf, 64);
    std::cout << "Recieved data: " << (int)buf[0] << (int)buf[1] << " " << res << std::endl;


/*
    // Toggle LED (cmd 0x80). The first byte is the report number (0x0).
    buf[0] = 0x0;
    buf[1] = 0x80;
    res = hid_write(handle, buf, 65);

    // Request state (cmd 0x81). The first byte is the report number (0x0).
    buf[0] = 0x0;
    buf[1] = 0x81;
    res = hid_write(handle, buf, 65);

    // Read requested state
    res = hid_read(handle, buf, 65);

    // Print out the returned buffer.
    for (i = 0; i < 4; i++)
        printf("buf[%d]: %d\n", i, buf[i]);
* /
    // Close the device
    hid_close(handle);

    // Finalize the hidapi library
    res = hid_exit();

    return 0;
}

 */


#include "LitBoard.h"



int main(int argc, char* argv[])
{
    auto driver = lbd::LitBoardDriver::getInstance();
    driver->run();
}


