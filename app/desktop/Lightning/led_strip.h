#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <QWidget>
#include "ledpointwidget.h"
#include "ledsegmentwidget.h"

namespace Ui {
class LedStrip;
}

class LedStrip : public QWidget
{
    Q_OBJECT

public:
    explicit LedStrip(QWidget *parent = nullptr);
    ~LedStrip();
signals:
    void lightPreparePacket(int r, int g, int b, int r_scale, int g_scale, int b_scale,
                            int cmd, uint8_t speed, uint16_t period, uint8_t brightness);
    void prepareLedScenePackets(QWidget *itemsParent, int r_scale,int g_scale,int b_scale, uint8_t brightness);
private:
    Ui::LedStrip *ui;
    QColor lightColor = QColor(0, 204, 204);

    void updateLightColorPreview();
};

#endif // LED_STRIP_H
