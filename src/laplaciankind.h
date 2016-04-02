/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from QWIDGET to select the laplacian type
 * **************************************************************************************************************/

#ifndef LAPLACIANKIND_H
#define LAPLACIANKIND_H

#include <QWidget>

namespace Ui {
class LaplacianKind;
}

class LaplacianKind : public QWidget
{
    Q_OBJECT

public:
    explicit LaplacianKind(QWidget *parent = 0);
    ~LaplacianKind();
    bool Sure;
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::LaplacianKind *ui;
signals:
    selectKind(int);
};

#endif // LAPLACIANKIND_H
