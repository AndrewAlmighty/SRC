#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <stdint.h>
#include <string>

#define CONTROLLER_ID 0
#define BLINDS_ID 1
#define CENTRAL_HEATING_ID 2

enum DeviceType : uint16_t;

struct DeviceInfo
{
    DeviceInfo(DeviceType dev)
    {
        type = dev;
        ip = "";
        port = 0;
        connected = false;
    }

    DeviceType type;
    int port;
    bool connected;
    std::string ip;
};

#endif // DEVICEINFO_H
