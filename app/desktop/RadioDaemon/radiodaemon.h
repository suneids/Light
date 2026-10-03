#ifndef RADIODAEMON_H
#define RADIODAEMON_H

#include <QObject>
#include <QSerialPort>
#include <QQueue>
#include <QTimer>

#include <QLocalServer>
#include <QLocalSocket>

#include <QSet>
#include <QHash>

#include "protocol.h"


class RadioDaemon : public QObject
{
    Q_OBJECT

public:
    explicit RadioDaemon(QObject *parent = nullptr);

    bool start(const QString &portName, qint32 baudRate);
    void radioEnqueueLatest(const QByteArray &packet, const QString &key, const QString &name, int afterDelayMs);


private slots:
    void onSerialReadyRead();

    void onNewConnection();
    void onClientReadyRead();

    void radioOnTimeout();

private:
    QSerialPort serial;
    QTimer serialReconnectTimer;

    QString serialPortName;
    qint32 serialBaudRate = 9600;

    bool serialOnline = false;

    void tryOpenSerial();
    void handleSerialDisconnect();
    void setSerialOnline(bool online);

    QByteArray rxBuffer;

    QQueue<RadioTxJob> radioQueue;
    RadioTxJob currentRadioJob;

    bool radioBusy = false;
    bool radioWaitingResponse = false;

    QTimer radioDelayTimer;
    QTimer radioTimeoutTimer;


    QLocalServer server;

    QSet<QLocalSocket *> clients;
    QHash<QLocalSocket *, QByteArray> clientBuffers;


    void radioEnqueue(const RadioTxJob &job);

    void radioEnqueueSendOnly(
        const QByteArray &packet,
        const QString &name,
        int afterDelayMs
        );

    void radioEnqueueRequest(
        const QByteArray &packet,
        const QString &name,
        uint8_t expectId,
        uint8_t expectCmd,
        int timeoutMs,
        int afterDelayMs,
        bool isPoll
        );

    void radioKick();
    void radioStartJob(const RadioTxJob &job);
    void radioFinishCurrentJob(bool ok);

    void radioClearPollJobs();

    void parseRadioBuffer();

    void handleParsedPacket(
        uint8_t id,
        uint8_t cmd,
        const QByteArray &payload
        );


    void handleClientMessage(
        QLocalSocket *client,
        const QByteArray &message
        );

    void sendJson(
        QLocalSocket *client,
        const QJsonObject &object
        );

    void broadcastJson(
        const QJsonObject &object
        );
};

#endif
