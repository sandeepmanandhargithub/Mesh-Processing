#include "mygl.h"
#include <QDebug>
#include "reader3d.h"
#include <QElapsedTimer>
#define BUFFER_SIZE 256
#define RENDERVERTEX 0
#define RENDERPOLY 1
#define RENDEREIGEN 2
#define RENDERCOMPRESSION 4
#define RENDERCURVATURE 5
unsigned int PickBuff[BUFFER_SIZE];
int RenderMode;
QElapsedTimer timer;
myGL::myGL(QWidget *parent) :
    QGLWidget(parent)
{
    QGLFormat fmt;
    fmt.setSwapInterval(1);
    
    timeInterval = 50;
    speed = 0.05;
    rotX = rotY = rotZ = 0;
    lightPos[0] = 0; lightPos[1] = 0;
    lightPos[2] = 3; lightPos[3] = 1;
    eyePos = Vector3d(0, 0, 10);
    viewPos = Vector3d(0, 0, 0);
    zfar = 0.01, znear =1000.0f;
    isObjectLoaded = false;
    drawEigen = false; compressing = false; renderCurvature = false;
    translateKeyboard = false;
    prompt = false; //flag to display prompt
    Light0 = true; Light1 = false;Light2 = false;
    amb[0] = 0.7; amb[1] = 0.9; amb[2] = 0.5; amb[3] = 1;
    spec[0] = 0.9; spec[1] = 0; spec[2] = 0; spec[3] = 1;
    diffuse[0] = 0.3; diffuse[1] = 0.9; diffuse[2] = 0.3; diffuse[3] = 1;
    white[0] = 0.9; white[1] = 0.9; white[2] = 0.9; white[3] = 1;
    lamb[0] = 0.7; lamb[1] = 0.7; lamb[2] = 0.4; lamb[3] = 1;
    // arcball =identity matrix
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++){
            if(i == j) arcBallTransform[i][i] = 1; continue;
            arcBallTransform[i][j] = 0;
        }

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++){
            if(i == j) originalModelView[i][i] = 1; continue;
            originalModelView[i][j] = 0;
        }
    //for multiple cameras implementation
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++){
            if(i == j) changedModelView[i][i] = 1; continue;
            changedModelView[i][j] = 0;
        }
    glGetFloatv(GL_MODELVIEW, (GLfloat *)originalModelView);
//to do:
    grid = true;
    typeLaplacian = 1;

//very very important
    if(timeInterval = 0) m_timer = 0;
        else{
            m_timer = new QTimer(this);
            connect(m_timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()) );
            m_timer->start(timeInterval);
        }
}


void myGL::initializeGL(){
    
    glPolygonMode(GL_FRONT,GL_FILL);
    glShadeModel(GL_FLAT); //GL_FLAT initially
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE); //required when rotation causes normal to change
    //  glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);//depth comparisons
    glDepthFunc(GL_LEQUAL);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_POINT_SMOOTH );
    glHint( GL_POINT_SMOOTH, GL_NICEST );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    talismanID = DrawTalisman(0,0, 2.5/640*this->width(), 0, 450, 360);
    gridID = DrawGLGrid(15);
    /*************lets put some lights shall we********************/
    ShineOn(); //working; added color picking
    /************************************************************/
    Materialize(); //not working; with sliders, check for sanity
}


void myGL::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*****************Trying some fancy stuff*************************/
    if(grid)    msg = QString("Grid On");
    else      msg = QString("Grid Off");
    DrawString(5, 10, msg);
    //not working during computation; To do: may be threads
    /****************************************************************/
    glMatrixMode(GL_MODELVIEW); //all the affine tranformation elements here
    glRotatef( rotAngle, axisVector.x(), axisVector.y(), axisVector.z() );
    glMultMatrixf((GLfloat *)arcBallTransform);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)arcBallTransform);
    //keep this modelview matrix for next sequence of transformation

    rotAngle = 0;
    axisVector = QVector3D(0,0,0);
    //surpressing all the rotation for now
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity(); //try removing this thing and suffer
                   //keeps traslation and rotation relative to origin
    glTranslatef(transVector.x(), transVector.y(), transVector.z());
    glMultMatrixf((GLfloat *)arcBallTransform);

    if(grid)
        glCallList(2);
    if(mesh.isP2PReady) {
/**********************************The drawing of Mesh*********************************/
 if(!drawEigen && !compressing && !renderCurvature)
            mesh.DrawObject(3); //normal mode

    }
    if(drawEigen && !compressing && !renderCurvature){
        mesh.DrawObject(RENDEREIGEN); //RENDEREIGEN = 2
    }
    else if(!drawEigen && compressing && !renderCurvature){
        mesh.DrawObject(RENDERCOMPRESSION); //RENDERCOMPRESSION = 4;
    }
    else if(!drawEigen && !compressing && renderCurvature){
        mesh.DrawObject(RENDERCURVATURE); //experimental
    }

    glDisable(GL_COLOR_MATERIAL);
/************************************************************************************/

/***********************************The rotation tracker****************************/
    glLoadIdentity();   //make current matrix Identity
                    //vital for affine transformations to be local to origin
    glPushMatrix();
    glDisable(GL_LIGHTING); //no lights please
    glViewport(0, 0, this->width()/4, this->height()/4);
    if(rotate == 1){
        //imitating arcball rotation
        glRotatef( rotAngle, axisVector.x(), axisVector.y(), axisVector.z() );
        glMultMatrixf((GLfloat *)arcBallTransform);
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)arcBallTransform);
    }
    glCallList(talismanID); //the rotation tracker
    DrawString(5, 10, msg);
    if(prompt == true){
        if(mesh.isAmatrixReady) DrawString(5,20, "Laplace matrix has been Cooked");
        else DrawString(5, 20, "Laplace matrix not ready");
        if(mesh.canIgetEigen) DrawString(5,30, "Eigen Decomposition Succeded");
        else DrawString(5, 30, "No eigen decomposition");

    }
    glPopMatrix();
    glViewport(0,0, this->width(), this->height());

}

void myGL::resizeGL(int w, int h){
    h == 0? h:1;

    GLfloat aspect = (GLfloat)w/(GLfloat)h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    
    gluPerspective(45.0f, aspect, zfar, znear);
    gluLookAt(eyePos[0], eyePos[1], eyePos[2],//0, 0, 10,
              viewPos[0], viewPos[1], viewPos[2],//0, 0, 0,
              0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myGL::timeOut(){
//may be some animation later
updateGL();
}

void myGL::keyPressEvent(QKeyEvent *e){
    
    switch(e->key()){
    case Qt::Key_Down :
        translateKeyboard = true;
        translate = rotate = false;
        Motion(QPoint(-2,0));   //down 0
        break;
    case Qt::Key_Up :
        translateKeyboard = true;
        translate = rotate = false;
        Motion(QPoint(2,0));    //up 1
        break;
        
    case Qt::Key_Left:
        translateKeyboard = true;
        translate = rotate = false;
        Motion(QPoint(-2,1)); //left 2
        break;
        
    case Qt::Key_Right:
        translateKeyboard = true;
        translate = rotate = false;
        Motion(QPoint(2,1)); //right 4
        break;
        
    case Qt::Key_G:
        grid = !grid;
        break;
    case Qt::Key_P:
        prompt = !prompt; //no use for now, may be later for gl viewport warnings and prompts
        break;
    case Qt::Key_S:
        ScreenShot();
        break;
    default:
        break;
    }
}

void myGL::timeOutSlot(){
    timeOut();
}

void myGL::AnalyseCurve()
{
    if(!mesh.isAmatrixReady) return;
    mesh.curvature();
    renderCurvature = true;
    drawEigen = false;
    compressing = false;
}

void myGL::SeekEigen(int n)
{   qDebug()<<"fetching eigen "<<n;
    if(mesh.SeekAndDestroy(n)){ //nth eigen vector
        qDebug()<<"ready to draw";
        drawEigen = true;
        compressing = false;
        renderCurvature = false;
        return;
    }
    drawEigen = false;
}

void myGL::AdjustCompression(int k){
   // qDebug()<<"compression level: "<<k;
    if(mesh.SmallLap(k)){
        emit SendVector(mesh.pfX);
        compressing = true;
        drawEigen = false;
        renderCurvature = false;
        return;
    }
    compressing = false;
}

void myGL::saveState()
{
    mesh.SaveMatrix();
}

void myGL::Light0State(bool state)
{
    
    Light0 = !Light0;
   // qDebug()<<Light0;
    ShineOn();
    Materialize();
    // std::cout<<Light0;
}
void myGL::Light1State(bool state)
{
    Light1 = !Light1;
    ShineOn();
    Materialize();
}
void myGL::Light2State(bool state)
{
    Light2 = state;
    ShineOn();
    Materialize();
}

void myGL::drawCube(float l){
    GLfloat alpha = 0.7;
    glBegin(GL_LINE_LOOP);
    glColor4f(0.1f,0.1f,0.1f, alpha);
    glVertex3f( l*1.0f, l*1.0f,l*-1.0f);
    glVertex3f(l*-1.0f, l*1.0f,l*-1.0f);
    glVertex3f(l*-1.0f, l*1.0f, l*1.0f);
    glVertex3f( 1.0*l, 1.0*l, 1.0*l);
    glEnd();
    glBegin(GL_LINE_LOOP);
    
    glVertex3f( 1.0*l,-1.0*l, 1.0*l);
    glVertex3f(-1.0*l,-1.0*l, 1.0*l);
    
    glVertex3f(-1.0*l,-1.0*l,-1.0*l);
    
    glVertex3f( 1.0*l,-1.*l,-1.*l);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f( 1.*l, 1.*l, 1.*l);
    
    glVertex3f(-1.*l, 1.*l, 1.*l);
    glVertex3f(-1.*l,-1.*l, 1.*l);
    glVertex3f( 1.*l,-1.*l, 1.*l);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f( 1.*l,-1.*l,-1.*l);
    glVertex3f(-1.*l,-1.*l,-1.*l);
    glVertex3f(-1.*l, 1.*l,-1.*l);
    glVertex3f( 1.*l, 1.*l,-1.*l);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor4f(0.9f,0.1f,0.1f, 0.7f);
    glVertex3f( l*1.0f, l*1.0f,l*-1.0f);glVertex3f(l*-1.0f, l*1.0f,l*-1.0f);glVertex3f(l*-1.0f, l*1.0f, l*1.0f);glVertex3f( 1.0*l, 1.0*l, 1.0*l);
    glColor4f(0.1f,0.91f,0.1f, 0.7f);
    glVertex3f( 1.0*l,-1.0*l, 1.0*l);glVertex3f(-1.0*l,-1.0*l, 1.0*l);glVertex3f(-1.0*l,-1.0*l,-1.0*l);glVertex3f( 1.0*l,-1.*l,-1.*l);
    glColor4f(0.1f,0.1f,0.91f, 0.7f);
    glVertex3f( 1.*l, 1.*l, 1.*l);glVertex3f(-1.*l, 1.*l, 1.*l);glVertex3f(-1.*l,-1.*l, 1.*l);glVertex3f( 1.*l,-1.*l, 1.*l);
    glColor4f(0.9f,0.9f,0.9f, 0.7f);
    glVertex3f( 1.*l,-1.*l,-1.*l);glVertex3f(-1.*l,-1.*l,-1.*l);glVertex3f(-1.*l, 1.*l,-1.*l);glVertex3f( 1.*l, 1.*l,-1.*l);
    glVertex3f(-1.*l, 1.*l, 1.*l);    glVertex3f(-1.*l, 1.*l,-1.*l);    glVertex3f(-1.*l,-1.*l,-1.*l);    glVertex3f(-1.*l,-1.*l, 1.*l);
    glVertex3f( 1.*l, 1.*l,-1.*l);    glVertex3f( 1.*l, 1.*l, 1.*l);    glVertex3f( 1.*l,-1.*l, 1.*l);    glVertex3f( 1.*l,-1.*l,-1.*l);
    glEnd();
}




int myGL::DrawTalisman(float cx, float cy, float r, float start_angle, float arc_angle, int num_segments)
{
    //gludisk would have done better but fine and late for now
    int id_list = glGenLists(1);
    glNewList(id_list, GL_COMPILE_AND_EXECUTE);
    GLfloat alpha = 0.7;
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    float theta = arc_angle / float(num_segments - 1);//theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open
    
    float tangetial_factor = tanf(theta);
    
    float radial_factor = cosf(theta);
    
    
    float x = r * cosf(start_angle);//we now start at the start angle
    float y = r * sinf(start_angle);
    
    
    glColor4f(1,1,1, alpha);
    glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
    for(int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2*3.1415926*ii/num_segments;
        float x = (r -.5/640*this->width())*cos(theta);
        float y = (r-.5/640*this->width())*sin(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
    glColor4f(.7,.7,.7, alpha);
    glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
    for(int ii = 0; ii < num_segments; ii++)
    {
        glVertex2f(x + cx, y + cy);
        
        float tx = -y;
        float ty = x;
        
        x += tx * tangetial_factor;
        y += ty * tangetial_factor;
        
        x *= radial_factor;
        y *= radial_factor;
    }
    glEnd(); 
    glTranslatef(0, 0, -r*.5);
    drawCube(r*.5);
    glColor4f(1,0,0, alpha);
    glEnable(GL_DEPTH_TEST);
    
    // glLoadIdentity();
    glTranslatef(0, 0, 0);
    glRotatef(90, 0, 0, 1);
    // glDisable(GL_DEPTH); //use this and suffer
    glPopMatrix();
    glEndList();
    return id_list;
}
void myGL::AdjustFrustum()
{
    /*To do: messes with the rotation tracker
     * implement proper pushing poping and glmatrixmult
     * */
    Vector3d Pmin(mesh.Vertex(0)), Pmax(mesh.Vertex(0));
    Vector3d center(0,0,0);
    for(int i = 0; i < mesh.VertexNumber(); i++){
        Vector3d P(mesh.Vertex(i));
        for(int j = 0; j < 2; j++){
            Pmin[j] = min(Pmin[j], P[j]); //find minimum of values be it x, y or z
            Pmax[j] = max(Pmax[j], P[j]);
        }
        center += P;
    }
    center /= mesh.VertexNumber(); //take average
    viewPos = center;
    double distance = (Pmax - Pmin).norm();
    eyePos = viewPos + Vector3d(0, 0, distance *4);
    znear = distance*0.1;
    zfar = distance*5;

    //gluLookAt(cam.camPos.x(), cam.camPos.y(), cam.camPos.z(),
        //                  cam.camView.x(), cam.camView.y(), cam.camView.z(),
        //                  cam.camUpVector.x(), cam.camUpVector.y(), cam.camUpVector.z());

}




void myGL::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() != Qt::LeftButton && e->buttons() != Qt::RightButton){
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if(e->buttons() == Qt::LeftButton){
        Motion(e->pos());
        cursorRotate = QCursor(QPixmap(":/cursorImg/mouseRot.png"));
        this->setCursor(cursorRotate);
    }
    else{
        rotate = 0; //no rotation anymore

    }
    if(e->buttons() == Qt::RightButton && translate){
        Motion(e->pos());
        cursorZoom = QCursor(QPixmap(":/cursorImg/mouseZoom.png")); //something nice
        this->setCursor(cursorZoom);
        // qDebug()<<"right moving";
    }
    else{
        translate = 0;

    }
}

void myGL::mouseReleaseEvent(QMouseEvent *e)
{
    this->setCursor(Qt::ArrowCursor); //when no rotation or zooming
}
void myGL::mousePressEvent(QMouseEvent *e){
    rotate = 0; translate = 0;  //resetting states
    if(e->button() == Qt::LeftButton){
        rotate = 1;
        prevArcBall = ArcBallMapping(e->pos());
    }
    else if(e->button() == Qt::RightButton){
        translate = 1;  //if translation intended
        prevMouse = e->pos();
    }
    
}


QVector3D myGL::ArcBallMapping(QPoint mouse)
{
    QVector3D v(0,0,0);
    static GLint params[4];
    
    glGetIntegerv(GL_VIEWPORT,params); //was behaving weird from params size 5, throwing mouse location
    
    v.setX((2.0 * mouse.x() - params[2] ) / params[2]);
    v.setY((params[3] - 2.0 * mouse.y() ) / params[3]);
    
    double  d = v.length();
    if( d > 1.0 ) d = 1.0;
    
    v.setZ(sqrt(1 - d*d));

    return v.normalized();
}
void myGL::Motion(QPoint mouse){
    if(rotate){
        currentArcBall = ArcBallMapping(mouse);
        axisVector = QVector3D::crossProduct(prevArcBall, currentArcBall);
        rotAngle = 90.0*(currentArcBall - prevArcBall).length()*1.5;
        prevArcBall = currentArcBall;
        return;
    }
    if(translate){
        int displacement = (mouse.y() - prevMouse.y());
        transVector.setZ(transVector.z() + 0.1*displacement);
        prevMouse = mouse;
        return;
    }
    if(translateKeyboard){
//translations from keyboard
//jumpy translation
        if(mouse.y() == 0) { //down up
            transVector.setY(transVector.y() + mouse.x());
            qDebug()<<transVector.y();
        }

        else if(mouse.y() == 1) { //down left
            transVector.setX(transVector.x() + mouse.x());
            qDebug()<<transVector.x();
        }
        translateKeyboard = false; return;
    }

    updateGL();
}





int myGL::DrawGLGrid(int span){
    //To do: make the span object size dependent
    int id_list = glGenLists(2);
    glNewList(id_list, GL_COMPILE_AND_EXECUTE);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    // Turn the lines GREEN
    glColor3ub(122, 122, 122);
    
    // Draw a 1x1 grid along the X and Z axis'
    for(float i = -span; i <= span; i += 1)
    {
        if(i == span || i == -span)
            glColor3ub(0,0,0);
        else
            glColor3ub(122, 122, 122);
        // Start drawing some lines
        glBegin(GL_LINES);
        
        // Do the horizontal lines (along the X)
        glVertex3f(-span, 0, i);
        glVertex3f(span, 0, i);
        
        // Do the vertical lines (along the Z)
        glVertex3f(i, 0, -span);
        glVertex3f(i, 0, span);
        
        // Stop drawing lines
        glEnd();
    }
    
    for(float i = -span; i <= span; i++)
    {
        // Start drawing some lines
        if(i == span || i == -span)
            glColor3ub(0,0,0);
        else
            glColor3ub(122, 122, 122);
        glBegin(GL_LINES);
        
        // Do the horizontal lines (along the Y)
        glVertex3f(0,-span, i);
        glVertex3f(0, span, i);
        
        // Do the vertical lines (along the Z)
        glVertex3f(0, i, -span);
        glVertex3f(0, i, span);
        
        // Stop drawing lines
        glEnd();
    }
    
    for(float i = -span; i <= span; i += 1)
    {
        if(i == span || i == -span)
            glColor3ub(0,0,0);
        else
            glColor3ub(122, 122, 122);
        // Start drawing some lines
        glBegin(GL_LINES);
        
        // Do the horizontal lines (along the X)
        glVertex3f(-span, i, 0);
        glVertex3f(span, i, 0);
        
        // Do the vertical lines (along the Y)
        glVertex3f(i, -span, 0);
        glVertex3f(i, span, 0);
        
        // Stop drawing lines
        glEnd();
    }
    glBegin(GL_LINES);
    glColor3ub(0, 255, 0);
    glVertex3f(-span, 0, 0);
    glVertex3f(span, 0, 0);
    glVertex3f(0,-span, 0);
    glVertex3f(0, span, 0);
    glVertex3f(0, 0, -span);
    glVertex3f(0, 0, span);
    
    glColor4f(0.3,0.3,0.3,1); //weird, i have to set color here
    
    glEnd();
    glEndList();
    return id_list;
}





void myGL::LoadObject(QString &filename){
    fname = filename;
  //  qDebug()<<fname;

    mesh.clearNeighbour();
  //  qDebug()<<"cleared";
    isObjectLoaded= ReadIv(mesh, fname.toStdString());
    
    emitVertexCount(mesh.VertexNumber());
    emitFaceCount(mesh.FaceNumber());
    if(isObjectLoaded) {
        drawEigen = false;
        compressing = false;
        renderCurvature = false;
        mesh.isDeltaPreped = false;
        mesh.isP2PReady = mesh.Build_P2P_Neigh();

//        mesh.Build_P2F_Neigh(); //not using this for now
//        mesh.ComputeFaceNormals();
//        mesh.ComputeVertexNormals();
    }
}

void myGL::ScreenShot()
{
    //lame function.
    paintGL();
    QImage snapshot = this->grabFrameBuffer();
    snapshot.save("shot.jpg");
}

void myGL::Smooth(int typeLaplacian){
//To do: distance weights
    mesh.isDeltaPreped = false;
    if(mesh.isP2PReady){
         timer.start();
      //  qDebug()<<"smoothing..";
        unsigned int VertexSize = mesh.VertexNumber();
        mesh.InitializeLaplaceMatrix(VertexSize);
        mesh.CookMatrix(typeLaplacian); //selected 1 for combinatorial
        if(typeLaplacian == 1) emit Status(QString("Laplacian type: Combinatorial created"));
        else if(typeLaplacian == 2 ) emit Status(QString("Laplacian type: Harmonic created"));
        timer.restart();
        mesh.EigenDecompose();
        emit Status(QString::number(double(timer.nsecsElapsed())/1000000)+" msec");
    }
}

void myGL::waterMark(int k)
{//inchoate
    mesh.waterMark(k);
}

void myGL::emitVertexCount(int count){
    emit vertexCount(count);
}

void myGL::emitFaceCount(int count){
    emit faceCount(count);
}


void myGL::ShineOn()
{
    
    /****setup****/
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHTING);
    if(Light0){
        glEnable(GL_LIGHT0);
        GLfloat ambience[] = {0.7f, 0.7f, 0.7f, 1.0f};
      //  GLfloat diffuse[] = {0.4f, 0.4f, 0.4f, 1.0f};
        GLfloat specular[] = {.7f, .7f, .7f, 1.0f};
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambience);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lamb); //changed from ambience to diffuse, ambience doesnt look good
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        
        /****set the light position****/
      //  GLfloat lightPosition[] = {0.5f, 0.5f, 0.5f, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    } else glDisable(GL_LIGHT0);
    if(Light1){
        glEnable(GL_LIGHT1);
        GLfloat ambience[] = {.1f, 1.0f, 0.1f, 1.0f};
        GLfloat diffuse[] = {0.8f, 0.2f, 0.2f, 1.0f};
        GLfloat specular[] = {.5f, .5f, .5f, 1.0f};
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambience);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
        
        /****set the light position****/
        GLfloat lightPosition[] = {-2.0f, 0.5f, 2.1f, 1.0};
        glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    }else glDisable(GL_LIGHT1);
    if(Light2){
        glEnable(GL_LIGHT2);
        GLfloat ambience[] = {0.1f, 0.1f, 1.0f, 1.0f};
        GLfloat diffuse[] = {0.3f, 0.3f, 0.8f, 1.0f};
        GLfloat specular[] = {.3f, .3f, .5f, 1.0f};
        glLightfv(GL_LIGHT2, GL_AMBIENT, ambience);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
        
        /****set the light position****/
        GLfloat lightPosition[] = {0.5f, -1.5f, -7.0f, 1.0};
        glLightfv(GL_LIGHT2, GL_POSITION, lightPosition);
    }else glDisable(GL_LIGHT2);
    updateGL();
    
    
}

void myGL::Materialize()
{

    double emissivity = 0.1;
   //  GLfloat materialEmission[] = {emissivity, emissivity, emissivity, 1.0f}; //no change for now

    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
  //   glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0);

    updateGL();

}


//void myGL:: DrawFriends(set<int> neighI){
//    glPointSize(4);
//    glBegin(GL_POINTS);
//    for(set<int>::iterator it(neighI.begin()); it!=neighI.end(); ++it){

//        Vector3d P(mesh.getVertex(*it));
//        glColor3f(1, 1, *it/neighI.size());
//        glVertex3f(P[0], P[1], P[2]);
//    }
//    glEnd();
//}

/*to draw string on gl viewport, not working properly on mesh calculation*/
void myGL:: DrawString(double x, double y, QString txt)
{

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    qglColor(Qt::white);
    renderText(x, y, txt, QFont("Arial", 7, QFont::Bold, false) );
    glEnable(GL_DEPTH_TEST);
}




