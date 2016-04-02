/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class derived from object3d
 * implements the creation of Laplacian mesh, eigen decomposition, mesh compression and some
 * experimental stuffs like mesh watermarking, curvature analysis
 * **************************************************************************************************************/
#ifndef LAPLACIANTHING_H
#define LAPLACIANTHING_H
#include<Eigen/Core>
#include<Eigen/Dense>
#define PI 3.141592653
#include <Eigen/Eigenvalues>

using namespace Eigen;
#include<cassert>
#include<cstring>
#include<vector>
#include<set>
#include<map>
#include<QDebug>
#include<iostream>
#include<fstream>
#include<GL/gl.h>
using namespace std;
#include "neighbour3d.h"
struct EigenIndex
    {
        int id;
        double value;
        MatrixXd vector;
        bool operator< (const EigenIndex other)const{ return value < other.value; } //ascending
        EigenIndex(int i, double val, MatrixXd vec) :id(i), value(val), vector(vec){}
    };
class LaplacianThing: public neighbour3D
{

public:
    MatrixXd matrix;
    MatrixXd matrixAnchored;
    MatrixXd smallL;
    MatrixXd eigenDiag;
    MatrixXd eigenVec;
    MatrixXd sortedEigenVector;
    VectorXd sortedEigenValue;
    MatrixXd sortedEigenValueMat;   //this will be diagonlaized version of eigen values

    vector<EigenIndex> eigenList;
    LaplacianThing();
    VectorXd Xx, Xy, Xz;//vectors of global coordinates;
    VectorXd Dx, Dy, Dz; //delta coordinates;
    VectorXd Rx, Ry, Rz; //recovered global coordinates;
    VectorXd pfX, pfY, pfZ;

    bool isDeltaPreped = false;
    bool isAmatrixReady, isDmatrixReady;
    bool canIgetEigen = false;
    int type = -1;
    void initIsReadyFlags(){ isAmatrixReady = false; isDmatrixReady = 0;}
    void InitializeLaplaceMatrix(unsigned int m){

        matrix.resize(m,m);
        matrix.setZero(m,m);
//        eLaplace.resize(m,m);
//        matrix.setZero();
      //  qDebug()<<"lets see";
        //matrixAnchored.setZero(m + 1, m);
        //qDebug()<<m*m;
        //  matrix.setZero();
    }
/******************************************************
 * Make Laplacian Matrix "uniformly weighed"*********
 * ***************************************************/
    void CookMatrix(int T){

        int j = 0; //counting 1s
        isAmatrixReady = false;
        int VertexSize = VertexNumber();
        set<int>:: iterator iterEnd;
        vector<double> cots;
    type = T;
if(T == 0){//combinatorial

        for(int i = 0; i < VertexSize; i++){
            iterEnd = P2P[i].end();
            matrix.coeffRef(i, i) = P2P.at(i).size();   //size of the connection

            for(set<int>::iterator iter(P2P[i].begin()); iter != iterEnd; ++iter){
                matrix.coeffRef(i, *iter) = -1;  //where the connections are
            }
        }
        isAmatrixReady = true;
       // SaveMatrix(matrix, QString("matrix.txt"));
}
else if(T == 2){

    for(int i = 0; i < VertexSize; i++){
        iterEnd = P2P[i].end();
         //size of the connection
        double sumOfCots = 0;
        cots.clear();
        for(set<int>::iterator iter(P2P[i].begin()); iter != iterEnd; ++iter){
            double cotWt = cotangentWeights(i, *iter);
            matrix.coeffRef(i, *iter) = -cotWt; //use it for un-normalized version
//               cots.push_back(cotWt);
//                sumOfCots += cotWt;
        }

//           for(int k = 0; k < cots.size(); k++){
//              cots.at(k) /= (sumOfCots + 1);
//           }
//           int j = 0;
//            for(set<int>::iterator iter(P2P[i].begin()); iter != iterEnd; ++iter,j++){
//                  matrix.coeffRef(i, *iter) = cots.at(j);
//            }


        }
        /***********for cotangent weights***********
        matrix(i, *iter) = -weight(i, *iter)/sumofallweights;
        *******************************************/
    }
 //   SaveMatrix(matrix, QString("matrix.txt"));
    isAmatrixReady = true;

    }

    void SaveMatrix(){
        std::ofstream file("matrix.txt");
        if(file.is_open()){
          //  file<<"Here is the cooked matrix \n"<<matrix<<"\n";
            file.close();return;
        }
        std::cerr<<"file cannot be opened";
        if(canIgetEigen){
            SaveMatrix(sortedEigenValueMat, QString("eigenvalues.txt"));
            SaveMatrix(sortedEigenVector, QString("eignvectors.txt"));
        }
    }

    void SaveMatrix(MatrixXd &src, QString filepath){
        std::ofstream file(filepath.toStdString().c_str());
        if(file.is_open()){
            file<<"Matrix :\n"<<src<<"\n";
            file.close();return;
        }
        std::cerr<<"file cannot be opened";
    }
    void SaveVector(VectorXd &src, QString filepath){
        std::ofstream file(filepath.toStdString().c_str());
        if(file.is_open()){
            file<<src<<"\n";
            file.close();return;
        }
        std::cerr<<"file cannot be opened";
    }

    double cotangentWeights(int i, int j){  //return 1/2*(cotAij + cotBji)
        //  qDebug()<<P2P.at(i).size()<<P2P.at(j).size();  //for icoseadron i = 1, j = 2, there are 6 points each
        //ok p2p is a vector of set of integers(vertex indices)
        //find the intersecting indices for each vertex
        set<int> intersected; //this set will hold the intersected vertices
        set_intersection(P2P.at(i).begin(), P2P.at(i).end(), P2P.at(j).begin(), P2P.at(j).end(),
                         std::inserter(intersected, intersected.begin()));
        //find slope of (vetex i, vetex p1) (vertex j, vertex p1)
        double CotAPlusCotB = 0;
        if(!intersected.empty()){
           // set<int>::iterator iter = intersected.begin();
            for(set<int>::iterator iter = intersected.begin(); iter != intersected.end(); ++iter)
            {
                CotAPlusCotB += Angle(i, j, *iter);
            }
            if(CotAPlusCotB < 0) CotAPlusCotB = 0;
            Vector3d a = getVertex(i);
            Vector3d b = getVertex(j);
            double distance = (a-b).norm();
         //   qDebug()<<distance<<" "<<CotAPlusCotB;
            return CotAPlusCotB;//distance; //test this with distance
        }
    }

    double Angle(unsigned int i, unsigned int j, unsigned int p){
        Vector3d a = getVertex(i);
        Vector3d b = getVertex(j);
        Vector3d o = getVertex(p);

        /*find angle at O*/
       Vector3d oa = (a - o); Vector3d ob = (b - o); double angle = acos((double)(oa.normalized()) .dot(ob.normalized()));
      /////////////////////  Vector3d ao = (o - a); Vector3d ab = (b - a); double angle = acos((double)(ao.normalized()) .dot(ab.normalized()));
      ///////////////////////////  return tan(angle/2); //work on this with voronoi cell
       return(1/(tan(angle)+0.0000001));
    }

    void EigenDecompose(){
        if(!isAmatrixReady || type == -1 ) return;
         canIgetEigen = false;  //not now
            if(type == 0) //combinatorial; use selfadjoint solver
            {
                SelfAdjointEigenSolver<MatrixXd> eigenPairs;    eigenPairs.compute(matrix);
                eigenDiag =(eigenPairs.eigenvalues().real());
                            eigenVec= (eigenPairs.eigenvectors().real());
            }
            else if(type == 2) //harmonic; cant use selfadjoint solver
            {
            EigenSolver<MatrixXd> eigenPairs(matrix);
            eigenDiag =(eigenPairs.eigenvalues().real());
            eigenVec= (eigenPairs.eigenvectors().real());
            }

//                        SaveMatrix(eigenVec, QString("eigenVector.txt"));
//                        SaveMatrix(eigenDiag, QString("eigenValue.txt"));

                        if(!eigenList.empty()) eigenList.clear();
                     //   qDebug()<<"pushing";
                        for(int i = 0; i < eigenVec.cols(); i++){
                            eigenList.push_back(EigenIndex(i, eigenDiag.coeffRef(i), eigenVec.col(i)));
                        }

                        sort(eigenList.begin(), eigenList.end());
                        /******************cooking the sorted eigen stuffs***********************/
                        int numElement = eigenVec.cols();
                        sortedEigenVector.resize(eigenVec.cols(), 1);
                        sortedEigenValue;
                        sortedEigenValue.setZero(0);
                      //  qDebug()<<"creating sorted eigen matrices";
                        for(int i = 0; i < numElement; i++){
                            sortedEigenValue.conservativeResize(1+i);
                            sortedEigenValue.coeffRef(i) = eigenList[i].value;
                            sortedEigenVector.conservativeResize(Eigen::NoChange, 1+i);
                            sortedEigenVector.col(i) = eigenList[i].vector;
                        }
                        sortedEigenValueMat.resize(sortedEigenValue.rows(), sortedEigenValue.rows());
                                sortedEigenValueMat = sortedEigenValue.asDiagonal();
                                SaveMatrix(sortedEigenValueMat, QString("sM.txt"));
                                SaveMatrix(sortedEigenVector, QString("sV.txt"));


                         canIgetEigen = true;   //yes you can
    }

    bool SeekAndDestroy(int pickEigen){
        if(!canIgetEigen) return false;
        if(pickEigen > sortedEigenVector.cols()) return false; //cant access non existent values
        VectorXd c;
        c.resize(sortedEigenVector.rows());
        std::cout<<sortedEigenValue.coeff(100);
        c = sortedEigenVector.col(pickEigen);

        mapEigenToColor(c, 0, 1);

        return true;
    }

    bool SmallLap(int k){ //compression; change to proper name
       /*********************************************
        * vertex and eigen vectors
        * ******************************************/
        if(!canIgetEigen) return false;

        VectorizeGlobal();
        int n = matrix.rows();

        pfX.resize(n); pfY.resize(n); pfZ.resize(n);
        MatrixXd m(sortedEigenVector.transpose());
        pfX = m*Xx;
        pfY = m*Xy;
        pfZ = m*Xz;
        for(int i = k; i<pfX.rows(); i++){
            pfX.coeffRef(i) = 0;
            pfY.coeffRef(i) = 0;
            pfZ.coeffRef(i) = 0;
        }
//        SaveVector(pfX, "px.txt");
//        SaveVector(pfY, "py.txt");
//        SaveVector(pfZ, "pz.txt");

        Rx.resize(n); Ry.resize(n); Rz.resize(n);
        Rx = (sortedEigenVector*pfX).transpose();
        Ry = (sortedEigenVector*pfY).transpose();
        Rz = (sortedEigenVector*pfZ).transpose();



        Recover();
        return true;
    }

    /***************vectorizing global coordinates*************/
    void VectorizeGlobal(){
        Xx.resize(0); Xy.resize(0); Xz.resize(0);
        for(int i = 0; i < VertexNumber(); i++){
            Vector3d V(getVertex(i));
            Xx.conservativeResize(1+i);
            Xx.coeffRef(i) = V[0];
            Xy.conservativeResize(1+i);
            Xy.coeffRef(i) = V[1];
            Xz.conservativeResize(1+i);
            Xz.coeffRef(i) = V[2];
        }
    }

    /*****************Delta storm **************/
    void DeltaStorm(){
        /******************here we will be using anchored everything********************/
        int originalRows = matrix.rows();
        int anchoredColumn = 5;
        matrixAnchored.resize(matrix.rows(), matrix.cols());
     //   matrixAnchored = matrix;
        for(int i = 55; i < sortedEigenValueMat.cols(); i++)
            sortedEigenValueMat.coeffRef(i, i) = 0;
        matrixAnchored = sortedEigenVector*sortedEigenValueMat*sortedEigenVector.transpose();

        qDebug()<<"anchormat #3";

        qDebug()<<"X size: "<<Xx.rows();
     //   Dx.resize(Xx.rows()+5); Dy.resize(Xy.rows()+5); Dz.resize(Xz.rows()+5);

        std::ofstream fileX("reX.txt");
        if(fileX.is_open()){
            fileX<<"Matrix :\n"<<Xx<<"\n";
            fileX.close();
        }
        Dx = matrixAnchored*Xx;
        Dy = matrixAnchored*Xy;
        Dz = matrixAnchored*Xz;
        std::ofstream fileD("reD.txt");
        if(fileD.is_open()){
            fileD<<"Matrix :\n"<<Dx<<"\n";
            fileD.close();
        }
        qDebug()<<"D size: "<<Dx.rows();
        VectorXd Rx, Ry, Rz;
      //  Rx.resize(originalRows +5); Ry.resize(originalRows +5); Rz.resize(originalRows +5);
        qDebug()<<"R size before anchored: "<<Rx.rows();
        Rx = matrixAnchored.colPivHouseholderQr().solve(Dx);
        Ry = matrixAnchored.colPivHouseholderQr().solve(Dy);
        Rz = matrixAnchored.colPivHouseholderQr().solve(Dz);
        qDebug()<<"R size anchored: "<<Rx.rows();
        std::ofstream fileR("reR.txt");
        if(fileR.is_open()){
            fileR<<"Matrix :\n"<<Rx<<"\n";
            fileR.close();
        }
        qDebug()<<"regaining";
        deltaVertices.clear();
        for(int i = 0; i < originalRows; ++i){
            Vector3d V(Rx[i], Ry[i], Rz[i]); //creating verties from vectorized delta
            deltaVertices.push_back(V);
        }
        qDebug()<<"pused already";

//        deltaVertices.at(2) = Vector3d(Dx[originalRows], Dy[originalRows], Dz[originalRows]);
//        deltaVertices.at(3) = Vector3d(Dx[originalRows+1], Dy[originalRows+1], Dz[originalRows+1]);
//        deltaVertices.at(4) = Vector3d(Dx[originalRows+2], Dy[originalRows+2], Dz[originalRows+2]);
//        deltaVertices.at(5) = Vector3d(Dx[originalRows+3], Dy[originalRows+3], Dz[originalRows+3]);
//        deltaVertices.at(6) = Vector3d(Dx[originalRows+4], Dy[originalRows+4], Dz[originalRows+4]);

//        deltaVertices.at(3) = Xx[originalRows + 1]
        qDebug()<<"pushed";
        isDeltaPreped = true;
        SaveMatrix(matrixAnchored, QString("Anchored.txt"));
        SaveMatrix(matrix, QString("mat.txt"));
    }

    void Recover(){
        deltaVertices.clear();
        for(int i = 0; i < VertexNumber(); ++i){
            Vector3d V(Rx[i], Ry[i], Rz[i]); //creating verties from vectorized deltas
            deltaVertices.push_back(V);
        }
    }

//vector<double>colormap
    void mapEigenToColor(VectorXd c, int a, int b){
        if(!colormap.empty())
            colormap.clear();
        int sz = c.rows();
        double minima = c.minCoeff();
        double maxima = c.maxCoeff();
        for(int i = 0; i < sz; ++i){
            colormap.push_back((b - a)*(c.coeffRef(i) - minima)/(maxima - minima) + a);
        }
        //qDebug()<<"colormap of size "<<colormap.size();
    }


    void curvature(){
        VectorizeGlobal();
        Dx = matrix*Xx;     Dy = matrix*Xy;     Dz = matrix*Xz;
        MatrixXd curvecolor;
        colorCurve.clear();
        Dx.normalized();  Dy.normalized(); Dz.normalized();
        curvecolor.resize(Dx.rows(), 3);
        for(int i =0; i < Dx.rows(); i++){

            double x = Dx.coeffRef(i)/Dx.maxCoeff();
            double y = Dy.coeffRef(i)/Dy.maxCoeff();
            double z = Dz.coeffRef(i)/Dz.maxCoeff();
            double d = sqrt(x*x + y*y + z*z);
          //  qDebug()<<d;
            colorCurve.push_back(DoubleToColor(d));
        }
    }

    bool waterMark(int k){
        if(!canIgetEigen) return false;

        VectorizeGlobal();
        int n = matrix.rows();

        pfX.resize(n); pfY.resize(n); pfZ.resize(n);
        MatrixXd m(sortedEigenVector.transpose());
        pfX = m*Xx;
        pfY = m*Xy;
        pfZ = m*Xz;
        //qDebug()<<"watermarking";
        for(int i = 0; i<k; i++){
            pfX.coeffRef(i) += .83; //ascii S
            pfY.coeffRef(i) += .65; //ascii A
            pfZ.coeffRef(i) += .78; //ascii N
        }

        Rx.resize(n); Ry.resize(n); Rz.resize(n);
        Rx = (sortedEigenVector*pfX).transpose();
        Ry = (sortedEigenVector*pfY).transpose();
        Rz = (sortedEigenVector*pfZ).transpose();

   // qDebug()<<"watermarking done";
    Recover();
    return true;

}
};

#endif // LAPLACIANTHING_H
