#include                  <ttkBoundaryBased.h>

using namespace std;
using namespace ttk;

vtkStandardNewMacro(ttkBoundaryBased)

int ttkBoundaryBased::doIt(vector<vtkDataSet *> &inputs, vector<vtkDataSet *> &outputs){

  Memory m;
  
  vtkDataSet *input = inputs[0];
  // vtkDataSet *output = outputs[0];
  
  Triangulation *triangulation = ttkTriangulation::getTriangulation(input);
 
  if(!triangulation)
    return -1;
  
  triangulation->setWrapper(this);
  boundaryBased_.setupTriangulation(triangulation);
  boundaryBased_.setWrapper(this);
 



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
  boundaryBased_.setInputDataPointer(inputScalarField->GetVoidPointer(0));
  vector<vector<SimplexId>> final_points;

  switch(inputScalarField->GetDataType()){
    ttkTemplateMacro(boundaryBased_.execute<VTK_TT>(final_points));
  }
  
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outputs[0]);
  

  vtkSmartPointer<vtkPoints> points=vtkSmartPointer<vtkPoints>::New();


  for (int i = 0; i < final_points.size(); i++)
  {
    points->InsertNextPoint(final_points[i][0], final_points[i][1], final_points[i][2]);
  }


  output->SetPoints(points);

  output->Allocate(final_points.size() / 2);

  for (int i = 0; i < final_points.size(); i+=2){
    vtkIdType edge[2];
    edge[0] = i;
    edge[1] = i + 1;
    output->InsertNextCell(VTK_LINE, 2, edge);

  }
  
  return 0;
}
