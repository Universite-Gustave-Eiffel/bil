# Elast Model — Linear Elasticity

> **Bil model:** src/Models/ModelFiles/Elast.c`

> **Input file:** `doc/mkdocs/Mechanics/Elast/Elast0`

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and mathematical model](#3-variables-and-mathematical-model)
4. [Input file description](#4-input-file-description)
5. [Expected results](#5-expected-results)

---

## 1. Context and objective

The **Elast** model solves the **linear elasticity** problem end to end. It is one of the most basic cases in continuum mechanics, modeling — for small deformations — a solid that deforms in a completely reversible and instantaneous manner under forces, pressures, or geometric constraints.

The example `Elast0` illustrates the behavior of a **multilayer cylindrical** solid. To save computation time, no complex 3D mesh is used; instead, symmetry is exploited by declaring the mesh in a two-dimensional axisymmetric coordinate system (usually denoted $r, z$). The scenario objective is to simulate the equilibrium of a structure with alternating soft and stiff layers, loaded by internal pressure (pressurized tube).

---

## 2. Assumptions

1. **Linear stress-strain relationship** (isotropic Hooke's law): Each layer is characterized exactly by a single pair of elastic parameters: Young's modulus ($E$) and Poisson's ratio ($\nu$).
2. **Small perturbations**: The equation is written on the original geometry without accounting for large geometric distortions $\boldsymbol{\varepsilon} = \frac{1}{2}(\nabla \mathbf{u} + \nabla^T \mathbf{u})$.
3. **Quasi-static loading**: Inertia forces are ignored.
4. **Macroscopic composite material**: Mesh elements are distributed across various "layers" defined by laws with distinct values.

---

## 3. Variables and mathematical model

The implementation (visible in `src/Models/ModelFiles/Elast.c`) describes a simple system of equations whose count is dictated by the space dimension. In *2 axis* mode (radial and longitudinal), there are two momentum conservation equations called `meca_1` ($u_r$) and `meca_2` ($u_z$).

### Unknowns
| Symbol | Meaning |
|---------|---------|
| $\mathbf{u}$   | Nodal displacements (primary unknown of the equilibrium) |

### Static Equilibrium
Without coupled thermal or hydraulic processes, the condition is based on:
$\nabla \cdot \boldsymbol{\sigma} + \rho_s \mathbf{g} = \mathbf{0}$

### Constitutive law
In `Elast.c`, each integration accounts for the deviatoric and volumetric tensor built as:
$\boldsymbol{\sigma} = \boldsymbol{\sigma}_0 + \mathbb{C} : \boldsymbol{\varepsilon}$
Here, $\boldsymbol{\sigma}_0$ is an optional pre-stress tensor. $\mathbb{C}$ is the fourth-order stiffness tensor.

---

## 4. Input file description (`Elast0`)

The main file is an excellent showcase for composite condition parameterization in Bil. It manages several regions and applies a piecewise dynamic load.

1. **Geometry & Mesh**
   ```text
   Geometry
   2 axis
   Mesh
   cylinder0.msh
   ```
   Specifies material coordinates $r, z$, which modifies on the fly the finite element volume integration by injecting the revolution Jacobian $2\pi r$.

2. **Material definitions (Layers)**
   Where layers under initial compression are juxtaposed:
   ```text
   Material # Layer 1 (stiff)
   Modele = Elast
   rho_s = 0 
   sig0_11 = -1e6        # Pre-stress of -1 MPa
   ...
   young =  10.e+09      # Stiffness E = 10 GPa
   poisson = 0.26 
   
   # (Layer 2 is soft with young = 1 GPa, Layer 3 is stiff at 10 GPa)
   ```
   Three physical domains are defined. All start with a negative residual pressure (compression of -1 MPa in radial, hoop, and axial directions), but differ in stiffness, mimicking for example a hooped tube.

3. **Field and Function (Transient loading)**
   ```text
   Fields
   1
   Value = 1e6   Gradient = 0 0 0 Point = 0 0 0
   Functions
   1
   N = 3  F(0) = 1  F(1) = 10 F(2) = 0.1
   ```
   - Field 1 models a base of 1 MPa.
   - The function dictates the loading history: at time 0 we start at $100\%$ of the field, at time 1 we peak at $10\times$ the nominal load (10 MPa), and at time 2 we drop to a mere $10\%$.

4. **Boundary Conditions (Displacements & Forces)**
   ```text
   Boundary Conditions
   1
   Region = 80 Unknown = u_2   Field = 0 Function = 0
   ```
   A section of the part identified as region 80 has its longitudinal movement blocked and set to zero (`u_2 = 0`). It is clamped.

   ```text
   Loads
   7
   Region = 10 Equation = meca_1 Type = pressure Field = 1 Function = 1
   Region = 20 Equation = meca_1 Type = pressure Field = 1 Function = 1
   Region = 30 Equation = meca_1 Type = pressure Field = 1 Function = 1
   ...
   ```
   The nodes on the walls of regions 10, 20, and 30 are subjected to the `pressure` load undergoing the cyclic variation `Function 1` (from 1 to 10 then 0.1 MPa) along the tube faces (possibly the radial interior of different sections).

5. **Time solver and Linear procedure**
   - `Dates: 0 1 2` requests output at the function boundaries.
   - `Iterative Process: Iterations = 1`. This is crucial: with non-evolutionary mechanical models (no degradation, no nonlinear plasticity, small deformations), no Newton method is needed. Direct matrix inversion $\mathbf{K} \cdot \mathbf{u} = \mathbf{F}$ is exact after the first pass; the tolerance serves as a verification margin.

---

## 5. Expected results

This model classically renders the stress/strain evolution for composite or meshed parts:
- The multilayer geometry creates abrupt **stress** jumps between soft (Layer 2) and stiff (Layer 1 and 3) layers.
- However, **displacement** vectors remain guaranteed continuous along the interface, as finite elements bind these geometric integrity limits together.
- At each target time — for example at $t=1$ the maximum tubular expansion effect due to very high pressure occurs, the wall expands radially along $u_1$. At $t=2$, the structure contracts and, depending on topology, may even fold under the effect of imposed pre-stresses (`sig0_ij = -1 MPa`), all in the strict absence of irreversible material softening.

![Elast - Simulation Results](Elast/Elast_results.png)

---

## 6. Bibliographic references

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*.
- **O.C. Zienkiewicz, R.L. Taylor, J.Z. Zhu** — *The Finite Element Method: Its Basis and Fundamentals*. (For general mathematical modeling by elastic finite elements and axisymmetric Jacobian integration.)
- **Hooke, R.** — Generalized *Hooke's Law* describing the pseudo-linear response of the isotropic elasticity tensor ($\mathbb{C}$).
