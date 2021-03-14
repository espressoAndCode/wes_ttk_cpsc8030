/// \ingroup base
/// \class tk::CriticalPoints 
/// \author Your Name Here <Your Email Address Here>
/// \date The Date Here.
///
/// \brief TTK %criticalPoints processing package.
///
/// %CriticalPoints is a TTK processing package that takes a scalar field on the input 
/// and produces a scalar field on the output.
///
/// \sa ttk::Triangulation
/// \sa ttkCriticalPoints.cpp %for a usage example.

#pragma once

// base code includes
#include                  <Triangulation.h>
#include                  <Wrapper.h>
#include                  <map>

using namespace std;


namespace ttk{
  
  class CriticalPoints : public Debug{

    public:
        
      CriticalPoints();
      
      ~CriticalPoints();


      template <class dataType>
        int execute(vector<vector<float>> &coordinates, vector<int> &type);

      template <class dataType> 
        int get_components_link(SimplexId v, bool is_lower_link);
    
      inline int setInputDataPointer(void *data){
        inputData_ = data;
        return 0;
      }

      inline int setupTriangulation(Triangulation *triangulation){
        triangulation_ = triangulation;
       
        if(triangulation_){
          triangulation_->preprocessVertexNeighbors();
          triangulation_->preprocessVertexLinks();
          triangulation_->preprocessEdges();
          triangulation_->preprocessTriangles();
        }
        
        return 0;
      }
    
    protected:
    
      void                  *inputData_;
      Triangulation         *triangulation_;
  };
}

// if the package is a pure template class, uncomment the following line
// #include                  <CriticalPoints.cpp>

// template functions
template <class dataType> int ttk::CriticalPoints::execute(vector<vector<float>> &coordinates, vector<int> &type) {

  Timer t;


  int dimension = triangulation_->getDimensionality(); //returns 2 if 2D, 3 if 3D
  SimplexId vertexNumber = triangulation_->getNumberOfVertices();

  for(SimplexId v=0; v<vertexNumber; v++){

    int c_lower = get_components_link<dataType>(v, true);
    int c_upper = get_components_link<dataType>(v, false);



    if (dimension == 2){
      //2D
      if (c_upper == 1 && c_lower == 1){
        //point is regular, do nothing
      } else {
        vector<float> coords(3);
        triangulation_->getVertexPoint(v, coords[0], coords[1], coords[2]);
        coordinates.push_back(coords);

        if (c_lower == 0) {
          type.push_back(0);    //minimum
        } else if (c_upper == 0){
          type.push_back(2);    //maximum
        } else if (c_upper == 2 && c_lower == 2){
          type.push_back(1);    //saddle
        } else {
          type.push_back(3);    //multi-saddle
        }
      }
    } else {
      // 3D
      if (c_upper == 1 && c_lower == 1){
        //point is regular, do nothing
      } else {
        vector<float> coords(3);
        triangulation_->getVertexPoint(v, coords[0], coords[1], coords[2]);
        coordinates.push_back(coords);

        if (c_lower == 0) {
          type.push_back(0);    //minimum
        } else if (c_upper == 0){
          type.push_back(2);    //maximum
        } else if (c_upper == 1 && c_lower == 2){
          type.push_back(1);    //1-saddle
        } else if (c_upper == 2 && c_lower == 1){
          type.push_back(1);    //2-saddle
        } else {
          type.push_back(3);    //multi-saddle
        }
        
      }

    }

  }

  cout << "Execution complete" << "\n";

  return 0;
}

template <class dataType> int ttk::CriticalPoints::get_components_link(SimplexId v, bool is_lower_link){

  dataType *inputData = (dataType *) inputData_;

  dataType field_v = is_lower_link ? -inputData[v] : inputData[v];

  SimplexId vertices = triangulation_->getVertexNeighborNumber(v);

  map<int, int> vertex_to_set;
  map<int, vector<int>* > set_to_vertices;

  //identify the vertices in the upper link and create initial sets
  SimplexId vertex;
  for(SimplexId j=0; j<vertices; j++){
    triangulation_->getVertexNeighbor(v,j,vertex);
    dataType field_neighbor = is_lower_link ? -inputData[vertex] : inputData[vertex];

    if(field_neighbor > field_v){

      vertex_to_set[vertex] = j;
      set_to_vertices[j] = new vector<int>();

      set_to_vertices[j]->push_back(vertex);

    }
  }

  SimplexId simplices = triangulation_->getVertexLinkNumber(v);
  SimplexId edge;
  for(SimplexId j=0; j<simplices; j++){
    triangulation_->getVertexLink(v,j,edge);


    int dimension = triangulation_->getDimensionality(); //returns 2 if 2D, 3 if 3D

    if (dimension == 2){
      //2D
        SimplexId v1,v2;
        dataType field_v1, field_v2;
        triangulation_->getEdgeVertex(edge, 0, v1);
        triangulation_->getEdgeVertex(edge, 1, v2);

        field_v1 = is_lower_link ? -inputData[v1] : inputData[v1];
        field_v2 = is_lower_link ? -inputData[v2] : inputData[v2];

        if (field_v1 > field_v && field_v2 > field_v){
          if (vertex_to_set[v1] != vertex_to_set[v2]){

            int label = vertex_to_set[v2];
            for (auto vs : *set_to_vertices[vertex_to_set[v2]]){
              set_to_vertices[vertex_to_set[v1]]->push_back(vs);
              vertex_to_set[vs] = vertex_to_set[v1];
            }
            set_to_vertices.erase(label);
          }
        }
    } else {
      //3D
        SimplexId v1,v2,v3;
        dataType field_v1, field_v2, field_v3;
        triangulation_->getTriangleVertex(edge, 0, v1);
        triangulation_->getTriangleVertex(edge, 1, v2);
        triangulation_->getTriangleVertex(edge, 2, v3);

        field_v1 = is_lower_link ? -inputData[v1] : inputData[v1];
        field_v2 = is_lower_link ? -inputData[v2] : inputData[v2];
        field_v3 = is_lower_link ? -inputData[v3] : inputData[v3];

        if (field_v1 > field_v && field_v2 > field_v){
          if (vertex_to_set[v1] != vertex_to_set[v2]){
            int label = vertex_to_set[v2];
            for (auto vs : *set_to_vertices[vertex_to_set[v2]]){
              set_to_vertices[vertex_to_set[v1]]->push_back(vs);
              vertex_to_set[vs] = vertex_to_set[v1];
            }
            set_to_vertices.erase(label);
          }
        }

        if (field_v1 > field_v && field_v3 > field_v){
          if (vertex_to_set[v1] != vertex_to_set[v3]){
            int label = vertex_to_set[v3];
            for (auto vs : *set_to_vertices[vertex_to_set[v3]]){
              set_to_vertices[vertex_to_set[v1]]->push_back(vs);
              vertex_to_set[vs] = vertex_to_set[v1];
            }
            set_to_vertices.erase(label);
          }
        }

        if (field_v2 > field_v && field_v3 > field_v){
          if (vertex_to_set[v2] != vertex_to_set[v3]){
            int label = vertex_to_set[v3];
            for (auto vs : *set_to_vertices[vertex_to_set[v3]]){
              set_to_vertices[vertex_to_set[v2]]->push_back(vs);
              vertex_to_set[vs] = vertex_to_set[v2];
            }
            set_to_vertices.erase(label);
          }
        }
    }
  }

  return set_to_vertices.size();

};