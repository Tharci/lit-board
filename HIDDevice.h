//
// Created by tharci on 2/7/21.
//

#ifndef LITBOARD_HIDDEVICE_H
#define LITBOARD_HIDDEVICE_H

#include <hidapi/hidapi.h>
#include <iostream>
#include <vector>


namespace lbd {

    class HIDDevice {
    public:
        static const uint8_t maxMsgLen = 64;

        HIDDevice() = default;
        ~HIDDevice();

        bool open();
        void close();
        bool isOpen();

        int write(std::vector<unsigned char> data);
        int write(unsigned char *data, size_t length);
        int write(const std::string &data);

    private:
        std::string path;
        unsigned short vendorId = 0x04d9;
        unsigned short productId = 0xa291;
        int interfaceNumber = 1;

        hid_device *device = nullptr;

        void searchKeyboard();

        static void printSentBuffer(unsigned char *data, size_t length) {
            std::cout << "[TRACE] Data has been sent: \n";
            for (size_t i = 0; i < length; i++) {
                std::cout << (int) data[i] << " ";
            }
            std::cout << "\n\n";
            std::flush(std::cout);
        }
    };
}


#endif //LITBOARD_HIDDEVICE_H
