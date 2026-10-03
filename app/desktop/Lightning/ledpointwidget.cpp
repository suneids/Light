#include "ledpointwidget.h"
#include "ui_ledpointwidget.h"

#include <QColorDialog>

LedPointWidget::LedPointWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedPointWidget)
{
    ui->setupUi(this);
    ui->spin_diode_number->setRange(0, 294);

    connect(ui->btn_color, &QPushButton::clicked, this, &LedPointWidget::chooseColor);
    connect(ui->btn_remove, &QPushButton::clicked, this, [this](){
        emit removeRequested(this);
    });

    updateColorPreview();
}


LedPointWidget::~LedPointWidget()
{
    delete ui;
}


int LedPointWidget::index() const{
    return ui->spin_diode_number->value();
}


QColor LedPointWidget::color() const{
    return pointColor;
}


void LedPointWidget::chooseColor(){
    QColor selected = QColorDialog::getColor(pointColor, this, "Цвет точки");

    if(!selected.isValid()) return;

    pointColor = selected;
    updateColorPreview();
}


void LedPointWidget::updateColorPreview(){
    ui->lbl_color_preview->setStyleSheet(QString(
        "background-color: rgb(%1, %2, %3);"
        "border: 1px solid #2A323D;"
        "border-radius: 4px;")
     .arg(pointColor.red())
     .arg(pointColor.green())
     .arg(pointColor.blue()));
    ui->lbl_color_preview->setText(pointColor.name(QColor::HexRgb).toUpper());
}
