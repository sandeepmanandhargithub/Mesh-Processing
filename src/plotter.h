/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from QWidet
 * plot the osicllation of eigenvectors
 * still experimental
 * **************************************************************************************************************/
#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>
#include<Eigen/Core>
using namespace Eigen;
using namespace std;
namespace Ui {
class plotter;
}

class plotter : public QWidget
{
    Q_OBJECT

public:
    explicit plotter(QWidget *parent = 0);
    ~plotter();

private:
    Ui::plotter *ui;

    QPolygonF polygon;
    void computeResults(VectorXd X);
    void paintEvent(QPaintEvent *e);
protected slots:
    void getVector(VectorXd X){
        if(X.rows() == 0) return;
        computeResults(X);
    }
};

#endif // PLOTTER_H
