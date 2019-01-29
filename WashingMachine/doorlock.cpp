#include "doorlock.h"
#include "guimanager.h"

DoorLock::DoorLock()
{

}

void DoorLock::acceptMessage(Message msg)
{
    if(msg.dev == dev_Controller && msg.integer == (m_isClosed == false ? 1 : 0) && !m_isRequested)
    {
        qDebug() << "Requested door lock change state.";
        m_isRequested  = true;
    }
}

void DoorLock::frame(double timeOffset)
{
    if(m_isRequested)
    {
        if(m_reactionTimer >= m_reaction)
        {
            m_reactionTimer = 0.0;
            m_isRequested = false;
            m_isClosed = !m_isClosed;
            m_controller->acceptMessage(Message{dev_DoorLock, (m_isClosed == true ? 1 : 0) });
            qDebug() << "State of doorlock changed to: " << (m_isClosed == true ? "Closed" : "Opened");
        }
       m_reactionTimer += timeOffset;
    }
}

void DoorLock::loop()
{
    qDebug() << "Connected to DoorLock.";
    Device::loop();
    qDebug() << "Disconnected from DoorLock.";

    m_isClosed = false;
    m_isRequested = false;
    m_reactionTimer = 0.0;
}

void DoorLock::pointController(Device *controller)
{
    m_controller = controller;
}
