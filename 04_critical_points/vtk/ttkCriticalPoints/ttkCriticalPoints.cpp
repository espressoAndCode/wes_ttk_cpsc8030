#include                  <ttkCriticalPoints.h>
#include <iostream>

using namespace std;
using namespace ttk;

vtkStandardNewMacro(ttkCriticalPoints)

int ttkCriticalPoints::doIt(vector<vtkDataSet *> &inputs, vector<vtkDataSet *> &outputs){

  Memory m;
  
  vtkDataSet *input = inputs[0];
  // vtkDataSet *output = outputs[0];
  
  Triangulation *triangulation = ttkTriangulation::getTriangulation(input);
 
  if(!triangulation)
    return -1;
  
  triangulation->setWrapper(this);
  criticalPoints_.setupTriangulation(triangulation);
  criticalPoints_.setWrapper(this);
 
  

  vtkDataArray *inputScalarField = NULL;
  
  if(ScalarField.length()){
    inputScalarField = input->GetPointData()->GetArray(ScalarField.data());
  }
  else{
    inputScalarField = input->GetPointData()->GetArray(0);
  }
  
  if(!inputScalarField)
    return -2;
  
    
  // calling the executing package
  criticalPoints_.setInputDataPointer(inputScalarField->GetVoidPointer(0));
  vector<vector<float>> coordinates;
  vector<int> type;
  switch(inputScalarField->GetDataType()){
    ttkTemplateMacro(criticalPoints_.execute<VTK_TT>(coordinates, type));
  }

  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outputs[0]);

  vtkSmartPointer<vtkPoints> points=vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkDoubleArray> funct=vtkSmartPointer<vtkDoubleArray>::New();
  funct->SetNumberOfComponents(1);
  funct->SetName("Scalar_");


  for(int i=0; i<coordinates.size(); i++){
      points->InsertNextPoint(coordinates[i][0],coordinates[i][1],coordinates[i][2]);

      funct->InsertNextTuple1((double)type[i]);
  }
  output->SetPoints(points);
  vtkPointData* pointData=output->GetPointData();
  pointData->AddArray(funct);


  output->Allocate(coordinates.size());

  for (int i = 0; i < coordinates.size(); i++){
    vtkIdType point[1];
    point[0] = i;
    if (type[i] == 0 || type[i] == 1 || type[i] == 2){
      output->InsertNextCell(VTK_VERTEX, 1, point);
    }
  }
  return 0;
}
