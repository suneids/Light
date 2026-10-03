#ifndef LEDPOINTWIDGET_H
#define LEDPOINTWIDGET_H

#include <QWidget>
#include <QColor>

namespace Ui {
class LedPointWidget;
}

class LedPointWidget : public QWidget
{
    Q_OBJECT

signals:
    void removeRequested(LedPointWidget *widget);

private slots:
    void chooseColor();

public:
    explicit LedPointWidget(QWidget *parent = nullptr);
    ~LedPointWidget();
    int index() const;
    QColor color() const;

private:
    void updateColorPreview();

private:
    Ui::LedPointWidget *ui;
    QColor pointColor = QColor(255, 255, 255);

};

#endif // LEDPOINTWIDGET_H
