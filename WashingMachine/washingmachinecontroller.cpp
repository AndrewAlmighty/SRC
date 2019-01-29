#include "washingmachinecontroller.h"
#include "guimanager.h"
#include "doorlock.h"
#include "pump.h"
#include "motor.h"
#include "heater.h"

#include <thread>
//#include <unistd.h>

WashingMachineController::WashingMachineController()
{
    this->setDelay(1);
}

void WashingMachineController::run()
{
    m_lock->run();
    m_heater->run();
    m_motor->run();
    m_pump->run();

    Device::run();
}

void WashingMachineController::acceptMessage(Message msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(msg);
}

void WashingMachineController::pointDoorLock(Device *lock)
{
    m_lock = lock;
}

void WashingMachineController::pointHeater(Device *heater)
{
    m_heater = heater;
}

void WashingMachineController::pointMotor(Device *motor)
{
    m_motor = motor;
}

void WashingMachineController::pointPump(Device *pump)
{
    m_pump = pump;
}

void WashingMachineController::checkMessageBox(Message &msg)
{
    msg.dev = dev_None;

    if (m_queue.empty())
        return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        msg = m_queue.front();
        m_queue.pop();
    }
}

void WashingMachineController::handleGui(const Message &msg)
{
    if (msg.dev == dev_Heater)
        GuiManager::GetInstance().setTemp(msg.integer);

    if (msg.dev == dev_Motor)
        GuiManager::GetInstance().setSpeed(msg.integer);

    if (m_doorLocked)
        GuiManager::GetInstance().setLockStatus("Locked");

    else
    {
        GuiManager::GetInstance().setLockStatus("Unlocked");
        GuiManager::GetInstance().setMachineStatus("Idle");
        return;
    }

    if (!m_timeEnded)
    {
        if (m_speedReached)
            GuiManager::GetInstance().setMachineStatus("Doing laundry");

        else if (!m_speedReached && m_tempReached)
            GuiManager::GetInstance().setMachineStatus("Prepared for laundry");

        else if (!m_tempReached && m_gotWater)
            GuiManager::GetInstance().setMachineStatus("Heating the water");

        else if (!m_gotWater)
            GuiManager::GetInstance().setMachineStatus("Pumping the water");
    }

    else
    {
        if (m_speedReached)
            GuiManager::GetInstance().setMachineStatus("Stopping the motor");

        else if (!m_speedReached && m_gotWater)
            GuiManager::GetInstance().setMachineStatus("Pumping out the water");

        else if (m_doorLocked)
            GuiManager::GetInstance().setMachineStatus("Unlocking door");
    }
}

void WashingMachineController::loop()
{
    qDebug() << "Starting washing mashine process.";
    Device::loop();
    resetSetup();
    qDebug() << "Ending washing mashine process.";
    GuiManager::GetInstance().setLaundry(false);    //unlock the button
}

void WashingMachineController::frame(double timeOffset)
{
    Message msg;
    msg.dev = dev_Controller;
    msg.integer = 1;    //order lock to close the door
    m_lock -> acceptMessage(msg);

    checkMessageBox(msg);

    if (!m_timeEnded)
    {
        //first, check message and react for it.
        if (!m_doorLocked && msg.dev == dev_DoorLock && msg.integer == 1)
            m_doorLocked = true;

        //next, fill washing machine with water
        else if (!m_gotWater && msg.dev == dev_Pump && msg.integer >= 42)
            m_gotWater = true;

       //heat the water
        else if (!m_tempReached && msg.dev == dev_Heater && msg.integer >= m_laundryTemp)
            m_tempReached = true;

        //if motor reached max speed
        else if (m_tempReached && msg.dev == dev_Motor && msg.integer >= m_laundrySpeed)
            m_speedReached = true;

        else if(m_speedReached)
        {
            m_timeElapsed += timeOffset;

            //until the washing time is ended
            if (m_timeElapsed >= static_cast<double>(m_laundryTime))
                m_timeEnded = true;
         }

        //lock the door if it's unlock
        if (!m_doorLocked)
            m_lock -> acceptMessage(Message{dev_Controller, 1});

        //start pumpin water
        else if (m_doorLocked && !m_gotWater)
            m_pump -> acceptMessage(Message{dev_Controller, 1});

        //start heating
        else if (m_gotWater && !m_tempReached)
            m_heater -> acceptMessage(Message{dev_Controller, m_laundryTemp});

        //start speeding up motor
        else if (m_tempReached)
            m_motor -> acceptMessage(Message{dev_Controller, m_laundrySpeed});
    }
    //when the laundry is m_finished, prepare to turn off.
    else
    {
        if(m_speedReached && msg.dev == dev_Motor && msg.integer == 0)
            m_speedReached = false;

        //pump out water
        else if (!m_speedReached && m_gotWater && msg.dev == dev_Pump && msg.integer == 0)
            m_gotWater = false;

        else if (!m_gotWater && m_doorLocked && msg.dev == dev_DoorLock && msg.integer == 0)
        {
            m_doorLocked = false;
            handleGui(msg);
            this->stop();
            return;
        }

        if (m_speedReached)
            m_motor -> acceptMessage(Message{dev_Controller, 0});

        else if (!m_speedReached && m_gotWater)
            m_pump -> acceptMessage(Message{dev_Controller, 2});

        else if (!m_gotWater && m_doorLocked)
            m_lock -> acceptMessage(Message{dev_Controller, 0});
    }

    handleGui(msg);
}

void WashingMachineController::setProgram(std::string mode)
{
    resetSetup();
    if (mode == "Fast laundry - 30°C")
    {
        m_laundryTime = 60;//1200; <- original time - 1200 sec -> 20 min
        m_laundryTemp = 30;
        m_laundrySpeed = 400;
    }

    else if (mode == "Everyday laundry - 40°C")
    {
        m_laundryTime = 120;//2400; <- original time - 2400 sec -> 40 min
        m_laundryTemp = 40;
        m_laundrySpeed = 300;
    }

    else if (mode == "Cotton - 60°C")
    {
        m_laundryTime = 150;//3600; <- original time - 3600 sec -> 60 min
        m_laundryTemp = 60;
        m_laundrySpeed = 200;
    }
}

void WashingMachineController::resetSetup()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty())
            m_queue.pop();
    }

    m_doorLocked = false;
    m_gotWater = false;
    m_tempReached = false;
    m_speedReached = false;
    m_timeEnded = false;
    m_finished = false;

    m_timeElapsed = 0.0;
    m_laundryTime = 0;
    m_laundryTemp = 0;
    m_laundrySpeed = 0;

    m_lock->stop();
    m_heater->stop();
    m_motor->stop();
    m_pump->stop();
}
