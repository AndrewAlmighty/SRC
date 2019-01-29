#ifndef DOORLOCK_H
#define DOORLOCK_H

#include "device.h"

class WashingMachineController;

class DoorLock : public Device
{
public:
    DoorLock();
    virtual ~DoorLock() override = default;
    virtual void acceptMessage(Message msg) override;

    void pointController(Device *controller);

protected:
    virtual void frame(double timeOffset) override;
    virtual void loop() override;

    Device* m_controller;
private:
    bool m_isClosed = false; //State (closed or opened)
    bool m_isRequested = false; //If change state requested
    double m_reactionTimer = 0.0;

    const double m_reaction = 1.0; //Time after which lock will react (change state)
};

#endif // DOORLOCK_H
