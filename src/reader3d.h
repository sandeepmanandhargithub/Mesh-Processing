/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides functions to read VRML 1.0 files
 * **************************************************************************************************************/
#ifndef READER3D_H
#define READER3D_H


#include <fstream>
#include <sstream>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "object3d.h"
#include <vector>
#include<string>
using namespace std;
using namespace Eigen;

string UpperCase( const string& s );
bool ReadIv(object3D& mesh, const string& file_name);
void dummy();
#endif // READER3D_H
