#include "receiver.h"
#include "message.h"
#include "deviceinfo.h"

#include <iostream>
#include <thread>
#include <sstream>

Receiver::Receiver()
{
    m_queuePtr = nullptr;
    m_devType = dev_NoDevice;
}

Receiver::~Receiver()
{
    delete m_socket;
}

void Receiver::giveAccessToMsgQueue(std::queue<Message> &queue)
{
    m_queuePtr = &queue;
}

void Receiver::giveAccessToDeviceList(DeviceInfo list[])
{
    m_devListPtr = list;
}

void Receiver::pushMessage(const Message msg)
{
    if (m_queuePtr == nullptr)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_queuePtr -> push(msg);
}

void Receiver::pushAddr(const ost::SyncSource &source, const DeviceType dev, const MessageType msgType, const bool connected, const int port)
{
    if ((msgType == ConnectionReqToBlinds && m_devType != dev_Blinds)
            || (msgType == ConnectionReqToCH && m_devType != dev_CentralHeating)
            || (msgType == ConnectionReqToCtrl && m_devType != dev_Controller))
        return;

    std::lock_guard<std::mutex> lock(m_mutex);

    std::stringstream sstream;
    sstream << source.getNetworkAddress();

    if (dev == dev_Controller)
    {
        m_devListPtr[CONTROLLER_ID].ip = sstream.str();
        m_devListPtr[CONTROLLER_ID].port = port;
        m_devListPtr[CONTROLLER_ID].connected = connected;
        if (connected)
            std::cerr << "Controller has been connected! Addr:" << source.getNetworkAddress() <<
                      ":" << port << std::endl;

        else
            std::cerr << "Controller has been disconnected!\n";
    }

    else if (dev == dev_Blinds)
    {
        m_devListPtr[BLINDS_ID].ip = sstream.str();
        m_devListPtr[BLINDS_ID].port = port;
        m_devListPtr[BLINDS_ID].connected = connected;
        if (connected)
            std::cerr << "Blinds have been connected! Addr:" << std::dec << source.getNetworkAddress() <<
                     ":" << port << std::endl;

        else
            std::cerr << "Blinds has been disconnected!\n";
    }

    else if (dev == dev_CentralHeating)
    {
        m_devListPtr[CENTRAL_HEATING_ID].ip = sstream.str();
        m_devListPtr[CENTRAL_HEATING_ID].port = port;
        m_devListPtr[CENTRAL_HEATING_ID].connected = connected;
        if (connected)
            std::cerr << "Central heating has been connected! Addr:" << source.getNetworkAddress() <<
                     ":" << port << std::endl;

        else
            std::cerr << "Central heating has been disconnected!\n";
    }
}

void Receiver::setUp(std::string localIP, int port)
{
    m_localIP = localIP.c_str();
    if (!m_localIP)
        std::cerr << "[!!!] -> Provided wrong IP to receiver!\n";

    m_port = static_cast<ost::tpport_t>(port);
    m_socket = new ost::RTPSession(m_localIP, m_port);
    if (!m_socket)
        std::cerr << "[!!!] -> Error during creation socket!\n";

    m_id = m_socket -> getLocalSSRC();

    if (m_socket)
        std::cerr << "[!] -> SSRC identifier of receiver: " << m_id << std::endl;

    start_listening();
}

void Receiver::setDeviceType(DeviceType type)
{
    m_devType = type;
}

void Receiver::start_listening()
{
    std::thread threadObj([&]
    {
        std::cerr << "[!] -> Started listening. Local SSRC id:" << m_id <<
                     " Addr:" << m_localIP << ":" << m_port << std::endl;
        m_socket -> setPayloadFormat(ost::StaticPayloadFormat(ost::sptMP2T));
        m_socket -> startRunning();

        while (true)
        {
            const ost::AppDataUnit *adu = nullptr;
            Message tmpMsg;

            while (!adu)
            {
                ost::Thread::sleep(10);
                adu = m_socket -> getData(m_socket -> getFirstTimestamp());
            }

            std::cerr << "-> Received new message!" << std::endl;
            memcpy(&tmpMsg, adu ->getData(), MSG_SIZE);

            if (tmpMsg.type == ConnectionReqToBlinds || tmpMsg.type == ConnectionReqToCH
                    || tmpMsg.type == ConnectionReqToCtrl)
                pushAddr(adu -> getSource(), tmpMsg.dev, tmpMsg.type, true, tmpMsg.integer);

            else if (tmpMsg.type == Disconnect)
                pushAddr(adu -> getSource(), tmpMsg.dev, tmpMsg.type, false,tmpMsg.integer);

            //std::cerr << "int:" << tmpMsg.integer << " type:" << (int)tmpMsg.type << " dev:" << (int)tmpMsg.dev << std::endl;
            pushMessage(tmpMsg);
            delete adu;
        }
    });

    threadObj.detach();
}




