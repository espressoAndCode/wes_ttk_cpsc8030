#include                  <ttkPersistence.h>

using namespace std;
using namespace ttk;

vtkStandardNewMacro(ttkPersistence)

int ttkPersistence::doIt(vector<vtkDataSet *> &inputs, vector<vtkDataSet *> &outputs){

  Memory m;
  
  vtkDataSet *input = inputs[0];
  // vtkDataSet *output = outputs[0];

  Triangulation *triangulation = ttkTriangulation::getTriangulation(input);
 
  if(!triangulation)
    return -1;
  
  triangulation->setWrapper(this);
  persistence_.setupTriangulation(triangulation);
  persistence_.setWrapper(this);
 
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
  persistence_.setInputDataPointer(inputScalarField->GetVoidPointer(0));

  vector<vector<float>> coordinates;
  vector<double> persistence;

  switch(inputScalarField->GetDataType()){
    ttkTemplateMacro(persistence_.execute<VTK_TT>(coordinates, persistence));
  }


//code provided start/////////////////////
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outputs[0]);
    vtkSmartPointer<vtkPoints> points=vtkSmartPointer<vtkPoints>::New();

    for(auto coords : coordinates){
        points->InsertNextPoint(coords[0],coords[1],coords[2]);
    }
    output->SetPoints(points);

    vtkSmartPointer<vtkDoubleArray> persOutput=vtkSmartPointer<vtkDoubleArray>::New();
    persOutput->SetNumberOfComponents(1);
    persOutput->SetName("Persistence");

    output->Allocate(coordinates.size()/2);
    int count=0;
    for(auto f : persistence) {
        cout << "In finalLoop - pre\n";
        vtkIdType edge[2];
        edge[0]= count++;
        edge[1]= count++;
        output->InsertNextCell(VTK_LINE, 2, edge);
        persOutput->InsertNextTuple1(f); //preparin a list of values
        cout << "In finalLoop - post\n";
    }
    cout << "Finished\n";
    vtkCellData* cellData = output->GetCellData();
    cellData->AddArray(persOutput); //attaching the list to the cells in my output

//code provided end/////////////////////////



  
  return 0;
}
