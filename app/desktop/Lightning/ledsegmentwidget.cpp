#include "ledsegmentwidget.h"
#include "ui_ledsegmentwidget.h"

#include <QColorDialog>

LedSegmentWidget::LedSegmentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedSegmentWidget)
{
    ui->setupUi(this);
    ui->spin_diode_start->setRange(0, 294);
    ui->spin_diode_end->setRange(0, 294);

    ui->spin_diode_end->setValue(10);

    connect(ui->btn_color, &QPushButton::clicked, this, &LedSegmentWidget::chooseColor);
    connect(ui->btn_remove, &QPushButton::clicked, this, [this](){
        emit removeRequested(this);
    });

    updateColorPreview();
}


LedSegmentWidget::~LedSegmentWidget()
{
    delete ui;
}


int LedSegmentWidget::startIndex() const{
    return ui->spin_diode_start->value();
}


int LedSegmentWidget::endIndex() const{
    return ui->spin_diode_end->value();;
}


QColor LedSegmentWidget::color() const{
    return segmentColor;
}


void LedSegmentWidget::chooseColor(){
    QColor selected = QColorDialog::getColor(segmentColor, this, "Цвет точки");

    if(!selected.isValid()) return;

    segmentColor = selected;
    updateColorPreview();
}


void LedSegmentWidget::updateColorPreview(){
    ui->lbl_color_preview->setStyleSheet(QString(
        "background-color: rgb(%1, %2, %3);"
        "border: 1px solid #2A323D;"
        "border-radius: 4px;")
     .arg(segmentColor.red())
     .arg(segmentColor.green())
     .arg(segmentColor.blue()));
    ui->lbl_color_preview->setText(segmentColor.name(QColor::HexRgb).toUpper());
}
