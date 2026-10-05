include("../../BilWrap.jl/src/BilWrap.jl")
using .BilWrap

d = DataSet_New("out")
opt = DataSet_GetOptions(d)
units = DataSet_GetUnits(d)
geom = DataSet_GetGeometry(d)
mesh = DataSet_GetMesh(d)
fields = DataSet_GetFields(d)
functions = DataSet_GetFunctions(d)
iconds = DataSet_GetIConds(d)
bconds = DataSet_GetBConds(d)
loads = DataSet_GetLoads(d)
dates = DataSet_GetDates(d)
points = DataSet_GetPoints(d)
obvals = DataSet_GetObVals(d)
iterprocess = DataSet_GetIterProcess(d)
timestep = DataSet_GetTimeStep(d)
materials = DataSet_GetMaterials(d)
models = DataSet_GetModels(d)
modul = DataSet_GetModule(d)
  
  Session_Open()

  Geometry_Set(geom,2,"axis")
  Mesh_Set(mesh,"cylinder.msh")
  
  Mesh_WriteInversePermutation(mesh,"out","hsl")
 
  Fields_EmplaceBack(fields,"affine",1.e6,[0.,0.,0.],[0.,0.,0.])

  Functions_EmplaceBack(functions,"piecewiseaffine",3,[0.,1.,2.],[1.,10.,0.1])


  BConds_EmplaceBack(bconds,"80","u_2",0,0)
  
  Loads_EmplaceBack(loads,"10","meca_1","pressure",1,1)
  Loads_EmplaceBack(loads,"20","meca_1","pressure",1,1)
  Loads_EmplaceBack(loads,"30","meca_1","pressure",1,1)
  Loads_EmplaceBack(loads,"50","meca_1","pressure",1,0)
  Loads_EmplaceBack(loads,"60","meca_1","pressure",1,0)
  Loads_EmplaceBack(loads,"70","meca_1","pressure",1,0)
  Loads_EmplaceBack(loads,"80","meca_1","pressure",1,0)

  Points_EmplaceBack(points,[0.15,0.15,0],"600")

  Dates_Set(dates,[0.,1.,2.])
  
  ObVals_EmplaceBack(obvals,"u_1",1.e-4)
  ObVals_EmplaceBack(obvals,"u_2",1.e-4)
  
  IterProcess_Set(iterprocess,5,1.e-4,0)
  
  TimeStep_Set(timestep,1.,100000)
  
  Models_EmplaceBack(models,"Elast")
  
    mat1 = Materials_EmplaceBack(materials,"Elast")
    Material_Set(mat1,"gravity",0.)
    Material_Set(mat1,"rho_s",0.)
    Material_Set(mat1,"sig0_11",-1.e6)
    Material_Set(mat1,"sig0_22",-1.e6)
    Material_Set(mat1,"sig0_33",-1.e6)
    Material_Set(mat1,"young",10.e9)
    Material_Set(mat1,"poisson",0.26)
    Material_Finalize(mat1)
  
    mat2 = Materials_EmplaceBack(materials,"Elast")
    Material_Set(mat2,"gravity",0.)
    Material_Set(mat2,"rho_s",0.)
    Material_Set(mat2,"sig0_11",-1.e6)
    Material_Set(mat2,"sig0_22",-1.e6)
    Material_Set(mat2,"sig0_33",-1.e6)
    Material_Set(mat2,"young",1.e9)
    Material_Set(mat2,"poisson",0.26)
    Material_Finalize(mat2)
  
    mat3 = Materials_EmplaceBack(materials,"Elast")
    Material_Set(mat3,"gravity",0.)
    Material_Set(mat3,"rho_s",0.)
    Material_Set(mat3,"sig0_11",-1.e6)
    Material_Set(mat3,"sig0_22",-1.e6)
    Material_Set(mat3,"sig0_33",-1.e6)
    Material_Set(mat3,"young",10.e9)
    Material_Set(mat3,"poisson",0.26)
    Material_Finalize(mat3);

  DataSet_PrintData(d,"all")
  
  DataSet_Finalize(d)
  
  Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor")
  
  Module_ComputeProblem(modul,d)
  
  #pf4gmsh = PosFilesForGMSH_Create(d)
  #PosFilesForGMSH_ParsedFileFormat(pf4gmsh)
  
  DataSet_Delete(d)
