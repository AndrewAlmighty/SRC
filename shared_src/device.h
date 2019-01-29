#ifndef DEVICE_H
#define DEVICE_H

#include "deviceinfo.h"
#include "message.h"
#include "receiver.h"
#include "transmitter.h"

#include <mutex>

class Device
{
public:
    Device(const std::string ip = "127.0.0.1");
    Device(const int port, const std::string ip = "127.0.0.1");
    virtual ~Device();

    virtual void ConnectToDevice(const DeviceType &dev, const std::string addr = "", const int &port = 0);
    virtual void setEnable(bool enable);
    virtual void run(bool detachThread);
    virtual void sendMessage(const Message &msg, DeviceType dev);

protected:
    virtual void handleMessage() = 0;
    virtual void frame(double timeOffset) = 0;
    virtual void loop();
    virtual void setDelay(unsigned delay);

    DeviceInfo m_devicesList[3];
    DeviceType m_devType;
    std::queue<Message> m_msgToHandle;
    std::mutex m_mutex;
    bool m_enable;

private:
    bool m_isRunning;
    Receiver m_receiver;
    Transmitter m_transmitter;
    unsigned m_threadDelay;
    int m_listeningPort;
};

#endif // DEVICE_H
