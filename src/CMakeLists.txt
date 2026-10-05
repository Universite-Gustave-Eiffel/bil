# Compiler flags
# --------------
include(SetCompilerFlags)


# Options (disabled by default)
# -----------------------------
option(ENABLE_OPENMP   "Enable OpenMP parallelization"    OFF)
option(ENABLE_PTHREADS "Enable Pthreads parallelization"  OFF)
option(ENABLE_MPI      "Enable MPI parallelization"       OFF)
option(ENABLE_AUTODIFF "Enable autodiff interface"        OFF)
option(ENABLE_FLTK     "Enable FLTK for GUI"              OFF)
option(ENABLE_PETSC    "Enable PETSc linear solvers"      OFF)
option(ENABLE_BLAS     "Enable BLAS for linear algebra"   OFF)
option(ENABLE_LAPACK   "Enable Lapack for linear algebra" OFF)
option(ENABLE_EIGEN    "Enable Eigen for linear algebra"  OFF)
option(ENABLE_SUPERLU  "Enable SuperLU solver"            OFF)
include(EnableBilOptions)


# Extra-libraries (alternatives to options)
# -----------------------------------------
include(SetExtralibs)
set_extralibs(BIL_EXTRALIBS BILEXTRALIBS_INSTALL_RPATH)


# External-libraries (enabling options)
# -------------------------------------

unset(BIL_EXTERNLIBS)
if(ENABLE_OPENMP)
  find_package(OpenMP)
  if(OpenMP_FOUND)
    Message(STATUS "Compiler supports OpenMP.")
    set(HAVE_OPENMP TRUE)
    #target_link_libraries(${BIL_EXE} OpenMP::OpenMP_C)
    list(APPEND BIL_EXTERNLIBS OpenMP::OpenMP_C)
  else()
    Message(STATUS "Compiler doesn't support OpenMP.")
  endif()
endif()


if(ENABLE_PTHREAD)
  find_package(Threads)
  if(Threads_FOUND)
    Message(STATUS "Compiler supports Pthread.")
    set(HAVE_PTHREAD TRUE)
    #target_link_libraries(${BIL_EXE} Threads::Threads)
    list(APPEND BIL_EXTERNLIBS Threads::Threads)
  else()
    Message(STATUS "Compiler doesn't support Pthread.")
  endif()
endif()


if(ENABLE_MPI)
  find_package(MPI)
  if(MPI_FOUND)
    Message(STATUS "Compiler supports MPI.")
    set(HAVE_MPI TRUE)
    #target_link_libraries(${BIL_EXE} MPI::MPI_CXX)
    list(APPEND BIL_EXTERNLIBS MPI::MPI_CXX)
  else()
    Message(STATUS "Compiler doesn't support MPI.")
  endif()
else()
  string(FIND "${BIL_EXTRALIBS}" "dist" dist)
  string(FIND "${BIL_EXTRALIBS}" "libpetsc" libpetsc)
  if(${dist} GREATER -1 OR ${libpetsc} GREATER -1)
    Message(FATAL_ERROR "Please enable MPI.")
  endif()
endif()


if(ENABLE_AUTODIFF)
  UNSET(AUTODIFF_PATH CACHE)
  find_path(AUTODIFF_PATH forward/real.hpp PATHS ${CMAKE_INSTALL_FULL_INCLUDEDIR}/autodiff)
  if(AUTODIFF_PATH)
    Message(STATUS "Compiler supports autodiff.")
    set(HAVE_AUTODIFF TRUE)
  else()
    Message(FATAL ERROR "Compiler doesn't support autodiff.")
  endif()
  
  find_package(Eigen3 REQUIRED)
  if(Eigen3_FOUND)
    Message(STATUS "Compiler supports Eigen3.")
  else()
    Message(FATAL ERROR "Compiler doesn't support Eigen3.")
  endif()
endif()


if(ENABLE_FLTK)
  # first, try to use fltk-config for fltk >= 1.3 (FindFLTK is buggy on Unix,
  # where e.g. xft and xinerama options are not dealt with)
  find_program(FLTK_CONFIG_SCRIPT fltk-config)
  if(FLTK_CONFIG_SCRIPT)
    execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --api-version
                    OUTPUT_VARIABLE FLTK_VERSION)
    string(STRIP "${FLTK_VERSION}" FLTK_VERSION)
    if(FLTK_VERSION VERSION_GREATER_EQUAL "1.3.0")
      #add_subdirectory(src/fltk)
      #set_config_option(HAVE_FLTK "Fltk")
      set(HAVE_FLTK TRUE)
      message(STATUS "Using fltk-config script for Fltk " ${FLTK_VERSION})
      execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --use-gl --use-images --includedir
                      OUTPUT_VARIABLE FLTK_INCLUDE_DIR)
      string(STRIP ${FLTK_INCLUDE_DIR} FLTK_INCLUDE_DIR)
      list(APPEND EXTERNAL_INCLUDES ${FLTK_INCLUDE_DIR})
      execute_process(COMMAND ${FLTK_CONFIG_SCRIPT} --use-gl --use-images --ldflags
                      OUTPUT_VARIABLE FLTK_LIBRARIES)
      string(STRIP ${FLTK_LIBRARIES} FLTK_LIBRARIES)
    endif()
  else()
  # then try the built-in FindFLTK module
    message(STATUS "The fltk-config script for Fltk is not found")
    #set(FLTK_SKIP_FORMS TRUE)
    #set(FLTK_SKIP_FLUID TRUE)
    find_package(FLTK)
    if(FLTK_FOUND)
      Message(STATUS "Compiler supports FLTK.")
      #add_subdirectory(src/fltk)
      #set_config_option(HAVE_FLTK "Fltk")
      set(HAVE_FLTK TRUE)
      #list(APPEND EXTERNAL_INCLUDES ${FLTK_INCLUDE_DIR})
    endif()
  endif()
  # workaround for Fedora/Suse messing up fltk-config (see issue #417)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    string(REGEX MATCH "X11" FLTK_X11 ${FLTK_LIBRARIES})
    if(NOT FLTK_X11)
      Message(STATUS "The library FLTK_X11 is not found")
      find_package(X11)
      if(X11_FOUND)
        Message(STATUS "Compiler supports X11.")
        #list(APPEND EXTERNAL_INCLUDES ${X11_INCLUDE_DIR})
        #list(APPEND EXTERNAL_LIBRARIES ${X11_LIBRARIES})
      endif()
    endif()
  endif()
endif()


if(ENABLE_PETSC)
  # new-style PETSc installations (in standard system directories)
  find_library(PETSC_LIBS petsc)
  find_path(PETSC_INC "petsc.h" PATH_SUFFIXES include/petsc)
  if(PETSC_LIBS AND PETSC_INC)
    Message(STATUS "Compiler supports PETSC.")
    set(HAVE_PETSC TRUE)
    #target_link_libraries(${BIL_EXE} ${PETSC_LIBS})
    list(APPEND BIL_EXTERNLIBS ${PETSC_LIBS})
    #list(APPEND EXTERNAL_LIBRARIES ${PETSC_LIBS})
    #list(APPEND EXTERNAL_INCLUDES ${PETSC_INC})
  else()
    Message(STATUS "Compiler doesn't support PETSC.")
  endif()
  if(NOT HAVE_MPI)
    Message(FATAL_ERROR "PETSC needs MPI, please enable MPI.")
  endif()
endif()



if(ENABLE_BLAS)
  # use the standard cmake tests,
  # which require a working Fortran compiler
  enable_language(Fortran)
  find_package(BLAS)
  if(BLAS_FOUND)
    Message(STATUS "Compiler supports Blas.")
    set(HAVE_BLAS TRUE)
    #target_link_libraries(${BIL_EXE} ${BLAS_LIBRARIES})
    list(APPEND BIL_EXTERNLIBS ${BLAS_LIBRARIES})
  else()
    Message(STATUS "Compiler doesn't support Blas.")
  endif()
endif()


if(ENABLE_LAPACK)
  # use the standard cmake tests,
  # which require a working Fortran compiler
  enable_language(Fortran)
  find_package(LAPACK)
  if(LAPACK_FOUND)
    Message(STATUS "Compiler supports Lapack.")
    set(HAVE_LAPACK TRUE)
    #target_link_libraries(${BIL_EXE} ${LAPACK_LIBRARIES})
    list(APPEND BIL_EXTERNLIBS ${LAPACK_LIBRARIES})
  else()
    Message(STATUS "Compiler doesn't support Lapack.")
  endif()
endif()


if(ENABLE_SUPERLU)
  find_library(SUPERLU_LIB superlu)
  find_path(SUPERLU_INC "slu_ddefs.h" PATH_SUFFIXES include/superlu)
  if(SUPERLU_LIB AND SUPERLU_INC)
    Message(STATUS "Compiler supports SuperLU.")
    set(HAVE_SUPERLU TRUE)
    #target_link_libraries(${BIL_EXE} ${SUPERLU_LIB})
    list(APPEND BIL_EXTERNLIBS ${SUPERLU_LIB})
  else()
    Message(STATUS "Compiler doesn't support SuperLU.")
  endif()
endif()

Message("External libraries (src) = ${BIL_EXTERNLIBS}")


# Bil source files
# ----------------
include(DirsAndSourcesOfBil)
dirs_and_sources_of_bil(BIL_DIRSALL BIL_SRCALL BIL_SRCMAIN)


#[[
  Building the shared library target
#]]
 
add_library(${BIL_LIB} SHARED ${BIL_SRCALL})

target_include_directories(${BIL_LIB}
    PRIVATE
    "${BIL_DIRSALL}"
    "${PROJECT_SOURCE_DIR}"
)

# autodiff requires a c++17 enabled compiler
target_compile_features(${BIL_LIB}
    PUBLIC c_std_11 cxx_std_17
)

set_target_properties(${BIL_LIB}
  PROPERTIES 
  INSTALL_RPATH "${BIL_INSTALL_FULL_LIBDIR};${BILEXTRALIBS_INSTALL_RPATH}"
  LIBRARY_OUTPUT_DIRECTORY ${BIL_LIBDIR}
)

target_link_libraries(${BIL_LIB} PRIVATE ${BIL_EXTERNLIBS} ${BIL_EXTRALIBS})

install(TARGETS ${BIL_LIB}
  DESTINATION ${BIL_INSTALL_FULL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  LIBRARY OPTIONAL)



#[[
  Install the headers
#]]

include(AppendBilFiles)
set(BIL_HDRALL ${BIL_PATH}/BilConfig.h;${BIL_PATH}/BilInfo.h;${BIL_PATH}/BilPath.h;${BIL_PATH}/BilVersion.h)
append_bil_files("${BIL_DIRSALL}" "h" BIL_HDRALL)
append_bil_files("${BIL_DIRSALL}" "hpp" BIL_HDRALL)

list(REMOVE_DUPLICATES BIL_HDRALL)
list(SORT BIL_HDRALL COMPARE FILE_BASENAME CASE INSENSITIVE)

install(FILES ${BIL_HDRALL} DESTINATION ${BIL_INSTALL_FULL_INCLUDEDIR})
include(CreateBilHeader)
create_bil_header("${BIL_HDRALL}" "${BIL_INCLUDEDIR}/bil.h")
install(FILES "${BIL_INCLUDEDIR}/bil.h" DESTINATION ${BIL_INSTALL_FULL_INCLUDEDIR})


#[[
  Building the executable target
#]]


add_executable(${BIL_EXE} ${BIL_SRCMAIN})

target_include_directories(${BIL_EXE}
    PRIVATE
    "${BIL_DIRSALL}"
    "${PROJECT_SOURCE_DIR}"
)

target_compile_features(${BIL_EXE}
    PUBLIC c_std_11 cxx_std_17
)

#target_compile_options(${BIL_EXE}
#    PUBLIC
#    -pedantic ${WFLAGS} ${OFLAGS}
#)


set_target_properties(${BIL_EXE}
  PROPERTIES 
  INSTALL_RPATH "${BIL_INSTALL_FULL_LIBDIR};${BILEXTRALIBS_INSTALL_RPATH}"
  RUNTIME_OUTPUT_DIRECTORY ${BIL_BINDIR}
)

#target_link_libraries(${BIL_EXE} ${BIL_LIB} ${BIL_EXTRALIBS})
target_link_libraries(${BIL_EXE} ${BIL_LIB})


configure_file(${BIL_PATH}/BilConfig.h.in 
               ${BIL_PATH}/BilConfig.h)


include(FeatureSummary)
feature_summary(INCLUDE_QUIET_PACKAGES DESCRIPTION "Found packages:" WHAT ALL)



target_link_options(${BIL_EXE}
  PUBLIC "-O3 -g -gdwarf-3"
)

install(TARGETS ${BIL_EXE}
  DESTINATION ${BIL_INSTALL_FULL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  RUNTIME OPTIONAL)




# Enable RPATH for installed binaries
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
# Ensure RPATH is not cleared when installing
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# Force the binary to always look in the relative "lib" directory where it's installed
set(CMAKE_INSTALL_RPATH ${BIL_LIBDIR})
# Automatically add directories outside the build tree to the install RPATH
# (This ensures it finds /usr/local/lib without needing ldconfig)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


install(CODE
  "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${BIL_INSTALL_FULL_BINDIR}/${BIL_EXE} ${BIL_INSTALL_FULL_BINDIR}/bil)"
)
 




