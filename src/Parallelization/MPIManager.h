#ifndef MPIMANAGER_H
#define MPIMANAGER_H

#include "BilConfig.h"

#if defined HAVE_MPI

#define MPIManager_GetInstance  MPIManager_t::GetInstance()

#define MPIManager_IsMPIActive  MPIManager_GetInstance.IsMPIActive()

#define MPIManager_IsMPIInactive  !(MPIManager_IsMPIActive)

#define MPIManager_NbOfProcessors  MPIManager_GetInstance.NbOfProcessors()

#define MPIManager_RankOfCallingProcess  MPIManager_GetInstance.RankOfCallingProcess()


struct MPIManager_t {
  private:
  bool am_i_owner;

  public:
  static MPIManager_t& GetInstance() {
    /* Returns a static block variable: "instance".
       It is initialized the first time control passes through its declaration.
       On all further calls the declaration is skipped.
       Because it is a static local variable, its lifetime is tied to the
       lifetime of the program itself. 
       It stays alive in the computer's memory, holding the MPI state.
       When the main() function finishes (or exit()) is called) and
       the entire application begins to shut down, the c++ runtime 
       automatically triggers the destructors of all static objects. 
       When ~MPIManager() runs, the library will cleanly call 
       MPI_Finalize() right before the process exits.
     */
    static MPIManager_t instance;
    
    return instance;
  }

  // Quick runtime check to see if MPI is fully active and safe to use
  bool IsMPIActive() const;

  MPIManager_t();
  // Delete copy/move operations to enforce the singleton pattern
  MPIManager_t(const MPIManager_t&) = delete;
  MPIManager_t& operator=(const MPIManager_t&) = delete;
  MPIManager_t(MPIManager_t&&) = delete;
  MPIManager_t& operator=(MPIManager_t&&) = delete;

  ~MPIManager_t();
  
  int NbOfProcessors();
  int RankOfCallingProcess();
};

#endif
#endif
