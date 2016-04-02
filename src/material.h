/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from QWIDGET to manipulate materials of 3D onject
 * ***************************************************************************************************************/
#ifndef MATERIAL_H
#define MATERIAL_H
#include<vector>
using namespace std;
#include <QWidget>

namespace Ui {
class material;
}

class material : public QWidget
{
    Q_OBJECT

public:
    explicit material(QWidget *parent = 0);
    ~material();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

private:
    Ui::material *ui;
    void computeResults(int A, int n);
    vector<double> yValues;
    double Y;
    QPolygonF polygon;
    void paintEvent(QPaintEvent *e);
    QColor colorAmbience, colorDiffuse, colorSpecular;  //feeds directly from color picker dialog
    QColor fracA, fracD, fracS; //from slider
signals:
    void amb(QColor);
    void spec(QColor);
    void diffuse(QColor);
};

#endif // MATERIAL_H
