/// \ingroup base
/// \class ttk::BoundaryBased 
/// \author Your Name Here <Your Email Address Here>
/// \date The Date Here.
///
/// \brief TTK %boundaryBased processing package.
///
/// %BoundaryBased is a TTK processing package that takes a scalar field on the input 
/// and produces a scalar field on the output.
///
/// \sa ttk::Triangulation
/// \sa ttkBoundaryBased.cpp %for a usage example.

#pragma once

// base code includes
#include                  <Triangulation.h>
#include                  <Wrapper.h>
#include                  <CriticalPoints.h>


namespace ttk{
  
  class BoundaryBased : public CriticalPoints{

    public:
        
      BoundaryBased();
      
      ~BoundaryBased();

      template <class dataType>
        int execute(vector<vector<SimplexId>> &all_paths) ;
    
    
    protected:
    
  };
}

// if the package is a pure template class, uncomment the following line
// #include                  <BoundaryBased.cpp>

// template functions
template <class dataType> int ttk::BoundaryBased::execute(vector<vector<SimplexId>> &all_paths) {
  dataType *inputData = (dataType*) inputData_;

//boundary based computation
  for ( int i = 0; i < triangulation_->getNumberOfVertices(); i++){
    map<int, vector<int>* > upper_link;
    map<int, vector<int>* > lower_link;

    int c_upper = get_components_link<dataType>(i, false, upper_link);
    int c_lower = get_components_link<dataType>(i, true, lower_link);

    if (c_upper == 2 && c_lower == 2){
      // on a critical saddle

      for (auto component: upper_link) {
        //compute ascending paths
        //select first vertex in the upper link
        SimplexId candidateVertex = component.second->at(0);
        for (auto v: *component.second){
          if(inputData[v] > inputData[candidateVertex]){
            candidateVertex = v;
          }
        }

        vector<SimplexId> vertices_in_path;
        vertices_in_path.push_back(i);


        //keep steepest ascent until reaching maximum
        SimplexId selected;
        do{
          selected = -1;
          vertices_in_path.push_back(candidateVertex);
          for ( int j = 0; j < triangulation_->getVertexNeighborNumber(candidateVertex); j++){
            SimplexId next;
            triangulation_->getVertexNeighbor(candidateVertex, j, next);

            if (inputData[next] > inputData[candidateVertex]) {
              if(selected = -1 || inputData[next] > inputData[selected]){
                selected = next;
              }
            }
          }
          candidateVertex = selected;
        }while(selected != -1);

        all_paths.push_back(vertices_in_path);
      }

      for (auto component: lower_link) {
        //compute descending paths
        //select first vertex in the lower link
        SimplexId candidateVertex = component.second->at(0);
        for (auto v: *component.second){
          if(inputData[v] < inputData[candidateVertex]){
            candidateVertex = v;
          }
        }

        vector<SimplexId> vertices_in_path;
        vertices_in_path.push_back(i);


        //keep steepest ascent until reaching maximum
        SimplexId selected;
        do{
          selected = -1;
          vertices_in_path.push_back(candidateVertex);
          for ( int j = 0; j < triangulation_->getVertexNeighborNumber(candidateVertex); j++){
            SimplexId next;
            triangulation_->getVertexNeighbor(candidateVertex, j, next);

            if (inputData[next] < inputData[candidateVertex]) {
              if(selected = -1 || inputData[next] < inputData[selected]){
                selected = next;
              }
            }
          }
          candidateVertex = selected;
        }while(selected != -1);

        all_paths.push_back(vertices_in_path);
      }





    }
  }
  return 0;
}
