#include "guimanager.h"

GuiManager & GuiManager::GetInstance()
{
    static GuiManager instance;
    return instance;
}

void GuiManager::setMachineStatus(QString status)
{
    if (status == m_machineStatus)
        return;

    m_machineStatus = status;
    emit machineStatusChanged();
}

QString GuiManager::machineStatus() const
{
    return m_machineStatus;
}

void GuiManager::setmode(QString mode)
{
    if (mode == m_laundryMode)
        return;

    m_laundryMode = mode;
    emit modeChanged();
}

QString GuiManager::mode() const
{
    return m_laundryMode;
}

void GuiManager::setLockStatus(QString status)
{
    if (status == m_lockStatus)
        return;

    m_lockStatus = status;
    emit lockStatusChanged();
}

QString GuiManager::lockStatus() const
{
    return m_lockStatus;
}

void GuiManager::setLaundry(bool isRunning)
{
    if (m_laundry == isRunning || m_laundryMode == "Idle")
        return;

    m_laundry = isRunning;
    emit laundryChanged();

    setSpeed(0);
    setTemp(0);

    if (m_laundry)
    {        
        m_controller->setProgram(m_laundryMode.toStdString());
        m_controller->run();
    }
}

bool GuiManager::laundry() const
{
    return m_laundry;
}

void GuiManager::setSpeed(int speed)
{
    if (m_speed == speed)
        return;

    m_speed = speed;
    emit speedChanged();
}

int GuiManager::speed() const
{
    return m_speed;
}

void GuiManager::setTemp(int temp)
{
    if (m_temp == temp)
        return;

    m_temp = temp;
    emit tempChanged();
}

int GuiManager::temp() const
{
    return m_temp;
}

void GuiManager::connectController(WashingMachineController *WMCtrl)
{
    m_controller = WMCtrl;
}

GuiManager::GuiManager(QObject *parent) : QObject(parent)
{
    m_machineStatus = "Idle";
    m_laundryMode = "Idle";
    m_lockStatus = "Unlock";
    m_laundry = false;
}
