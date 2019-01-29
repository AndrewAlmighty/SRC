#ifndef WASHINGMACHINECONTROLLER_H
#define WASHINGMACHINECONTROLLER_H

#include <string>
#include <queue>
#include <mutex>

#include "device.h"

class DoorLock;
class Heater;
class Motor;
class Pump;

class WashingMachineController : public Device
{
public:
    WashingMachineController();

    virtual void acceptMessage(Message msg) override;
    virtual void run() override;

    void setProgram(std::string mode);
    void pointDoorLock(Device *lock);
    void pointHeater(Device *heater);
    void pointMotor(Device *motor);
    void pointPump(Device *pump);

private:
    virtual void frame(double timeOffset) override;
    virtual void loop() override;

    void checkMessageBox(Message &msg);
    void handleGui(const Message &msg);
    void resetSetup();

    std::queue<Message> m_queue;
    std::mutex m_mutex;

    Device *m_lock;
    Device *m_heater;
    Device *m_motor;
    Device *m_pump;

    bool m_doorLocked;
    bool m_gotWater;
    bool m_tempReached;
    bool m_speedReached;
    bool m_timeEnded;
    bool m_finished;

    double m_timeElapsed;

    int m_laundryTime;
    int m_laundryTemp;
    int m_laundrySpeed;
};

#endif // WASHINGMACHINECONTROLLER_H
