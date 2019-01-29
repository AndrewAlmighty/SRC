#include "transmitter.h"
#include "message.h"

Transmitter::Transmitter()
{
}

Transmitter::~Transmitter()
{
}

void Transmitter::setUp(const std::string &localIP)
{
    m_localIP = localIP.c_str();

    if (!m_localIP)
        std::cerr << "[!!!] -> Provided wrong IP to transmitter!\n";

    m_socket = new ost::RTPSession(m_localIP);

    if (!m_socket)
        std::cerr << "[!!!] -> Error during creation socket!\n";

    m_id = m_socket -> getLocalSSRC();

    if (m_socket)
        std::cerr << "[!] -> SSRC identifier of transmitter: " << m_id << std::endl;

    m_socket -> setPayloadFormat(ost::StaticPayloadFormat(ost::sptMP2T));
    m_socket -> startRunning();
}

void Transmitter::send(const Message &msg, const std::string &ip, const int &port)
{
    setUp();
    m_socket -> addDestination(ost::InetHostAddress(ip.c_str()), static_cast<ost::tpport_t>(port));
    unsigned char frame[MSG_SIZE];
    memcpy(frame, &msg, MSG_SIZE);
    m_socket -> putData(0, frame, MSG_SIZE + 1);
    ost::Thread::sleep(10);
    std::cerr << "[!] -> Sent a message to device with address: " << ip.c_str() << ":" << port << std::endl;
    m_socket -> forgetDestination(ost::InetHostAddress(ip.c_str()), static_cast<ost::tpport_t>(port));
    delete m_socket;
}
