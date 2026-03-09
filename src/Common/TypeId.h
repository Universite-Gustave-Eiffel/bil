#ifndef TYPEID_H
#define TYPEID_H


/* Forward declarations */
struct TypeId_t;
struct BConds_t;
struct BCond_t;
struct Buffer_t;
struct CommandLine_t;
struct Context_t;
struct Curve_t;
struct Curves_t;
struct CurvesFile_t;
struct Damage_t;
struct DataFile_t;
struct DataSet_t;
struct Date_t;
struct Dates_t;
struct Elasticity_t;
struct Element_t;
struct Elements_t;
struct ElementSol_t;
struct ElementsSol_t;
struct Exception_t;
struct FEM_t;
struct FEM2_t;
struct Field_t;
struct Fields_t;
struct Function_t;
struct Functions_t;
struct FVM_t;
struct Geometry_t;
struct Graph_t;
struct ICond_t;
struct IConds_t;
struct InternationalSystemOfUnits_t;
struct IntFct_t;
struct IntFcts_t;
struct IterProcess_t;
struct Load_t;
struct Loads_t;
struct Material_t;
struct Materials_t;
struct Math_t;
struct Matrix_t;
struct Mesh_t;
struct Message_t;
struct Model_t;
struct Models_t;
struct Module_t;
struct Modules_t;
struct Node_t;
struct Nodes_t;
struct NodeSol_t;
struct NodesSol_t;
struct ObVals_t;
struct ObVal_t;
struct Options_t;
struct OutputFile_t;
struct OutputFiles_t;
struct Periodicity_t;
struct Periodicities_t;
struct Plasticity_t;
struct Point_t;
struct Points_t;
struct Result_t;
struct Results_t;
struct Session_t;
struct ShapeFct_t;
struct ShapeFcts_t;
struct Solution_t;
struct Solutions_t;
struct Solver_t;
struct Solvers_t;
struct TextFile_t;
struct TimeStep_t;
struct Unit_t;
struct Units_t;
struct View_t;
struct Views_t;


#define TypeId_GetIdNumber(TID)      ((TID)->GetIdNumber())
#define TypeId_GetSize(TID)          ((TID)->GetSize())
#define TypeId_GetData(TID)          ((TID)->GetData())
#define TypeId_GetDelete(TID)        ((TID)->GetDelete())

#define TypeId_Set(TID,X)            ((TID)->Set(X))
#define TypeId_SetDelete(TID,X)      ((TID)->SetDelete(X))
#define TypeId_Holds(TID,T)          ((TID)->Holds<T>())


#define TypeId_Create()              TypeId_t::Create()
#define TypeId_Delete(TID)           ((TID)->Delete())
#define TypeId_DeleteData(TID,X)     ((TID)->DeleteData(X))

#define TypeId_ConvertFromSizeToInt(X)      TypeId_t::ConvertFromSizeToInt(X)




#define TypeId_ListCPP \
        void*\
        ,char*\
        ,int*\
        ,size_t*\
        ,double*

#define TypeId_ListBIL \
        ,BCond_t*\
        ,BConds_t*\
        ,Buffer_t*\
        ,CommandLine_t*\
        ,Context_t*\
        ,Curve_t*\
        ,Curves_t*\
        ,CurvesFile_t*\
        ,Damage_t*\
        ,DataFile_t*\
        ,DataSet_t*\
        ,Date_t*\
        ,Dates_t*\
        ,Elasticity_t*\
        ,Element_t*\
        ,Elements_t*\
        ,ElementSol_t*\
        ,ElementsSol_t*\
        ,Exception_t*\
        ,FEM_t*\
        ,FEM2_t*\
        ,Field_t*\
        ,Fields_t*\
        ,Function_t*\
        ,Functions_t*\
        ,FVM_t*\
        ,Geometry_t*\
        ,Graph_t*\
        ,ICond_t*\
        ,IConds_t*\
        ,InternationalSystemOfUnits_t*\
        ,IntFct_t*\
        ,IntFcts_t*\
        ,IterProcess_t*\
        ,Load_t*\
        ,Loads_t*\
        ,Material_t*\
        ,Materials_t*\
        ,Math_t*\
        ,Matrix_t*\
        ,Mesh_t*\
        ,Message_t*\
        ,Model_t*\
        ,Models_t*\
        ,Module_t*\
        ,Modules_t*\
        ,Node_t*\
        ,Nodes_t*\
        ,NodeSol_t*\
        ,NodesSol_t*\
        ,ObVals_t*\
        ,ObVal_t*\
        ,Options_t*\
        ,OutputFile_t*\
        ,OutputFiles_t*\
        ,Periodicity_t*\
        ,Periodicities_t*\
        ,Plasticity_t*\
        ,Point_t*\
        ,Points_t*\
        ,Result_t*\
        ,Results_t*\
        ,Session_t*\
        ,ShapeFct_t*\
        ,ShapeFcts_t*\
        ,Solution_t*\
        ,Solutions_t*\
        ,Solver_t*\
        ,Solvers_t*\
        ,TextFile_t*\
        ,TimeStep_t*\
        ,Unit_t*\
        ,Units_t*\
        ,View_t*\
        ,Views_t*


#include "BilConfig.h"
#ifdef HAVE_SUPERLUDIST
  //#include "superlu.h"
  struct dScalePermstruct_t;
  struct dLUstruct_t;
  struct gridinfo_t;
  
  #define TypeId_ListSUPERLU \
          ,dScalePermstruct_t*\
          ,dLUstruct_t*\
          ,gridinfo_t*
#else
  #define TypeId_ListSUPERLU
#endif

#ifdef HAVE_PETSC
  #include <petsc.h>
  //struct KSP;
  //struct PC;
  #define TypeId_ListPETSC \
          ,KSP*\
          ,PC*
#else
  #define TypeId_ListPETSC
#endif


#define TypeId_List \
        TypeId_ListCPP \
        TypeId_ListBIL \
        TypeId_ListSUPERLU \
        TypeId_ListPETSC


#include <variant>
//#include <type_traits>
using TypeId_Variant_t = std::variant<TypeId_List>;

//#include "GenericObject.h"


#include <stdio.h>
#include <stdexcept>
#include <limits>
#include <stdarg.h>
#include "Mry.h"


constexpr size_t TypeId_MaxSizeToInt = static_cast<size_t>((std::numeric_limits<int>::max)());


struct TypeId_t {
  private:
  size_t _index ;          /* Index of the alternative value */
  size_t _size ;           /* Size of data pointed to by the alternative */
  TypeId_Variant_t _var;
  void (*_delete)(void*);
  //GenericObject_Delete_t* _delete;
  /*
   * _var.index() = index of the alternative held by _var.
   * std::holds_alternative<T>(_var) returns true if _var holds T false otherwise.
   * std::get<T>(_var) or std::get<index>(_var) returns a reference to the value stored in _var
   * std::get_if<T>(&_var) or std::get_if<index>(&_var) returns a pointer to the value stored in _var
   */
  
  public:
  TypeId_t() {}
  ~TypeId_t(){}
  
  /* The getters */
  size_t GetIdNumber(){return(_index);}
  size_t GetSize(){return(_size);}
  auto   GetData(){
    return(std::visit([](auto const& x)->void*{return((void*)x);},_var));
  }
  void (*GetDelete(void))(void*){return(_delete);}
  //GenericObject_Delete_t* GetDelete(){return(_delete);}
  
  /* The setters */
  template<typename T> 
  void Set(T* x){
    try {
      _var   = x;
    } catch (const std::bad_variant_access& ex) {
      fflush(stdout);
      fprintf(stderr,"\nError in TypeId_t::Set...\n");
      fprintf(stderr,"bad variant access: %s",ex.what());
      fprintf(stderr,"\n...stop\n");
      fflush(stderr);
      exit(EXIT_SUCCESS);
    }
    _index = _var.index();
    _size  = sizeof(T);
  }
  void SetDelete(void (*x)(void*)){_delete = x;}
  
  /* Other member functions */
  template<typename T> 
  bool Holds() {return(std::holds_alternative<T*>(_var));}

  static TypeId_t* Create() {
    TypeId_t* tid = (TypeId_t*) Mry_New(TypeId_t) ;

    tid->_index = 0;
    tid->_size  = 0;
    tid->_delete = NULL;
  
    return(tid) ;
  }

  void  Delete() {
    if(this) {}
  }
  
  inline void  DeleteData(void*);
  
  /* Safely convert from size_t to int. */
  static int ConvertFromSizeToInt(size_t size) {
    if(size > TypeId_MaxSizeToInt) {
      throw std::overflow_error("size_t value is too big to fit into an int.");
    }
    return static_cast<int>(size);
  }
} ;




#if 0
extern void Damage_Delete(void*);
extern void DataSet_Delete(void*);
extern void Elasticity_Delete(void*);
extern void ElementsSol_Delete(void*);
extern void Exception_Delete(void*);
extern void FEM_Delete(void*);
extern void FEM2_Delete(void*);
extern void FVM_Delete(void*);
extern void InternationalSystemOfUnits_Delete(void*);
extern void Math_Delete(void*);
//extern void Message_Delete(void*);
extern void Options_Delete(void*);
extern void Plasticity_Delete(void*);
extern void Solutions_Delete(void*);
extern void Solver_Delete(void*);
extern void Solvers_Delete(void*);
#ifdef HAVE_SUPERLUDIST
extern void dScalePermstructFree(void*);
extern void dLUstructFree(void*);
extern void superlu_gridexit(void*);
#endif
#else
#include "Damage.h"
#include "DataSet.h"
#include "Elasticity.h"
#include "ElementsSol.h"
#include "Exception.h"
#include "FEM.h"
#include "FEM2.h"
#include "FVM.h"
#include "InternationalSystemOfUnits.h"
#include "Math_.h"
#include "Message.h"
#include "Options.h"
#include "Plasticity.h"
#include "Solutions.h"
#include "Solver.h"
#include "Solvers.h"
#endif
#ifdef HAVE_SUPERLUDIST
#include "superlu.h"
#endif
#ifdef HAVE_PETSC
#include <petsc.h>
#endif


void  TypeId_t::DeleteData(void* self) {
    if(this->Holds<char>()) {
    } else if(this->Holds<double>()) {
    } else if(this->Holds<int>()) {
    } else if(this->Holds<Damage_t>()) {
      Damage_Delete(self);
    } else if(this->Holds<DataSet_t>()) {
      DataSet_Delete(self);
    } else if(this->Holds<Elasticity_t>()) {
      Elasticity_Delete(self);
    } else if(this->Holds<ElementsSol_t>()) {
      ElementsSol_Delete(self);
    } else if(this->Holds<Exception_t>()) {
      Exception_Delete(self);
    } else if(this->Holds<FEM_t>()) {
      FEM_Delete(self);
    } else if(this->Holds<FEM2_t>()) {
      FEM2_Delete(self);
    } else if(this->Holds<FVM_t>()) {
      FVM_Delete(self);
    } else if(this->Holds<InternationalSystemOfUnits_t>()) {
      InternationalSystemOfUnits_Delete(self);
    } else if(this->Holds<Math_t>()) {
      Math_Delete(self);
    } else if(this->Holds<Message_t>()) {
      Message_Delete(self);
    } else if(this->Holds<Options_t>()) {
      Options_Delete(self);
    } else if(this->Holds<Plasticity_t>()) {
      Plasticity_Delete(self);
    } else if(this->Holds<Solutions_t>()) {
      Solutions_Delete(self);
    } else if(this->Holds<Solver_t>()) {
      Solver_Delete(self);
    } else if(this->Holds<Solvers_t>()) {
      Solvers_Delete(self);
    /* from SuperLU_DIST */
    #ifdef HAVE_SUPERLUDIST
    } else if(this->Holds<dScalePermstruct_t>()) {
      dScalePermstructFree(self);
    } else if(this->Holds<dLUstruct_t>()) {
      dLUstructFree(self);
    } else if(this->Holds<gridinfo_t>()) {
      superlu_gridexit(self);
    #endif
    /* from Petsc */
    #ifdef HAVE_PETSC
    } else if(this->Holds<KSP>()) {
      KSPDestroy((KSP*) self);
    } else if(this->Holds<PC>()) {
      PCDestroy((PC*) self);
    #endif
    } else {
      Message_FatalError("DeleteData: unknown type") ;
    }

    return ;
}



#if 0
constexpr std::size_t TypeId_SizeOfList = std::variant_size_v<TypeId_Variant_t>;

template<std::size_t I>
using TypeId_TypeOfIndex_t = std::variant_alternative_t<I,TypeId_Variant_t>;

template <typename T,std::size_t... Is>
constexpr std::size_t TypeId_Index_impl(std::index_sequence<Is...>) {
  return((std::is_same_v<T,TypeId_TypeOfIndex_t<Is>> * Is) + ...);
}

template <typename T>
constexpr std::size_t TypeId_IndexOfType = TypeId_Index_impl<T*>(std::make_index_sequence<TypeId_SizeOfList>{});
#endif

#endif
