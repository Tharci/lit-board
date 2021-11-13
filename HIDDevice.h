//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_HIDDEVICE_H
#define LITBOARD_HIDDEVICE_H

#include <mutex>
#include <hidapi/hidapi.h>
#include <vector>

namespace lbd {
    class HIDDevice {
    public:
        static const uint8_t maxMsgLen = 64;

        HIDDevice(unsigned short vendorId, unsigned short productId, int interfaceNumber);
        ~HIDDevice();

        bool open();
        void close();
        bool isOpen();

        int write(std::vector<uint8_t> data);
        int write(uint8_t* data, size_t length);
        int write(const std::string &data);
        int read(uint8_t* data, size_t length);

    private:
        std::string path;
        const unsigned short vendorId;
        const unsigned short productId;
        const int interfaceNumber;
        std::atomic<hid_device*> device = nullptr;

        void searchDevice();
        static void printBuffer(unsigned char *data, size_t length);
    };
}


#endif //LITBOARD_HIDDEVICE_H
