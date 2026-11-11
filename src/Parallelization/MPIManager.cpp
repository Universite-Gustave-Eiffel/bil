#include "BilConfig.h"

#if defined HAVE_MPI

#include <mpi.h>
#include <stdexcept>
#include <iostream>
#include "MPIManager.h"

  bool MPIManager_t::IsMPIActive() const {
    int is_initialized = 0;
    int is_finalized = 0;
    
    MPI_Initialized(&is_initialized);
    MPI_Finalized(&is_finalized);
    
    return(is_initialized && !is_finalized);
  }


  MPIManager_t::MPIManager_t() : am_i_owner(false) {
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

  MPIManager_t::~MPIManager_t() {
    if (am_i_owner) {
      int is_finalized = 0;
      
      MPI_Finalized(&is_finalized);
      
      if (!is_finalized) {
        MPI_Finalize();
        std::cout << "[Library] MPI finalized cleanly by the library.\n";
      }
    }
  }

  int MPIManager_t::NbOfProcessors(){
    int size = 1;

    if(IsMPIActive()){
      MPI_Comm_size(MPI_COMM_WORLD,&size);
    } else {
      std::cerr << "[Library Error] Cannot run compute function: MPI is not active!\n";
    }

    return(size);
  }


  int MPIManager_t::RankOfCallingProcess(){
    int rank = 1;

    if(IsMPIActive()){
      MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    } else {
      std::cerr << "[Library Error] Cannot run compute function: MPI is not active!\n";
    }

    return(rank);
  }

#endif
