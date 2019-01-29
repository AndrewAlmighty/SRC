#include "device.h"
#include "timescale.h"

#include <thread>

Device::Device(const std::string ip) :
    m_devicesList{DeviceInfo(dev_Controller), DeviceInfo(dev_Blinds), DeviceInfo(dev_CentralHeating)}
{
    m_receiver.giveAccessToMsgQueue(m_msgToHandle);
    m_receiver.giveAccessToDeviceList(m_devicesList);
    m_receiver.setUp(ip);
    m_enable = false;
    m_isRunning = false;
    setDelay(10);
    m_listeningPort = 9000;
}

Device::Device(const int port, const std::string ip) :
    m_devicesList{DeviceInfo(dev_Controller), DeviceInfo(dev_Blinds), DeviceInfo(dev_CentralHeating)}
{
    m_receiver.giveAccessToMsgQueue(m_msgToHandle);
    m_receiver.giveAccessToDeviceList(m_devicesList);
    m_receiver.setUp(ip, port);
    m_enable = false;
    m_isRunning = false;
    setDelay(10);
    m_listeningPort = port;
}

Device::~Device()
{
    setEnable(false);
    m_isRunning = false;
}

void Device::run(bool detachThread)
{
    m_receiver.setDeviceType(m_devType);
    m_enable = true;
    m_isRunning = true;
    std::thread threadObj([&]
    {
        loop();
    });

    if (detachThread)
        threadObj.detach();

    else
        threadObj.join();
}

void Device::setDelay(unsigned delay)
{
    m_threadDelay = delay;
}

void Device::loop()
{
    typedef std::chrono::high_resolution_clock clock;
    auto timeBefore = clock::now();
    while (m_isRunning)
    {
        std::chrono::duration <double, std::milli> offset = clock::now() - timeBefore;
        timeBefore = clock::now();
        this -> frame(offset.count() * Timescale::getScale() / 1000.0);
        std::this_thread::sleep_for(std::chrono::duration<long, std::milli> {m_threadDelay});
    }
}

void Device::ConnectToDevice(const DeviceType &dev, const std::string addr, const int &port)
{
    Message msg;
    msg.dev = m_devType;
    msg.integer = m_listeningPort;

    if (dev == dev_Controller)
        msg.type = ConnectionReqToCtrl;

    else if (dev == dev_Blinds)
        msg.type = ConnectionReqToBlinds;

    else if (dev == dev_CentralHeating)
        msg.type = ConnectionReqToCH;

    for (int i = 0; i < 3; i++)
    {
        if (m_devicesList[i].type == dev)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_devicesList[i].ip = addr;
            m_devicesList[i].port = port;
            m_devicesList[i].connected = false;
            break;
        }
    }

    sendMessage(msg, dev);
}

void Device::setEnable(bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_enable == enable)
        return;

    else if (enable == true)
        m_enable = true;

    else if (enable == false)
    {
        m_enable = false;        

        while(!m_msgToHandle.empty())
            m_msgToHandle.pop();
    }
}

void Device::sendMessage(const Message &msg, DeviceType dev)
{
    unsigned i;
    for (i = 0; i < 4; i++)
    {
        if (m_devicesList[i].type == dev)
            break;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_transmitter.send(msg, m_devicesList[i].ip, m_devicesList[i].port);
}
