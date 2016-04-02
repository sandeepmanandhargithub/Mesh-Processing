/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from QGLWIDGET
 * All the opnegl, mouse and keyboard stuff implemented here
 * *******************************************************************************************************************/
#ifndef MYGL_H
#define MYGL_H

#include <QGLWidget>
#include <math.h>
#include<GL/glu.h>
#include <QString>
#include<QTimer>
#include<QKeyEvent>
#include<QMatrix4x4>
#include "object3d.h"
#include "laplacianthing.h"
#include <QVector3D>
#include <QDebug>


class myGL : public QGLWidget
{
    Q_OBJECT
private:

    LaplacianThing mesh; bool isObjectLoaded;
    QString fname;
    GLfloat rotX, rotY, rotZ;
    QTimer *m_timer;
    int timeInterval;
    Vector3d eyePos, viewPos, upVec;
    float speed, zfar, znear ;
    int rotate, translate;
    //arcball thing
    float arcBallTransform[4][4];// = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    QVector3D axisVector, transVector; //axis and translation
    QVector3D prevArcBall, currentArcBall;
    QPoint currentMouse, prevMouse;
    float rotAngle;
    //Light thing
    GLfloat lightPos[4]; //last one makes it homogenous; 0 = directional light 1 = point light
    bool Light0 = true, Light1 = false, Light2 = false; //state of light
    //Materials
    GLfloat amb[4];// = {0, 0.4, 0, 1};
    GLfloat spec[4];// = {1, 0, 0, 1};
    GLfloat diffuse[4];// = {0, 0, 0.2, 1};
    GLfloat white[4];// = {0.8, 0.8, 0.8, 1};
    //Lights
    GLfloat lamb[4];// = {0, .9, 0, 1};


    float originalModelView[4][4];// = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    float changedModelView[4][4] ;//= {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
    QCursor cursorRotate;
    QCursor cursorZoom;
    bool grid;// = true;
    int typeLaplacian;// = 1; //1 : combinatorial; 2 : distance; 3 : harmonic

public:
    myGL(QWidget *parent = 0);
    QString msg;
    bool drawEigen = false, compressing = false, renderCurvature = false;
    bool translateKeyboard = false;
    bool prompt = false; //flag to display prompt
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void drawCube(float l);
    void drawTetrahedron();
    int DrawTalisman(float cx, float cy, float r, float start_angle, float arc_angle, int num_segments) ; int talismanID;
    void AdjustFrustum();
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

  //  void renderText( double x, double y, double z, const QString & str, const QFont & font = QFont(), int listBase = 2000 );
 //   void wheelEvent(QWheelEvent *e);
    void timeOut();


    int DrawGLGrid(int span); int gridID;
 //   void SetMouseMapping(QPoint mousePos); //q = real mouse coordinate


    //arcball thing
    QVector3D ArcBallMapping(QPoint mouse);
    void Motion(QPoint mouse);
    void LoadObject(QString &filename);
    void ScreenShot();
    void emitVertexCount(int count);
    void emitFaceCount(int count);
    void ShineOn();
    void Materialize();
    void DrawString(double x, double y, QString txt);
    void DrawFriends(set<int>);
    void Smooth(int);
    void waterMark(int);
       void setVertexName();
       void updateDisplay(QPoint);
        void saveState();
    signals:
    void vertexCount(int count);
    void faceCount(int count);
    void SendVector(VectorXd X);
    void Status(QString);

protected slots:
        void timeOutSlot();
        void AnalyseCurve();
        void SeekEigen(int);    //found if true
        void AdjustCompression(int);
       //save all the matrices;
        void Light0State(bool);
        void Light1State(bool);
        void Light2State(bool);
        void LightPosX(int x){
            lightPos[0] = double(x)/10;
            ShineOn();
            updateGL();

        }
        void LightPosY(int y){
            lightPos[1] = double(y)/10;
            ShineOn();
            updateGL();

        }
        void LightPosZ(int z){
            lightPos[2] = double(z)/10;
            ShineOn();
            updateGL();
        }
        void SelectLaplace(int i){
            Smooth(i);
        }



        void LightColor(QColor light){
            lamb[0] = light.redF(); lamb[1] = light.blueF(); lamb[2] = light.greenF();
        }

        void glAmb(QColor q){

            amb[0] = q.redF();
            amb[1] = q.greenF();
            amb[2] = q.blueF();
            amb[3] = q.alphaF();
            Materialize();
        }

        void glDiffuse(QColor q){

            diffuse[0] = q.redF();
            diffuse[1] = q.greenF();
            diffuse[2] = q.blueF();
            diffuse[3] = q.alphaF();
            Materialize();
        }

        void glSpec(QColor q){

            spec[0] = q.redF();
            spec[1] = q.greenF();
            spec[2] = q.blueF();
            spec[3] = q.alphaF();
            Materialize();
        }

        void selectShade(int i){
            if(i == 1) glShadeModel(GL_FLAT);
            else if(i == 2) glShadeModel(GL_SMOOTH);
            updateGL();
        }


};

#endif // MYGL_H
