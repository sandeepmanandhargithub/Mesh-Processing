/********************************************************************************************************************
 * Sandeep Manandhar, Peixi Li
 * University of Brgundy, Le Creusot, France
 * This header file provides class to load mesh parameters and opengl drawing functions
 * Building point to point, point to face neighbourhood and most of the magic goes here
 * Most of the parts derived from the work of Dr. Yohan Fougerolle
 * **************************************************************************************************************/
#ifndef OBJECT3D_H
#define OBJECT3D_H
#include<Eigen/Core>
#include<Eigen/Geometry>
#include <cstdlib>
#include<vector>
#include<cassert>

using namespace std;
using namespace Eigen;
class object3D
{
private:
    // Vertex array
    vector<Vector3d> vertices;

    // Face array has indices of the vertices of its face
    vector<Vector3i> faces;

    // Face normal array
    vector<Vector3d> face_normals;

    // Vertex normal array
    vector<Vector3d> vertex_normals;


public:
    object3D();
    vector<double>colormap, curveColor ;
    vector<Vector3d> colorCurve;
    vector<Vector3d> deltaVertices;
    vector<Vector3d> RecoveredVertices;

    inline int VertexNumber() const {
        return (int)vertices.size();
    }

    // Vertex array
    inline vector<Vector3d>& Vertices() {
        return vertices;
    }
    inline const vector<Vector3d>& Vertices() const {
        return vertices;
    }

    inline Vector3d getVertex(int i){
        return vertices.at(i);
    }

    inline Vector3i face(int i){
        return faces[i];
    }

    // Add vertex v in vertex array
    inline void AddVertex( const Vector3d& v ) {
        vertices.push_back( v );
    }

    // Clear vertex array
    inline void ClearVertices() {
        vertices.clear();
    }

    inline void ClearColorma(){
        colormap.clear();
        colorCurve.clear();
    }

    inline void ClearDeltasAndRecovered(){
        deltaVertices.clear();
        RecoveredVertices.clear();
    }

    // Vertex #i
    inline Vector3d& Vertex(int i) {
        assert( (i>=0) && (i<VertexNumber()) );
        return vertices[i];
    }

    // Vertex #i (constant)
    inline const Vector3d& Vertex(int i) const {
        assert( (i>=0) && (i<VertexNumber()) );
        return vertices[i];
    }

    // Vertex of face #f with index #v
    // v is in range 0 to 2
    inline Vector3d& Vertex(int f, int v) {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<VertexNumber()) );
        return vertices[faces[f][v]];
    }

    // Vertex of face #f with index #v (constant)
    // v is in range 0 to 2
    inline const Vector3d& Vertex(int f, int v) const {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<VertexNumber()) );
        return vertices[faces[f][v]];
    }

    //==
    //
    // Face Interface
    //
    //==

    // Face number
    inline int FaceNumber() const {
        return (int)faces.size();
    }

    // Face array
    inline vector<Vector3i>& Faces() {
        return faces;
    }

    // Face array (constant)
    inline const vector<Vector3i>& Faces() const {
        return faces;
    }

    // Add face f to face array
    inline void AddFace( const Vector3i& f ) {
        faces.push_back( f );
    }

    // Clear face array
    inline void ClearFaces() {
        if(!faces.empty())
          faces.clear();
    }

    // Face #i
    inline Vector3i& Face(int i) {
        assert( (i>=0) && (i<FaceNumber()) );
        return faces[i];
    }

    // Face #i (constant)
    inline const Vector3i& Face(int i) const {
        assert( (i>=0) && (i<FaceNumber()) );
        return faces[i];
    }

    // Index #v of face #f
    // v is in range 0 to 2
    inline int& FaceIndex(int f, int v) {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        return faces[f][v];
    }

    // Index #v of face #f (constant)
    // Index #v is in range 0 to 2
    inline const int& FaceIndex(int f, int v) const {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        return faces[f][v];
    }

    // Test if face #f has vertex #v
    inline bool FaceHasVertex( int f, int v ) const {
        return (FaceIndex(f,0) == v) || (FaceIndex(f,1) == v) || (FaceIndex(f,2) == v);
    }


    inline int FaceNormalNumber() const {
        return (int)face_normals.size();
    }

    // Face normal array
    inline vector<Vector3d>& FaceNormals() {
        return face_normals;
    }

    // Face normal array (constant)
    inline const vector<Vector3d>& FaceNormals() const {
        return face_normals;
    }

    // Add face normal n to face array
    inline void AddFaceNormal( const Vector3d& n ) {
        face_normals.push_back( n );
    }

    // Clear face array
    inline void ClearFaceNormals() {
        if(!face_normals.empty())
        face_normals.clear();
    }

    // Face normal #i
    inline Vector3d& FaceNormal(int i) {
        assert( (i>=0) && (i<FaceNormalNumber()) );
        return face_normals[i];
    }

    // Face normal #i (constant)
    inline const Vector3d& FaceNormal(int i) const {
        assert( (i>=0) && (i<(int)FaceNormalNumber()) );
        return face_normals[i];
    }

    //--
    //
    // Vertex Normal Interface
    //
    //--

    // Vertex normal number
    inline int VertexNormalNumber() const {
        return (int)vertex_normals.size();
    }

    // Vertex normal array
    inline vector<Vector3d>& VertexNormals() {
        return vertex_normals;
    }

    // Vertex normal array (constant)
    inline const vector<Vector3d>& VertexNormals() const {
        return vertex_normals;
    }

    // Add vertex normal n to vertex normal array
    inline void AddVertexNormal( const Vector3d& n ) {
        vertex_normals.push_back( n );
    }

    // Clear vertex normal array
    inline void ClearVertexNormals() {
        if(!vertex_normals.empty())
        vertex_normals.clear();
    }

    // Vertex normal #i
    inline Vector3d& VertexNormal(int i) {
        assert( (i>=0) && (i<VertexNormalNumber()) );
        return vertex_normals[i];
    }

    // Vertex normal #i (constant)
    inline const Vector3d& VertexNormal(int i) const {
        assert( (i>=0) && (i<VertexNormalNumber()) );
        return vertex_normals[i];
    }

    // Vertex normal of face #f with index #v
    // v is in range 0 to 2
    inline Vector3d& VertexNormal(int f, int v) {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<VertexNormalNumber()) );
        return vertex_normals[faces[f][v]];
    }

    // Vertex normal of face #f with index #v (constant)
    // v is in range 0 to 2
    inline const Vector3d& VertexNormal(int f, int v) const {
        assert( (f>=0) && (f<FaceNumber()) );
        assert( (v>=0) && (v<=2) );
        assert( (faces[f][v]>=0) && (faces[f][v]<VertexNormalNumber()) );
        return vertex_normals[faces[f][v]];
    }

    //--
    //
    // Normal computation
    //
    //--

    // Compute face normals
    void ComputeFaceNormals();

    // Compute vertex normals
    // Assume that face normals are computed
    void ComputeVertexNormals();

    // Compute raw normal of face #f
    inline Vector3d ComputeRawFaceNormal( int f ) const {
        return (Vertex(f,1)-Vertex(f,0)) .cross ( (Vertex(f,2)-Vertex(f,0)) );
    }

    // Compute raw normal of face {va, vb, vc}
    inline Vector3d ComputeRawFaceNormal( int va, int vb, int vc ) const {
        return (Vertex(vb)-Vertex(va)) .cross ( (Vertex(vc)-Vertex(va)) );
    }

    // Compute unit normal of face #f
    inline Vector3d ComputeFaceNormal( int f ) const {
        assert( (f>=0) && (f<(int)FaceNumber()) );
        return ComputeRawFaceNormal(f).normalized();
    }

    // Compute unit normal of face {va, vb, vc}
    inline Vector3d ComputeFaceNormal( int va, int vb, int vc ) const {
        return ComputeRawFaceNormal(va, vb, vc).normalized();
    }

    // Compute area of face #i
    inline double ComputeFaceArea( int i ) const {
        return 0.5 * ComputeRawFaceNormal(i).norm();
    }

    // Compute area of face {va, vb ,vc}
    inline double ComputeFaceArea( int va, int vb, int vc ) const {
        return 0.5 * ComputeRawFaceNormal(va, vb, vc).norm();
    }

    //clean
    void ClearAll();

    void DrawObject(int MODE);
    void DrawFaceNormal(int i);
    Vector3d DoubleToColor(const double d);

    //experimental

};

#endif // OBJECT3D_H
