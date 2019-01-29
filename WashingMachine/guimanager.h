#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <QObject>
#include "washingmachinecontroller.h"

class GuiManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString machineStatus READ machineStatus WRITE setMachineStatus NOTIFY machineStatusChanged)
    Q_PROPERTY(QString mode READ mode WRITE setmode NOTIFY modeChanged)
    Q_PROPERTY(QString lockStatus READ lockStatus WRITE setLockStatus NOTIFY lockStatusChanged)
    Q_PROPERTY(int temp READ temp WRITE setTemp NOTIFY tempChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool laundry READ laundry WRITE setLaundry NOTIFY laundryChanged)

public:
    static GuiManager & GetInstance();

    Q_INVOKABLE void setMachineStatus(QString status);
    QString machineStatus() const;

    Q_INVOKABLE void setmode(QString mode);
    QString mode() const;

    Q_INVOKABLE void setLockStatus(QString status);
    QString lockStatus() const;

    Q_INVOKABLE void setLaundry(bool isRunning);
    bool laundry() const;

    Q_INVOKABLE void setSpeed(int speed);
    int speed() const;

    Q_INVOKABLE void setTemp(int temp);
    int temp() const;

    void connectController(WashingMachineController *WMCtrl);
signals:
    void machineStatusChanged();
    void modeChanged();
    void lockStatusChanged();
    void laundryChanged();
    void tempChanged();
    void speedChanged();

private:
    GuiManager(QObject *parent = nullptr);
    GuiManager(const GuiManager &);
    GuiManager& operator =(const GuiManager&);

    WashingMachineController *m_controller;
    QString m_machineStatus;
    QString m_laundryMode;
    QString m_lockStatus;
    int m_speed;
    int m_temp;
    bool m_laundry;
};

#endif // GUIMANAGER_H
