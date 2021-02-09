//
// Created by tharci on 2/7/21.
//

#include "HIDDevice.h"

namespace lbd {

    void HIDDevice::searchKeyboard() {
        struct hid_device_info *devs, *cur_dev;
        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;
        while (cur_dev) {
            if( cur_dev->vendor_id == vendorId && cur_dev->product_id == productId && cur_dev->interface_number == 1) {
                printf("[TRACE] Device: vid/pid: %04hx/%04hx\n  path: %s\n  serial_number: %ls usage_page: %x, usage: %x",
                       cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number, cur_dev->usage_page, cur_dev->usage);
                printf("\n");
                printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
                printf("  Product:      %ls\n", cur_dev->product_string);
                printf("  Interface:    %d\n", cur_dev->interface_number);
                printf("\n");

                this->path = cur_dev->path;
                break;
            }

            cur_dev = cur_dev->next;
        }

        hid_free_enumeration(devs);
    }

    HIDDevice::~HIDDevice() {
        if (device) close();
    }

    bool HIDDevice::open() {
        if (!isOpen()) {
            close();
            searchKeyboard();
            device = hid_open_path(path.c_str());
        }

        return device != nullptr;
    }

    void HIDDevice::close() {
        hid_close(device);
        device = nullptr;
        path = "";
    }

    int HIDDevice::write(std::vector<unsigned char> data) {
        return write(data.data(), data.size());
    }

    int HIDDevice::write(unsigned char *data, size_t length) {
        if (device) {
            auto result = hid_write(device, data, length);

            return result;
        }

        return -1;
    }

    int HIDDevice::write(const std::string &data) {
        return write((unsigned char *) data.c_str(), data.length());
    }

    bool HIDDevice::isOpen() {
        if (device == nullptr)
            return false;

        unsigned char dummy;
        return write(&dummy, 1) != -1;
    }
}
