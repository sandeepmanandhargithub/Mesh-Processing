#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "gllightspos.h"
#include "laplaciankind.h"
#include "material.h"
#include "plotter.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();
    void on_btnSmooth_clicked();

    void on_actionPosition_triggered();

    void on_pushButton_3_clicked();

    void on_eigenSpinner_valueChanged(int arg1);

    void on_actionEdit_Material_triggered();

    void on_actionLoad_triggered();

    void on_eigenComponentSlider_sliderMoved(int position);

    void on_pushButton_4_clicked();

    void on_actionCurvature_Analysis_triggered();

    void on_actionFLAT_triggered();

    void on_actionSMOOTH_triggered();

    void setStatus(QString);



    void on_pushButton_clicked();

    void on_actionExit_triggered();

    void on_actionSave_Laplace_Matrix_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    glLightsPos posLight;
    LaplacianKind selectLaplacian;
    plotter plotWin;
    material matWin;
    int eigenNumber;
signals:
    void sendEigenIndex(int);
    void sendCompressionVolume(int);
    void analyseCurve();
    void shadingMode(int);
};

#endif // MAINWINDOW_H
