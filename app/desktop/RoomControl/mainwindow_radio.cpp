#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"


void MainWindow::radioInitScheduler()
{
    greenhouse_pg->pollTimer.setInterval(5000);
    hygrometerPollTimer.setInterval(7000);


    connect(
        &greenhouse_pg->pollTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            /*
             * Это всё ещё логика RoomControl:
             * не спрашиваем теплицу во время LED-сцены
             * или активного управления гексаподом.
             */
            if(ledSceneSending || currentMove != 0u) {
                return;
            }


            QByteArray pkt =
                makePacket(
                    DEV_GREENHOUSE,
                    CMD_STATUS_REQUEST,
                    QByteArray()
                    );


            radioClient.request(
                pkt,
                "GREENHOUSE STATUS REQUEST",
                DEV_GREENHOUSE,
                CMD_STATUS_RESPONSE,
                700,
                200,
                true
                );
        }
        );


    connect(
        &hygrometerPollTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            if(ledSceneSending || currentMove != 0u) {
                return;
            }


            QByteArray pkt =
                makePacket(
                    DEV_HYGROMETER,
                    CMD_STATUS_REQUEST,
                    QByteArray()
                    );


            radioClient.request(
                pkt,
                "HYGROMETER STATUS REQUEST",
                DEV_HYGROMETER,
                CMD_STATUS_RESPONSE,
                700,
                200,
                true
                );
        }
        );


    greenhouse_pg->pollTimer.start();
    hygrometerPollTimer.start();
}



void MainWindow::sendHexapodMove()
{
    /*
     * Если страница гексапода закрыта —
     * вообще не посылаем команды движения.
     */
    if(ui->stwidget_section->currentWidget() != hexapod_pg) {
        return;
    }


    /*
     * Теперь проверяем не serial,
     * а соединение с radio-daemon.
     */
    if(!radioClient.isConnected()) {
        return;
    }


    if(currentMove == 0u &&
        lastHexapodMoveSent == 0u)
    {
        return;
    }


    QByteArray payload;

    payload.append(
        static_cast<char>(currentMove)
        );


    QByteArray pkt =
        makePacket(
            DEV_HEXAPOD,
            CMD_HEXAPOD_MOVE,
            payload
            );


    /*
     * ВАЖНО:
     *
     * движение гексапода нельзя просто складывать
     * в обычную FIFO, иначе daemon может накопить:
     *
     * forward
     * forward
     * left
     * stop
     *
     * и потом честно проиграть устаревшие движения.
     *
     * Поэтому это будет специальная команда:
     * "в очереди должно остаться только последнее
     * состояние управления".
     */
    radioClient.sendLatest(
        "hexapod-move",
        pkt,
        QString("HEXAPOD MOVE %1")
            .arg(currentMove, 2, 16, QChar('0')),
        0
        );


    lastHexapodMoveSent = currentMove;
}
