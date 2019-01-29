#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "guicontroller.h"
#include "controller.h"

bool checkIP(std::string ip)
{
    int dots = 0;
    int digits = 0;

    for (unsigned i = 0; i < ip.length(); i++)
    {
        if (isdigit(ip[i]))
            digits++;

        else if (ip[i] == '.')
        {
            dots++;

            if (digits == 0 || digits > 3 || dots > 3)
                return false;

            digits = 0;
        }

        else
            return false;
    }

    if (dots != 3 || (digits > 3))
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    Controller *ctrl;

    if (argc < 2)
    {
        ctrl = new Controller(10000);
        ctrl -> run(true);
        GuiController::getController().setPtrToCtrl(*ctrl);
    }

    else
    {
        int port = 0;
        std::string ip;

        for (int i = 1; i < argc; i++)
        {
            if (checkIP(argv[i]))
                ip = argv[i];

            else if (isdigit(*argv[i]))
                port = atoi(argv[i]);
        }

        if (ip.empty() || port == 0)
        {
            std::cout << "Usage:\n./MainController\n./MainController <ip> <port>\n";
            return 0;
        }
        ctrl = new Controller(port, ip);
        ctrl -> run(true);
        GuiController::getController().setPtrToCtrl(*ctrl);
    }

    engine.rootContext() -> setContextProperty("GuiController", &GuiController::getController());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(&engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);
    return app.exec();
}
