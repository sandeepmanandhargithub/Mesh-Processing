#ifndef GLLIGHTSPOS_H
#define GLLIGHTSPOS_H
/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from QWIDGET to manipulate one of the light source used in this program.
 * Sliders and a color Picker has been provided to maneuver
 * ******************************************************************************************************************/
#include <QWidget>
#include<QColorDialog>
namespace Ui {
class glLightsPos;
}

class glLightsPos : public QWidget
{
    Q_OBJECT

public:
    explicit glLightsPos(QWidget *parent = 0);
    ~glLightsPos();
private slots:
    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_Y_sliderMoved(int position);

    void on_horizontalSlider_Z_sliderMoved(int position);

    void on_pushButton_2_clicked();

private:
    Ui::glLightsPos *ui;

signals:
    posX(int);
    posY(int);
    posZ(int);
    void sendLightColor(QColor);
};

#endif // GLLIGHTSPOS_H
