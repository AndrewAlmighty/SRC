#include "guicontroller.h"

GuiController & GuiController::getController()
{
    static GuiController ctrl;
    return ctrl;
}

GuiController::GuiController(QObject *parent) : QObject(parent)
{
    setHeatingOn(false, false);
    setDesiredTemp(0, false);
    setCurrentTemp(0);
    setCurrentCoverage(0);
    setDesiredCoverage(0, false);
    setBlindsConnected(false);
    setHeatingConnected(false);
    setHeatingIP("");
    setBlindsIP("");
    m_ctrl = nullptr;
}

GuiController::~GuiController()
{

}

void GuiController::getIPandPort(QString &ip, int &port)
{
    QString tmp;
    bool gotPort = false;

    for (int i = 0; i < ip.length(); i++)
    {
        if (!gotPort && ip[i] != ":")
            tmp += ip[i];

        else if (!gotPort && ip[i] == ":")
            gotPort = true;

        else if (gotPort)
        {
            port = ip.mid(i, ip.length() - i).toInt();
            break;
        }
    }

    ip = tmp;
}

void GuiController::setDesiredCoverage(const int coverage, bool changedByUser)
{
    if (m_desiredCoverage == coverage)
        return;

    m_desiredCoverage = coverage;
    emit desiredCoverageChanged();

    if (changedByUser)
        m_ctrl -> setDesiredCoverage(coverage);
}

int GuiController::desiredCoverage() const
{
    return m_desiredCoverage;
}

void GuiController::setCurrentCoverage(const int coverage)
{
    if (m_currentCoverage == coverage)
        return;

    m_currentCoverage = coverage;
    emit currentCoverageChanged();
}

int GuiController::currentCoverage() const
{
    return m_currentCoverage;
}

void GuiController::setHeatingOn(const bool isHeating, bool changedByUser)
{
    if (m_heatingOn == isHeating)
        return;

    m_heatingOn = isHeating;
    emit heatingOnChanged();

    if (changedByUser)
        m_ctrl -> setDesiredHeatingOn(isHeating);
}

bool GuiController::heatingOn() const
{
    return m_heatingOn;
}

void GuiController::setCurrentTemp(const int temp)
{
    if (m_currentTemp == temp)
        return;

    m_currentTemp = temp;
    emit currentTempChanged();
}

int GuiController::currentTemp() const
{
    return m_currentTemp;
}

void GuiController::setDesiredTemp(const int temp, bool changedByUser)
{
    if (m_desiredTemp == temp || temp < 30 || temp > 80)
        return;

    m_desiredTemp = temp;
    emit desiredTempChanged();

    if (changedByUser)
        m_ctrl -> setDesiredTemp(temp);
}

int GuiController::desiredTemp() const
{
    return m_desiredTemp;
}

void GuiController::setHeatingConnected(const bool isConnected)
{
    if (isConnected == m_heatingConnected)
        return;

    m_heatingConnected = isConnected;
    emit heatingConnectedChanged();
}

bool GuiController::heatingConnected() const
{
    return m_heatingConnected;
}

void GuiController::setBlindsConnected(const bool isConnected)
{
    if (isConnected == m_blindsConnected)
        return;

    m_blindsConnected = isConnected;
    emit blindsConnectedChanged();
}

bool GuiController::blindsConnected() const
{
    return m_blindsConnected;
}

void GuiController::setBlindsIP(const QString ip)
{
    if (ip == m_blindsIP)
        return;

    m_blindsIP = ip;
    emit blindsIPChanged();
    setBlindsConnected(false);

    int port = 0;
    QString just_ip = ip;
    getIPandPort(just_ip, port);
    m_ctrl -> ConnectToDevice(dev_Blinds, just_ip.toStdString(), port);
}

QString GuiController::blindsIP() const
{
    return m_blindsIP;
}

void GuiController::setHeatingIP(const QString ip)
{
    if (ip == m_heatingIP)
        return;

    m_heatingIP = ip;
    emit heatingIPChanged();
    setHeatingConnected(false);

    int port = 0;
    QString just_ip = ip;
    getIPandPort(just_ip, port);
    m_ctrl -> ConnectToDevice(dev_CentralHeating, just_ip.toStdString(), port);
}

QString GuiController::heatingIP() const
{
    return m_heatingIP;
}

void GuiController::setPtrToCtrl(Controller &ctrl)
{
    m_ctrl = &ctrl;
}
