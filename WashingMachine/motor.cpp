#include "motor.h"

Motor::Motor()
{

}
void Motor::acceptMessage(Message msg)
{
    if(msg.integer == m_requestedSpeed)
        return;

    if(msg.dev == dev_Controller && msg.integer == 0)
    {
        qDebug() << "Turning off the motor.";
        m_requestedSpeed = msg.integer;
    }
    else if(msg.dev == dev_Controller && msg.integer > 0)
    {
        qDebug() << "Turning on the motor.";
        m_requestedSpeed = msg.integer;
    }
}

void Motor::frame(double timeOffset)
{
    if(m_requestedSpeed > 0)
    {
        m_rpm += m_speedingRate * timeOffset;
        if(m_rpm > m_requestedSpeed)
            m_rpm = m_requestedSpeed;
    }
    else if(m_requestedSpeed == 0 && m_rpm > 0)
    {
        m_rpm -= m_speedingRate * timeOffset;

        if(m_rpm < 0)
            m_rpm = 0;
    }

    m_controller->acceptMessage(Message{dev_Motor, static_cast<int>(m_rpm)});
}

void Motor::loop()
{
    qDebug() << "Connected to Motor.";
    Device::loop();
    qDebug() << "Disconnected from Motor.";

    m_rpm = 0.0;
}

void Motor::pointController(Device *controller)
{
    m_controller = controller;
}
