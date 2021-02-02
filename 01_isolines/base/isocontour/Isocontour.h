/// \ingroup base
/// \class ttk::Isocontour
/// \author Your Name Here <Your Email Address Here>
/// \date The Date Here.
///
/// \brief TTK %isocontour processing package.
///
/// %Isocontour is a TTK processing package that takes a scalar field on the input
/// and produces a scalar field on the output.
///
/// \sa ttk::Triangulation
/// \sa ttkIsocontour.cpp %for a usage example.

#pragma once

// base code includes
#include <Triangulation.h>
#include <Wrapper.h>
#include <vector>

using namespace std;

namespace ttk
{

  class Isocontour : public Debug
  {

  public:
    Isocontour();

    ~Isocontour();

    template <class dataType>
    int computeIsolines(int &argument, vector<vector<float>> &final_points);

    template <class dataType>
    vector<float> compute_coordinates(int v1, int v2, dataType isovalue);

    inline int setInputDataPointer(void *data)
    {
      inputData_ = data;
      return 0;
    }

    inline int setupTriangulation(Triangulation *triangulation)
    {
      triangulation_ = triangulation;

      if (triangulation_)
      {
        triangulation_->preprocessVertexNeighbors();
      }

      return 0;
    }

  protected:
    void *inputData_;
    Triangulation *triangulation_;
  };
} // namespace ttk

// if the package is a pure template class, uncomment the following line
// #include                  <Isocontour.cpp>

// template functions
template <class dataType>
int ttk::Isocontour::computeIsolines(
    int &argument, vector<vector<float>> &final_points)
{

  dataType *inputData = (dataType *)inputData_;

  dataType isovalue = 1.2; //define the isovalue

  for (int i = 0; i < triangulation_->getNumberOfTriangles(); i++)
  {

    //for each vertex
    //test if the scalar value is higher or lower than the isovalue

    vector<bool> isLower(3);
    vector<SimplexId> name_vertices(3);

    for (int j = 0; j < 3; j++)
    {

      int vertex_index;
      triangulation_->getTriangleVertex(i, j, vertex_index);

      isLower[j] = inputData[vertex_index] < isovalue;
      name_vertices[j] = vertex_index;
    }

    vector<vector<float>> points;

    if (isLower[0] != isLower[1])
    {
      vector<float> newPoint = compute_coordinates(name_vertices[0], name_vertices[1], isovalue);
      points.push_back(newPoint);
    }

    if (isLower[0] != isLower[2])
    {
      vector<float> newPoint = compute_coordinates(name_vertices[0], name_vertices[2], isovalue);
      points.push_back(newPoint);
    }

    if (isLower[1] != isLower[2])
    {
      vector<float> newPoint = compute_coordinates(name_vertices[1], name_vertices[2], isovalue);
      points.push_back(newPoint);
    }

    if (points.size() == 2)
    {
      final_points.push_back(points[0]);
      final_points.push_back(points[1]);
    }
  }

  return 0;
}

template <class dataType>
vector<float> ttk::Isocontour::compute_coordinates(const SimplexId v1, const SimplexId v2, dataType isovalue)
{
  dataType *inputData = (dataType *)inputData_;
  dataType f1 = inputData[v1];
  float coordsv1[3];
  triangulation_->getVertexPoint(v1, coordsv1[0], coordsv1[1], coordsv1[2]);

  dataType f2 = inputData[v2];
  float coordsv2[3];
  triangulation_->getVertexPoint(v2, coordsv2[0], coordsv2[1], coordsv2[2]);

  vector<float> coords_P(3);

  dataType L1 = sqrt(pow(isovalue - f2, 2));
  dataType L2 = sqrt(pow(isovalue - f1, 2));

  for (int i = 0; i < 3; i++)
  {
    coords_P[i] = (L1 * coordsv1[i]) / (L1 + L2) + (L1 * coordsv2[i]) / (L1 + L2);
  }

  return coords_P;
}