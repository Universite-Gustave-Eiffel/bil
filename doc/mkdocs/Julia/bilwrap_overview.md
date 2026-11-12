# Julia interface: BilWrap.jl

`BilWrap.jl` is a Julia module that wraps the C++ code of Bil (through [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl)). It lets you

- build the **dataset** of a boundary value problem from a Julia script, instead of writing an input file,
- solve the problem by the finite element or finite volume method, exactly as `bil my_file` does,
- generate output files for post-processing with Gmsh.

The problem is described with the same concepts as in [Running Bil](running_bil.md) (geometry, mesh, materials, fields, functions, initial and boundary conditions, loads, points, dates, objective variations, iterative process, time steps), but each keyword of the input file becomes a **section** of a `DataSet` object that is filled by function calls.

* * *

## Installation

The module relies on two shared libraries:

- `libbil-<version>-<buildtype>.so`, the library of Bil, which contains all the code of Bil,
- `libbil-<version>-<buildtype>_julia.so`, the **Julia library**, built from the C++ wrapper code (`jlWrap.cpp`) with CxxWrap. It contains only the wrappers and depends on the first library.

Both must be built and installed beforehand, with the option `ENABLE_JLCXX` of the build, see [The Julia library](../library/bil_library.md#the-julia-library). The build step of the Julia package then generates the file `deps/deps_vars.jl` that gives the location of the wrapper library (`CPP_SHARED_LIB_PATH`). If this file is missing, loading the module stops with an error asking you to build the package first:

```julia
import Pkg
Pkg.add("CxxWrap")
Pkg.build("https://github.com/Universite-Gustave-Eiffel/bil:BilWrap.jl")
```

Then load the module. In the examples, it is included directly from its source file:

```julia
include("path/to/bil-master/BilWrap.jl/src/BilWrap.jl")
using .BilWrap
```

All the functions described in the [function reference](bilwrap_reference.md) are exported by `BilWrap`. They are the Julia counterparts of the C/C++ functions of the Bil library, which can also be used directly in a C++ program, see [Using the Bil library](../library/bil_library.md).

* * *

## From the input file to Julia

Each keyword of a Bil input file corresponds to a section of the dataset. A section is obtained with a getter `DataSet_GetXxx(d)` and is filled with `Xxx_Set` (single-entry sections) or `Xxx_EmplaceBack` (sections with several entries, which are appended one after the other).

| Input file keyword      | Getter                    | Filling function(s)                              |
|-------------------------|---------------------------|--------------------------------------------------|
| `Units`                 | `DataSet_GetUnits`        | `Units_EmplaceBack`                              |
| `Geometry`              | `DataSet_GetGeometry`     | `Geometry_Set`                                   |
| `Periodicities`         | `Geometry_GetPeriodicities` | `Periodicities_EmplaceBack`                    |
| `Mesh`                  | `DataSet_GetMesh`         | `Mesh_Set`                                       |
| `Model`, `Material`     | `DataSet_GetModels`, `DataSet_GetMaterials` | `Models_EmplaceBack`, `Materials_EmplaceBack`, `Material_Set`, `Material_Finalize` |
| `Fields`                | `DataSet_GetFields`       | `Fields_EmplaceBack`                             |
| `Initialization`        | `DataSet_GetIConds`       | `IConds_EmplaceBack`                             |
| `Functions`             | `DataSet_GetFunctions`    | `Functions_EmplaceBack`                          |
| `Boundary Conditions`   | `DataSet_GetBConds`       | `BConds_EmplaceBack`                             |
| `Loads`                 | `DataSet_GetLoads`        | `Loads_EmplaceBack`                              |
| `Points`                | `DataSet_GetPoints`       | `Points_EmplaceBack`                             |
| `Dates`                 | `DataSet_GetDates`        | `Dates_Set`                                      |
| `Objective Variations`  | `DataSet_GetObVals`       | `ObVals_EmplaceBack`                             |
| `Iterative Process`     | `DataSet_GetIterProcess`  | `IterProcess_Set`                                |
| `Time Steps`            | `DataSet_GetTimeStep`     | `TimeStep_Set`                                   |

Command line options map onto functions as follows.

| Command line                     | Julia                                                        |
|----------------------------------|--------------------------------------------------------------|
| `bil my_file`                    | fill the dataset, `DataSet_Finalize`, then `Module_ComputeProblem` |
| `bil -iperm my_file`             | `Mesh_WriteInversePermutation(mesh, "out", "hsl")`           |
| `bil -solver ... -nthreads ...`  | `Options_Set(opt, "-solver ... -nthreads ...")`              |
| `bil -post GmshParsed my_file`   | `PosFilesForGMSH_Create` + `PosFilesForGMSH_ParsedFileFormat` |
| `bil -post GmshASCII my_file`    | `PosFilesForGMSH_Create` + `PosFilesForGMSH_ASCIIFileFormat` |
| `bil -debug all my_file`         | `DataSet_PrintData(d, "all")`                                |
| `bil -with "Monolithic N"`       | `Module_Set(modul, "Monolithic")` |

* * *

## Workflow of a simulation

A script always follows the same sequence.

1. **Create the dataset**, then get all its sections.

    ```julia
    d = DataSet_New("out")
    geom = DataSet_GetGeometry(d)
    mesh = DataSet_GetMesh(d)
    # ... one getter per section (see the table above)
    modul = DataSet_GetModule(d)
    ```

2. **Open a session**: `Session_Open()`.
3. **Fill the sections**: geometry, mesh, fields, functions, initial and boundary conditions, loads, points, dates, objective variations, iterative process, time steps (and units if needed).
4. **Define the models and materials**: `Models_EmplaceBack` returns a model; `Materials_EmplaceBack` creates a material of this model; the properties are set one by one with `Material_Set`, and `Material_Finalize` closes the material.
5. **Finalize the dataset**: `DataSet_Finalize(d)`. `DataSet_PrintData(d, "all")` prints everything that has been parsed, which is useful to check the data.
6. *(optional)* **Set the options** (solver, ...) with `Options_Set`.
7. **Solve**: `Module_ComputeProblem(modul, d)`.
8. *(optional)* **Create the Gmsh post-processing files**, and **delete the dataset** with `DataSet_Delete(d)`.

!!! note "Indices"
    Fields, functions and regions are referred to by the same numbering as in the input file: fields and functions are numbered in the order in which they are appended (starting from 1), and the value `0` means *no field* or *no function*. Regions are given as strings (e.g. `"10"`).

* * *

## Minimal example

The shortest complete script is the elastic problem `Elast` (see [Julia examples](bilwrap_examples.md) for the full version with comments). Its structure is:

```julia
using BilWrap

d = DataSet_New("out")
geom, mesh, fields = DataSet_GetGeometry(d), DataSet_GetMesh(d), DataSet_GetFields(d)
# ... other getters ...
modul = DataSet_GetModule(d)

Session_Open()

Geometry_Set(geom, 2, "axis")
Mesh_Set(mesh, "cylinder.msh")
Fields_EmplaceBack(fields, "affine", 1.e6, [0.,0.,0.], [0.,0.,0.])
# ... functions, boundary conditions, loads, points, dates, ...

mod1 = Models_EmplaceBack(models, "Elast")
mat1 = Materials_EmplaceBack(materials, mod1)
Material_Set(mat1, "young", 10.e9)
Material_Set(mat1, "poisson", 0.26)
Material_Finalize(mat1)

DataSet_Finalize(d)
Module_ComputeProblem(modul, d)
DataSet_Delete(d)
```

* * *

## Output files

The output files are the same as those produced by `bil` (see [Running Bil](running_bil.md#output-files)): `.pi` files for the results at points and `.ti` files for the results at dates, whose first lines starting with `#` describe the columns. They can be read directly in Julia:

```julia
using DelimitedFiles

# results at date index 2 (columns 1-3: node coordinates)
data = readdlm("out.t2"; comments=true, comment_char='#')
x = data[:, 1]
```

For post-processing with Gmsh, create the `.pos` files after the computation:

```julia
pf4gmsh = PosFilesForGMSH_Create(d)
PosFilesForGMSH_ParsedFileFormat(pf4gmsh)   # or PosFilesForGMSH_ASCIIFileFormat(pf4gmsh)
```

!!! warning "Lifetime of objects"
    The section objects (`geom`, `mesh`, `materials`, ...) belong to the dataset `d`. Do not use them after `DataSet_Delete(d)`.
