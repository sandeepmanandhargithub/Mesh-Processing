/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from Object3d
 * Building point to point, point to face and most of the magic goes here
 * Most of the parts derived from the work of Dr. Yohan Fougerolle
 * **************************************************************************************************************/
#ifndef NEIGHBOUR3D_H
#define NEIGHBOUR3D_H
//#include<Eigen/Core>
//#include <Eigen/Geometry>
//#include <Eigen/Eigenvalues>
//using namespace Eigen;
#include<cassert>
#include<cstring>
#include<vector>
#include<set>
#include<map>
using namespace std;
#include "object3d.h"
class neighbour3D : public object3D
{
public:
    neighbour3D();
    virtual ~neighbour3D();

    vector< set<int> > P2P; //point2point
    vector< set<int> >P2F;
    vector< set<int> >F2F;
    bool isP2PReady = false;
    map< pair<int, int>, set<int> > Edges;
    inline map<pair<int, int>, set<int> > ::iterator Get_Edge(int i1, int i2){
        pair <int, int> mypair;
        mypair.first = std::max(i1, i2);
        mypair.second = std::min(i1, i2);
        return Edges.find(mypair);
    }

    bool Build_P2P_Neigh();
    bool Build_P2F_Neigh();
    bool Build_F2F_Neigh();
    bool Build_Edges();
    void BuildDistanceLabels(int A);

    set<int> GetP2P_Neigh(int, int);
    set<int> GetF2F_Neigh(int, int);
    void IllustratePointNeighbour(int , int);

    void DrawPoints ( set <int> s);
    void DrawP2P(int i);
    void renderP2P(int n);
    void clearNeighbour(){
        if(!P2P.empty())    P2P.clear();
        if(!P2F.empty())    P2F.clear();
        if(!F2F.empty())    F2F.clear();
        if(!Edges.empty())  Edges.empty();
        ClearAll();
    }

};

#endif // NEIGHBOUR3D_H
