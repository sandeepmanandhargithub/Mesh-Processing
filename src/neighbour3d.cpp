#include "neighbour3d.h"
#include <GL/gl.h>
#include <algorithm>
neighbour3D::neighbour3D(){}
neighbour3D::~neighbour3D(){}

bool neighbour3D::Build_P2P_Neigh(){
    if(VertexNumber() == 0 || FaceNumber() == 0) return false;
    //building an array of empty sets of size of vertices
    int VertexSize = VertexNumber();
    for(int i = 0; i < VertexSize; i++){
        set<int> myset;
        P2P.push_back(myset);
    }
    int FaceSize = FaceNumber();
    for(int i = 0; i<FaceSize; i++){
        Vector3i V(Face(i)); //now we have three vertex INDICES of the face[i] in V
        P2P.at(V[0]).insert(V[1]);P2P.at(V[0]).insert(V[2]);
        P2P.at(V[1]).insert(V[2]);P2P.at(V[1]).insert(V[0]);
        P2P.at(V[2]).insert(V[0]);P2P.at(V[2]).insert(V[1]);
    }
    return true;
}

set<int> neighbour3D::GetP2P_Neigh( int p_index, int n)
{

        set <int> previous; previous.insert(p_index);
        set <int> new_ring = P2P[p_index];

        if ( n == 0) return previous;
        if ( n == 1) return new_ring;

        set < int > myset;

        for(int i = 1; i<n; i++)
        {
            // compute the 1 neighbourhood of the previously computed ring

            myset.clear();

            for ( set <int> :: iterator it(new_ring.begin()); it != new_ring.end(); it++)
            {
                for (set<int> :: iterator it2(P2P[*it].begin()); it2 != P2P[*it].end(); it2++)
                {
                myset.insert(*it2);
                }
            }

            set <int> dum; //seems uneasy to remove elements while updating the set at the same time ==> dum set for performing the boolean difference

            //extract previous from my set
            set_difference( myset.begin(), myset.end(),
                            previous.begin(), previous.end(),
                            insert_iterator< set<int> >(dum,dum.begin())
                            );
            myset = dum; // copy dum result into myset... I whish I could have avoided this

            //previous = myset INTERSECTED with new ring
            previous.clear();
            set_intersection( myset.begin(), myset.end(),
                            new_ring.begin(), new_ring.end(),
                            insert_iterator< set<int> >(previous,previous.begin())
                    );

            //new_ring = myset MINUS previous
            new_ring.clear();
            set_difference( myset.begin(), myset.end(),
                            previous.begin(), previous.end(),
                            insert_iterator< set<int> >(new_ring,new_ring.begin())
                            );

        }

        return new_ring;
}


void neighbour3D ::DrawPoints ( set <int> s)
{
    glBegin(GL_POINTS);
    for( set<int>::iterator it(s.begin()); it != s.end(); it++)
    {
        Vector3d P(getVertex(*it));
        glVertex3f(P[0],P[1],P[2]);
    }
    glEnd();
}

void neighbour3D::IllustratePointNeighbour(int p_index, int n)
{
    glPointSize(5);
    for(int i=0; i<n; i++)
    {
        set <int> tmp = GetP2P_Neigh(p_index, i);
        if (i%2 == 0)    glColor3f(1,1-i/double(n),0);
        else glColor3f(0,1-i/double(n),1);
        DrawPoints ( tmp );
    }
}



void neighbour3D::DrawP2P(int i){
    if(P2P.empty() || i >= P2P.size()) return;
    glPointSize(1);
    glDisable(GL_LIGHTING);
    Vector3d P(getVertex(i));
    glColor3f(0,0,0);
//    glPushMatrix();
//    glTranslatef(P[0], P[1], P[2]);
    // do some displaying here
//    glPopMatrix();
    glBegin(GL_POINTS);
    glColor3f(1,0,0);
    glVertex3f(P[0], P[1], P[2]);
    //render some neighbours
    glColor3f(0, 1, 0);
    for(set<int>::iterator it = P2P[i].begin(); it != P2P[i].end(); ++it ){
        P = getVertex(*it);
        glVertex3f(P[0], P[1], P[2]);
    }
    glEnd();
}

bool neighbour3D :: Build_P2F_Neigh()
{
    int vn = VertexNumber();
    int fn = FaceNumber();
    if(vn == 0 || fn == 0) return false;

    for( int i=0; i<vn; i++)
    {
        set <int> myset;
        P2F.push_back(myset);
    }

    // now span all the faces, and adds the faces within the corresponding sets

    for( int i=0; i<fn; i++)
    {
        Vector3i F(face(i));
        for( int j=0; j<3; j++)
            P2F[F[j]].insert(i);
    }
    return true;
}



