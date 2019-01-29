#include "pump.h"

Pump::Pump()
{

}

void Pump::acceptMessage(Message msg)
{
    if(msg.integer == m_state)
        return;

    if(msg.dev == dev_Controller && msg.integer == 1)
    {
        qDebug() << "Pumping in the water.";
        m_state = msg.integer;
    }
    else if(msg.dev == dev_Controller && msg.integer == 2)
    {
        qDebug() << "Pumping out the water.";
        m_state = msg.integer;
    }
    else if(msg.dev == dev_Controller && msg.integer == 0)
    {
        qDebug() << "Stopping the pump.";
        m_state = msg.integer;
    }
}

void Pump::frame(double timeOffset)
{
    if(m_state == 1 && m_liters < m_waterCapacity)
        m_liters += m_fillingRate * timeOffset;

    else if(m_state == 2 && m_liters > 0)
        m_liters -= m_fillingRate * timeOffset;

    if(m_liters < 0)
        m_liters = 0;

    if(m_state != 0)
        m_controller->acceptMessage(Message{dev_Pump, static_cast<int>(m_liters)});
}

void Pump::loop()
{
    qDebug() << "Connected to Pump.";
    Device::loop();
    qDebug() << "Disconnected from Pump.";

    m_liters = 0;
    m_state = 0;
}

void Pump::pointController(Device *controller)
{
    m_controller = controller;
}
