#include "centralheating.h"

#define MAX_TEMP 80
#define MIN_TEMP 30

CentralHeating::CentralHeating() : Device(), m_heatingRate(0.5)
{
    m_turnedOn = false;
    m_currentTemp = 0;
    m_desiredTemp = 0;
    m_devType = dev_CentralHeating;
}

CentralHeating::CentralHeating(const int port, const std::string ip) : Device(port, ip), m_heatingRate(0.5)
{
    m_turnedOn = false;
    m_currentTemp = 0;
    m_desiredTemp = 0;
    m_devType = dev_CentralHeating;
}

CentralHeating::~CentralHeating()
{
    if (m_devicesList[CONTROLLER_ID].connected)
        disconnect();
}

void CentralHeating::disconnect()
{
    Message msg;
    msg.dev = m_devType;
    msg.type = Disconnect;
    sendMessage(msg, dev_Controller);
    std::cerr << "[!!!] -> sent disconnect message to controller!";
    std::lock_guard<std::mutex> lock(m_mutex);
    m_devicesList[CONTROLLER_ID].connected = false;
}

void CentralHeating::handleMessage()
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

        if (msg.type == SetTemperature && msg.dev == dev_Controller)
            setDesiredTemp(msg.integer);

        else if (msg.type == TurnCH && msg.dev == dev_Controller)
            setTurnedOn(msg.integer);

        else if (msg.type == ConnectionReqToCH)
        {
            /*In controller when we send connectionAck with value
             * between 0 and 99, it will read this value as desiredtemp.
             * If 100, it means that heating is on, if 101, heating is off.
             */

            DeviceType from_dev = msg.dev;
            msg.dev = m_devType;
            msg.type = ConnectionAck;
            msg.integer = m_desiredTemp;
            sendMessage(msg, from_dev);

            if (m_turnedOn)
                msg.integer = 101;

            else
                msg.integer = 100;

            sendMessage(msg, from_dev);

            msg.type = ReportTemperature;
            msg.integer = m_currentTemp;
            sendMessage(msg, from_dev);
        }

        else if (msg.type == Disconnect)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_devicesList[CENTRAL_HEATING_ID].connected = false;
        }
    }
}

void CentralHeating::frame(double timeOffset)
{
    static double temp = 0;

    handleMessage();
    if (m_turnedOn)
    {
        if (m_currentTemp <= MAX_TEMP && m_desiredTemp >= MIN_TEMP && m_currentTemp < m_desiredTemp)
        {
            temp += m_heatingRate * timeOffset;
            setCurrentTemp(static_cast<int>(temp));
        }

        else if (m_currentTemp <= MAX_TEMP && m_desiredTemp >= MIN_TEMP && m_currentTemp > m_desiredTemp)
        {
            temp -= m_heatingRate * timeOffset;
            setCurrentTemp(static_cast<int>(temp));
        }
    }

    else
    {
        if (m_currentTemp > 0)
        {
            temp -= m_heatingRate * timeOffset;
            setCurrentTemp(static_cast<int>(temp));
        }
    }
}

void CentralHeating::setTurnedOn(const bool enabled)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_turnedOn == enabled)
            return;

        m_turnedOn = enabled;
    }

    Message msg;
    msg.dev = m_devType;
    msg.type = ReportCHStatus;
    msg.integer = m_turnedOn;
    sendMessage(msg, dev_Controller);

    if (m_turnedOn)
    {
        std::cerr << "[INFO]:Heating turned on" << std::endl;
        setDesiredTemp(30);
    }

    else
    {
        std::cerr << "[INFO]:Heating turned off" << std::endl;
        setDesiredTemp(0);
    }
}

void CentralHeating::setDesiredTemp(const int temp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_desiredTemp == temp)
        return;

    m_desiredTemp = temp;
    std::cerr << "[INFO]:User requested temperature:" << m_desiredTemp << std::endl;
}

void CentralHeating::setCurrentTemp(const int temp)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentTemp == temp)
            return;

        m_currentTemp = temp;
    }
    std::cerr << "[INFO]:Current temperature:" << m_currentTemp << std::endl;

    Message msg;
    msg.dev = m_devType;
    msg.type = ReportTemperature;
    msg.integer = m_currentTemp;
    sendMessage(msg, dev_Controller);
}
