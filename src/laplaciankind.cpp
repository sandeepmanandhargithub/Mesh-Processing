#include "laplaciankind.h"
#include "ui_laplaciankind.h"

LaplacianKind::LaplacianKind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LaplacianKind)
{
    Sure = false;
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
}

LaplacianKind::~LaplacianKind()
{
    delete ui;
}

void LaplacianKind::on_comboBox_currentIndexChanged(int index)
{

}

void LaplacianKind::on_pushButton_2_clicked()
{
    Sure = true;
    emit selectKind(ui->comboBox->currentIndex());
    this->close();
}

void LaplacianKind::on_pushButton_clicked()
{
    Sure = false;
    this->close();
}
