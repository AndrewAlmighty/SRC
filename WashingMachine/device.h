#ifndef DEVICE_H
#define DEVICE_H

#include <thread>
#include <chrono>
#include <QDebug>

#include "message.h"

class Device
{
public:
    Device();
    virtual     ~Device() = default;

    virtual void acceptMessage(Message message) = 0;

    virtual void run(); //Starting the thread
    virtual void stop(); //Stopping the loop and joining thread

    void setDelay(unsigned int delay);
protected:
    //Time offset in seconds
    virtual void frame(double timeOffset) = 0; //Called in every frame with time offset between each frame
    virtual void loop(); //Creating while loop

    bool        m_isRunning;
private:
    unsigned int m_threadDelay;
    std::thread m_thread;
};

#endif // DEVICE_H
