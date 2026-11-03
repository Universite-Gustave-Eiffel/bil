# Julia examples

Three scripts are provided with BilWrap.jl, each reproducing a problem of the Bil reference base. They are run with

```
julia Elast.jl
```

from the folder of the example (the scripts include `../../BilWrap.jl/src/BilWrap.jl`). The model, the physics and the reference results are described in the pages of the models ([Elast](Elast_model.md), [Duracem](Duracem_model.md), [Richards](Richards.md)); this page only describes how the same problem is set up from Julia.

All scripts start in the same way:

```julia
include("../../BilWrap.jl/src/BilWrap.jl")
using .BilWrap

d = DataSet_New("out")
options     = DataSet_GetOptions(d)
units       = DataSet_GetUnits(d)
geom        = DataSet_GetGeometry(d)
mesh        = DataSet_GetMesh(d)
fields      = DataSet_GetFields(d)
functions   = DataSet_GetFunctions(d)
iconds      = DataSet_GetIConds(d)
bconds      = DataSet_GetBConds(d)
loads       = DataSet_GetLoads(d)
dates       = DataSet_GetDates(d)
points      = DataSet_GetPoints(d)
obvals      = DataSet_GetObVals(d)
iterprocess = DataSet_GetIterProcess(d)
timestep    = DataSet_GetTimeStep(d)
materials   = DataSet_GetMaterials(d)
models      = DataSet_GetModels(d)
modul       = DataSet_GetModule(d)

Session_Open()
```

* * *

## Elast: elastic cylinder under pressure

2D axisymmetric problem on the mesh `cylinder.msh`, with three elastic materials and a time-dependent pressure on the boundary regions.

### Geometry, mesh and numbering

```julia
Geometry_Set(geom, 2, "axis")
Mesh_Set(mesh, "cylinder.msh")
Mesh_WriteInversePermutation(mesh, "out", "hsl")   # node renumbering (bil -iperm)
```

### Field and function

One constant field (1e6) and one piecewise affine function of time (values 1, 10 and 0.1 at t = 0, 1 and 2):

```julia
Fields_EmplaceBack(fields, "affine", 1.e6, [0.,0.,0.], [0.,0.,0.])
Functions_EmplaceBack(functions, "piecewiseaffine", 3, [0.,1.,2.], [1.,10.,0.1])
```

### Boundary conditions and loads

The displacement `u_2` is blocked on region 80. A pressure (field 1) is applied to regions 10 to 80. On regions 10, 20 and 30 it is multiplied by function 1; on regions 50 to 80 no function is used (last argument 0).

```julia
BConds_EmplaceBack(bconds, "80", "u_2", 0, 0)

Loads_EmplaceBack(loads, "10", "meca_1", "pressure", 1, 1)
Loads_EmplaceBack(loads, "20", "meca_1", "pressure", 1, 1)
Loads_EmplaceBack(loads, "30", "meca_1", "pressure", 1, 1)
Loads_EmplaceBack(loads, "50", "meca_1", "pressure", 1, 0)
# ... same for regions 60, 70 and 80
```

### Output, objective variations and numerical parameters

```julia
Points_EmplaceBack(points, [0.15,0.15,0], "600")
Dates_Set(dates, [0.,1.,2.])

ObVals_EmplaceBack(obvals, "u_1", 1.e-4)
ObVals_EmplaceBack(obvals, "u_2", 1.e-4)

IterProcess_Set(iterprocess, 5, 1.e-4, 0)
TimeStep_Set(timestep, 1., 100000)
```

### Model and materials

Three materials of the model `Elast` are defined, with the same initial stress and Poisson ratio and with Young moduli of 10 GPa, 1 GPa and 10 GPa:

```julia
mod1 = Models_EmplaceBack(models, "Elast")

mat1 = Materials_EmplaceBack(materials, mod1)
Material_Set(mat1, "gravity", 0.)
Material_Set(mat1, "rho_s", 0.)
Material_Set(mat1, "sig0_11", -1.e6)
Material_Set(mat1, "sig0_22", -1.e6)
Material_Set(mat1, "sig0_33", -1.e6)
Material_Set(mat1, "young", 10.e9)
Material_Set(mat1, "poisson", 0.26)
Material_Finalize(mat1)

mat2 = Materials_EmplaceBack(materials, mod1)
# ... same properties, with "young" = 1.e9
Material_Finalize(mat2)

mat3 = Materials_EmplaceBack(materials, mod1)
# ... same properties, with "young" = 10.e9
Material_Finalize(mat3)
```

### Solving

```julia
DataSet_PrintData(d, "all")
DataSet_Finalize(d)

#Options_Set(options, "-solver petscksp -ksp_type cg -pc_type sor")

Module_ComputeProblem(modul, d)

#pf4gmsh = PosFilesForGMSH_Create(d)
#PosFilesForGMSH_ParsedFileFormat(pf4gmsh)

DataSet_Delete(d)
```

The two commented blocks show how to select a PETSc solver and how to generate the Gmsh files.

* * *

## Duracem: 1D chemistry-transport problem

1D problem with the model `Duracem`, with a change of units and an inline mesh. It illustrates the definition of initial conditions, of curves in the material properties and of several functions and fields.

### Units, geometry and mesh

```julia
Units_EmplaceBack(units, "Length", "decimeter")
Units_EmplaceBack(units, "Mass", "hectogram")

Geometry_Set(geom, 1, "plane")
Mesh_Set(mesh, "3 0. 0. 0.6\n0.0002\n1 100\n1 1")   # inline 1D mesh, no file
```

### Fields and functions

Nine fields are defined (constants through `affine` with zero gradient, plus one field read from the grid file `CN_satb`), and two functions of time:

```julia
Fields_EmplaceBack(fields, "affine", 1., [0.,0.,0.], [0.,0.,0.])
Fields_EmplaceBack(fields, "affine", -0.158, [0.,0.,0.], [0.,0.,0.])
Fields_EmplaceBack(fields, "affine", -70.5e6, [0.,0.,0.], [0.,0.,0.])
# ...
Fields_EmplaceBack(fields, "grid", "CN_satb")
Fields_EmplaceBack(fields, "affine", -1.5, [0.,0.,0.], [0.,0.,0.])

Functions_EmplaceBack(functions, "piecewiseaffine", 2, [0.,3600.], [1.,0.16])
Functions_EmplaceBack(functions, "piecewiseaffine", 8,
    [0.,60.,360.,3600.,36000.,60000.,86400.,172800.],
    [-15,-13.5,-12,-10.5,-6,-4.9,-3,-2.4])
```

### Initial and boundary conditions

Each unknown of the problem is initialized in region 2 with its own field (and function for `logc_co2`); the boundary region 1 carries conditions on `psi`, `p_l` and `logc_co2`:

```julia
IConds_EmplaceBack(iconds, "2", "logc_co2", 1, 2)
IConds_EmplaceBack(iconds, "2", "p_l", 8, 0)
IConds_EmplaceBack(iconds, "2", "psi", 0, 0)
IConds_EmplaceBack(iconds, "2", "z_si", 6, 0)
IConds_EmplaceBack(iconds, "2", "z_ca", 7, 0)
IConds_EmplaceBack(iconds, "2", "logc_oh", 9, 0)

BConds_EmplaceBack(bconds, "1", "psi", 0, 0)
BConds_EmplaceBack(bconds, "1", "p_l", 3, 0)
BConds_EmplaceBack(bconds, "1", "logc_co2", 1, 2)
```

### Dates, objective variations, numerical parameters

```julia
Dates_Set(dates, [0., 86400., 172800.])

ObVals_EmplaceBack(obvals, "logc_co2", 0.1)
ObVals_EmplaceBack(obvals, "p_l", 1.e5)
# ... one objective variation per main unknown (z_ca, psi, logc_na, logc_k, z_si, logc_oh)

IterProcess_Set(iterprocess, 20, 1.e-3, 0)
TimeStep_Set(timestep, 10., 3600)
```

### Model and material

Material properties are given by name. The `Curves` and `Curves_log` keys can be repeated: each call adds a curve, given by the name of a curve file or by its inline definition.

```julia
mod1 = Models_EmplaceBack(models, "Duracem")
mat1 = Materials_EmplaceBack(materials, mod1)

Material_Set(mat1, "InitialPorosity", 0.379)
Material_Set(mat1, "IntrinsicPermeability_liquid", 1.4e-17)
Material_Set(mat1, "InitialContent_portlandite", 3.9)
Material_Set(mat1, "InitialContent_csh", 2.4)
Material_Set(mat1, "InitialConcentration_sodium", 0.019)
Material_Set(mat1, "InitialConcentration_potassium", 0.012)
Material_Set(mat1, "FractionalLengthOfPoreBodies", 0.8)
Material_Set(mat1, "PorosityFractionAtVanishingPermeability", 0.7)

Material_Set(mat1, "Curves", "desorbCN")                       # curve file
Material_Set(mat1, "Curves", "relpermCN  s_l = Range{x1 = 0 , x2 = 1 , n = 101} kl_r = Mualem_liq(1){m = 0.45}")
Material_Set(mat1, "Curves", "V_CSH")
Material_Set(mat1, "Curves_log", "csh4t S_CH = Range{...} X_CSH = CSHss(1){...} S_SH = CSHss(1){...}")
Material_Set(mat1, "UseAutodiff", 0.)
Material_Finalize(mat1)
```

(the full `csh4t` definition is in `Duracem.jl`.)

### Solving

```julia
DataSet_Finalize(d)
DataSet_PrintData(d, "all")
Module_ComputeProblem(modul, d)
```

This example defines no output point, no load and no Gmsh file.

* * *

## Richards-2d: drainage of a composite column

2D plane problem with the model `Richards` on the mesh `columncomposite.msh`: a column made of two materials of different permeabilities, in a gravity field.

### Geometry and mesh

```julia
Geometry_Set(geom, 2, "plane")
Mesh_Set(mesh, "columncomposite.msh")
Mesh_WriteInversePermutation(mesh, "out", "hsl")
```

### Hydrostatic field and function

An affine field with the gradient $\rho g$ along $y$, equal to zero at $y = 0.2$, is used to initialize the liquid pressure, and a function of time decreasing from 1 to 0 over 360 s is used for the boundary condition. Here `Functions_EmplaceBack` is called without the number of points, which is optional:

```julia
g = [0,-9810.,0]
x = [0,0.2,0]
Fields_EmplaceBack(fields, "affine", 0., g, x)

t = [0.,360.]
f = [1.,0.]
Functions_EmplaceBack(functions, "piecewiseaffine", t, f)
```

### Initial and boundary conditions, output points

```julia
IConds_EmplaceBack(iconds, "100", "p_l", 1, 0)
IConds_EmplaceBack(iconds, "101", "p_l", 1, 0)

BConds_EmplaceBack(bconds, "11", "p_l", 1, 1)

Points_EmplaceBack(points, [0.01,0.1,0], "101")
Points_EmplaceBack(points, [0.01,0.175,0], "100")
```

### Dates and numerical parameters

```julia
Dates_Set(dates, [0.,200.,400.,600.,800.,1000.,1200.,1400.,1600.,1800.,2000.,2200.,2400.,2600.,2800.,3000.])

ObVals_EmplaceBack(obvals, "p_l", 1.e3)
IterProcess_Set(iterprocess, 20, 1.e-6, 2)
TimeStep_Set(timestep, 1., 1000.)
```

### Model and materials

The two materials differ only by the intrinsic permeability (8.9e-12 and 8.9e-13 m²) and share the retention and relative permeability curves `billes`:

```julia
mod1 = Models_EmplaceBack(models, "Richards")

mat1 = Materials_EmplaceBack(materials, mod1)
Material_Set(mat1, "Gravity", -9.81)
Material_Set(mat1, "Porosity", 0.38)
Material_Set(mat1, "LiquidMassDensity", 1000.)
Material_Set(mat1, "IntrinsicPermeability", 8.9e-12)
Material_Set(mat1, "ReferenceGasPressure", 0.)
Material_Set(mat1, "LiquidViscosity", 0.001)
Material_Set(mat1, "Curves", "billes")
Material_Finalize(mat1)

mat2 = Materials_EmplaceBack(materials, mod1)
# ... same properties, with "IntrinsicPermeability" = 8.9e-13
Material_Finalize(mat2)
```

### Solving and post-processing

```julia
DataSet_Finalize(d)
DataSet_PrintData(d, "all")

Module_ComputeProblem(modul, d)

pf4gmsh = PosFilesForGMSH_Create(d)
PosFilesForGMSH_ParsedFileFormat(pf4gmsh)

DataSet_Delete(d)
```

The `.pos` files can be opened directly in Gmsh to visualize the pressure fields at the dates defined above.
