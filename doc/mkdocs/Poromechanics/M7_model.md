# M7 Model — Saturated/Unsaturated Poroelasticity (Biot Consolidation)

> **Bil model:** `src/Models/ModelFiles/M7.c`

> **Input file:** `doc/mkdocs/Poromechanics/M7/M7`
>
> **Model authors:** P. Dangla (Université Gustave Eiffel)

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Equilibrium and conservation equations](#41-equilibrium-and-conservation-equations)
   - 4.2 [Poroelastic constitutive laws](#42-poroelastic-constitutive-laws)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: Terzaghi consolidation of a soil layer (`test_examples/M7`)](#6-test-case-terzaghi-consolidation-of-a-soil-layer)
7. [Model material parameterization](#7-model-material-parameterization)
8. [Step-by-step file description](#8-step-by-step-file-description)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The **M7** model solves the coupled equations of **Biot poroelasticity**. It combines continuum mechanics (deformation of the solid skeleton) with subsurface hydraulics (fluid flow, typically water, governed by Darcy's law). It is a fundamental model in geomechanics, used to compute **soil settlement** (consolidation over time) under the weight of structures (foundations, embankments), as well as the associated dissipation of pore pressure.

The Bil "Saturated/Unsaturated" implementation extends the classical Biot theory to account for unsaturated states, where the presence of air reduces water permeability and modifies the notion of "effective stress" via an equivalent pore pressure $\pi$.

---

## 2. Assumptions

1. **Two-phase/single-phase generalization**: Model designed for use in fully saturated mode (no air, $S_l = 1$) or unsaturated mode (gas pressure $p_g$ assumed purely atmospheric and instantaneously equilibrated).
2. **Linear isotropic elastic skeleton**: The intrinsic mechanical behavior of the empty solid matrix obeys Hooke's law (Young's modulus $E$, Poisson's ratio $\nu$).
3. **Validity of Biot relations**: The total stress tensor decomposes into effective stresses and equivalent pore pressure via the Biot coefficient $b$.
4. **Small strain kinematics**: Advection of the solid skeleton (porous walls) is not tracked in the Eulerian frame.

---

## 3. Variables and notation

Coupled hydro-mechanical model, supporting $1 + \text{Dimension}$ equations (one for hydraulics, and a vector for mechanics).

### Primary unknowns

| Symbol | Meaning | BIL internal |
|---------|---------|-------------|
| $p_l$ | Liquid pressure | `p_l` |
| $\mathbf{u}$ | Solid displacement vector | `u_1, u_2, u_3` |

### Behavior variables

| Symbol | Meaning |
|---------|---------|
| $\boldsymbol{\varepsilon}$ | Linearized strain tensor $\frac{1}{2}(\nabla \mathbf{u} + \nabla^T \mathbf{u})$ |
| $\boldsymbol{\sigma}$ | Total stress tensor |
| $\pi$ or $p_p$ | Equivalent pore pressure (relevant for unsaturated soil) |
| $\phi$ | Instantaneous porosity of the medium (modified by deformation) |

---

## 4. Mathematical model

Resolution is based on a semi-implicit principle of nodal residual minimization by Newton-Raphson method.

### 4.1 Equilibrium and conservation equations

1. **Momentum balance** (stationary):

$$\nabla \cdot \boldsymbol{\sigma} + (\rho_s + m_l) \mathbf{g} = \mathbf{0}$$

*(Dynamic inertia is zero — quasi-static. The body force includes the dry soil mass $\rho_s$ and pore water mass $m_l$.)*

2. **Hydraulic diffusion equation** (water mass):

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{W}_l = 0$$

### 4.2 Poroelastic constitutive laws

- **Hooke/Biot stress law**:

$$\boldsymbol{\sigma} = \boldsymbol{\sigma}_0 + \lambda \, \text{Tr}(\boldsymbol{\varepsilon}) \mathbf{I} + 2\mu \boldsymbol{\varepsilon} - b \, (p_p - p_{p0}) \mathbf{I}$$

*(Where $\lambda$ and $\mu$ are the Lamé parameters of the solid medium, and $b$ is the Biot coefficient coupling the pressure contribution.)*

- **Porosity evolution**:

$$\phi = \phi_0 + b \, \text{Tr}(\boldsymbol{\varepsilon}) + N \, (p_p - p_{p0})$$

*($N \ge 0$ optional parameter representing intrinsic pore compressibility.)*

- **Flux and accumulation**:

$$\mathbf{W}_l = - \frac{\rho_l k_{\text{int}} k_{rl}(p_c)}{\mu_l} \nabla \left( p_{l} \right) + \frac{\rho^2_l k_{\text{int}} k_{rl}}{\mu_l} \mathbf{g}$$

$$m_l = \rho_l \, \phi \, S_l(p_c)$$

---

## 5. Boundary and initial conditions

Boundary conditions cover both physical phenomena:

- Mechanical (`Reg X Inc = u_y Field = z`): Typically used to create blocked boundaries (rigid rock substratum, rollers at boundaries).
- Mechanical (Force) (`Load Equation = meca_1 = force`): Directly represents an imposed force (e.g., embankment placed on soil).
- Hydraulic: Draining surfaces ($p_l = P_{atmos}$) or impermeable faces (no prescription, so implicitly zero flux via Neumann).

---

## 6. Test case: Terzaghi consolidation of a soil layer (`test_examples/M7/`)

This academic case is the one-dimensional **Terzaghi Consolidation** problem. A 1D column of saturated soil over $H=1\text{m}$ (coordinates from $z=-1$ to $z=0$) is considered.
- At $t=0$, a continuous surface load (-1 Pa) is suddenly applied to the geometric top, which is free to drain water ($p_l=0$).
- The base does not move mechanically (`u=0` imposed at the bottom).

### Simulation results

The soil, with low permeability and saturated with nearly incompressible water, sees its matrix contract immediately but the water opposing settlement instantly captures 100% of the load: this is the peak *"excess pore pressure"*. Then water slowly dissipates toward the draining surface at $t = 0.01$, then $t = 0.1$.
This transfer of total stress to soil grains causes "settlement" (deformation $\Delta u_z$).

![Terzaghi Consolidation - M7 Simulation](M7/M7-1/M7_results.png)

*(Left: Gradual isochronal decay/dissipation of fluid pressure. Right: Progressive increase of vertical settlement of geological horizons as complete drainage occurs.)*

---

## 7. Model material parameterization

The model physics combines standard mechanical data with the hydraulic model:

| Parameter | `M7` file case | Physical role |
|-----------|----------------|---------------|
| `young`, `poisson` | 0.83, 0.25 | Represents the rigid matrix stiffness of dry behavior. |
| `rho_s` | 0 | Grain mass zeroed to remove self-weight confinement. |
| `b` | 1 | Biot coefficient (often = 1 for Terzaghi due to intrinsic incompressibility of solid grains). |
| `N` | 0 | Void compressibility. |
| `sig0_xx` | 0 | Pre-established macroscopic total stresses in situ. |

---

## 8. Step-by-step file description

### 8.1 Control file `test_examples/M7/M7`

1. **Geometry & Mesh**: 1D requirements (one segment) located between $z=-1$ and $z=0$ (`4 -1 -1 0 0`). The virtual thickness of `1` keeps the volumetric case in standard volumes.
2. **Initial properties**: `p_l0 = 0` in the material block sets the starting pressure to equilibrium (zero isobar, no gradient, complete rest).
3. **Boundary Conditions**:
   - `Reg 1 Inc = u_1`: The bottom ($z=-1$) is fixed. Zero displacement.
   - `Reg 3 Inc = p_l`: The top ($z=0$) is a pure seeping surface ($p=0$, atmospheric).
4. **Loads**: `Reg 3 Equation = meca_1 Type = force Field = 1` with `Fields 1` defined as `Value = -1`. This inserts a load oriented toward negative x (-1) applicable to all boundary elements of region 3.
5. **Criteria and time (`Dates`, `Iterative Process`)**: Tight tolerance `1.e-6` because loads initially generate large shocks for nonlinear solvers. Dates set to `0.01` then `0.1`.

### 8.2 Internal model `src/Models/ModelFiles/M7.c`

1. **`ComputeInitialState` (Line 201)**: Calls methods specific to Finite Elements (`FEM.h` instead of `FVM.h` to ensure nodal mechanical tracking rather than interface surface tracking). Computes base constants (Lamé, shear modulus).
2. **Stress `SIG` and body force `F_MASS` computation**: Performs the tensorial abstraction (spatial derivative) of displacements at Gauss points ($p=0..N_{integ}$). `sig[...] += lame*tre - b*(pp - pp0)` — adds shape energy and subtracts Biot potential.
3. **Stiffness `c7` and transport `k7` assemblies**: Analytically prepares, on the integration matrix cells (`kp` and `kc`), the relative influence between the water variable `I_p_l` and the kinematic components of `I_u`.
4. **`ComputeResidu`**: Builds the virtual mechanical balance using `FEM_ComputeStrainWorkResidu` (virtual force balance against internal stiffness $\int \boldsymbol{\sigma} \colon \nabla \delta \mathbf{u} \, d\Omega$). For E_liq, it is rather the error on porous compression and the balance of transient hydraulic fluxes.

---

## 9. Bibliographic references

- **Biot, M. A.** (1941). General theory of three-dimensional consolidation. *Journal of applied physics*, 12(2), 155-164.
- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons.
- **Terzaghi, K.** (1943). *Theoretical soil mechanics*.
