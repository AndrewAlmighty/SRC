#ifndef PUMP_H
#define PUMP_H

#include "device.h"

class WashingMachineController;

class Pump : public Device
{
public:
    Pump();
    virtual ~Pump() override = default;
    virtual void acceptMessage(Message msg) override;

    void pointController(Device *controller);

protected:
    virtual void frame(double timeOffset) override;
    virtual void loop() override;

    Device* m_controller;
private:
    int m_state = 0;
    double m_liters = 0.0;

    const double m_fillingRate = 1.5; //Its liters per program time unit
    const double m_waterCapacity = 42; //42 liters of water
};

#endif // PUMP_H
