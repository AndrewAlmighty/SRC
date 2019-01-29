#include "blinds.h"

Blinds::Blinds() : Device(), m_coverageSpeed(1.0)
{
    m_desiredCoverage = 0;
    m_currentCoverage = 0;
    m_devType = dev_Blinds;
}

Blinds::Blinds(const int port, const std::string ip) : Device(port, ip), m_coverageSpeed(1.0)
{
    m_desiredCoverage = 0;
    m_currentCoverage = 0;
    m_devType = dev_Blinds;
}

Blinds::~Blinds()
{
    if (m_devicesList[CONTROLLER_ID].connected)
        disconnect();
}

void Blinds::disconnect()
{
    Message msg;
    msg.dev = m_devType;
    msg.type = Disconnect;
    sendMessage(msg, dev_Controller);
    std::cerr << "[!!!] -> sent disconnect message to controller!";
    std::lock_guard<std::mutex> lock (m_mutex);
    m_devicesList[CONTROLLER_ID].connected = false;
}

void Blinds::handleMessage()
{
    while (!m_msgToHandle.empty())
    {
        m_mutex.lock();
        Message msg;

        if (!m_msgToHandle.empty())
        {
            msg = m_msgToHandle.front();
            m_msgToHandle.pop();
        }

        m_mutex.unlock();

        if (msg.type == SetCoverage && msg.dev == dev_Controller)
            setDesiredCoverage(msg.integer);

        else if (msg.type == ConnectionReqToBlinds)
        {
            /* We send value of current desired coverage
             * with ConnectionAck message in integer field.
             */

            DeviceType from_dev = msg.dev;
            msg.dev = m_devType;
            msg.type = ConnectionAck;
            msg.integer = m_desiredCoverage;
            sendMessage(msg, from_dev);

            msg.type = ReportCoverage;
            msg.integer = m_currentCoverage;
            sendMessage(msg, from_dev);
        }

        else if (msg.type == Disconnect)
            m_devicesList[CENTRAL_HEATING_ID].connected = false;
    }
}

void Blinds::frame(double timeOffset)
{
    static double coverage = 0;
    handleMessage();

    if (m_currentCoverage < 100 && m_desiredCoverage > 0 && m_desiredCoverage > m_currentCoverage)
    {
        coverage += m_coverageSpeed * timeOffset;
        setCurrentCoverage(static_cast<int>(coverage));
    }

    else if (m_desiredCoverage < 100 && m_currentCoverage > 0 && m_desiredCoverage < m_currentCoverage)
    {
        coverage -= m_coverageSpeed * timeOffset;
        setCurrentCoverage(static_cast<int>(coverage));
    }
}

void Blinds::setCurrentCoverage(const int coverage)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentCoverage == coverage)
            return;

        m_currentCoverage = coverage;
    }

    std::cerr << "[INFO]:Current coverage:" << m_currentCoverage << std::endl;

    Message msg;
    msg.dev = m_devType;
    msg.type = ReportCoverage;
    msg.integer = m_currentCoverage;
    sendMessage(msg, dev_Controller);
}

void Blinds::setDesiredCoverage(const int coverage)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_desiredCoverage == coverage)
        return;

    m_desiredCoverage = coverage;
    std::cerr << "[INFO]:User requested Coverage:" << m_desiredCoverage << std::endl;
}
