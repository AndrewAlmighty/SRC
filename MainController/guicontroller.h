//Class managing the GUI interface. Singleton

#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <QObject>
#include "controller.h"

class GuiController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int desiredCoverage READ desiredCoverage WRITE setDesiredCoverage NOTIFY desiredCoverageChanged)
    Q_PROPERTY(int currentCoverage READ currentCoverage WRITE setCurrentCoverage NOTIFY currentCoverageChanged)
    Q_PROPERTY(bool heatingOn READ heatingOn WRITE setHeatingOn NOTIFY heatingOnChanged)
    Q_PROPERTY(int currentTemp READ currentTemp WRITE setCurrentTemp NOTIFY currentTempChanged)
    Q_PROPERTY(int desiredTemp READ desiredTemp WRITE setDesiredTemp NOTIFY desiredTempChanged)
    Q_PROPERTY(bool heatingConnected READ heatingConnected WRITE setHeatingConnected NOTIFY heatingConnectedChanged)
    Q_PROPERTY(bool blindsConnected READ blindsConnected WRITE setBlindsConnected NOTIFY blindsConnectedChanged)
    Q_PROPERTY(QString blindsIP READ blindsIP WRITE setBlindsIP NOTIFY blindsIPChanged)
    Q_PROPERTY(QString heatingIP READ heatingIP WRITE setHeatingIP NOTIFY heatingIPChanged)

public:
    static GuiController & getController();

    Q_INVOKABLE void setDesiredCoverage(const int coverage, bool changedByUser = true);
    int desiredCoverage() const;

    Q_INVOKABLE void setCurrentCoverage(const int coverage);
    int currentCoverage() const;

    Q_INVOKABLE void setHeatingOn(const bool isHeating, bool changedByUser = true);
    bool heatingOn() const;

    Q_INVOKABLE void setCurrentTemp(const int temp);
    int currentTemp() const;

    Q_INVOKABLE void setDesiredTemp(const int temp, bool changedByUser = true);
    int desiredTemp() const;

    Q_INVOKABLE void setHeatingConnected(const bool isConnected);
    bool heatingConnected() const;

    Q_INVOKABLE void setBlindsConnected(const bool isConnected);
    bool blindsConnected() const;

    Q_INVOKABLE void setBlindsIP(const QString ip);
    QString blindsIP() const;

    Q_INVOKABLE void setHeatingIP(const QString ip);
    QString heatingIP() const;

    void setPtrToCtrl(Controller &ctrl);

signals:
    void desiredCoverageChanged();
    void currentCoverageChanged();
    void heatingOnChanged();
    void currentTempChanged();
    void desiredTempChanged();
    void heatingConnectedChanged();
    void blindsConnectedChanged();
    void blindsIPChanged();
    void heatingIPChanged();

private:
    GuiController(QObject *parent = nullptr);
    GuiController(const GuiController &);
    GuiController& operator=(const GuiController &);
    ~GuiController();
    void getIPandPort(QString &ip, int &port);

    Controller *m_ctrl;
    int m_desiredCoverage;
    int m_currentCoverage;
    int m_currentTemp;
    int m_desiredTemp;
    bool m_heatingOn;
    bool m_heatingConnected;
    bool m_blindsConnected;
    QString m_blindsIP;
    QString m_heatingIP;
};

#endif // GUICONTROLLER_H
