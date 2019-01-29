#include "device.h"
#include "timescale.h"

Device::Device() : m_isRunning(false), m_threadDelay(10)
{
}

void Device::run()
{
    m_isRunning = true;
    m_thread = std::thread{&Device::loop, this};
    m_thread.detach();
}

void Device::stop()
{
    if(m_isRunning)
    {
        m_isRunning = false;
        //m_thread.join();
    }
}

void Device::loop()
{
    typedef std::chrono::high_resolution_clock clock;
    auto timeBefore = clock::now();
    while(m_isRunning)
    {
        std::chrono::duration<double, std::milli> offset = clock::now() - timeBefore;
        timeBefore = clock::now();
        this->frame(offset.count() * Timescale::getScale() / 1000.0);
        std::this_thread::sleep_for(std::chrono::duration<long, std::milli>{m_threadDelay});
    }
}

void Device::setDelay(unsigned int delay)
{
    m_threadDelay = delay;
}
