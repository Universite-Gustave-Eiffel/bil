# Code Architecture

> **Sources:**
> `src/Models/Model.h` · `src/Models/Model.c` · `src/Models/Models.h`
> `src/Models/ListOfModels.h` · `src/Models/ModelFiles/Template.c`
> `src/Models/ModelFiles/TemplateFEM.c` · `src/Models/ModelFiles/TemplateFVM.c`
> `src/Modules/ModuleFiles/Monolithic.c`
> `src/Models/Methods/MaterialPointMethod.h` · `src/Models/Methods/CustomValues.h`

---

## Table of contents

1. [Overview](#1-overview)
2. [Fundamental principle: plugin architecture in C](#2-fundamental-principle-plugin-architecture-in-c)
3. [The `Model_t` interface — the vtable in C](#3-the-model_t-interface--the-vtable-in-c)
4. [The model registry — `ListOfModels.h`](#4-the-model-registry--listofmodelsh)
5. [Startup resolution — `Model_Initialize`](#5-startup-resolution--model_initialize)
6. [What a physical model must implement](#6-what-a-physical-model-must-implement)
7. [The computation loop — `Monolithic.c`](#7-the-computation-loop--monolithicc)
8. [Complete data flow](#8-complete-data-flow)
9. [Solution history management](#9-solution-history-management)
10. [Comparison with other patterns](#10-comparison-with-other-patterns)

---

## 1. Overview

Bil is organized into **two strictly separated layers**:

| Layer | Files | Role |
|-------|-------|------|
| **Generic framework** | `Monolithic.c`, `SNIA.c`, `Solver.c`, `FEM.c`, `Model.c`… | Time loop, global matrix assembly, linear system solver, I/O — **knows nothing about physics** |
| **Physical plugin** | `M7.c`, `BBM.c`, `Richards.c`, `Fick.c`… | Implements the equations: implicit terms, tangent matrix, residual, outputs — **knows nothing about the solver** |

The link between the two is an **interface contract** materialized by the `Model_t` struct, which is a table of function pointers (C equivalent of a C++ *vtable*).

---

## 2. Fundamental principle: plugin architecture in C

The natural question is: "since each simulation corresponds to a different `.c` file, where is the genericity?"

The answer is that **the genericity is in the framework**, not in the models. The framework always calls the same function signatures, regardless of the physics:

```
framework → model->computematrix(el, t, dt, K)
framework → model->computeimplicitterms(el, t, dt)
framework → model->computeresidu(el, t, dt, r)
```

This is exactly the same mechanism as an interface in Java or an abstract class in C++, but implemented in pure C with function pointers.

```
                 ┌─────────────────────────────────────┐
                 │         Generic Framework           │
                 │  Monolithic.c  ·  SNIA.c  ·  FEM.c  │
                 │                                     │
                 │   Mesh_ComputeMatrix(mesh, ...)     │
                 │   Mesh_ComputeResidu(mesh, ...)     │
                 │   Solver_Solve(solver, ...)         │
                 └──────────────┬──────────────────────┘
                                │ calls via function pointers
                                │ (Model_t as interface)
          ┌─────────────────────┼──────────────────────────┐
          │                     │                          │
   ┌──────▼──────┐      ┌───────▼──────┐      ┌───────────▼──────┐
   │    M7.c     │      │   BBM.c      │      │   Richards.c     │
   │ Unsaturated │      │ Barcelona    │      │ Unsaturated flow │
   │ poroelas-   │      │ Basic Model  │      │ (Darcy)          │
   │ ticity      │      │ (plasticity) │      │                  │
   └─────────────┘      └──────────────┘      └──────────────────┘
```

---

## 3. The `Model_t` interface — the vtable in C

Defined in `src/Models/Model.h`, the `Model_t` struct contains **exclusively function pointers**:

```c
/* src/Models/Model.h — method signatures of the interface */

typedef int  (Model_SetModelProperties_t)      (Model_t*) ;
typedef int  (Model_ReadMaterialProperties_t)  (Material_t*, DataFile_t*) ;
typedef int  (Model_PrintModelProperties_t)    (Model_t*, FILE*) ;
typedef int  (Model_DefineElementProperties_t) (Element_t*, IntFcts_t*, ShapeFcts_t*) ;
typedef int  (Model_ComputeInitialState_t)     (Element_t*, double) ;
typedef int  (Model_ComputeExplicitTerms_t)    (Element_t*, double) ;
typedef int  (Model_ComputeImplicitTerms_t)    (Element_t*, double, double) ;
typedef int  (Model_ComputeMatrix_t)           (Element_t*, double, double, double*) ;
typedef int  (Model_ComputeResidu_t)           (Element_t*, double, double, double*) ;
typedef int  (Model_ComputeLoads_t)            (Element_t*, double, double, Load_t*, double*) ;
typedef int  (Model_ComputeOutputs_t)          (Element_t*, double, double*, Result_t*) ;
typedef int  (Model_ComputePropertyIndex_t)    (const char*) ;
typedef void (Model_ComputeMaterialProperties_t)(Element_t*, double) ;

struct Model_t {
  Model_SetModelProperties_t*        setmodelprop ;        /* initializes all other pointers */
  Model_ReadMaterialProperties_t*    readmatprop ;         /* reads parameters from input file */
  Model_PrintModelProperties_t*      printmodelprop ;      /* displays model help */
  Model_DefineElementProperties_t*   defineelementprop ;   /* allocates per-element arrays */
  Model_ComputeInitialState_t*       computeinitialstate ; /* computes the initial state */
  Model_ComputeExplicitTerms_t*      computeexplicitterms ;/* explicit terms (permeability…) */
  Model_ComputeImplicitTerms_t*      computeimplicitterms ;/* implicit terms (storage, flux…) */
  Model_ComputeMatrix_t*             computematrix ;       /* stiffness/coupling matrix */
  Model_ComputeResidu_t*             computeresidu ;       /* Newton-Raphson residual */
  Model_ComputeLoads_t*              computeloads ;        /* load vector */
  Model_ComputeOutputs_t*            computeoutputs ;      /* post-processing fields */
  Model_ComputePropertyIndex_t*      computepropertyindex ;/* index of a material property */
  Model_ComputeMaterialProperties_t* ComputeMaterialProperties ; /* properties at Gauss point */
  /* + metadata: codename, shorttitle, authors, nbofequations, nameofequations… */
} ;
```

The framework only manipulates `Model_t*`. It is completely unaware of whether the model behind it is M7, BBM, or a custom model.

---

## 4. The model registry — `ListOfModels.h`

`src/Models/ListOfModels.h` is the **declaration file for all compiled models**. It uses the **X-macro** technique to generate both the name list and the function pointer list from a single source of truth:

```c
/* src/Models/ListOfModels.h */

#define ListOfModels_Nb          41

#define ListOfModels_Names       "BBM","BBMgas","BExM","CHMBWP",\
"Cryspom","DWS1","Elasd","Fick","Frostaco","Frostaco3d","Gascoal",\
"HydrateThermoPoroplasticity","M1","M10","M2","M7","NSFSHao",\
"Plastold","Shen","Sulfaco","Sulfaco3d","SulfacoESA3d","Sulfacocl",\
"Sulfuricem","TVIThermoPoroplast","Thermoporoplast","Yuan1",\
"hydrapel","usoil","BBMGas","Chloricem","Duracem","Duracemold",\
"Elast","Ficknew","Frostsoil","MechaMic","Plast","Poroplast",\
"Richards","Sulfaconew"

#define ListOfModels_Methods(m)  BBM##m,BBMgas##m,BExM##m,CHMBWP##m,\
Cryspom##m,DWS1##m,Elasd##m,Fick##m,Frostaco##m,Frostaco3d##m,\
Gascoal##m,HydrateThermoPoroplasticity##m,M1##m,M10##m,M2##m,M7##m,\
NSFSHao##m,Plastold##m,Shen##m,Sulfaco##m,Sulfaco3d##m,\
SulfacoESA3d##m,Sulfacocl##m,Sulfuricem##m,TVIThermoPoroplast##m,\
Thermoporoplast##m,Yuan1##m,hydrapel##m,usoil##m,BBMGas##m,\
Chloricem##m,Duracem##m,Duracemold##m,Elast##m,Ficknew##m,\
Frostsoil##m,MechaMic##m,Plast##m,Poroplast##m,Richards##m,\
Sulfaconew##m
```
This file is automatically generated with `src/Models/ListOfModels.inc`:

```c
/* src/Models/ListOfModels.inc */

SELECTEDMODELS =  Elast Plastold Plast Elasd M1 M10 M2 DWS1 Cryspom \
M7 Frostaco Frostaco3d usoil hydrapel Poroplast Duracem Chloricem \
Shen Gascoal Yuan1 Fick Sulfuricem Sulfaco Sulfacocl Sulfaco3d \
MechaMicold MechaMic CHMBWP BBM BBMgas BBMGas TVIThermoPoroplast \
Thermoporoplast NSFSHao HydrateThermoPoroplasticity BExM \
SulfacoESA3d Frostsoil Richards Sulfaconew Ficknew Duracemold
```

**How the X-macro works:**

The call `ListOfModels_Methods(_SetModelProp)` is expanded by the preprocessor to:

```c
BBM_SetModelProp, BBMgas_SetModelProp, ..., M7_SetModelProp, ..., Sulfaconew_SetModelProp
```

This allows the construction, **statically at compile time**, of two parallel arrays indexed from 0 to 38:

```
index │ name (modelnames[i])            │ function (xModel_SetModelProperties[i])
──────┼─────────────────────────────────┼─────────────────────────────────────────
  0   │ "BBM"                           │ BBM_SetModelProp
  1   │ "BBMgas"                        │ BBMgas_SetModelProp
  …   │ …                               │ …
 16   │ "M7"                            │ M7_SetModelProp
  …   │ …                               │ …
 38   │ "Sulfaconew"                    │ Sulfaconew_SetModelProp
```

---

## 5. Startup resolution — `Model_Initialize`

When Bil reads `Model = M7` in the input file, it calls `Model_Initialize` (`src/Models/Model.c`):

```c
/* src/Models/Model.c — simplified */

Model_t* Model_Initialize(Model_t* model, const char* codename, ...)
{
  int n_models = Models_NbOfModels ;                               // 38
  const char* modelnames[] = {Models_ListOfNames} ;               // {"BBM","BBMgas",...,"M7",...}
  Model_SetModelProperties_t* xModel_SetModelProperties[] =
                              {Models_ListOfSetModelProp} ;        // {BBM_SetModelProp,...,M7_SetModelProp,...}

  int i = 0 ;
  while(i < n_models && strcmp(modelnames[i], codename)) i++ ;   // search "M7" → i=15

  Model_GetSetModelProperties(model) = xModel_SetModelProperties[i] ; // assign M7_SetModelProp
  Model_SetModelProperties(model) ;   // call M7_SetModelProp(model)
                                      // → populates ALL other Model_t pointers

  return(model) ;
}
```

After this call, `model->computematrix` points to `k7`, `model->computeresidu` to `c7`, etc. — all functions from `M7.c`.

---

## 6. What a physical model must implement

Each model file must provide a `SetModelProp` function and several computation functions. Here is the minimal skeleton from `src/Models/ModelFiles/Template.c`:

```c
/* MyModel.c */
#include "CommonModel.h"
#include "FEM.h"                 /* or "FVM.h" depending on the discretization */

#define TITLE   "Model name"
#define AUTHORS "Author"

#include "PredefinedModelMethods.h"  /* macro that declares SetModelProp automatically */

#define NEQ  2   /* number of equations = number of nodal unknowns */
#define NVI  9   /* implicit terms per Gauss point */
#define NVE  2   /* explicit terms per Gauss point */

/* --- 1. Material property registry --- */
int pm(const char* s)
{
  if(strcmp(s,"young")   == 0) return 0 ;
  if(strcmp(s,"poisson") == 0) return 1 ;
  return -1 ;  /* unknown parameter */
}

/* --- 2. Model properties (populate Model_t) --- */
int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  Model_CopyNameOfEquation(model, 0, "meca") ;
  Model_CopyNameOfEquation(model, 1, "mass") ;
  Model_CopyNameOfUnknown(model,  0, "u")   ;
  Model_CopyNameOfUnknown(model,  1, "p_l") ;
  return 0 ;
}

/* --- 3. Read parameters from input file --- */
int ReadMatProp(Material_t* mat, DataFile_t* datafile)
{
  Material_ScanProperties(mat, datafile, pm) ;
  return 2 ;  /* number of scalar parameters */
}

/* --- 4. Explicit terms: computed from values at previous step --- */
int ComputeExplicitTerms(Element_t* el, double t) { ... }

/* --- 5. Implicit terms: storage, flux, stresses… --- */
int ComputeImplicitTerms(Element_t* el, double t, double dt) { ... }

/* --- 6. Tangent stiffness matrix --- */
int ComputeMatrix(Element_t* el, double t, double dt, double* k) { ... }

/* --- 7. Residual for Newton-Raphson --- */
int ComputeResidu(Element_t* el, double t, double dt, double* r) { ... }

/* --- 8. Post-processing: output fields --- */
int ComputeOutputs(Element_t* el, double t, double* s, Result_t* r) { ... }
```

**Correspondence with historical names in M7.c** (legacy API via `OldMethods.h`):

| Modern interface method | Name in M7.c (old API) | Role |
|------------------------|------------------------|------|
| `SetModelProp`         | `dm7` + initialization | Declares equations and unknowns |
| `ComputeImplicitTerms` | `mxnd`                 | Computes $M_l$, $W_l$, $\sigma$, $\phi$ at Gauss points |
| `ComputeMatrix`        | `k7`                   | Assembles the stiffness matrix $K$ |
| `ComputeResidu`        | `c7`                   | Computes residual $R = F_\text{ext} - F_\text{int}$ |
| `ComputeInitialState`  | `rsnd`                 | Initializes fields at time $t=0$ |
| `ComputeOutputs`       | `so7` (via `Views`)    | Produces post-processing fields |

---

## 7. The computation loop — `Monolithic.c`

The module `src/Modules/ModuleFiles/Monolithic.c` is the orchestrator. Its inner loop (simplified) is:

```
For each time step [t_n → t_{n+1}]:
│
├── Mesh_ComputeExplicitTerms(mesh, t_n)
│     └── for each element: model->computeexplicitterms(el, t_n)
│         [computes permeability, saturation… from previous step values]
│
├── Newton-Raphson iterations:
│   │
│   ├── Mesh_ComputeImplicitTerms(mesh, t_{n+1}, dt)
│   │     └── for each element: model->computeimplicitterms(el, t, dt)
│   │         [computes flux, storage, stresses with current unknowns]
│   │
│   ├── Mesh_ComputeMatrix(mesh, a, t_{n+1}, dt)
│   │     └── for each element: model->computematrix(el, t, dt, K_e)
│   │         [assembles tangent matrix K_e into K_global]
│   │
│   ├── Mesh_ComputeResidu(mesh, r, loads, t_{n+1}, dt)
│   │     └── for each element: model->computeresidu(el, t, dt, r_e)
│   │         [assembles r_global = F_ext - F_int]
│   │
│   ├── Solver_Solve(solver, K_global, r_global, du)
│   │     [solves K · Δu = r via SuperLU / PETSc / Crout…]
│   │
│   └── Update unknowns: u ← u + Δu
│       Check convergence (||Δu|| / ||u|| < tol)
│
└── If convergence: advance t_n ← t_{n+1}, write outputs if t ∈ Dates
```

At **no point** does `Monolithic.c` reference `M7`, `BBM`, or any other model name. It only operates through the `Model_t` pointers.

---

## 8. Complete data flow

```
Input file (M7-1)
  "Model = M7"
        │
        ▼
  Parser (Flex/Bison)
        │ reads the string "M7"
        ▼
  Model_Initialize("M7")                    [Model.c]
        │ linear search in modelnames[]
        │ assigns xModel_SetModelProperties[15] = M7_SetModelProp
        │
        ▼
  M7_SetModelProp(model)                    [M7.c]
        │ model->computematrix        = k7
        │ model->computeresidu        = c7
        │ model->computeimplicitterms = mxnd
        │ model->computeinitialstate  = rsnd
        │ model->readmatprop          = dm7 (via pm)
        │ model->computeoutputs       = so7
        │
        ▼
  Monolithic.c — time loop
        │ calls model->computeexplicitterms(el, t)   → M7 physics
        │ calls model->computeimplicitterms(el, t, dt)→ M7 physics
        │ calls model->computematrix(el, t, dt, K)   → M7 physics
        │ calls model->computeresidu(el, t, dt, r)   → M7 physics
        │
        ▼
  Solver.c — solve K·Δu = r              [SuperLU / PETSc / …]
        │
        ▼
  Result files (.t0, .t1, …)
```

---

## 9. Solution history management

The solver module maintains a **circular history** of solutions at the last instants $t_n, t_{n-1}, t_{n-2}, \ldots$ as a circular linked list. This structure allows `ComputeExplicitTerms` to access values at the previous step without extra copying.

```
Before time advance:    tn-3 ← tn-2 ← tn-1 ← tn ←•

After advance tn → tn+1: tn-2 ← tn-1 ← tn ← tn+1 ←•
                          (tn-3 is reused to store tn+1)
```

The number of solutions kept simultaneously in memory is controlled by the option:

```bash
bil -with "Monolithic N" my_file   # N = number of solutions in memory (default 2)
```

---

## 10. Comparison with other patterns

| Concept | In C (Bil) | In C++ | In Python |
|---------|-----------|--------|-----------|
| Interface / contract | `struct Model_t` with function pointers | Abstract class with `virtual` methods | Base class with methods to override |
| Implementation | Free C functions assigned in `SetModelProp` | Subclass overriding virtual methods | Subclass redefining methods |
| Registry | `ListOfModels.h` (X-macro, static, compile-time) | Factory pattern, `std::string` → constructor map | Name → class dictionary |
| Dispatch | Call via function pointer `model->computematrix(...)` | Dispatch via C++ vtable (automatic) | Dispatch via Python MRO |
| Discovery | Manual declaration in `ListOfModels.h` | Same or RTTI | Same or `importlib` dynamic |

The choice of C with X-macros guarantees **zero runtime overhead** (no dynamic lookup) and maximum compatibility with C89/C++17 compilers used by the project.
