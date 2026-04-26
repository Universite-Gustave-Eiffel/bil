# Frostaco3D Model — Freeze Action in 3D Concrete (Full Poromechanical Coupling)

> **Bil model:** `src/Models/ModelFiles/Frostaco3d.c`

> **Input file:** `doc/mkdocs/PhaseChange/Frostaco3d/Frostaco3d`
>
> **Model authors:** Tahiri, P. Dangla (Université Gustave Eiffel)

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equations and mechanical equilibrium](#41-conservation-equations-and-mechanical-equilibrium)
   - 4.2 [Flux laws](#42-flux-laws)
   - 4.3 [Ice-liquid thermodynamic equilibrium](#43-ice-liquid-thermodynamic-equilibrium)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: freezing in spherical geometry (`test_examples/Frostaco3d/`)](#6-test-case-freezing-in-spherical-geometry)
7. [Numerical discretization](#7-numerical-discretization)
8. [Step-by-step file description](#8-step-by-step-file-description)
   - 8.1 [Driver file `test_examples/Frostaco3d/Frostaco3d`](#81-driver-file-test_examplesfrostaco3dfrostaco3d)
   - 8.2 [Model file `src/Models/ModelFiles/Frostaco3d.c`](#82-model-file-srcmodelsmodelfilesfrostaco3dc)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The **Frostaco3d** (Frost actions in 3D concrete) model is a multidimensional evolution of the original Frostaco model. Unlike the 1D version which handles mechanical stresses in "post-processing", this version integrates the full coupled resolution of the tensor field of **mechanical displacements** of the solid skeleton (porous walls) under the effect of ice expansion and cryosuction. It retains thermo-hydro-chemical coupled transfers with activity of a dissolved salt (NaCl, CaCl₂).

---

## 2. Assumptions

1. **Three-dimensional spatial representation** (1D spherical, 2D plane, or full 3D).
2. Active resolution of **solid displacements** and mechanical equilibrium according to Coussy's poromechanics theory.
3. The medium is modeled as an **isotropic elastic matrix**. Its elastic moduli (Young's modulus $E$, Poisson's ratio $\nu$) can be prescribed, or automatically derived from the *Mori-Tanaka* homogenization scheme based on the bulk properties of the fine matrix.
4. Liquid phase transfer governed by Darcy's law (without interstitial ice transport).
5. Thermodynamic activity of water affected by salt via a simplified law (cryoscopic depression).

---

## 3. Variables and notation

The model supports $3 + \text{dim}$ equations where $\text{dim}$ is the space dimension. In 3D, 6 primary variables are solved at each node.

### Primary unknowns

| Symbol | Meaning | Unit |
|---------|---------|------|
| $u_1, u_2, u_3$ | Mechanical displacement vector of the skeleton | m |
| $p_{l}$ (or $p_{\max}$) | Liquid pressure or maximum pressure | Pa |
| $c_s$ | Salt concentration in the pore solution | mol/m³ |
| $T$ | Temperature | K |

### Behavior variables

| Symbol | Meaning |
|---------|---------|
| $\sigma$ | Total stress tensor |
| $\varepsilon$ | Strain tensor ($\nabla^s \mathbf{u}$) |
| $S_l, S_i$ | Liquid / ice saturation degrees |

---

## 4. Mathematical model

### 4.1 Conservation equations and mechanical equilibrium

The model solves the following strongly coupled system:

**1. Vector mechanical equilibrium**:

$$\nabla \cdot \boldsymbol{\sigma} + \mathbf{f} = \mathbf{0}$$

with $\boldsymbol{\sigma} = \mathbf{C} : \boldsymbol{\varepsilon} - b (S_l p_l + S_i p_i) \mathbf{I} - 3K \alpha_s (T - T_0) \mathbf{I}$, where $\mathbf{C}$ is the elastic stiffness tensor. The presence of ice increases the apparent pressure experienced by the matrix (poroelasticity).

**2. Total water mass balance**:

$$\frac{\partial}{\partial t}(\phi S_l \rho_l + \phi S_i \rho_i) + \nabla \cdot \mathbf{W}_l = 0$$

**3. Salt mass balance**:

$$\frac{\partial}{\partial t}(\phi S_l \rho_l c_s) + \nabla \cdot \mathbf{W}_{\text{salts}} = 0$$

**4. Entropy balance (Heat)**:

$$\frac{\partial S_{\text{tot}}}{\partial t} + \nabla \cdot \left( \frac{\mathbf{q}}{T} + s_l \mathbf{W}_l \right) = 0$$

### 4.2 Flux laws

They are natively vectorized for 3D:
- $\mathbf{W}_l$: Darcy flux affected by relative permeability (Mualem).
- $\mathbf{q}$: Fourier thermal conductivity flux.
- $\mathbf{W}_{\text{salts}}$: Multi-component pure advection and Fick diffusion transport. Air is excluded from the model.

### 4.3 Ice-liquid thermodynamic equilibrium
The coexistence of fluid/solid phases at pore scales depends on the chemical balance $\mu_{l}(p_l, T, c_s) = \mu_{i}(p_i, T)$. The lowering of the freezing point by the osmotic effect of sodium chloride is reflected linearly or logarithmically on the water activity.

---

## 5. Boundary and initial conditions

Boundary prescriptions typically include mechanical clamping (nodes fixed at $u=0$) or stress imposition. Thermohydric fluxes can be restricted or supplied via Dirichlet conditions on `tem` or hydraulic pressure.

---

## 6. Test case: freezing in spherical geometry

The `Frostaco3d/` directory contains the file (`Frostaco3d`) testing the application of a thermo-saline cycle to a reservoir wall modeled over a reduced spherical prism.

### Simulation parameters
| Parameter | Value |
|-----------|-------|
| Geometry | `1 sphe` (1D probe formulated in spherical coordinates). Domain of 5 mm thickness. |
| Mesh | 100 cells |
| Properties | Internal Mori-Tanaka algorithm to find limiting `Young` and `Poisson` from $K_s=31.8$ GPa, $G_s=19.1$ GPa, and Porosity = 0.2. |
| Loading | Application of a concentrated salt increase up to $1000 \text{ mol/m}^3$ (advective thermal front going from 273K to 323K). An imposed tangential displacement is allowed via $u_1$. |

### Test results and physical comments

A simulation on the spherical geometric sample `Frostaco3d` illustrates the fluid-structure interaction under a thermohydric shock:

1. **Thermo-saline cycle (warming and salt shock)**: Through boundary 3 (exterior), temperature increases (from 273 K to 323 K) and the base is bathed by highly saline water that diffuses into the material by Fick's law.
2. **Spherical dilation and displacement**:
   - The heat rise initiates deep thawing and full aqueous re-saturation of the medium, relaxing the initially frozen internal macroscopic pressures $p_i$.
   - The Biot properties (elasticity) converted by homogenization drive the mechanical equation via a non-negative imposed displacement of $u_1 = - b (S_l p_l) + \alpha \Delta T$.
3. **Node tracking (.t)**: The module records in real time the salt distribution $c_s$ and the solid radial movement $u_x$. The salt peak tends to chemically deform the system (apparent osmotic pressure at the crumbling limit). The correlation $u(x, t)$ shows the very fast reactivity of elastic deformation against the slow diffusion of the temperature front toward the center of the spherical nodule.

---

## 7. Numerical discretization

Unlike 1D Frostaco, this model relies entirely on **Finite Element (FEM)** approaches via the `FEM.h` and `Elasticity.h` libraries. Hooke's law is encapsulated in the local elasticity tensor, and computation proceeds through quadrature integration points on the mesh (handling any brick: hexahedron, tetrahedron). The resolution remains nonlinear (Newton-Raphson).

---

## 8. Step-by-step file description

### 8.1 Driver file `Frostaco3d`

1. **Geometry & Mesh**: The scalar parameter `1 sphe` forces the internal Finite Element solver to consider a radial projection (axial spherical symmetry in 1D) on a segment going from radius 0 to 5 mm (step `5.e-5` m over 100 elements).
2. **Material (`Model = Frostaco3d`)**: Integrates mechanical traits ($k_s, g_s$) and porosity. If global Hooke data (`Young`, `Poisson`) are not provided, *Bil* runs an automatic (*Mori-Tanaka*) on-the-fly homogenization calibration.
3. **Fields and Initialization**: The medium starts uniformly at local freezing conditions via fields (e.g., $T=273$ K). The system rests in its pre-existing poroelastic equilibrium.
4. **Functions & Boundary Conditions**: The fixed condition on the sample intensively solicits the domain base (`Region = 3`) with a cyclic load over ten time units (salt from 0 to $1000$ mol/m³, and temperature sweep from 273 K to 323 K). The other end manages a mechanical anchoring condition `Unknown = u_1` forced to the value of the base function (0).
5. **Objective Variations**: The algorithm includes the additional tolerance criterion `u_1 = 1.e-4` to guarantee the stability of the elastic iteration against rapid thermohydric variations.

### 8.2 Model file `src/Models/ModelFiles/Frostaco3d.c`

1. **Declarations and Mechanical Initialization (Lines 1–550)**:
   - Configures a generic dimensional solver: $3 + \text{dim}$ equations.
   - In `ReadMatProp()`, generates the limiting poroelastic data targets via `Elasticity_ComputeStiffnessTensor()` which converts a pair (E, $\nu$) into a Hooke tensor (rank 4).
2. **Integral evaluation (`ComputeVariables`, `ComputeInitialState`)**:
   - Switches to Finite Element logic: derivatives are performed at `IntPoints` (the Gauss-Legendre quadrature points of the 2D/3D mesh). The expansion manages additional extensive variables computed via the gradient operator at this point (including the volumetric module `I_SIG` equal to the matrix trace).
3. **Stiffness Matrix (Lines 1620+)**:
   - `ComputeTangentCoefficients` specifically adds all cross-derivatives. It derives the poromechanical laws by $10^{-4}$ to find $\partial \sigma / \partial p_l$, or the hydraulic sensitivities with respect to skeleton deformability ($\partial S_l / \partial u_x$).
4. **Residual (`ComputeResidu`)**:
   - Combines two building blocks: the call to the standard finite element function `FEM_ComputeMassAndFluxResidu` for water and heat propagation, and `FEM_ComputeSolidDivergenceResidu` which formally evaluates the scalar Cauchy equilibrium (zero divergence of stresses + body forces) on the structural mass.

---

## 9. Bibliographic references

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons. — Generalized mechanical modeling of fluids confined by deformable matrices.
- Numerical tools related to *Bil* porting: *Tahiri & Dangla*, Université Gustave Eiffel.
- **Mori, T., & Tanaka, K.** (1973). Energy evaluation model for multi-sphere materials.
