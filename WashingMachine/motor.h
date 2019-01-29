#ifndef MOTOR_H
#define MOTOR_H

#include "device.h"

class WashingMachineController;

class Motor : public Device
{
public:
    Motor();
    virtual ~Motor() override = default;
    virtual void acceptMessage(Message msg) override;

    void pointController(Device *controller);

protected:
    virtual void frame(double timeOffset) override;
    virtual void loop() override;

    Device* m_controller;
private:
    int m_requestedSpeed = 0;
    double m_rpm = 0;

    const double m_speedingRate = 60.0;
};

#endif // MOTOR_H
