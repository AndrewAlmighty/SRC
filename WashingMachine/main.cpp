#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "washingmachinecontroller.h"
#include "motor.h"
#include "pump.h"
#include "heater.h"
#include "doorlock.h"
#include "guimanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    WashingMachineController controller;
    DoorLock lock;
    Heater heater;
    Pump pump;
    Motor motor;

    controller.pointDoorLock(&lock);
    controller.pointHeater(&heater);
    controller.pointMotor(&motor);
    controller.pointPump(&pump);

    lock.pointController(&controller);
    heater.pointController(&controller);
    pump.pointController(&controller);
    motor.pointController(&controller);

    GuiManager::GetInstance().connectController(&controller);
    QQmlApplicationEngine engine;
    engine.rootContext() -> setContextProperty("GuiManager", &GuiManager::GetInstance());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
