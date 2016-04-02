#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include<QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->tab_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->btnSmooth->setEnabled(false);
    ui->menuExperimenta->setEnabled(false);
    ui->actionSave_Laplace_Matrix->setEnabled(false);
    connect(ui->glWidget, SIGNAL(faceCount(int)), ui->faceCount, SLOT(display(int)));
    connect(ui->glWidget, SIGNAL(vertexCount(int)), ui->vertexCount, SLOT(display(int)));
    connect(ui->radioButton, SIGNAL(toggled(bool)), ui->glWidget, SLOT(Light0State(bool)));
    connect(ui->radioButton_2, SIGNAL(toggled(bool)), ui->glWidget, SLOT(Light1State(bool)));
    connect(ui->radioButton_3, SIGNAL(toggled(bool)), ui->glWidget, SLOT(Light2State(bool)));

    connect(ui->glWidget,SIGNAL(SendVector(VectorXd)), &plotWin, SLOT(getVector(VectorXd)));

    connect(&posLight, SIGNAL(posX(int)), ui->glWidget, SLOT(LightPosX(int)));
    connect(&posLight, SIGNAL(posY(int)), ui->glWidget, SLOT(LightPosY(int)));
    connect(&posLight, SIGNAL(posZ(int)), ui->glWidget, SLOT(LightPosZ(int)));

    connect(&matWin, SIGNAL(amb(QColor)), ui->glWidget, SLOT(glAmb(QColor)));
    connect(&matWin, SIGNAL(diffuse(QColor)), ui->glWidget, SLOT(glDiffuse(QColor)));
    connect(&matWin, SIGNAL(spec(QColor)), ui->glWidget, SLOT(glSpec(QColor)));
    connect(&posLight, SIGNAL(sendLightColor(QColor)), ui->glWidget, SLOT(LightColor(QColor)));


    connect(&selectLaplacian, SIGNAL(selectKind(int)), ui->glWidget, SLOT(SelectLaplace(int)));
    connect(this, SIGNAL(sendEigenIndex(int)), ui->glWidget, SLOT(SeekEigen(int)));
    connect(this, SIGNAL(sendCompressionVolume(int)), ui->glWidget, SLOT(AdjustCompression(int)));
    connect(this, SIGNAL(analyseCurve()), ui->glWidget, SLOT(AnalyseCurve()));
    connect(ui->glWidget, SIGNAL(Status(QString)), this, SLOT(setStatus(QString)));
    connect(this, SIGNAL(shadingMode(int)), ui->glWidget, SLOT(selectShade(int)));
}

MainWindow::~MainWindow()
{
    matWin.close();
    posLight.close();
    selectLaplacian.close();
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_btnSmooth_clicked()
{
    selectLaplacian.Sure = false;
    selectLaplacian.show();
    ui->eigenSpinner->setMaximum(ui->vertexCount->value() - 1);
    ui->eigenComponentSlider->setMaximum(ui->vertexCount->value()-1);
    ui->eigenComponentSpin->setMaximum(ui->vertexCount->value()-1);
    ui->tab_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->menuExperimenta->setEnabled(true);
    ui->actionSave_Laplace_Matrix->setEnabled(true);
}

void MainWindow::on_actionPosition_triggered()
{
    posLight.show();
}

void MainWindow::on_pushButton_3_clicked()
{

    emit sendEigenIndex(ui->eigenSpinner->value());
}

void MainWindow::on_eigenSpinner_valueChanged(int arg1)
{
        //no need
}

void MainWindow::on_actionEdit_Material_triggered()
{
    matWin.show();
}

void MainWindow::on_actionLoad_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open VRML file"), "..//vrml", tr("Image Files (*.wrl);;Text files(*.txt)"));
    if(filename == NULL) return;
    ui->glWidget->LoadObject(filename);
    this->setWindowTitle(filename);
    ui->btnSmooth->setEnabled(true);
}

void MainWindow::on_eigenComponentSlider_sliderMoved(int position)
{
    emit sendCompressionVolume(ui->eigenComponentSlider->value());
}

void MainWindow::on_pushButton_4_clicked()
{
    emit sendCompressionVolume(ui->eigenComponentSlider->value());
    plotWin.show();
}

void MainWindow::on_actionCurvature_Analysis_triggered()
{
    emit analyseCurve();
}

void MainWindow::on_actionFLAT_triggered()
{
    emit shadingMode(1);
}

void MainWindow::on_actionSMOOTH_triggered()
{
    emit shadingMode(2);
}

void MainWindow::setStatus(QString txt)
{
    ui->status->setText(txt);
}

void MainWindow::on_pushButton_clicked()
{
    ui->glWidget->waterMark(2);
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionSave_Laplace_Matrix_triggered()
{
    ui->glWidget->saveState();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
     msgBox.setText("About this software");
     msgBox.setInformativeText("Developers: \nSandeep Manandhar\n Peixi Li \n Univeristy of Burgundy \n Le Creusot, France");
     msgBox.setStandardButtons(QMessageBox::Ok);
     msgBox.setDefaultButton(QMessageBox::Ok);
     msgBox.exec();
}
