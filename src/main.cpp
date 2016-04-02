/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * MSCV6 first semester
 * Software Engineer project
 *Introduction to Spectral Mesh Analysis :
* Toward a simple implementation in C++
 * Supervised by Dr. Yohan Fougerolle
 * Uses OpenGL and Eigen library
 * Only loads VRML 1.0 files
 *
 * **************************************************************************************************************/
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
