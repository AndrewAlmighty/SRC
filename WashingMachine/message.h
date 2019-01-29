#ifndef MESSAGE_H
#define MESSAGE_H

enum DeviceType : short
{
    dev_None,                   //Empty message
    dev_Controller,
    dev_Motor,
    dev_Heater,
    dev_Pump,
    dev_DoorLock
};

struct Message
{
    DeviceType dev;         //who is sending message
    int integer;            //the message
};

#endif // MESSAGE_H
