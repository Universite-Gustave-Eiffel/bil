---
title: "M1 Model"
number-sections: false
---

# M1 Model — Richards Equation (1D liquid flow)

> **Source files:**
> `src/Models/ModelFiles/M1.c` · `doc/mkdocs/Flow/M1/M1`
>
> **Model authors:** P. Dangla (Université Gustave Eiffel)

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equation](#41-conservation-equation)
   - 4.2 [Flux law (generalized Darcy)](#42-flux-law-generalized-darcy)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: drainage of a bead column (`test_examples/M1`)](#6-test-case-drainage-of-a-bead-column)
7. [Numerical discretization](#7-numerical-discretization)
8. [Step-by-step file description](#8-step-by-step-file-description)
   - 8.1 [Driver file `test_examples/M1/M1`](#81-driver-file-test_examplesm1m1)
   - 8.2 [Model file `src/Models/ModelFiles/M1.c`](#82-model-file-srcmodelsmodelfilesm1c)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The **M1** model solves the classical one-dimensional **Richards** equation. This nonlinear partial differential equation represents the movement of water in an unsaturated porous medium under gravity and capillary forces, assuming the gas phase (air) is at constant atmospheric pressure.

This model is widely used in hydrology, hydrogeology, and civil engineering to predict rainfall infiltration in soils, drainage, or capillary rise.

---

## 2. Assumptions

1. **Single "active" phase**: Air flow is neglected; interstitial air is assumed at constant internal pressure $p_g$ = 1 atm. Modeling focuses exclusively on liquid phase transfer.
2. **Rigid solid matrix**: Deformation of the soil or materials is ignored. Porosity $\phi$ is constant.
3. **Isothermal**: Temperature is uniform, constantly affecting water viscosity and density.
4. **Validity of Darcy's law**: Applied to the water phase velocity filtering through the matrix, extended to unsaturated conditions via relative permeability $k_{rl}$ linked to the liquid saturation degree.

---

## 3. Variables and notation

The model supports 1 scalar equation whose unknown is the spatial distribution of the liquid pressure.

### Primary unknown

| Symbol | Meaning | Unit |
|---------|---------|------|
| $p_l$   | Liquid phase pore pressure | Pa |

### Behavior variables and constants

| Symbol | Meaning | Unit |
|---------|---------|------|
| $p_c$ | Capillary pressure ($p_g - p_l$) | Pa |
| $S_l$ | Liquid saturation (derived from $p_c$) | - |
| $k_{rl}$| Liquid relative permeability (depends on $p_c$) | - |
| $m_l$ | Local stored water mass: $\phi S_l \rho_l$ | kg/m$^3$ |

---

## 4. Mathematical model

### 4.1 Conservation equation

The conservation of liquid water mass (without source/sink term):

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{w}_l = 0$$


### 4.2 Flux law (generalized Darcy)

Water flows from higher to lower hydraulic head potential.

$$\mathbf{w}_l = \frac{\rho_l k_{\text{int}} k_{rl}(p_c)}{\mu_l} \left( - \nabla p_l + \rho_l \mathbf{g}\right)$$

Here, $\mathbf{g}$ is the gravitational vector and $\mu_l$ the dynamic viscosity. The nonlinearity stems from the strong coupling: motion depends on $k_{rl}$, which collapses as the medium drains and desaturates (decrease in $S_l$).

---

## 5. Boundary and initial conditions

The initial condition requires mapping the entire medium with a pressure field $p_l$. Often representing a free water column imposing a hydrostatic head, this field can be gradual ("affine"). Standard boundary conditions either fix impermeability at impermeable walls (zero flux = blocked gradient) or an external constant pressure managed by a Dirichlet condition.

---

## 6. Test case: drainage of a bead column

The `M1/` directory contains the reference simulation, modeling gravity-driven drainage of a bead column with high porosity (38%), representing a very permeable granular medium (e.g., gravel, glass beads).

### Simulation parameters

| Parameter | Value |
|-----------|-------|
| Geometry / Mesh | 1D plane (`1 plan`). Length = 20 cm, 100 cells |
| $\phi$ (Porosity) | 0.38 |
| Permeability $k_{\text{int}}$ | $8.9 \times 10^{-12}$ m² (very permeable medium) |
| Gravity $g$ | -9.81 m/s² |
| Gas pressure $p_g$ | $10^5$ Pa |

The macroscopic interaction curve is loaded via `Curves = billes`, injecting the empirical conductivity coefficients related to this bead size distribution.

### Test results and physical comments

Analyzing the solver evolution history (cut files `.t0, .t1, .t2` evaluated at $t=0, 120s, 300s$), the model faithfully reproduces the phenomenology of hydrogravitational drainage:

1. **Initial state ($t=0$)**: The field is initialized with a decreasing `affine` type, simulating the pressure gradient along the column axis from base to top.
2. **Transient drainage phase**: Without constraint to retain the vast majority of liquid mass given the high permeability $k \sim 10^{-12}$, water percolates at moderate speed toward the bottom. Due to gravity in the negative-x direction, saturation at the column top collapses drastically ($S_l \to S_{residual}$).
3. **Final capillary threshold**: When drainage stops, an equilibrium between the weight of the depleted water column and the suction/capillary tension force resists complete disintegration of the liquid mass. The column top maintains a constant static degree (pendular water).

![Pressure and saturation profiles during bead column drainage](M1/M1_results.png)

*(The graph above shows the spatio-temporal evolution: progressive drying of the upper column is observed, with residual saturation established by hydrostatic equilibrium at the final state $t=300s$.)*

---

## 7. Numerical discretization

The discretization relies on a robust spatial approach via the **Cell-centered Finite Volume Method** (`FVM.h`). The model evaluates the exchanged flux not individually at the node, but through the inter-volume edge cross-section, and associates it with a time step (Implicit Euler).

An **upwind** mechanism can be activated via the software constant `schema`, drastically improving stability in highly advective contexts by forcing evaluation of mobility $k_{rl}(S_l)$ according to the "donor" volume of the fluid, rather than a simple average of the two pressures (`M1.c` - Line 223).

---

## 8. Step-by-step file description

### 8.1 Driver file `M1`

1. **Geometry & Mesh**: Opens a simple `1 plan` vector, followed by its mesh geometry dividing the 20 cm thick block into 100 slices (`100 1`).
2. **Material**: Points to the compiled mathematical module `Model = M1`. The required constants for Richards equation are specified here (porosity `phi`, density `rho_l`, gravity).
3. **Fields & Initialization**: Provides a spatial function (`Type = affine`) with a value at the local origin of `1.e5` and a gradient of `-3400`. This recreates the hydrostatic law: $P_l(z) = 1.e5 - 3400 \times z$, harmoniously pre-setting the void stage profile.
4. **Boundary Conditions**: `Reg = 1` sets an impermeable or static pressure boundary at the top or bottom of the column by freezing the equation through the nodal constant.
5. **Objective Variations**: The flag `p_l = 10` tells the BIL adaptive time step manager that the local hydration pressure must not jump by more than $10$ Pa between two transient sequences, ensuring sudden drainage does not break the Newton method abruptly.

### 8.2 Model file `src/Models/ModelFiles/M1.c`

1. **Global architecture (Lines 14–45)**:
   - Very simple model with 1 equation `E_liq` for 1 unknown `I_p_l`. Registers `M_l()`, `W_l()` store nodal mass/flux information.
2. **`ComputeInitialState` (Line 151)**:
   - Extracts column parameters (`gravite`, `rho_l`). Immediately establishes for the integration points the contained fluid volume via the surface call to the `SATURATION()` curve.
   - Loads the matrix flow vector flux under the dual gravitational impulse and the existing hydraulic pressure gradient on cells zero and one.
3. **Transient Explicit Law (`ComputeExplicitTerms` - Line 200)**:
   - If `schema = 1` is defined, the code injects upwind treatment to capture the transfer pressure (from an inter-node to the downstream Darcy node). The active permeability `K_l` is refreshed accordingly.
4. **Diffusion Jacobian (`TangentCoefficients` - Line 471)**:
   - Re-evaluates the storage term differential (mass matrix proportional to $\partial S_l / \partial p_c$) and the flux divergence operator module. This matrix `c[i*nn + j]` locally conditions how a water increment changes the spatial unknown `p_l` nearby.
5. **Analytical balance (`ComputeResidu` - Line 333)**:
   - The control step: injects into residual $R$ the exact analytical difference between the liquid volume loss `M_ln - M_l` versus net advection `dt*surf*W_l`, validating at the node that strict water conservation was observed during the drop.

---

## 9. Bibliographic references

- **Richards, L. A.** (1931). Capillary conduction of liquids through porous mediums. *Journal of Applied Physics*, 1(5), 318–333. — The foundational article deriving the transient unsaturated soil equation.
- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*. — Universal basis for linking saturation with capillary pressure in these FVM solutions.
- **Celia, M. A., Bouloutas, E. T., & Zarba, R. L.** (1990). A general mass-conservative numerical solution for the unsaturated flow equation. *Water Resources Research*, 26(7), 1483–1496. — Explains the robustness and behavior of the FVM code coupled with implicit time discretization (Newton-Raphson) facing saturation asymmetries.
