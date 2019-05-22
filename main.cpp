#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QDate>
#include <QTime>

#include "fvideo.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/app.ico"));

    QQmlApplicationEngine engine;
    qmlRegisterType<FVideo>("FVideo", 1,0, "FVideo");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
