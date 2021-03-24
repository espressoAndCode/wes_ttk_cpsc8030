

#pragma once

// base code includes
#include                  <Triangulation.h>
#include                  <Wrapper.h>
#include                  <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef bg::model::polygon<point, false, false> polygon; // ccw, open polygon
typedef std::pair<box, unsigned> value;

using namespace std;

namespace ttk{
  
  class VectorFieldPaths : public Debug{

    public:
        
      VectorFieldPaths();
      
      ~VectorFieldPaths();

      void setupRTree();

      SimplexId queryRTree(float x, float y);

      double triangleArea(vector<pair<float,float> >& coordinates);

      template <class dataType>
        int execute(int steps, double delta, vector<pair<float,float> >& line);

      template <class dataType>
        bool pickNextVertex(pair<float,float> point, float delta, pair<float,float>& newPoint);

      template <class dataType>
        void bilinearInterpolation(pair<float,float> point, SimplexId triangle, dataType& componentX, dataType& componentY);

      inline int setInputDataPointer(void *datax, void* datay){
          inputDatax_ = datax;
          inputDatay_ = datay;
        return 0;
      }


      inline int setupTriangulation(Triangulation *triangulation){
        triangulation_ = triangulation;
       
        if(triangulation_){
          triangulation_->preprocessVertexNeighbors();
        }

        setupRTree();
        return 0;
      }
    
    protected:
    
      void                  *inputDatax_, *inputDatay_;
      Triangulation         *triangulation_;

      bgi::rtree< value, bgi::rstar<16, 4> > rtree;
  };
}

// if the package is a pure template class, uncomment the following line
// #include                  <VectorFieldPaths.cpp>

// template functions
template <class dataType> int ttk::VectorFieldPaths::execute(int steps, double delta, vector<pair<float,float>>& line){

    //this function implements the Euler method

    //define the first seed vertex
    pair<float,float> seed;
    seed.first = 0;
    seed.second = -1;

    pair<float, float> point;
    point = seed;

    for (int i =0; i < steps; i++){
      line.push_back(point);
      pair<float, float> newPoint;

      if(!pickNextVertex<dataType>(point, delta, newPoint)){
        break;
      }
      
      point = newPoint;
    }


  return 0;
}


template <class dataType>
bool ttk::VectorFieldPaths::pickNextVertex(pair<float,float> point, float delta, pair<float,float>& newPoint) {

    //this function implements the selection of the next vertex in the stream path
    SimplexId triangle = queryRTree(point.first, point.second);

    if (triangle == -1) {
      return false;
    }

    dataType componentX;
    dataType componentY;
    bilinearInterpolation(point, triangle, componentX, componentY);

    newPoint.first = point.first + componentX * delta;
    newPoint.second = point.second + componentY * delta;
    return true;
}

template <class dataType>
void ttk::VectorFieldPaths::bilinearInterpolation(pair<float,float> point, SimplexId triangle, dataType& componentX, dataType& componentY){

    dataType *inputDatax = (dataType *)inputDatax_; //this encodes the x component for each vertex
    dataType *inputDatay = (dataType *)inputDatay_; //this encodes the y component for each vertex

    //implement here linea interpolation and compute componentX and componentY
    SimplexId v1, v2, v3;
    triangulation_->getTriangleVertex(triangle, 0, v1);
    triangulation_->getTriangleVertex(triangle, 1, v2);
    triangulation_->getTriangleVertex(triangle, 2, v3);

    float z;
    pair<float, float> v1coords;
    pair<float, float> v2coords;
    pair<float, float> v3coords;

    triangulation_->getVertexPoint(v1, v1coords.first, v1coords.second, z);
    triangulation_->getVertexPoint(v2, v2coords.first, v2coords.second, z);
    triangulation_->getVertexPoint(v3, v3coords.first, v3coords.second, z);

    vector<pair<float, float>> t1;
    t1.push_back(v2coords);
    t1.push_back(v3coords);
    t1.push_back(point);

    vector<pair<float, float>> t2;
    t2.push_back(v1coords);
    t2.push_back(v3coords);
    t2.push_back(point);

    vector<pair<float, float>> t3;
    t3.push_back(v1coords);
    t3.push_back(v2coords);
    t3.push_back(point);

    double area_t1 = triangleArea(t1);
    double area_t2 = triangleArea(t2);
    double area_t3 = triangleArea(t3);

    double sum_area = area_t1 + area_t2 + area_t3;

    //implement bilinear interpolation
    componentX = area_t1 * inputDatax[v1]/sum_area + 
                 area_t2 * inputDatax[v2]/sum_area + 
                 area_t3 * inputDatax[v3]/sum_area;

    componentY = area_t1 * inputDatay[v1]/sum_area + 
                 area_t2 * inputDatay[v2]/sum_area + 
                 area_t3 * inputDatay[v3]/sum_area;

}