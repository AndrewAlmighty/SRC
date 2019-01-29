#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../shared_src/device.h"

class Controller : public Device
{
public:
    Controller(const std::string ip = "127.0.0.1");
    Controller(const int port, const std::string ip = "127.0.0.1");

    //Current is set here, in controller and send info to gui.
    void setCurrentTemp(const int temp);
    void setCurrentCoverage(const int coverage);
    void setHeatingOn(const bool heatingOn);

    //Desired is set in GUI and info is send here, then is send to devices.
    void setDesiredTemp(const int temp, bool changedByUser = true);
    void setDesiredCoverage(const int coverage, bool changedByUser = true);
    void setDesiredHeatingOn(const bool enable, bool changedByUser = true);

    //When we read info about status of devices, we need to update gui.
    void updateGUI_isHeatingOn() const;
    void updateGUI_DesiredTemp() const;
    void updateGUI_DesiredCoverage() const;
    void updateGUI_DeviceConnected(const DeviceType dev, const bool connected);

protected:
    virtual void handleMessage() override;
    virtual void frame(double timeOffset) override;

private:
    int m_desiredCoverage;
    int m_currentCoverage;
    int m_currentTemp;
    int m_desiredTemp;
    bool m_heatingOn;
    bool m_desiredHeatingOn;

    bool m_desiredTempChanged;
    bool m_desiredCoverageChanged;
    bool m_desiredHeatingOnChanged;
};

#endif // CONTROLLER_H
