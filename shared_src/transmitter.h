#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <ccrtp/rtp.h>
#include <queue>

struct Message;

class Transmitter
{
public:
    Transmitter();
    ~Transmitter();
    void send(const Message &msg, const std::string &ip, const int &port);

private:
    void setUp(const std::string &localIP = "127.0.0.1");

    ost::RTPSession *m_socket;
    ost::InetHostAddress m_localIP;
    uint32 m_id;
};

#endif // TRANSMITTER_H
