#ifndef CENTRALHEATING_H
#define CENTRALHEATING_H

#include "../shared_src/device.h"

class CentralHeating : public Device
{
public:
  CentralHeating();
  CentralHeating(const int port, const std::string ip = "127.0.0.1");
  virtual ~CentralHeating();
  void disconnect();

protected:
  void handleMessage() override;
  void frame(double timeOffset) override;

private:
  void setTurnedOn(const bool enabled);
  void setDesiredTemp(const int temp);
  void setCurrentTemp(const int temp);

  const double m_heatingRate;

  bool m_turnedOn;
  int m_currentTemp;
  int m_desiredTemp;
};

#endif // CENTRALHEATING_H
