#include <QCoreApplication>
#include <QDebug>

#include "radiodaemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString port =
        "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0";

    qint32 baud = 9600;

    if(argc >= 2)
        port = QString::fromLocal8Bit(argv[1]);

    if(argc >= 3) {
        bool ok = false;
        qint32 value =
            QString::fromLocal8Bit(argv[2]).toInt(&ok);

        if(!ok) {
            qCritical() << "Invalid baud rate";
            return 1;
        }

        baud = value;
    }

    RadioDaemon daemon;

    if(!daemon.start(port, baud))
        return 1;

    return app.exec();
}
