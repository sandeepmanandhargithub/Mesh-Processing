#define GL_GLEXT_PROTOTYPES
#include "object3d.h"
#include<GL/gl.h>
#include<iostream>

#include<QDebug>

object3D::object3D()
{
}

//--
//
// ComputeFaceNormals
//
//--
// Compute unit normal of every faces
void object3D::ComputeFaceNormals()
{
    // Resize face normal array
    face_normals.resize( FaceNumber() );

    // For every face
    int MaxFaceNumber = FaceNumber();
    for( int i=0; i<MaxFaceNumber; i++ )
    {
        // Compute unit face normal
        face_normals[i] = ComputeFaceNormal(i);
    }
}

//--
//
// ComputeVertexNormals
//
//--
// Compute unit normal of every vertex
void object3D::ComputeVertexNormals()
{

    int i;

    // Assume that face normals are computed
    assert( FaceNormalNumber() == FaceNumber() );

    // Resize and initialize vertex normal array
    vertex_normals.assign( VertexNumber(), Vector3d(0,0,0) );

    // For every face
    for( i=0 ; i<FaceNumber() ; i++ )
    {
        // Add face normal to vertex normal
        VertexNormal(i,0) += FaceNormal(i);
        VertexNormal(i,1) += FaceNormal(i);
        VertexNormal(i,2) += FaceNormal(i);
    }

    // For every vertex
    for( i=0 ; i<VertexNumber() ; i++)
    {
        // Normalize vertex normal
        VertexNormal(i).normalized();
        //qDebug()<<vertex_normals[i];
        //	logfile<<vertex_normals[i]<<endl;
    }

}

void object3D::ClearAll(){
    ClearVertices();
    ClearFaces();
    ClearFaceNormals();
    ClearVertexNormals();
    ClearVertices();
    ClearDeltasAndRecovered();
}

void object3D::DrawObject(int MODE)
{
    int f = faces.size();
    switch(MODE){
    case 1: //only vertex
    {

        glDisable(GL_LIGHTING);

        glPointSize(2.0);
        glBegin(GL_POINTS);
        // bool color_on = false;
        glColor3ub(0,255,0);

        GLuint selectBuf[1024];
        GLint hits;
        glSelectBuffer(1024, selectBuf);
        glInitNames();

        for(int i = 0; i < vertices.size(); i++){
            glPushName(i);
            glVertex3d(vertices[i][0], vertices[i][1], vertices[i][2]);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }break;
    case 2: //EIGENRENDER
    {
        if(colormap.size() == 0) return;
         glDisable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        glBegin(GL_TRIANGLES);

        for(int i = 0; i < f;i++)
            for(int j = 0; j < 3; j++){
                int vertexIdx = faces[i][j];
                Vector3d C(DoubleToColor(colormap.at(vertexIdx)));
                Vector3d N(vertex_normals[vertexIdx]);
                Vector3d V(vertices[vertexIdx]);
                glNormal3d(N[0], N[1], N[2]);
                glColor3d(C[0], C[1], C[2]);
                glVertex3d(V[0], V[1], V[2]);
            }
        glEnd();
glEnable(GL_LIGHTING);

    }break;
    case 3:
    {
      //  glDisable(GL_LIGHTING);

   //     ComputeVertexNormals();


        glEnable(GL_LIGHTING);
    //    glEnable(GL_COLOR_MATERIAL);
        glBegin(GL_TRIANGLES);
         for(int i = 0; i < f;i++)
             for(int j = 0; j < 3; j++){
                 int vertexIdx = faces[i][j];
              //   if(colormap.size() == 0) return;
                 Vector3d N(vertex_normals[vertexIdx]);
                 Vector3d V(vertices[vertexIdx]);

                 glNormal3d(N[0], N[1], N[2]);
                 glVertex3d(V[0], V[1], V[2]);
             }
        glEnd();
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
    }break;

    case 4: //RENDERCOMPRESSION
    {

glEnable(GL_LIGHTING);
        if(deltaVertices.empty()) return;
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < f;i++)
                     for(int j = 0; j < 3; j++){
                         int vertexIdx = faces[i][j];
                         Vector3d N(vertex_normals[vertexIdx]);
                         Vector3d V(deltaVertices[vertexIdx]);
                         glNormal3d(N[0], N[1], N[2]);
                         glVertex3d(V[0], V[1], V[2]);
                     }
        glEnd();

    }break;

    case 5: //RENDERCURVATURE
    {

        if(colorCurve.empty()) return;
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < f;i++)
                     for(int j = 0; j < 3; j++){
                         int vertexIdx = faces[i][j];
                         Vector3d C(colorCurve[vertexIdx]);
                         Vector3d N(vertex_normals[vertexIdx]);
                         Vector3d V(vertices[vertexIdx]);
                         glColor3d(C[0], C[1], C[2]);
                         glNormal3d(N[0], N[1], N[2]);
                         glVertex3d(V[0], V[1], V[2]);
                     }
        glEnd();

    }break;
    }

}

void object3D::DrawFaceNormal(int i){
    for(int j=0; j<3; j++)	{
        //  glNormal3d(face_normals[i][0],face_normals[i][1],face_normals[i][2]);
        Vector3d V=Vertex(i,j);
        glVertex3d(V[0],V[1],V[2]);
    }
}

Vector3d object3D::DoubleToColor(const double d)
{
            if(d<0) return Vector3d(0,0,0);
            if(0<=d && d<0.25)   { return Vector3d(0 , d*4.0 ,1);}

            if(0.25<=d && d<0.5)	{ return Vector3d(0 , 1.0 , 2.0-4.0*d);}

            if(0.5<=d && d<0.75)	{ return Vector3d(4.0*d - 2.0 , 1.0 ,0);	}

            if(0.75<=d && d<=1.0)	{ return Vector3d(1.0 , 4.0-4.0*d ,0);}

            return Vector3d(1,1,1);
}

