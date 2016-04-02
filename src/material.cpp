#include "material.h"
#include "ui_material.h"
#include "cmath"
#include "QPainter"
#include <QDebug>
#include <QColorDialog>

#define PI 3.1415
material::material(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::material)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    ui->horizontalSlider_2->setMaximum(56);
    //at 20 percent
    ui->horizontalSlider->setValue(20);
    ui->horizontalSlider_2->setValue(16);
    ui->horizontalSlider_3->setValue(20);
}

material::~material()
{
    delete ui;
    computeResults(1, 1);
}

void material::computeResults(int A, int n){
    yValues.clear();
    polygon.clear();
    int X = -(ui->widget->width()/2)+50;
    int intv = ui->widget->width()/2;
    for(double i = -PI/2; i < PI/2; i+=PI/(intv)){
        X++;
        Y = -A*pow(cos(i),n)+50;
        yValues.push_back(Y);
        polygon<<QPointF(X,Y);
    }
}

void material::paintEvent(QPaintEvent *e){
    QPainter painter(this);
    QPen penHLines(QColor("#fafaf0"), 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen(penHLines);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = ui->widget->width();
    int h = ui->widget->height();
    painter.translate(w/2, h/2);
    painter.fillRect(QRect(-w/2+50, -w/2+50, w/2, w/2), QColor("#505050"));

    painter.drawPolyline(polygon);
    QPen bLines(QColor("#ffffff"), 0.5, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen(bLines);
    painter.drawRect(-w/2+50, -w/2+50, w/2, w/2);
    update();
}

void material::on_horizontalSlider_valueChanged(int value)  //ambience
{
    colorAmbience.setRedF(fracA.redF()*value/ui->horizontalSlider->maximum());
    colorAmbience.setGreenF(fracA.greenF()*value/ui->horizontalSlider->maximum());
    colorAmbience.setBlueF(fracA.blue()*value/ui->horizontalSlider->maximum());
        emit amb(colorAmbience);

}

void material::on_horizontalSlider_2_valueChanged(int value)    //specular
{
    this->computeResults(value, ui->horizontalSlider_3->value());
    colorSpecular.setRedF(fracS.redF()*value/ui->horizontalSlider_2->maximum());
    colorSpecular.setGreenF(fracS.greenF()*value/ui->horizontalSlider_2->maximum());
    colorSpecular.setBlueF(fracS.blueF()*value/ui->horizontalSlider_2->maximum());
    emit spec(colorSpecular);
}

void material::on_horizontalSlider_3_valueChanged(int value)    //diffuse
{
    this->computeResults(ui->horizontalSlider_2->value(), value);
    colorDiffuse.setRedF(fracD.redF()*value/ui->horizontalSlider_3->maximum());
    colorDiffuse.setGreenF(fracD.greenF()*value/ui->horizontalSlider_3->maximum());
    colorDiffuse.setBlueF(fracS.blueF()*value/ui->horizontalSlider_3->maximum());
    emit diffuse(colorDiffuse);
}

void material::on_toolButton_clicked()
{
    colorAmbience = QColorDialog::getColor(Qt::white, this);
    fracA = colorAmbience;
    emit amb(colorAmbience);
}

void material::on_toolButton_2_clicked()
{
    colorSpecular = QColorDialog::getColor(Qt::white, this);
    fracS = colorSpecular;
    emit spec(colorSpecular);
}

void material::on_toolButton_3_clicked()
{
    colorDiffuse = QColorDialog::getColor(Qt::white, this);
    fracD = colorDiffuse;
    emit diffuse(colorDiffuse);
}
