#include "plotter.h"
#include "ui_plotter.h"
#include<QDebug>
#include "cmath"
#include "QPainter"
plotter::plotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter)
{
    ui->setupUi(this);
}

plotter::~plotter()
{
    delete ui;
}

void plotter::computeResults(VectorXd X)
{
    polygon.clear();
  //  qDebug()<<"getting X hahaha"<<X.rows();
    int n = X.rows();
    int w = ui->widget->width();
    int h = ui->widget->height();
    int a = 0,j=0;
    double minima = X.minCoeff();
    double maxima = X.maxCoeff();
    for(double i = 0; i < w; i += (double)w/n){
 //   for(int i = 0; i < n; i++){
        double x = X.coeffRef(j);
        j++;
        x = (h/2)*(x - minima)/(maxima - minima) - h/2; //scaling function bad
                polygon<<QPointF(i,x);
    }
}



void plotter::paintEvent(QPaintEvent *e){

    QPainter painter(this);
    QPen penHLines(QColor("#FF0000"), 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen(penHLines);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = ui->widget->width();
    int h = ui->widget->height();
    painter.translate(1, h/2);
    //painter.fillRect(QRect(0, 0, w, h), QColor("#505050"));
    painter.drawPolyline(polygon);

    update();
}
