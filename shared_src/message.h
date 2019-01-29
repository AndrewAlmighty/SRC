#ifndef MESSAGE_H
#define MESSAGE_H

#include <inttypes.h>

#define MSG_SIZE 8

enum DeviceType : uint16_t
{
    dev_NoDevice,
    dev_Controller,
    dev_CentralHeating,
    dev_Blinds
};

enum MessageType : uint16_t
{
    EmptyMessage,
    ConnectionReqToCH,
    ConnectionReqToBlinds,
    ConnectionReqToCtrl,
    ConnectionAck,
    Disconnect,
    RaportRequest,  //Request info
    SetCoverage,    //send info to device to change the coverage level
    ReportCoverage, //device report current level of coverage
    TurnCH,         //send info if Central Heating should be on/off.
    ReportCHStatus, //device send info if Central Heating is on/off.
    SetTemperature, //send info to Central Heating that it should change temp.
    ReportTemperature,  //device send info about current temperature.
};

struct Message
{
    Message()
    {
        dev = dev_NoDevice;
        type = EmptyMessage;
        integer = -1;
    }

    bool operator==(const Message &msg)
    {
        if (type == msg.type &&
                dev == msg.dev &&
                integer == msg.integer)
            return true;

        else return false;
    }

    void operator=(const Message &msg)
    {
        dev = msg.dev;
        type = msg.type;
        integer = msg.integer;
    }

    DeviceType dev;
    MessageType type;
    int32_t integer;
};

#endif // MESSAGE_H
