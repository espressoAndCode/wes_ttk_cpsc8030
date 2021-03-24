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
 
  // use a pointer-base copy for the input data -- to adapt if your wrapper does
  // not produce an output of the type of the input.
  // output->ShallowCopy(input);
  
  // in the following, the target scalar field of the input is replaced in the 
  // variable 'output' with the result of the computation.
  // if your wrapper produces an output of the same type of the input, you 
  // should proceed in the same way.
  vtkDataArray *inputVectorField1 = NULL;
  vtkDataArray *inputVectorField2 = NULL;
  
  if(VectorField[0].length()){
    inputVectorField1 = input->GetPointData()->GetArray(VectorField[0].data());
    inputVectorField2 = input->GetPointData()->GetArray(VectorField[1].data());
  }
  else{
    inputVectorField1 = input->GetPointData()->GetArray(0);
    inputVectorField2 = input->GetPointData()->GetArray(1);
  }
  
  if(!inputVectorField1)
    return -2;
  

  // calling the executing package
  vectorFieldPaths_.setInputDataPointer(inputVectorField1->GetVoidPointer(0), inputVectorField2->GetVoidPointer(0));

  vector<pair<float,float>> line;

  switch(inputVectorField1->GetDataType()){
    ttkTemplateMacro(vectorFieldPaths_.execute<VTK_TT>(SomeIntegerArgument, SomeDoubleArgument, line));
  }

  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outputs[0]);
  
  cout << "line.size() = " << line.size() << "\n";

  for (auto pairs: line){
    cout << "("<< pairs.first << ", " << pairs.second << ")\n";
  }

    vtkSmartPointer<vtkPoints> points=vtkSmartPointer<vtkPoints>::New();

    for(auto pairs : line){
        points->InsertNextPoint(pairs.first,pairs.second,0);

    }
    output->SetPoints(points);


    output->Allocate(line.size()*2);

    for(auto pairs : line) {
        vtkIdType edge[2];
        edge[0]= pairs.first;
        edge[1]= pairs.second;
        output->InsertNextCell(VTK_LINE, 2, edge);

    }



  return 0;
}
