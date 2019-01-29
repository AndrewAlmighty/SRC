#include "controller.h"
#include "guicontroller.h"

Controller::Controller(const std::string ip) : Device(ip)
{
    m_desiredCoverage = 0;
    m_currentCoverage = 0;
    m_currentTemp = 0;
    m_desiredTemp = 0;
    m_heatingOn = 0;
    m_devType = dev_Controller;
}

Controller::Controller(const int port, const std::string ip) : Device(port, ip)
{
    m_desiredCoverage = 0;
    m_currentCoverage = 0;
    m_currentTemp = 0;
    m_desiredTemp = 0;
    m_heatingOn = 0;
    m_devType = dev_Controller;
}

void Controller::setCurrentTemp(const int temp)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentTemp == temp)
            return;

        m_currentTemp = temp;
    }

    GuiController::getController().setCurrentTemp(temp);
}

void Controller::setCurrentCoverage(const int coverage)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentCoverage == coverage)
            return;

        m_currentCoverage = coverage;
    }

    GuiController::getController().setCurrentCoverage(coverage);
}

void Controller::setHeatingOn(const bool heatingOn)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_heatingOn == heatingOn)
            return;

        m_heatingOn = heatingOn;
    }

    if (m_heatingOn)
    {
        setDesiredTemp(30);
        updateGUI_DesiredTemp();
    }
}

void Controller::setDesiredTemp(const int temp, bool changedByUser)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_desiredTemp == temp)
        return;

    m_desiredTemp = temp;
    m_desiredTempChanged = changedByUser;
}

void Controller::setDesiredCoverage(const int coverage, bool changedByUser)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_desiredCoverage == coverage)
        return;

    m_desiredCoverage = coverage;
    m_desiredCoverageChanged = changedByUser;
}

void Controller::setDesiredHeatingOn(const bool enable, bool changedByUser)
{
    if (m_desiredHeatingOn == enable)
        return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_desiredHeatingOn = enable;
        m_desiredHeatingOnChanged = changedByUser;
    }

    setDesiredTemp(30);
}

void Controller::updateGUI_isHeatingOn() const
{
    GuiController::getController().setHeatingOn(m_heatingOn);
}

void Controller::updateGUI_DesiredTemp() const
{
    GuiController::getController().setDesiredTemp(m_desiredTemp, false);
}

void Controller::updateGUI_DesiredCoverage() const
{
    GuiController::getController().setDesiredCoverage(m_desiredCoverage);
}

void Controller::updateGUI_DeviceConnected(const DeviceType dev, const bool connected)
{
    if (dev == dev_Blinds)
        GuiController::getController().setBlindsConnected(connected);

    else if (dev == dev_CentralHeating)
        GuiController::getController().setHeatingConnected(connected);
}

void Controller::handleMessage()
{
    if (!m_enable)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_msgToHandle.empty()) {}
            m_msgToHandle.pop();
    }

    else
    {
        Message msg;

        while (!m_msgToHandle.empty())
        {
            m_mutex.lock();

            if (!m_msgToHandle.empty())
            {
                msg = m_msgToHandle.front();
                m_msgToHandle.pop();
            }

            m_mutex.unlock();

            if (msg.type == ReportCHStatus && msg.dev == dev_CentralHeating)
                setHeatingOn(msg.integer);

            else if (msg.type == ReportCoverage && msg.dev == dev_Blinds)
                setCurrentCoverage(msg.integer);

            else if (msg.type == ReportTemperature && msg.dev == dev_CentralHeating)
                setCurrentTemp(msg.integer);

            else if (msg.type == ConnectionAck)
            {
                /*
                 * To avoid aditional mechanism, used integer to
                 * report status of new connected device.
                 */

                if (msg.dev == dev_Blinds)
                {
                    //In message we send status of desired coverage.
                    m_mutex.lock();
                    m_devicesList[BLINDS_ID].connected = true;
                    m_mutex.unlock();
                    updateGUI_DeviceConnected(dev_Blinds, true);

                    if (msg.integer >= 0)
                        setDesiredCoverage(msg.integer);
                }

                else if (msg.dev == dev_CentralHeating)
                {
                    /*
                     * If integer is between 0 and 100, it means we got temp,
                     * if 100 - heating is off, if 101, it's on.
                     */

                    m_mutex.lock();
                    m_devicesList[CENTRAL_HEATING_ID].connected = true;
                    m_mutex.unlock();
                    updateGUI_DeviceConnected(dev_CentralHeating, true);

                    if (msg.integer > -1 && msg.integer < 100)
                    {
                        setDesiredTemp(msg.integer, false);
                        updateGUI_DesiredTemp();
                    }

                    else if (msg.integer == 101)
                    {
                        setHeatingOn(true);
                        updateGUI_isHeatingOn();
                    }

                    else if (msg.integer == 100)
                    {
                        setHeatingOn(false);
                        updateGUI_isHeatingOn();
                    }
                }
            }

            else if (msg.type == Disconnect)
            {
                if (msg.dev == dev_Blinds)
                {                    
                    updateGUI_DeviceConnected(dev_Blinds, false);
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_devicesList[BLINDS_ID].connected = false;
                }

                else if (msg.dev == dev_CentralHeating)
                {                    
                    updateGUI_DeviceConnected(dev_CentralHeating, false);
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_devicesList[CENTRAL_HEATING_ID].connected = false;
                }
            }
        }
    }
}

void Controller::frame(double timeOffset)
{
    handleMessage();
    Message msg;
    msg.dev = m_devType;

    if (m_enable)
    {
        if (m_devicesList[BLINDS_ID].connected)
        {
            if (m_desiredCoverageChanged && m_currentCoverage != m_desiredCoverage)
            {
                msg.type = SetCoverage;
                msg.integer = m_desiredCoverage;
                sendMessage(msg, dev_Blinds);
                m_desiredCoverageChanged = false;
            }
        }

        if (m_devicesList[CENTRAL_HEATING_ID].connected)
        {
            if (m_desiredTempChanged && m_desiredTemp != m_currentTemp && m_heatingOn && m_desiredTemp >= 30)
            {
                msg.type = SetTemperature;
                msg.integer = m_desiredTemp;
                sendMessage(msg, dev_CentralHeating);
                m_desiredTempChanged = false;
            }

            if (m_desiredHeatingOnChanged && m_desiredHeatingOn != m_heatingOn)
            {
                msg.type = TurnCH;
                msg.integer = m_desiredHeatingOn;
                sendMessage(msg, dev_CentralHeating);
                m_desiredHeatingOnChanged = false;
            }
        }
    }

    m_desiredTempChanged = false;
    m_desiredCoverageChanged = false;
    m_desiredHeatingOnChanged = false;
}
