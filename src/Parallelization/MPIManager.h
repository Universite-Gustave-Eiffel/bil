#ifndef MPIMANAGER_H
#define MPIMANAGER_H

#include <mpi.h>
#include <stdexcept>
#include <iostream>


#define MPIManager_GetInstance  MPIManager_t::GetInstance()

#define MPIManager_IsMPIActive  MPIManager_GetInstance.IsMPIActive()

#define MPIManager_IsMPIInactive  !(MPIManager_IsMPIActive)

#define MPIManager_NbOfProcessors  MPIManager_GetInstance.NbOfProcessors()

#define MPIManager_RankOfCallingProcess  MPIManager_GetInstance.RankOfCallingProcess()


struct MPIManager_t {
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
  bool IsMPIActive() const {
    int is_initialized = 0;
    int is_finalized = 0;
    
    MPI_Initialized(&is_initialized);
    MPI_Finalized(&is_finalized);
    
    return(is_initialized && !is_finalized);
  }

  // Delete copy/move operations to enforce the singleton pattern
  MPIManager_t(const MPIManager_t&) = delete;
  MPIManager_t& operator=(const MPIManager_t&) = delete;
  MPIManager_t(MPIManager_t&&) = delete;
  MPIManager_t& operator=(MPIManager_t&&) = delete;

  private:
  bool am_i_owner;

  MPIManager_t() : am_i_owner(false) {
    int is_initialized = 0;
    int is_finalized = 0;

    MPI_Initialized(&is_initialized);
    MPI_Finalized(&is_finalized);

    if (is_finalized) {
      throw std::runtime_error("[Library] Cannot run calculations. MPI has already been finalized.");
    }

    if (!is_initialized) {
      // No threading needed, standard initialization is perfect
      MPI_Init(nullptr, nullptr);
      am_i_owner = true;
      std::cout << "[Library] MPI initialized successfully by the library.\n";
    } else {
      //std::cout << "[Library] Attached to the application's existing MPI runtime.\n";
    }
  }

  ~MPIManager_t() {
    if (am_i_owner) {
      int is_finalized = 0;
      
      MPI_Finalized(&is_finalized);
      
      if (!is_finalized) {
        MPI_Finalize();
        std::cout << "[Library] MPI finalized cleanly by the library.\n";
      }
    }
  }
  
  public:
  int NbOfProcessors(){
    int size = 1;

    if(IsMPIActive()){
      MPI_Comm_size(MPI_COMM_WORLD,&size);
    } else {
      std::cerr << "[Library Error] Cannot run compute function: MPI is not active!\n";
    }

    return(size);
  }

  int RankOfCallingProcess(){
    int rank = 1;

    if(IsMPIActive()){
      MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    } else {
      std::cerr << "[Library Error] Cannot run compute function: MPI is not active!\n";
    }

    return(rank);
  }
};
#endif
