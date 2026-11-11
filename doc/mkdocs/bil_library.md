# Using the Bil library in a C/C++ program

The command line `bil my_file` described in [Running Bil](../running_bil.md) reads the problem from an input file. The same computation can be done from a program of your own, in which the dataset is defined by function calls and the problem is solved without reading any input file. This is made possible by the shared library built together with the executable.

* * *

## What is built

The build produces two targets from the same sources:

| Target       | File                                   | Role                                                   |
|--------------|----------------------------------------|--------------------------------------------------------|
| library      | `libbil-<version>-<buildtype>.so`      | all the code of Bil (models, modules, solvers, ...)     |
| executable   | `bil-<version>-<buildtype>.exe`        | the command line program, linked to the library        |

`<version>` is the version of Bil and `<buildtype>` is the CMake build type (`Release` by default, or `Debug`, ...). At installation, a symbolic link `bil` to the executable is created in the `bin` directory, which is the name used in the rest of this documentation.

The installation also copies all the header files of Bil into the directory `<prefix>/include/bil`, together with a header `bil.h` which includes all of them. A program using the library only needs

```cpp
#include <bil/bil.h>
```

### Options of the build

The external libraries needed by the library are chosen at configuration time. They are linked into `libbil` itself, which records them as its own dependencies: a program using the library does not have to list them on its link line (see [Compiling and linking](#compiling-and-linking)).

| Option             | Effect                                                      | Requirements                                   |
|--------------------|-------------------------------------------------------------|------------------------------------------------|
| `ENABLE_MPI`       | MPI parallelization                                         | an MPI installation                            |
| `ENABLE_PETSC`     | PETSc linear solvers                                        | PETSc library and `petsc.h`; **needs `ENABLE_MPI`** |
| `ENABLE_SUPERLU`   | SuperLU solver                                              | SuperLU library and `slu_ddefs.h`              |
| `ENABLE_BLAS`      | BLAS for linear algebra                                     | BLAS, and a Fortran compiler                   |
| `ENABLE_LAPACK`    | LAPACK for linear algebra                                   | LAPACK, and a Fortran compiler                 |
| `ENABLE_OPENMP`    | OpenMP parallelization                                      | OpenMP-capable compiler                        |
| `ENABLE_PTHREAD`   | Pthreads parallelization                                    | Threads                                        |
| `ENABLE_AUTODIFF`  | automatic differentiation interface                         | `autodiff` and Eigen3                          |
| `ENABLE_EIGEN`     | Eigen for linear algebra                                    |                                                |
| `ENABLE_FLTK`      | FLTK for the graphical interface                            | FLTK                                           |
| `ENABLE_JLCXX`     | Julia wrapper, see [BilWrap.jl](../julia/bilwrap_overview.md) | CxxWrap / libcxxwrap-julia                   |

All options are `OFF` by default. The file `OPTIONS` lists the options enabled in a given build, for instance:

```
ENABLE_MPI      = ON
ENABLE_PETSC    = ON
ENABLE_JLCXX    = ON
ENABLE_OPENMP   = OFF
...
```

If PETSc is enabled while MPI is not, the configuration stops with an error.

### Building and installing

In-source builds are refused. Build in a separate directory:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_MPI=ON -DENABLE_PETSC=ON ..
make
make install
```

By default the library goes to `<prefix>/lib` (`/usr/local/lib`) and the headers to `<prefix>/include/bil`. Some systems use `lib64` instead of `lib`, as chosen by `GNUInstallDirs`; check the directory where `libbil-<version>-<buildtype>.so` has been installed. The library is installed with an `rpath`, so that the executable `bil` finds it without `ldconfig`.

* * *

## Structure of a program

A program defines the dataset exactly as an input file does, section by section (see [From the input file to Julia](../julia/bilwrap_overview.md#from-the-input-file-to-julia) for the correspondence between the input file keywords and the sections). The sequence is the same as in the Julia interface:

```cpp
#include <bil/bil.h>

int main()
{
  /* 1. dataset and its sections */
  DataSet_t*     d           = DataSet_New("out");
  Geometry_t*    geom        = DataSet_GetGeometry(d);
  Mesh_t*        mesh        = DataSet_GetMesh(d);
  Fields_t*      fields      = DataSet_GetFields(d);
  Functions_t*   functions   = DataSet_GetFunctions(d);
  IConds_t*      iconds      = DataSet_GetIConds(d);
  BConds_t*      bconds      = DataSet_GetBConds(d);
  Loads_t*       loads       = DataSet_GetLoads(d);
  Dates_t*       dates       = DataSet_GetDates(d);
  Points_t*      points      = DataSet_GetPoints(d);
  ObVals_t*      obvals      = DataSet_GetObVals(d);
  IterProcess_t* iterprocess = DataSet_GetIterProcess(d);
  TimeStep_t*    timestep    = DataSet_GetTimeStep(d);
  Materials_t*   materials   = DataSet_GetMaterials(d);
  Models_t*      models      = DataSet_GetModels(d);
  Module_t*      modul       = DataSet_GetModule(d);
  Options_t*     opt         = DataSet_GetOptions(d);
  /* (Units_t* units = DataSet_GetUnits(d) if needed) */

  /* 2. session */
  Session_Open();

  /* 3. fill the sections */
  Geometry_Set(geom,2,"axis");
  Mesh_Set(mesh,"cylinder.msh");
  /* ... fields, functions, conditions, loads, points, dates, ... */

  /* 4. model and materials */
  Model_t* mod1 = Models_EmplaceBack(models,"Elast");
  Material_t* mat1 = Materials_EmplaceBack(materials,mod1);
  Material_Set(mat1,"young",10.e9);
  Material_Set(mat1,"poisson",0.26);
  Material_Finalize(mat1);

  /* 5. finalize the dataset, and solve */
  DataSet_Finalize(d);
  DataSet_PrintData(d,"all");
  Module_ComputeProblem(modul,d);

  /* 6. post-processing files, and clean up */
  PosFilesForGMSH_t* pf4gmsh = PosFilesForGMSH_Create(d);
  PosFilesForGMSH_ParsedFileFormat(pf4gmsh);

  DataSet_Delete(d);
  Mry_Free(d);
}
```

The functions are those described in the [function reference](../julia/bilwrap_reference.md), which was written for Julia but follows the C++ names and arguments one to one (`Fields_EmplaceBack(fields, ...)`, `Material_Set(mat, ...)`, ...). The meaning of every argument is the same; only the way arrays are passed differs (see below).

The solver and the other run options of the command line can be set with `Options_Set`, and the resolution module can be chosen with `Module_Set`; both are shown, commented, in the examples:

```cpp
//Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor");
//Module_Set(modul,"Monolithic");    // or "SNIA"
```

### Passing arrays and strings in C++

| Argument                         | C++ type and example                                          |
|----------------------------------|---------------------------------------------------------------|
| gradient and point of an affine field | `double g[3] = {0,-9810.,0};` then `Fields_EmplaceBack(fields,"affine",0.,g,x);` |
| times and values of a function   | `std::vector<double>`: `Functions_EmplaceBack(functions,"piecewiseaffine",t,f);` |
| output dates                     | initializer list: `Dates_Set(dates,{0,1,2});`                  |
| output point                     | initializer list: `Points_EmplaceBack(points,{0.15,0.15,0},"600");` |
| regions, unknowns, properties    | C strings (`"80"`, `"u_2"`, `"young"`)                         |
| field and function indices       | integers, numbered from 1; `0` means none                      |

Two points deserve attention.

- `IConds_EmplaceBack` has two overloads with four arguments, whose third argument is either an integer or a string. A literal `0` is then ambiguous, and must be written `(size_t) 0`, as in `IConds_EmplaceBack(iconds,"2","psi",(size_t) 0,0);` in `Duracem.cpp`.
- At the end of the program, `DataSet_Delete(d)` is followed by `Mry_Free(d)` in all the examples.

### Model with given equations and unknowns

In `Duracem.cpp`, the model is declared with the names of the equations and of the unknowns to be used, passed as two vectors of strings:

```cpp
std::vector<std::string> equ = {"carbon","silicon"};
std::vector<std::string> unk = {"logc_co2","z_si"};

mod1 = Models_EmplaceBack(models,"Duracem",equ,unk);
```

For the models of the other examples, the name alone is enough: `Models_EmplaceBack(models,"Elast")`.

* * *

## Compiling and linking

The program is compiled with a C++17 compiler, and linked to the library:

```
c++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib \
    -lbil-<version>-Release -DBASENAME=" " -fpermissive
```

The same command is used whatever the external libraries included in `libbil` (PETSc, MPI, SuperLU, BLAS, LAPACK, ...): they are dependencies of the library, and the headers do not call them from the code of the program. There is no need to add `-lpetsc_real`, `-lmpi`, ..., nor to use `mpic++`.

| Flag                            | Role                                                                 |
|---------------------------------|----------------------------------------------------------------------|
| `-std=c++17`                    | the library is built with C++17, and so are the examples             |
| `-L<dir>`, `-l bil-<version>-<buildtype>` | location and name of the library (without `lib` and `.so`) |
| `-Wl,-rpath,<dir>`              | lets the program find the library at run time                        |
| `-DBASENAME=...`                | name used by the headers for the namespace of the model code; `" "` when no external model is included, see [Defining your own model](#defining-your-own-model) |
| `-fpermissive`                  | needed by g++ for some constructs used in the examples (for instance the compound literals `(double[]){0.,0.,0.}` of `Duracem.cpp`) |
| `-g`                            | debugging information (for the `Debug` build)                        |

For a `Debug` build, use the library `-lbil-<version>-Debug` and add `-g`.

The build type in `-lbil-<version>-<buildtype>` must be one of those installed. The program is run like any executable, from the directory containing the files it reads (here `cylinder.msh`):

```
./a.out
```

and the output files are those described in [Running Bil](../running_bil.md#output-files).

* * *

## Examples

Three examples reproduce the problems of the reference base, and each has a counterpart in Julia (see [Julia examples](../julia/bilwrap_examples.md), which describes the problem in detail):

| Problem       | C++ file                       | Julia file     |
|---------------|--------------------------------|----------------|
| Elast         | `base/Elast/Elast.cpp`         | `Elast.jl`     |
| Duracem       | `base/Duracem/Duracem.cpp`     | `Duracem.jl`   |
| Richards-2d   | `base/Richards-2d/Richards-2d.cpp` | `Richards-2d.jl` |

Their compilation commands are given in the first lines of each file. `Duracem.cpp` defines no output point and no load, and the 1D mesh is given inline; `Elast.cpp` and `Richards-2d.cpp` also write the inverse permutation of the nodes (`Mesh_WriteInversePermutation(mesh,"out","hsl")`) and the Gmsh post-processing files.

* * *

## Defining your own model

A model is the C++ code that defines the equations of a problem. Those of the library (`Elast`, `Richards`, `Duracem`, ...) are listed by `bil -model`. With the library, a model can also be written **outside of the library**, in the source file of the user program, and used in the dataset like a built-in one. The example `base/Richards-2d/RichardsIncluded.cpp` solves the problem of `Richards-2d.cpp` with the model `MyRichards` of `MyRichards.cpp`.

### Using the model in the program

The file of the model is included after the header of Bil, an instance of the model is created, and it is declared in the dataset with `Models_EmplaceBack(models, model)`, which returns a `Model_t*` to be used for the materials, exactly as for a built-in model:

```cpp
#include <bil/bil.h>

#include "MyRichards.cpp"

int main()
{
  /* ... same dataset as in Richards-2d.cpp ... */

  MyRichards::MyRichards mod0;      // the model
  Model_t* mod1;

  Session_Open();
  /* ... */

  mod1 = Models_EmplaceBack(models,mod0);

  {
    Material_t* mat1 = Materials_EmplaceBack(materials,mod1);

    Material_Set(mat1,"Gravity",-9.81);
    Material_Set(mat1,"Porosity",0.38);
    /* ... the properties read by MyRichards ... */
    Material_Set(mat1,"Curves","billes");
    Material_Finalize(mat1);
  }
  /* ... */
}
```

The code of the model is put in a namespace whose name is given by the macro `BaseName()`, which expands to the value of `BASENAME`. For this reason the program is compiled with the name of the model file, here `MyRichards` (this is the namespace in `MyRichards::MyRichards`):

```
c++ -std=c++17 RichardsIncluded.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib \
    -lbil-<version>-Debug -DBASENAME="MyRichards" -fpermissive -g
```

### Structure of a model file

`MyRichards.cpp` is organized in the following parts, which are the same for any model.

**1. Constants.** Title, authors, number of equations and indices of the equations and of the unknowns:

```cpp
#define TITLE   "Richards Equation (3D)"
#define AUTHORS "Dangla"
#define NEQ     (1)
#define E_LIQ   (0)      // equation index
#define U_LIQ   (0)      // unknown index (generic)
#define U_P_L   U_LIQ    // unknown index
```

**2. Values at the integration points.** Four structures, templates on the numerical type `T`, are grouped by `CustomValues_t`:

```cpp
namespace BaseName() {
template<typename T = double> struct ImplicitValues_t {   // evaluated at each iteration
  T Pressure_liquid;  T GradPressure_liquid[3];
  T Mass_liquid;      T MassFlow_liquid[3];
};
template<typename T = double> struct ExplicitValues_t {   // evaluated at the previous time
  T Permeability_liquid;
};
template<typename T = double> struct ConstantValues_t {};
template<typename T = double> struct OtherValues_t {      // other terms
  T SaturationDegree_liquid;
};

template<typename T>
using Values_t = CustomValues_t<T,ImplicitValues_t,ExplicitValues_t,ConstantValues_t,OtherValues_t>;
}
```

The implicit values are the primary variables, their gradients and the terms of the balance equations (mass and mass flow of the liquid). The explicit values are the transfer coefficients: the flux is computed with the permeability of the previous time (`val_n.Permeability_liquid`), and the permeability of the current time is stored for the next one.

**3. Parameters of the material.** The structure `Parameters_t` contains the material properties, all of type `double`:

```cpp
struct Parameters_t {
  double Porosity;
  double IntrinsicPermeability;
  double ReferenceGasPressure;
  double Gravity;
  double LiquidMassDensity;
  double LiquidViscosity;
  double MacroGradient[3];
  double MacroFunctionIndex[3];
};
```

The function `pm(const char* s)` gives the index of a property in this structure from its name. The names are those used in the calls `Material_Set(mat, "Porosity", 0.38)` of the program (and in the input file of the executable). Here `MacroGradient` and `MacroFunctionIndex` can be given with a suffix, as in `MacroGradient_2`, for the problems with periodicities.

**4. The material point method.** The structure `MPM_t`, derived from `MaterialPointMethod_t<Values_t>`, gives the constitutive behaviour at an integration point:

| Method                          | Role                                                                      |
|---------------------------------|---------------------------------------------------------------------------|
| `SetInputs`                     | computes the pressure and its gradient from the nodal unknowns (and adds the macroscopic gradient for periodic geometries) |
| `Integrate`                     | computes the liquid mass, the fluxes and the permeability from the pressure, with the saturation and relative permeability curves of the material |
| `Initialize`                    | initializes the explicit values (permeability) at the initial time        |
| `SetTangentMatrix`              | derivative of the liquid mass with respect to the unknown                 |
| `SetTransferMatrix`             | transfer coefficients for the liquid mass flux                            |
| `SetIndexOfPrimaryVariables`    | position of the primary variable in the values                            |
| `SetIncrementOfPrimaryVariables`| increment of the primary variable, here 1% of the objective variation     |

`Integrate` is a template on the numerical type. When the library is built with `ENABLE_AUTODIFF`, it is also instantiated for the type `real` of the automatic differentiation library.

The saturation and the relative permeability are given by the two curves of the material (`Element_GetCurve(el)` and the following one), which are those defined with `Material_Set(mat, "Curves", ...)`:

```cpp
#define SATURATION_CURVE       (Element_GetCurve(el))
#define RELATIVEPERM_CURVE     (Element_GetCurve(el) + 1)
```

**5. The model.** The model is a structure derived from `UserModel_t<MPM_t>`:

```cpp
namespace BaseName() {
struct MyRichards : public UserModel_t<MPM_t> {
  private:
  MPM_t& mpm = GetInstanceOfMaterialPointMethod();

  public:
  int SetModelProperties();
  int ReadMatProp(Material_t* mat,DataFile_t* datafile);
  int ComputeMatrix(Element_t* el,double t,double dt,double* k);
  int ComputeResidu(Element_t* el,double t,double dt,double* r);
  int ComputeOutputs(Element_t* el,double t,double* s,Result_t* r);
};
}
```

| Method                | Role                                                                                        |
|-----------------------|---------------------------------------------------------------------------------------------|
| `SetModelProperties`  | name of the model (`GetCodeNameOfModel`, here `"MyRichards"`), title, authors, number of equations (`SetNbOfEquations`), names of the equations and of the unknowns, the function `pm` (`SetComputePropertyIndex`), the discretization method (here `UseFEM()`) |
| `ReadMatProp`         | sets the properties of a material to 0, reads them with `Material_ScanProperties(mat,datafile,pm)` and returns the number of properties |
| `ComputeMatrix`       | element matrix; here `mpm.ComputeMassConservationMatrixByFEM(el,t,dt,k)`                    |
| `ComputeResidu`       | element residual; here `mpm.ComputeMassConservationResiduByFEM(el,t,dt,r,E_LIQ,imass,iflow)` for the conservation of the liquid mass |
| `ComputeOutputs`      | the quantities written in the output files (here `pressure`, `flow` and `saturation`), stored with `Result_Store` |

The name given by `GetCodeNameOfModel` (`"MyRichards"`) is the name of the model for Bil; it is different from the built-in `"Richards"`. Since the model is not part of the library, it is not listed by `bil -model`; it exists only in the programs which include it.

!!! note "Julia"
    The models of the library can be used from [BilWrap.jl](../julia/bilwrap_overview.md), but a model written outside the library is C++ code, which must be compiled in a program like `RichardsIncluded.cpp`.
