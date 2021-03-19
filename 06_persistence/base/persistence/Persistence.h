/// \ingroup base
/// \class ttk::Persistence 
/// \author Your Name Here <Your Email Address Here>
/// \date The Date Here.
///
/// \brief TTK %persistence processing package.
///
/// %Persistence is a TTK processing package that takes a scalar field on the input 
/// and produces a scalar field on the output.
///
/// \sa ttk::Triangulation
/// \sa ttkPersistence.cpp %for a usage example.

#pragma once

// base code includes
#include                  <Triangulation.h>
#include                  <Wrapper.h>
#include                  <boundarymatrix.h>


namespace ttk{
  
  class Persistence : public Debug{

    public:
        
      Persistence();
      
      ~Persistence();

      template <class dataType>
        int execute(vector<vector<float> > &coordinates, vector<double> &persistence);

      template <class dataType>
        void computeBoundaryMatrix(BoundaryMatrix<dataType> &bdmatrix);
    

      inline int setInputDataPointer(void *data){
        inputData_ = data;
        return 0;
      }

      inline int setupTriangulation(Triangulation *triangulation){
        triangulation_ = triangulation;
       
        if(triangulation_){
          
          triangulation_->preprocessVertexNeighbors();
          triangulation_->preprocessTriangleEdges();

        }
        
        return 0;
      }

      template <class dataType>
      inline dataType computeFunctionValue(SimplexId name, uint d){
        dataType *inputData = (dataType *) inputData_;
        dataType functionValue = -10000;

        if (d == 0){
          functionValue = functionValue < inputData[name] ? inputData[name] : functionValue;
          return inputData[name];
        }


        for (int j = 0; j <= d; j++){
          SimplexId v;

          if(d==1){
            triangulation_->getEdgeVertex(name,j,v);
          } else if (d==2){
            triangulation_->getTriangleVertex(name, j, v);
          }

          functionValue = functionValue < inputData[v] ? inputData[v] : functionValue;

        }

        return functionValue;
      }
    
    protected:
    
      void                  *inputData_, *outputData_;
      Triangulation         *triangulation_;
  };
}

// if the package is a pure template class, uncomment the following line
// #include                  <Persistence.cpp>

// template functions
template <class dataType> int ttk::Persistence::execute(
  vector<vector<float> > &coordinates, vector<double> &persistence){
  // dataType *inputData = (dataType *) inputData_;

  BoundaryMatrix<dataType> bdmatrix = BoundaryMatrix<dataType>();

  computeBoundaryMatrix(bdmatrix);

  bdmatrix.sort();
  bdmatrix.reduce();

  map<int, int> pairs;
  bdmatrix.getPairs(pairs, true);

  //work with pairs and prepare output
  for (auto pair : pairs){
    SimplexId firstIndex = pair.first;    //i-simplex
    SimplexId secondIndex = pair.second;  //(i+1)-simplex
    SimplexId firstRealIndex;    
    SimplexId secondRealIndex;  
    uint iSimplex = 0;
    vector<float> coords(3);

    if(firstIndex < triangulation_->getNumberOfVertices()) {
      //vertex
      cout << "\nVertex" << "\n";
      firstRealIndex = firstIndex;
      secondRealIndex = secondIndex + triangulation_->getNumberOfVertices();

    }else if(firstIndex < triangulation_->getNumberOfVertices()+triangulation_->getNumberOfEdges() ){
      //edge
      cout << "\nEdge" << "\n";
      firstRealIndex = firstIndex + triangulation_->getNumberOfVertices();
      secondRealIndex = secondIndex + triangulation_->getNumberOfEdges();
      iSimplex = 1;
    }else{
      //triangle
    }

    triangulation_->getVertexPoint(firstRealIndex, coords[0], coords[1], coords[2]);
    dataType persistenceValue = computeFunctionValue<dataType>(secondRealIndex, iSimplex+1) - computeFunctionValue<dataType>(firstRealIndex, iSimplex);

    coordinates.push_back(coords);
    persistence.push_back(persistenceValue);




  }

  return 0;
}


template <class dataType>
  void ttk::Persistence::computeBoundaryMatrix(BoundaryMatrix<dataType> &bdmatrix){
    dataType *inputData = (dataType *) inputData_;

    // bdmatrix.addValue(idx of simplex, idx of boundary simplex, function value);

    for (int i=0; i < triangulation_->getNumberOfVertices(); i++){
      bdmatrix.addValue(i,-1, inputData[i]);
    }

    SimplexId shift = triangulation_->getNumberOfVertices();
    for (int i=0; i < triangulation_->getNumberOfEdges(); i++){

      SimplexId v;
      for (int j = 0; j<2; j++){
        triangulation_->getEdgeVertex(i,j,v);
        bdmatrix.addValue(i+shift, v, computeFunctionValue<dataType> (i,1));

      }
    }

    shift = triangulation_->getNumberOfVertices() + triangulation_->getNumberOfEdges();

    for (int i=0; i < triangulation_->getNumberOfTriangles(); i++){

      SimplexId e;
      for (int j = 0; j<3; j++){
        triangulation_->getTriangleEdge(i,j,e);
        bdmatrix.addValue(i+shift, e+triangulation_->getNumberOfVertices(), computeFunctionValue<dataType> (i,2));

      }
    }

  }