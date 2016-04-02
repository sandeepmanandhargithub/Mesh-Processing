#include "gllightspos.h"
#include "ui_gllightspos.h"

glLightsPos::glLightsPos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::glLightsPos)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
}

glLightsPos::~glLightsPos()
{
    delete ui;
}

void glLightsPos::on_horizontalSlider_sliderMoved(int position)
{
    emit posX(position);
}

void glLightsPos::on_horizontalSlider_Y_sliderMoved(int position)
{
    emit posY(position);
}

void glLightsPos::on_horizontalSlider_Z_sliderMoved(int position)
{
    emit posZ(position);
}

void glLightsPos::on_pushButton_2_clicked()
{
    QColor lightColor = QColorDialog::getColor(Qt::white, this);
    emit sendLightColor(lightColor);
}
