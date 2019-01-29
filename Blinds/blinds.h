#ifndef BLINDS_H
#define BLINDS_H

#include "../shared_src/device.h"

class Blinds : public Device
{
public:
    Blinds();
    Blinds(const int port, const std::string ip = "127.0.0.1");
    virtual ~Blinds();
    void disconnect();

protected:
    void handleMessage() override;
    void frame(double timeOffset) override;

private:
    void setCurrentCoverage(const int coverage);
    void setDesiredCoverage(const int coverage);

    const double m_coverageSpeed;
    int m_currentCoverage;
    int m_desiredCoverage;
};

#endif // BLINDS_H
