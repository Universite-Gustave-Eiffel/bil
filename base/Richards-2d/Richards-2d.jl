include("../../BilWrap.jl/src/BilWrap.jl")
using .BilWrap

  d =  DataSet_New("out") 
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
  opt = DataSet_GetOptions(d)
  
  Session_Open()

  Geometry_Set(geom,2,"plane") 
  Mesh_Set(mesh,"columncomposite.msh")
  
  Mesh_WriteInversePermutation(mesh,"out","hsl")
 
  
    g = [0,-9810.,0]
    x = [0,0.2,0]
    
    Fields_EmplaceBack(fields,"affine",0.,g,x)
  

  
    t = [0.,360.]
    f = [1.,0.]
    
    Functions_EmplaceBack(functions,"piecewiseaffine",t,f)
  

  IConds_EmplaceBack(iconds,"100","p_l",1,0)
  IConds_EmplaceBack(iconds,"101","p_l",1,0)

  BConds_EmplaceBack(bconds,"11","p_l",1,1)

  Points_EmplaceBack(points,[0.01,0.1,0],"101")
  Points_EmplaceBack(points,[0.01,0.175,0],"100")

  Dates_Set(dates,[0.,200.,400.,600.,800.,1000.,1200.,1400.,1600.,1800.,2000.,2200.,2400.,2600.,2800.,3000.])
  
  ObVals_EmplaceBack(obvals,"p_l",1.e3)
  
  IterProcess_Set(iterprocess,20,1.e-6,2)
  
  TimeStep_Set(timestep,1.,1000.)
  
  mod1 = Models_EmplaceBack(models,"Richards")
  

  
    mat1 = Materials_EmplaceBack(materials,mod1)
    
    Material_Set(mat1,"Gravity",-9.81)
    Material_Set(mat1,"Porosity",0.38)
    Material_Set(mat1,"LiquidMassDensity",1000.)
    Material_Set(mat1,"IntrinsicPermeability",8.9e-12)
    Material_Set(mat1,"ReferenceGasPressure",0.)
    Material_Set(mat1,"LiquidViscosity",0.001)
    Material_Set(mat1,"Curves","billes")
    Material_Finalize(mat1)
  
  
  
    mat2 = Materials_EmplaceBack(materials,mod1)
    
    Material_Set(mat2,"Gravity",-9.81)
    Material_Set(mat2,"Porosity",0.38)
    Material_Set(mat2,"LiquidMassDensity",1000.)
    Material_Set(mat2,"IntrinsicPermeability",8.9e-13)
    Material_Set(mat2,"ReferenceGasPressure",0.)
    Material_Set(mat2,"LiquidViscosity",0.001)
    Material_Set(mat2,"Curves","billes")
    Material_Finalize(mat2)
  
  
  
  DataSet_Finalize(d)

  DataSet_PrintData(d,"all") 
  
  #Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor")
  
  Module_ComputeProblem(modul,d)
  
  
    pf4gmsh = PosFilesForGMSH_Create(d)
    
    PosFilesForGMSH_ParsedFileFormat(pf4gmsh)
  
  
  DataSet_Delete(d)
