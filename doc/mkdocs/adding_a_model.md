# How to Add a New Model in Bil

> **Sources:**
> `src/Models/ModelFiles/Template.c` · `src/Models/ModelFiles/TemplateFEM.c`
> `src/Models/ModelFiles/TemplateFVM.c` · `src/Models/ListOfModels.h`
> `src/Models/Methods/MaterialPointMethod.h` · `src/Models/Methods/CustomValues.h`
> `src/Models/DataBases/`

---

## Table of contents

1. [Where and how](#1-where-and-how)
2. [Internal variables](#2-internal-variables)
3. [The 11 basic functions of a model](#3-the-11-basic-functions-of-a-model)
4. [The `MaterialPointMethod.h` interface (C++)](#4-the-materialpointmethodh-interface-c)
5. [Physico-chemical databases](#5-physico-chemical-databases)
6. [Learning from existing models](#6-learning-from-existing-models)

---

## 1. Where and how

Adding a new model to Bil requires **only two steps** and does not modify any framework file.

### Step 1 — Create the model file

Create `src/Models/ModelFiles/MyModel.c` (or `.cpp`) using one of the available templates:

| Template | Discretization | Use case |
|----------|---------------|----------|
| `Template.c` | Any | Minimal skeleton |
| `TemplateFEM.c` | FEM | Finite element models |
| `TemplateFVM.c` | FVM | Finite volume models |

Upper camel case convention is used for model file names (`MyModel.c`). The same convention applies to attributes and methods.

### Step 2 — Register the model

Add the model to `src/Models/ListOfModels.inc`:

```c
/* Before */
SELECTEDMODELS =  ... Duracemold

/* After */
SELECTEDMODELS =  ... Duracemold MyModel
```

### Step 3 — Recompile

```bash
cd build
cmake ..
make
```

The input file can now use `Model = MyModel`. **No other framework file is modified.**

A short description of the available models can be displayed at any time with:

```bash
bil -model              # list all models
bil -model mymodel      # show example input for mymodel
```

---

## 2. Internal variables

At each **Gauss point** (FEM) or **node** (FVM), three types of internal variables are stored and updated at different rates:

| Type | Array | When recomputed | Typical content |
|------|-------|----------------|-----------------|
| **Implicit** (`NVI`) | `vi[k]` | Every Newton-Raphson iteration | Fluxes, stresses, storage terms |
| **Explicit** (`NVE`) | `ve[k]` | Once per time step (from previous step) | Permeability, saturation, conductivity |
| **Constant** (`NV0`) | `v0[k]` | Never after initialization | Initial state, material constants per element |

These three counters must be defined in each model file:

```c
#define NVI  9   /* number of implicit variables per Gauss point */
#define NVE  2   /* number of explicit variables per Gauss point */
#define NV0  4   /* number of constant variables per Gauss point */
```

Choosing the correct type for each variable is important for correctness:
- Use **implicit** for quantities that depend on the current unknown values.
- Use **explicit** for quantities evaluated at the previous time step that remain fixed during Newton iterations (avoids recomputing expensive constitutive laws at each iteration).
- Use **constant** for quantities that are spatially variable but time-independent.

---

## 3. The 11 basic functions of a model

Every model file must implement **11 functions** that correspond to the methods of the `Model_t` interface. The `PredefinedModelMethods.h` macro handles the `SetModelProp` boilerplate automatically.

| Function | Signature | Description |
|----------|-----------|-------------|
| `SetModelProp` | `int (Model_t*)` | Set model properties (populates `Model_t`) |
| `ReadMatProp` | `int (Material_t*, DataFile_t*)` | Read material properties from input file |
| `PrintModelProp` | `int (Model_t*, FILE*)` | Print model properties (for `-model` option) |
| `DefineElementProp` | `int (Element_t*, IntFcts_t*, ShapeFcts_t*)` | Define per-element arrays and integration order |
| `ComputeInitialState` | `int (Element_t*, double)` | Compute initial state at $t = 0$ |
| `ComputeExplicitTerms` | `int (Element_t*, double)` | Compute explicit terms from previous step |
| `ComputeImplicitTerms` | `int (Element_t*, double, double)` | Compute implicit terms at current step |
| `ComputeMatrix` | `int (Element_t*, double, double, double*)` | Assemble tangent stiffness matrix |
| `ComputeResidu` | `int (Element_t*, double, double, double*)` | Compute Newton-Raphson residual |
| `ComputeLoads` | `int (Element_t*, double, double, Load_t*, double*)` | Compute load vector |
| `ComputeOutputs` | `int (Element_t*, double, double*, Result_t*)` | Compute post-processing output fields |

### Minimal skeleton

```c
/* MyModel.c */
#include "CommonModel.h"
#include "FEM.h"      /* or FVM.h */

#define TITLE   "My model title"
#define AUTHORS "Author name"

#include "PredefinedModelMethods.h"

#define NEQ  2    /* number of equations / nodal unknowns */
#define NVI  5    /* implicit variables per Gauss point */
#define NVE  2    /* explicit variables per Gauss point */
#define NV0  0    /* constant variables per Gauss point */

/* Material property index */
int pm(const char* s)
{
  if(strcmp(s,"young")   == 0) return 0 ;
  if(strcmp(s,"poisson") == 0) return 1 ;
  return(-1) ;
}

/* Populate Model_t */
int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  Model_CopyNameOfEquation(model, 0, "eq1") ;
  Model_CopyNameOfEquation(model, 1, "eq2") ;
  Model_CopyNameOfUnknown(model,  0, "u1") ;
  Model_CopyNameOfUnknown(model,  1, "u2") ;
  return(0) ;
}

int ReadMatProp(Material_t* mat, DataFile_t* datafile)
{
  Material_ScanProperties(mat, datafile, pm) ;
  return(2) ;
}

int PrintModelProp(Model_t* model, FILE* f)
{
  /* Print example input for bil -model mymodel */
  fprintf(f, "young = 1.e9\n") ;
  fprintf(f, "poisson = 0.3\n") ;
  return(0) ;
}

int DefineElementProp(Element_t* el, IntFcts_t* intfcts, ShapeFcts_t* shpfcts)
{
  /* Allocate NVI, NVE, NV0 arrays */
  Element_GetNbOfImplicitTerms(el) = NVI ;
  Element_GetNbOfExplicitTerms(el) = NVE ;
  Element_GetNbOfConstantTerms(el) = NV0 ;
  return(0) ;
}

int ComputeInitialState(Element_t* el, double t) { return(0) ; }
int ComputeExplicitTerms(Element_t* el, double t) { return(0) ; }
int ComputeImplicitTerms(Element_t* el, double t, double dt) { return(0) ; }
int ComputeMatrix(Element_t* el, double t, double dt, double* k) { return(0) ; }
int ComputeResidu(Element_t* el, double t, double dt, double* r) { return(0) ; }
int ComputeLoads(Element_t* el, double t, double dt, Load_t* cg, double* r) { return(0) ; }
int ComputeOutputs(Element_t* el, double t, double* s, Result_t* r) { return(0) ; }
```

---

## 4. The `MaterialPointMethod.h` interface (C++)

For C++ models, Bil provides a higher-level interface that significantly simplifies implementation. Instead of manually writing all 11 functions, one derives a class `MPM_t` from `MaterialPointMethod_t`.

### 4.1 Define internal variables with `CustomValues.h`

```cpp
#include "CustomValues.h"

template<typename T> struct ImplicitValues_t;  /* vary at each iteration */
template<typename T> struct ExplicitValues_t;  /* vary at each time step */
template<typename T> struct ConstantValues_t;  /* constant over time */

template<typename T>
using V = CustomValues_t<T, ImplicitValues_t, ExplicitValues_t, ConstantValues_t, ...>;
```

The three variable types correspond to the per-Gauss-point arrays described in [section 2](#2-internal-variables).

### 4.2 Derive the `MPM_t` class

```cpp
#include "MaterialPointMethod.h"

struct MPM_t : public MaterialPointMethod_t<V> {

  /* Initialize val with nodal unknowns and their gradients */
  V<double>* SetInputs(Element_t* el, double const& t, int const& p,
                       double const* const* u, V<double>& val);

  /* Initialize all internal variables (initial state) */
  V<double>* Initialize(Element_t* el, double const& t, V<double>& val);

  /* Integrate the constitutive law from t-dt to t */
  V<double>* Integrate(Element_t* el, double const& t, double const& dt,
                       V<double> const& val_n, V<double>& val);

  /* Fill the k-th column of the tangent matrix */
  int SetTangentMatrix(Element_t* el, double const& t, double const& dt,
                       int const& p, V<double> const& val, V<double> const& dval,
                       int const& k, double* c);

  /* Fill the transfer matrix (FVM) */
  int SetTransferMatrix(Element_t* el, double const& dt, int const& p,
                        V<double> const& val, double* c);

  /* Compute fluxes between nodes i and j (FVM) */
  V<double>* SetFluxes(Element_t* el, double const& t, int const& i, int const& j,
                       V<double> const& grdval, V<double>* val);

  /* Give the index in V of each primary unknown */
  void SetIndexOfPrimaryVariables(Element_t* el, int* ind);

  /* Give the increment Δu used for numerical derivatives */
  void SetIncrementOfPrimaryVariables(Element_t* el, double* dui);
};
```

### 4.3 Automatic differentiation with `autodiff.h` (requires AUTODIFF to be enabled in OPTIONS)

The tangent matrix can be obtained by **automatic differentiation** rather than finite differences:

```cpp
#define USE_AUTODIFF
#include "autodiff.h"
```

When `USE_AUTODIFF` is defined, the `Differentiate` operator analytically computes the derivative of `Integrate` with respect to the primary unknowns, without manual Jacobian implementation.

Otherwise, numerical differentiation is used automatically via `SetIncrementOfPrimaryVariables`:

```cpp
void SetIncrementOfPrimaryVariables(Element_t* el, double* dui) {
  ObVal_t* obval = Element_GetObjectiveValue(el);
  dui[0] = 1.e-2 * ObVal_GetValue(obval + 0);  /* increment = 1% of objective variation */
}
```

---

## 5. Physico-chemical databases

Bil provides a library of physico-chemical data accessible from any model via header inclusion. These data are stored in `src/Models/DataBases/`.

```c
/* Water viscosity as a function of temperature (in K) */
#include "WaterViscosity.h"
double mu_w = WaterViscosity(293);   /* Pa·s at 293 K */

/* Diffusion coefficient of an ion in water */
#include "DiffusionCoefficientOfMoleculeInWater.h"
double d_ca = DiffusionCoefficientOfMoleculeInWater(Ca, 293);  /* m²/s */

/* Diffusion coefficient in air */
#include "DiffusionCoefficientOfMoleculeInAir.h"
double d_co2_air = DiffusionCoefficientOfMoleculeInAir(CO2, 293);
```

Available databases:

| Header | Content |
|--------|---------|
| `WaterViscosity.h` | Water viscosity as a function of T |
| `AirViscosity.h` | Air viscosity |
| `DiffusionCoefficientOfMoleculeInWater.h` | Ionic diffusion coefficients in aqueous solution |
| `DiffusionCoefficientOfMoleculeInAir.h` | Diffusion coefficients in gas phase |
| `HardenedCementChemistry.h` | Chemistry of hydrated cement-based binders |
| `CementSolutionChemistry.h` | Chemistry of cement pore solution |
| `ElectricChargeOfIonInWater.h` | Electric charges of ions |
| `EquilibriumConstantOfHomogeneousReactionInWater.h` | Equilibrium constants of reactions in solution |
| `IonizationConstantOfWater.h` | Ionization constant of water |
| `HenrysLawConstantForSolubilityOfGasInWater.h` | Henry's law constant (gas solubility) |
| `AtmosphericPressure.h` | Reference atmospheric pressure |

---

## 6. Learning from existing models

The best way to implement a new model is to study an existing one with similar physics. The following models are good starting points:

### Flow problems

| Model | File | Description |
|-------|------|-------------|
| `M1` | `M1.c` | 1D unsaturated flow (Richards equation, FVM) |
| `Fick` | `Fick.c` | Diffusion by Fick's second law |
| `Richards` | `Richards.c` | 2D/3D unsaturated flow (FEM) |

These are relatively compact models that illustrate:
- scalar unknowns (pressure, concentration)
- explicit/implicit variable split for permeability/saturation
- simple retention curves via `Curves` input

### Coupled mechanical problems

| Model | File | Description |
|-------|------|-------------|
| `Elast` | `Elast.c` | Linear elasticity (FEM) |
| `Plast` | `Plast.c` | Elastoplasticity with Drucker-Prager criterion |
| `M7` | `M7.c` | Unsaturated poromechanics (Biot, FEM) |
| `BBM` | `BBM.c` | Barcelona Basic Model (unsaturated plasticity) |

These models illustrate:
- vector unknowns (displacement)
- stress integration (return mapping algorithm)
- coupled hydro-mechanical behavior
- the full use of `NVI`, `NVE`, `NV0`

### FEM² multi-scale

| Model | File | Description |
|-------|------|-------------|
| `MechaMic` | `MechaMic.c` | Multi-scale mechanics via FEM² |

This model shows how to couple a macro-scale mesh with a micro-scale representative volume element (RVE). Periodic boundary conditions on the RVE are handled via the `PERIODICITIES` block in the input file:

```
PERIODICITIES
2
MasterRegion = 10  SlaveRegion = 11  PeriodVector = 2 0 0
MasterRegion = 20  SlaveRegion = 21  PeriodVector = 0 2 0
```
