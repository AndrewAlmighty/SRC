#ifndef HEATER_H
#define HEATER_H

#include "device.h"

class WashingMachineController;

class Heater : public Device
{
public:
    Heater();
    virtual ~Heater() override = default;
    virtual void acceptMessage(Message msg) override;

    void pointController(Device *controller);
protected:
    virtual void frame(double timeOffset) override;
    virtual void loop() override;

    Device* m_controller;
private:
    int m_requestedTemp = 0;
    double m_temperature = 10;

    const double m_heatingRate = 2.0; //2 degrees celcius increase per time unit.
};

#endif // HEATER_H
