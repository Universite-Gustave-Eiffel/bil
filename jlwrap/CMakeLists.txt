# CMakeLists.txt for the wrapper only
# usage the directory build: 
# cmake -DCMAKE_PREFIX_PATH=$(julia -e 'using CxxWrap; print(CxxWrap.prefix_path())') ..

execute_process(
  COMMAND julia -e "using CxxWrap; print(CxxWrap.prefix_path())"
  OUTPUT_VARIABLE CXXWRAP_PREFIX_PATH
)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CXXWRAP_PREFIX_PATH})


# Library of Bil_Julia
# --------------------
set(BIL_LIB_JULIA ${BIL_LIB}_julia)


add_custom_target(jlwrap DEPENDS ${BIL_LIB_JULIA})



# Compiler flags
# --------------
include(SetCompilerFlags)



# Options (enabled by default)
# -----------------------------
option(ENABLE_JLCXX   "Enable JlCxx"   OFF)
include(EnableBilOptions)


unset(JULIA_LIB)
if(ENABLE_SUPERLU)
    Message(FATAL_ERROR "There are conflicting types in SuperLU and JlCxx, please disable SUPERLU.")
endif()

if(ENABLE_JLCXX)
  find_package(JlCxx REQUIRED)
  if(JlCxx_FOUND)
    Message(STATUS "Compiler supports JlCxx.")
    # set(HAVE_JLCXX TRUE)

    #target_link_libraries(${BIL_LIB_JULIA} JlCxx::cxxwrap_julia)
    list(APPEND JULIA_LIB JlCxx::cxxwrap_julia)
  else()
    Message(STATUS "Compiler doesn't support JlCxx.")
  endif()
endif()

Message("julia library (jlwrap) = ${JULIA_LIB}")


# source files for the julia wrappers
# -----------------------------------
set(BIL_SRCALL_JULIA jlWrap.cpp)

# Compile definition: BASENAME
foreach(i IN ITEMS ${BIL_SRCALL_JULIA})
  get_filename_component(b ${i} NAME_WE)
  set_source_files_properties(${i} PROPERTIES COMPILE_DEFINITIONS "BASENAME=${b}")
endforeach()


# Force the use of c++
foreach(i IN ITEMS ${BIL_SRCALL_JULIA})
  set_source_files_properties(${i} PROPERTIES LANGUAGE CXX)
endforeach()


    
add_custom_command(OUTPUT ${BIL_LIB_JULIA}
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  
#[[
  Building the shared library target for julia
#]]
    
add_library(${BIL_LIB_JULIA} SHARED EXCLUDE_FROM_ALL ${BIL_SRCALL_JULIA})
    
# autodiff requires a c++17 enabled compiler
target_compile_features(${BIL_LIB_JULIA} PUBLIC cxx_std_17)

set_target_properties(${BIL_LIB_JULIA}
  PROPERTIES 
  INSTALL_RPATH "${BIL_INSTALL_FULL_LIBDIR};${BILEXTRALIBS_INSTALL_RPATH}"
  LIBRARY_OUTPUT_DIRECTORY ${BIL_LIBDIR}
)

#target_link_libraries(${BIL_LIB_JULIA} ${BIL_LIB} ${EXTERNAl_LIBS} ${BIL_EXTRALIBS})
#target_link_libraries(${BIL_LIB_JULIA} ${BIL_LIB})
target_link_libraries(${BIL_LIB_JULIA} ${BIL_LIB} ${JULIA_LIB})

      
add_custom_target(install_jlwrap
  COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}
  --component jlwrap_component --prefix ${CMAKE_INSTALL_PREFIX}
  COMMENT "Installing ${BIL_LIB_JULIA}"
  VERBATIM)
    
install(TARGETS ${BIL_LIB_JULIA}
  DESTINATION ${BIL_INSTALL_FULL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  EXCLUDE_FROM_ALL
  COMPONENT jlwrap_component
  LIBRARY OPTIONAL)
  

