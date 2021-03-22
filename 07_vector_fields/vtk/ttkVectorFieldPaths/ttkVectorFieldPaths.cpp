#include                  <ttkVectorFieldPaths.h>

using namespace std;
using namespace ttk;

vtkStandardNewMacro(ttkVectorFieldPaths)

int ttkVectorFieldPaths::doIt(vector<vtkDataSet *> &inputs, vector<vtkDataSet *> &outputs){

  Memory m;
  
  vtkDataSet *input = inputs[0];
  // vtkDataSet *output = outputs[0];
  
  Triangulation *triangulation = ttkTriangulation::getTriangulation(input);
 
  if(!triangulation)
    return -1;
  
  triangulation->setWrapper(this);
  vectorFieldPaths_.setupTriangulation(triangulation);
  vectorFieldPaths_.setWrapper(this);
 
  vtkDataArray *inputScalarField = NULL;
  
  if(VectorField.length()){
    inputScalarField = input->GetPointData()->GetArray(VectorField.data());
  }
  else{
    inputScalarField = input->GetPointData()->GetArray(0);
  }
  
  if(!inputScalarField)
    return -2;
  
  
  // calling the executing package
  vectorFieldPaths_.setInputDataPointer(inputScalarField->GetVoidPointer(0));
  vectorFieldPaths_.setOutputDataPointer(outputScalarField_->GetVoidPointer(0));
  switch(inputScalarField->GetDataType()){
    ttkTemplateMacro(vectorFieldPaths_.execute<VTK_TT>(int steps, double delta, vector<pair<float,float>>& line));
  }

  
  return 0;
}
