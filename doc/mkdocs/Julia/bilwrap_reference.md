# BilWrap.jl: function reference

All functions below are exported by `BilWrap`. Wrapper functions are named `O_F`, after Bil's own syntax, where `O` is the name of a Bil object (`DataSet`, `Mesh`, `Fields`, `BConds`, ...) and `F` is the name of the function applied to it. Three cases occur:

- **Functions applied to an object**: the object is the first argument, `O_F(o, args...)`. This covers the member functions of the Bil objects (`Fields_EmplaceBack(fields, ...)`) and the functions taking a pointer to the object (`Mesh_Set(mesh, ...)`, `DataSet_Delete(d)`).
- **Static functions**: no object, `O_F(args...)` (`Session_Open()`, `Session_Close()`).
- **Constructors and factories**: `DataSet_New(...)` and `PosFilesForGMSH_Create(d)` return a new object.

- `Xxx_Set(section, ...)` sets the content of a single-entry section.
- `Xxx_EmplaceBack(section, ...)` appends a new entry to a multi-entry section. The entries are numbered from 1 in the order of insertion.
- `DataSet_GetXxx(d)` returns the section `Xxx` of the dataset `d`.

Conventions used in the signatures: `d` is a dataset, `region` is a string (`"10"`), `field` and `function` are integer indices (0 = none), `unknown` is the name of a nodal unknown (`"p_l"`, `"u_2"`, ...).

* * *

## Session and dataset

### `Session_Open()` / `Session_Close()`

Open and close a Bil session. `Session_Open()` is called after the dataset sections have been retrieved and before any section is filled.

### `d = DataSet_New(name::String)` / `DataSet_New(name::String, context)`

Creates an empty dataset. `name` is the name of the dataset, the counterpart of the `my_file` of the command line. A context object can optionally be given as second argument; it is not needed in the examples.

### `DataSet_Delete(d)`

Releases the dataset and the objects it owns.

### `DataSet_Finalize(d)`

Closes the definition of the dataset. It is called once all the sections are filled and before `Module_ComputeProblem`.

### `DataSet_PrintData(d, what::String)`

Prints the data of the dataset. `DataSet_PrintData(d, "all")` is the counterpart of `bil -debug all my_file`.

### Section getters

Each getter returns the section to be filled:

| Getter                        | Section                         |
|-------------------------------|---------------------------------|
| `DataSet_GetOptions(d)`       | solver and run options          |
| `DataSet_GetUnits(d)`         | units                           |
| `DataSet_GetGeometry(d)`      | geometry                        |
| `DataSet_GetMesh(d)`          | mesh                            |
| `DataSet_GetFields(d)`        | fields                          |
| `DataSet_GetFunctions(d)`     | time functions                  |
| `DataSet_GetIConds(d)`        | initial conditions              |
| `DataSet_GetBConds(d)`        | boundary conditions             |
| `DataSet_GetLoads(d)`         | loads                           |
| `DataSet_GetDates(d)`         | output dates                    |
| `DataSet_GetPoints(d)`        | output points                   |
| `DataSet_GetObVals(d)`        | objective variations            |
| `DataSet_GetIterProcess(d)`   | iterative process               |
| `DataSet_GetTimeStep(d)`      | time steps                      |
| `DataSet_GetMaterials(d)`     | materials                       |
| `DataSet_GetModels(d)`        | models                          |
| `DataSet_GetModule(d)`        | resolution module               |

* * *

## Units, geometry and mesh

### `Units_EmplaceBack(units, quantity::String, unit::String)`

Changes the unit of a physical quantity (keyword `Units`).

```julia
Units_EmplaceBack(units, "Length", "decimeter")
Units_EmplaceBack(units, "Mass", "hectogram")
```

### `Geometry_Set(geom, dim::Int, symmetry::String)`

Dimension and symmetry of the problem (keyword `Geometry`).

```julia
Geometry_Set(geom, 2, "axis")    # 2D axisymmetric
Geometry_Set(geom, 2, "plane")   # 2D plane
Geometry_Set(geom, 1, "plane")   # 1D
```

### `Geometry_GetPeriodicities(geom)` and `Periodicities_EmplaceBack(periodicities, region1::String, region2::String, translation::Vector)`

Define the periodicities of the mesh (optional keyword `Periodicities`): `Geometry_GetPeriodicities` returns the section, and each call to `Periodicities_EmplaceBack` declares two regions that are periodic, with a vector of 3 components. These functions are not used in the provided examples.

### `Mesh_Set(mesh, definition::String)`

Defines the mesh. The string is either the name of a Gmsh mesh file or, for 1D problems, the inline mesh definition:

```julia
Mesh_Set(mesh, "cylinder.msh")                    # Gmsh file
Mesh_Set(mesh, "3 0. 0. 0.6\n0.0002\n1 100\n1 1")   # inline 1D mesh (Duracem example)
```

### `Mesh_WriteInversePermutation(mesh, name::String, method::String)`

Writes the file of the inverse node permutation, which is the counterpart of `bil -iperm my_file`. As for the command line, it is recommended for 2D and 3D problems solved with the default `crout` solver.

```julia
Mesh_WriteInversePermutation(mesh, "out", "hsl")
```

* * *

## Fields and functions

### `Fields_EmplaceBack(fields, "affine", value, gradient::Vector, point::Vector)`

Appends an affine field, i.e. a spatial function equal to `value` at `point` and varying with the gradient `gradient`:

$$
f(\mathbf{x}) = \text{value} + \text{gradient}\cdot(\mathbf{x}-\text{point})
$$

```julia
Fields_EmplaceBack(fields, "affine", 1.e6, [0.,0.,0.], [0.,0.,0.])           # constant 1e6
Fields_EmplaceBack(fields, "affine", 0., [0,-9810.,0], [0,0.2,0])            # hydrostatic
```

### `Fields_EmplaceBack(fields, "grid", file::String)`

Appends a field defined on a grid read from a file (example: `"CN_satb"` in Duracem).

### `Fields_EmplaceBack(fields, type::String, value)` and `Fields_EmplaceBack(fields, type::String, value1, value2)`

Shorter forms, taking only one or two scalar values after the type. They are not used in the examples.

### `Functions_EmplaceBack(functions, "piecewiseaffine", [n,] t::Vector, f::Vector)`

Appends a piecewise affine function of time through the points `(t[i], f[i])`. The number of points `n` is optional: when it is omitted, it is deduced from the length of the vectors. Both forms are found in the examples: with `n` (Elast, Duracem) and without it (Richards-2d).

A third form, `Functions_EmplaceBack(functions, type::String, s::String)`, takes a string in place of the vectors. It is not used in the examples.

```julia
Functions_EmplaceBack(functions, "piecewiseaffine", 3, [0.,1.,2.], [1.,10.,0.1])
Functions_EmplaceBack(functions, "piecewiseaffine", [0.,360.], [1.,0.])
```

* * *

## Initial conditions, boundary conditions and loads

### `IConds_EmplaceBack(iconds, region, unknown, field, function)`

Initial value of `unknown` in `region`, given by `field` (and modulated by `function`).

```julia
IConds_EmplaceBack(iconds, "2", "logc_co2", 1, 2)
IConds_EmplaceBack(iconds, "100", "p_l", 1, 0)
```

A variant, `IConds_EmplaceBack(iconds, region, unknown, s::String, function)`, accepts a string in place of the field index. It is not used in the examples.

### `BConds_EmplaceBack(bconds, region, unknown, field, function)`

Imposes `unknown` on `region`, with `field` and `function`. With `field = 0` and `function = 0`, the value is zero.

```julia
BConds_EmplaceBack(bconds, "80", "u_2", 0, 0)     # u_2 = 0 on region 80
BConds_EmplaceBack(bconds, "11", "p_l", 1, 1)
```

A variant of `BConds_EmplaceBack` with a third string argument placed before `field` and `function` also exists. It is not used in the examples.

### `Loads_EmplaceBack(loads, region, equation::String, type::String, field, function)`

Applies a load on `region`, to the equation `equation` (e.g. `"meca_1"`), of the given type (e.g. `"pressure"`), with `field` and `function`.

```julia
Loads_EmplaceBack(loads, "10", "meca_1", "pressure", 1, 1)
```

* * *

## Output points, dates and numerical parameters

### `Points_EmplaceBack(points, coordinates::Vector [, region::String])`

Appends an output point at `coordinates`. The region containing the point can optionally be given. The results are written in the corresponding `.pi` file.

### `Dates_Set(dates, t::Vector)`

Output dates (the first one is the initial date).

### `ObVals_EmplaceBack(obvals, unknown::String, value)`

Objective variation of an unknown, used by Bil to adapt the time step.

A longer form, `ObVals_EmplaceBack(obvals, unknown::String, value, s::String, value2)`, takes an additional string and an additional value. It is not used in the examples.

### `IterProcess_Set(iterprocess, iterations::Int, tolerance, repetitions::Int)`

Parameters of the iterative process: maximum number of iterations, convergence tolerance, number of repetitions.

### `TimeStep_Set(timestep, dtini, dtmax)`

Initial and maximum time steps.

* * *

## Models and materials

### `model = Models_EmplaceBack(models, name::String)`

Declares the model (e.g. `"Elast"`, `"Richards"`, `"Duracem"`) and returns it. The available models are listed with `bil -model`.

Two other forms exist and are not used in the examples: one takes three strings (the model name followed by two more strings), the other takes an existing model object.

### `mat = Materials_EmplaceBack(materials, model)`

Creates a material of the given model, which is either the object returned by `Models_EmplaceBack` (as in the examples) or the name of the model as a string. Materials are created in the same order as in the input file.

```julia
mat1 = Materials_EmplaceBack(materials, mod1)
mat1 = Materials_EmplaceBack(materials, "Elast")   # equivalent, by model name
```

### `Material_Set(mat, key::String, value)`

Sets a property of the material. `value` is a number or a string. The keys are the property names of the model, as documented in the page of the model (or displayed by `bil -model mymodel`).

```julia
Material_Set(mat1, "young", 10.e9)
Material_Set(mat1, "Curves", "billes")
```

`Material_Set` can be called several times with the same key `"Curves"` (or `"Curves_log"`) to define several curves; the string is then the definition of the curve file or of the curve itself, as in the input file:

```julia
Material_Set(mat1, "Curves", "relpermCN  s_l = Range{x1 = 0 , x2 = 1 , n = 101} kl_r = Mualem_liq(1){m = 0.45}")
```

### `Material_Finalize(mat)`

Closes the definition of the material. It must be called after the last `Material_Set` of each material.

* * *

## Options and resolution

### `Options_Set(options, line::String)`

Sets the run options with the same syntax as the command line.

```julia
Options_Set(opt, "-solver petscksp -ksp_type cg -pc_type sor")
```

### `Module_Set(modul, name::String)`

Selects the resolution module by its name, as the option `-with` does on the command line. It is not used in the provided examples, which rely on the default module.

### `Module_ComputeProblem(modul, d)`

Solves the problem defined by the dataset `d`.

* * *

## Post-processing files

### `pf = PosFilesForGMSH_Create(d)`

Creates the object that generates the `.pos` files for Gmsh.

### `PosFilesForGMSH_ParsedFileFormat(pf)` / `PosFilesForGMSH_ASCIIFileFormat(pf)`

Write the files in the Gmsh *parsed* or *ASCII* format (counterparts of `bil -post GmshParsed` and `bil -post GmshASCII`).

### `PosFilesForGMSH_Delete(pf)`

Releases the object.

* * *

## Argument types

The arguments are converted by CxxWrap as follows.

| C++ argument           | Julia value                                              |
|------------------------|----------------------------------------------------------|
| `char const*`          | `String`                                                 |
| `size_t const&`, `int const&`, `unsigned short` | `Int`                           |
| `double const&`        | `Float64`                                                |
| `double const*`        | vector of `Float64`, e.g. `[0.,0.,0.]`                   |
| pointer to a Bil object (`Mesh_t*`, `DataSet_t*`, ...) | the object returned by the corresponding getter or constructor |

Functions are overloaded on the number and the types of their arguments, as in C++. Julia dispatches on them in the usual way, which is why several forms of `Fields_EmplaceBack`, `Functions_EmplaceBack`, ... coexist.

* * *

## Adding a function to the wrapper

The wrapper is defined in `jlWrap.cpp`, with the macros of `jlWrap.h`. To expose a function of a Bil object `O` under the name `O_F` (so far, up to 9 arguments are supported):

```cpp
MEM_METHOD(O, F, argtype1, argtype2);          // member function: O_F(o, a1, a2)
FREE_METHOD(O, F, O_t*, argtype1);             // non-member function: O_F(o, a1)
MEM_STATICMETHOD(O, F, argtype1);              // static member function: O_F(a1)
```

Each Bil object type must also be declared once with `MIRROR(O_t)` (before the module definition) and `ADD_TYPE(O_t)` (inside `define_julia_module`). A function taking an array in C++ as a pointer plus a size can be given a more convenient Julia form with a lambda taking a `jlcxx::ArrayRef<double>` (see `Dates_Set` and `Functions_EmplaceBack`). Finally, add the new name to the `export` list of `BilWrap.jl`.
