include("/home/dangla/Documents/Softwares/bil/BilWrap/src/BilWrap.jl")
using .BilWrap

d = DataSet_New("out")
options = DataSet_GetOptions(d)
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
  
Units_EmplaceBack(units,"Length","decimeter")
Units_EmplaceBack(units,"Mass","hectogram")

Geometry_Set(geom,1,"plane")
Mesh_Set(mesh,"3 0. 0. 0.6\n0.0002\n1 100\n1 1")
 
Fields_EmplaceBack(fields,"affine",1.,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",-0.158,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",-70.5e6,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",0.,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",0.03,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",1.,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"affine",1.,[0.,0.,0.],[0.,0.,0.])
Fields_EmplaceBack(fields,"grid","CN_satb")
Fields_EmplaceBack(fields,"affine",-1.5,[0.,0.,0.],[0.,0.,0.])

Functions_EmplaceBack(functions,"piecewiseaffine",2,[0.,3600.],[1.,0.16])
Functions_EmplaceBack(functions,"piecewiseaffine",8,[0.,60.,360.,3600.,36000.,60000.,86400.,172800.],[-15,-13.5,-12,-10.5,-6,-4.9,-3,-2.4])

IConds_EmplaceBack(iconds,"2","logc_co2",1,2)
IConds_EmplaceBack(iconds,"2","p_l",8,0)
IConds_EmplaceBack(iconds,"2","psi",0,0)
IConds_EmplaceBack(iconds,"2","z_si",6,0)
IConds_EmplaceBack(iconds,"2","z_ca",7,0)
IConds_EmplaceBack(iconds,"2","logc_oh",9,0)

BConds_EmplaceBack(bconds,"1","psi",0,0)
BConds_EmplaceBack(bconds,"1","p_l",3,0)
BConds_EmplaceBack(bconds,"1","logc_co2",1,2)

Dates_Set(dates,[0.,86400.,172800.])
  
ObVals_EmplaceBack(obvals,"logc_co2",0.1)
ObVals_EmplaceBack(obvals,"p_l",1.e5)
ObVals_EmplaceBack(obvals,"z_ca",0.1)
ObVals_EmplaceBack(obvals,"psi",1.)
ObVals_EmplaceBack(obvals,"logc_na",1.e-1)
ObVals_EmplaceBack(obvals,"logc_k",1.e-1)
ObVals_EmplaceBack(obvals,"z_si",1.e-1)
ObVals_EmplaceBack(obvals,"logc_oh",1.)
  
IterProcess_Set(iterprocess,20,1.e-3,0)
  
TimeStep_Set(timestep,10.,3600)
  
Models_EmplaceBack(models,"Duracem","carbon,silicon","logc_co2,z_si")
  

  mat1 = Materials_EmplaceBack(materials,"Duracem")
    
  Material_Set(mat1,"InitialPorosity",0.379)
  Material_Set(mat1,"IntrinsicPermeability_liquid",1.4e-17)
  Material_Set(mat1,"InitialContent_portlandite",3.9)
  Material_Set(mat1,"InitialContent_csh",2.4)
  Material_Set(mat1,"InitialConcentration_sodium",0.019)
  Material_Set(mat1,"InitialConcentration_potassium",0.012)
  Material_Set(mat1,"FractionalLengthOfPoreBodies",0.8)
  Material_Set(mat1,"PorosityFractionAtVanishingPermeability",0.7)
  Material_Set(mat1,"Curves","desorbCN")
  Material_Set(mat1,"Curves","relpermCN  s_l = Range{x1 = 0 , x2 = 1 , n = 101} kl_r = Mualem_liq(1){m = 0.45}")
  Material_Set(mat1,"Curves","V_CSH")
  Material_Set(mat1,"Curves_log","csh4t S_CH = Range{x0 = 1.e-20 , x1 = 1 , n = 201} X_CSH = CSHss(1){n = 4 , x1 = 1, y1 = 1.5 , logk1 = 12.5 , x2 = 1.25 , y2 = 1.25 , logk2 = 18.1 , x3 = 1.5 , y3 = 1 , logk3 = 25.3 , x4 = 0 , y4 = 1 , logk4 = -2.7 , logkch = 22.8 , logksh = -2.7} S_SH = CSHss(1){n = 4 , x1 = 1, y1 = 1.5 , logk1 = 12.5 , x2 = 1.25 , y2 = 1.25 , logk2 = 18.1 , x3 = 1.5 , y3 = 1 , logk3 = 25.3 , x4 = 0 , y4 = 1 , logk4 = -2.7 , logkch = 22.8 , logksh = -2.7}")
  Material_Set(mat1,"UseAutodiff",0.)
  Material_Finalize(mat1)

  
DataSet_Finalize(d)


DataSet_PrintData(d,"all")

Module_ComputeProblem(modul,d)

