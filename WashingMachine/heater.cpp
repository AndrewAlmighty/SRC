#include "heater.h"

Heater::Heater()
{

}

void Heater::acceptMessage(Message msg)
{
    if(msg.integer == m_requestedTemp)
        return;

    if(msg.dev == dev_Controller && msg.integer == 0)
    {
        qDebug() << "Truning off the heater";
        m_requestedTemp = msg.integer;
    }
    else if(msg.dev == dev_Controller && msg.integer > 0)
    {
        qDebug() << "Truning on the heater";
        m_requestedTemp = msg.integer;
    }
}

void Heater::frame(double timeOffset)
{
    if(m_requestedTemp > 0)
    {
        m_temperature += m_heatingRate * timeOffset;

        if(m_temperature > m_requestedTemp)
            m_temperature = m_requestedTemp;

        m_controller->acceptMessage(Message{dev_Heater, static_cast<int>(m_temperature)});
    }
}

void Heater::loop()
{
    qDebug() << "Connected to Heater.";
    Device::loop();
    qDebug() << "Disconnected from Heater.";

    m_requestedTemp = 0.0;
    m_temperature = 10.0;
}

void Heater::pointController(Device *controller)
{
    m_controller = controller;
}
