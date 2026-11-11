#include "TypeId.h"
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