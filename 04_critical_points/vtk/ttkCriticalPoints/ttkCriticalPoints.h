/// \ingroup vtk
/// \class ttkCriticalPoints
/// \author Your Name Here <Your Email Address Here>
/// \date The Date Here.
///
/// \brief TTK VTK-filter that wraps the criticalPoints processing package.
///
/// VTK wrapping code for the @CriticalPoints package.
/// 
/// \param Input Input scalar field (vtkDataSet)
/// \param Output Output scalar field (vtkDataSet)
///
/// This filter can be used as any other VTK filter (for instance, by using the 
/// sequence of calls SetInputData(), Update(), GetOutput()).
///
/// See the related ParaView example state files for usage examples within a 
/// VTK pipeline.
///
/// \sa ttk::CriticalPoints
#pragma once

// VTK includes -- to adapt
#include                  <vtkCharArray.h>
#include                  <vtkDataArray.h>
#include                  <vtkDataSet.h>
#include                  <vtkDataSetAlgorithm.h>
#include                  <vtkDoubleArray.h>
#include                  <vtkFiltersCoreModule.h>
#include                  <vtkFloatArray.h>
#include                  <vtkInformation.h>
#include                  <vtkIntArray.h>
#include                  <vtkObjectFactory.h>
#include                  <vtkPointData.h>
#include                  <vtkSmartPointer.h>

// ttk code includes
#include                  <CriticalPoints.h>
#include                  <ttkWrapper.h>

// in this example, this wrapper takes a data-set on the input and produces a 
// data-set on the output - to adapt.
// see the documentation of the vtkAlgorithm class to decide from which VTK 
// class your wrapper should inherit.
#ifndef TTK_PLUGIN
class VTKFILTERSCORE_EXPORT ttkCriticalPoints
#else
class ttkCriticalPoints
#endif
  : public vtkDataSetAlgorithm, public ttk::Wrapper{

  public:
    
    static ttkCriticalPoints* New();
    vtkTypeMacro(ttkCriticalPoints, vtkDataSetAlgorithm)
    
    // default ttk setters
    vtkSetMacro(debugLevel_, int);
    
    void SetThreadNumber(int threadNumber){
      ThreadNumber = threadNumber;
      SetThreads();
    }
    void SetUseAllCores(bool onOff){
      UseAllCores = onOff;
      SetThreads();
    }
    // end of default ttk setters

    vtkSetMacro(SomeIntegerArgument, int);
    vtkGetMacro(SomeIntegerArgument, int);
   
    vtkSetMacro(SomeDoubleArgument, double);
    vtkGetMacro(SomeDoubleArgument, double);
    
    vtkSetMacro(SomeOption, bool);
    vtkGetMacro(SomeOption, bool);
    
    vtkSetMacro(ScalarField, std::string);
    vtkGetMacro(ScalarField, std::string);
    // end of TODO-4


    int FillInputPortInformation(int port, vtkInformation *info) override {
      
      switch(port){
        case 0:
          info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid"); 
          break;
        default:
          break;
      }
      
      return 1;
    }

    int FillOutputPortInformation(int port, vtkInformation *info) override {
      
      switch(port){
        case 0:
          info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid"); 
          break;
        default:
          break;
      }
      
      return 1;
    }

        
  protected:
   
    ttkCriticalPoints(){
      
        // init
      SomeIntegerArgument = 1;
      SomeDoubleArgument = 1;
      SomeOption = true;
      // outputScalarField_ = NULL;
      
      UseAllCores = true;
      ThreadNumber = 1;
      debugLevel_ = 3;
      
      SetNumberOfInputPorts(1);
      SetNumberOfOutputPorts(1);

    }
    
    ~ttkCriticalPoints(){};
    
    TTK_SETUP();
    
    
  private:
    
    int                   SomeIntegerArgument;
    double                SomeDoubleArgument;
    bool                  SomeOption;
    std::string           ScalarField;
    ttk::CriticalPoints            criticalPoints_;
    
};
