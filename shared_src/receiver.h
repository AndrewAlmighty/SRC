#ifndef RECEIVER_H
#define RECEIVER_H

#include <ccrtp/rtp.h>
#include <queue>
#include <mutex>

struct Message;
struct DeviceInfo;
enum DeviceType : uint16_t;
enum MessageType : uint16_t;

class Receiver
{
public:
    Receiver();
    ~Receiver();
    void giveAccessToMsgQueue(std::queue<Message> &queue);
    void giveAccessToDeviceList(DeviceInfo list[]);
    void setUp(std::string localIP = "127.0.0.1", int port = 9000);
    void setDeviceType(DeviceType type);

private:
    void pushMessage(const Message msg);
    void pushAddr(const ost::SyncSource& source, const DeviceType dev, const MessageType msgType, const bool connected, const int port);
    void start_listening();

    ost::RTPSession *m_socket;
    std::queue<Message> *m_queuePtr;
    DeviceInfo *m_devListPtr;
    ost::InetHostAddress m_localIP;
    uint32 m_id;
    ost::tpport_t m_port;
    std::mutex m_mutex;
    DeviceType m_devType;
};

#endif // RECEIVER_H
