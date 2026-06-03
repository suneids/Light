#ifndef LEDSEGMENTWIDGET_H
#define LEDSEGMENTWIDGET_H

#include <QWidget>
#include <QColor>

namespace Ui {
class LedSegmentWidget;
}

class LedSegmentWidget : public QWidget
{
    Q_OBJECT

signals:
    void removeRequested(LedSegmentWidget *widget);

private slots:
    void chooseColor();

public:
    explicit LedSegmentWidget(QWidget *parent = nullptr);
    ~LedSegmentWidget();
    int startIndex() const;
    int endIndex() const;
    QColor color() const;

private:
    void updateColorPreview();

private:
    Ui::LedSegmentWidget *ui;
    QColor segmentColor = QColor(255, 255, 255);

};

#endif // LEDSEGMENTWIDGET_H
