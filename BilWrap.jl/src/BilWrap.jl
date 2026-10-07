# src/BilWrap.jl
  
"""
    BilWrap
    
    Julia wrappers for the c++ code Bil.
      - Construct dataset for boundary value problem
      - Solve the problem by FEM or FVM
      - Generate output files for GMSH post-treatment
      
    See examples in dedicated folder
"""

module BilWrap
  using CxxWrap

  # 1. Target the auto-generated variables file
  const deps_config_file = joinpath(@__DIR__, "..", "deps", "deps_vars.jl")

  # 2. Distribution Check: Ensure the user's local build stage occurred
  if !isfile(deps_config_file)
    error("MyPackage dependencies are unbuilt. Please resolve by running: import Pkg; Pkg.build(\"MyPackage\")")
    # exit(0)
  end

  # 3. Splice the generated file into scope (provides 'CPP_SHARED_LIB_PATH')
  include(deps_config_file)

  # 4. Supply the computed dynamic string to CxxWrap's macro
  @wrapmodule(() -> CPP_SHARED_LIB_PATH)

  # 5. Lock down module bindings at runtime initialization
  function __init__()
    @initcxx
  end
  
  export Session_Open
  
  export DataSet_New,
         DataSet_Delete,
         DataSet_PrintData,
         DataSet_GetOptions,
         DataSet_GetUnits,
         DataSet_GetGeometry,
         DataSet_GetMesh,
         DataSet_GetFields,
         DataSet_GetFunctions,
         DataSet_GetIConds,
         DataSet_GetBConds,
         DataSet_GetLoads,
         DataSet_GetDates,
         DataSet_GetPoints,
         DataSet_GetObVals,
         DataSet_GetIterProcess,
         DataSet_GetTimeStep,
         DataSet_GetMaterials,
         DataSet_GetModels,
         DataSet_GetModule,
         DataSet_Finalize,
         DataSet_Print
  
  export Options_Set
  
  export Units_EmplaceBack
  
  export Geometry_Set
  
  export Mesh_Set,
         Mesh_WriteInversePermutation
  
  export Fields_EmplaceBack
  
  export Functions_EmplaceBack
  
  export IConds_EmplaceBack
  
  export BConds_EmplaceBack
  
  export Loads_EmplaceBack
  
  export Dates_Set
  
  export Points_EmplaceBack
  
  export ObVals_EmplaceBack
  
  export PosFilesForGMSH_Create,
         PosFilesForGMSH_Delete,
         PosFilesForGMSH_ParsedFileFormat,
         PosFilesForGMSH_ASCIIFileFormat
  
  export IterProcess_Set
  
  export TimeStep_Set
  
  export Models_EmplaceBack
  
  export Materials_EmplaceBack
  
  export Material_Set,
         Material_Finalize
  
  export Module_Set,
         Module_ComputeProblem
end
