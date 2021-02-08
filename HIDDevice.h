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

        explicit HIDDevice(const hid_device_info *deviceInfo) :
                path(deviceInfo->path),
                vendorId(deviceInfo->vendor_id),
                productId(deviceInfo->product_id),
                serialNumber(*deviceInfo->serial_number),
                releaseNumber(deviceInfo->release_number),
                manufacturerString(deviceInfo->manufacturer_string),
                productString(deviceInfo->product_string),
                usagePage(deviceInfo->usage_page),
                usage(deviceInfo->usage),
                interfaceNumber(deviceInfo->interface_number) {}

        ~HIDDevice() {
            if (isOpen()) close();
        }

        void open() {
            device = hid_open_path(path.c_str());
        }

        void close() {
            hid_close(device);
        }

        int write(std::vector<unsigned char> data) {
            return write(data.data(), data.size());
        }

        int write(unsigned char *data, size_t length) {
            if (device) {
                auto result = hid_write(device, data, length);

                printSentBuffer(data, length);

                return result;
            }

            return -1;
        }

        int write(const std::string &data) {
            return write((unsigned char *) data.c_str(), data.length());
        }

        bool isOpen() {
            unsigned char dummy = 0;
            return write(&dummy, 1) >= 0;
        }

    private:
        std::string path;
        unsigned short vendorId;
        unsigned short productId;
        long serialNumber;
        unsigned short releaseNumber;
        std::wstring manufacturerString;
        std::wstring productString;
        unsigned short usagePage;
        unsigned short usage;
        int interfaceNumber;

        hid_device *device = nullptr;

        static void printSentBuffer(unsigned char *data, size_t length) {
            std::cout << "Data has been sent: \n";
            for (size_t i = 0; i < length; i++) {
                std::cout << (int) data[i] << " ";
            }
            std::cout << "\n\n";
            std::flush(std::cout);
        }
    };
}


#endif //LITBOARD_HIDDEVICE_H
