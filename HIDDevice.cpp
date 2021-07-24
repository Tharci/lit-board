//
// Created by tmarc on 14/07/2021.
//

#include "HIDDevice.h"
#include <iostream>
#include <vector>


lbd::HIDDevice::HIDDevice(unsigned short vendorId, unsigned short productId, int interfaceNumber)
        : vendorId(vendorId), productId(productId), interfaceNumber(interfaceNumber) { }

void lbd::HIDDevice::printBuffer(uint8_t* data, size_t length) {
    std::cout << "[TRACE] Data has been sent: \n";
    for (size_t i = 0; i < length; i++) {
        std::cout << (int) data[i] << " ";
    }
    std::cout << "\n\n";
    std::flush(std::cout);
}

void lbd::HIDDevice::searchKeyboard() {
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        if( cur_dev->vendor_id == vendorId && cur_dev->product_id == productId && cur_dev->interface_number == interfaceNumber) {
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

lbd::HIDDevice::~HIDDevice() {
    if (device) close();
}

bool lbd::HIDDevice::open() {
    if (!isOpen()) {
        close();
        searchKeyboard();
        device = hid_open_path(path.c_str());
    }

    return device != nullptr;
}

void lbd::HIDDevice::close() {
    hid_close(device);
    device = nullptr;
    path = "";
}

int lbd::HIDDevice::write(std::vector<uint8_t> data) {
    return write(data.data(), data.size());
}

int lbd::HIDDevice::write(uint8_t* data, size_t length) {
    if (device) {
        auto result = hid_write(device, data, length);
        return result;
    }

    return -1;
}

int lbd::HIDDevice::write(const std::string &data) {
    return write((uint8_t*) data.c_str(), data.length());
}

bool lbd::HIDDevice::isOpen() {
    if (device == nullptr)
        return false;

    uint8_t dummy[] = {0, 0};
    return write(dummy, 2) != -1;
}
