# M10 Model — Richards Equation: gravity drainage of a bead column (1D)

> **Source files:**
> `src/Models/ModelFiles/M10.c` · `test_examples/m10-1/m10-1` · `test_examples/m10-1/billes`
>
> **Bil model author:** P. Dangla (Université Gustave Eiffel)

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equation](#41-conservation-equation)
   - 4.2 [Darcy flux law](#42-darcy-flux-law)
   - 4.3 [Retention curve and relative permeability](#43-retention-curve-and-relative-permeability)
   - 4.4 [Condensed form — Richards equation](#44-condensed-form--richards-equation)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: bead column under gravity drainage (`test_examples/m10-1`)](#6-test-case-bead-column-under-gravity-drainage)
7. [Results](#7-results)
8. [Numerical discretization](#8-numerical-discretization)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The M10 model solves the **Richards equation** for single-phase liquid water flow in a partially saturated porous medium. The gas phase pressure is assumed uniform and constant; only the **liquid pressure** $p_l$ is an unknown of the problem.

The test case `test_examples/m10-1` simulates the **gravitational drainage of a vertical glass bead column**. This is a classic validation case for the Richards equation implementation: the medium (glass beads) has a well-characterized retention curve with a low air-entry pressure (~500 Pa) and rapid desaturation beyond this threshold. This type of experiment is used to calibrate unsaturated porous medium transfer codes.

The domain is a vertical one-dimensional column of length $L = 0.2$ m, composed of a single homogeneous material:

| Parameter | Value |
|-----------|-------|
| Porosity $\phi$ | 0.38 |
| Intrinsic permeability $k_\text{int}$ | $8.9 \times 10^{-12}$ m² |

---

## 2. Assumptions

1. **Single-phase liquid**: only the liquid phase (water) is mobile. The gas phase is assumed at constant pressure $p_g = 10^5$ Pa (connection to atmosphere).
2. **Isothermal**: temperature is uniform and constant; viscosity and density are constants.
3. **Rigid porosity**: the solid skeleton is non-deformable; $\phi = \text{const}$.
4. **Generalized Darcy's law** with gravity for liquid phase transport.
5. **Local capillary equilibrium**: saturation $s_l$ is a single-valued function of capillary pressure $p_c = p_g - p_l$.
6. **Gravity**: $g = -9.81$ m/s² oriented in the decreasing $x$ direction ($x$ axis vertical upward).

---

## 3. Variables and notation

### Primary unknown

| Symbol | Meaning | Unit |
|---------|---------|------|
| $p_l$ | Liquid phase pressure | Pa |

Capillary pressure derives directly:

$$p_c = p_g - p_l$$

### Secondary variables

| Symbol | Meaning |
|---------|---------|
| $s_l(p_c)$ | Liquid water saturation degree |
| $k_{rl}(p_c)$ | Water relative permeability |
| $K_l$ | Effective hydraulic conductivity |
| $m_l$ | Mass water content |
| $W_l$ | Liquid mass flux |

### Physicochemical constants

| Symbol | Value | Meaning |
|---------|-------|---------|
| $\rho_l$ | 1000 kg/m³ | Liquid water density |
| $\mu_l$ | $10^{-3}$ Pa·s | Dynamic water viscosity |
| $k_\text{int}$ | $8.9 \times 10^{-12}$ m² | Intrinsic permeability |
| $\phi$ | 0.38 | Porosity |
| $p_g$ | $10^5$ Pa | Gas pressure (constant, atmospheric) |
| $g$ | $-9.81$ m/s² | Gravitational acceleration |

---

## 4. Mathematical model

### 4.1 Conservation equation

The system consists of a **single liquid water mass balance equation**, integrated over the representative elementary volume (REV):

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{W}_l = 0$$

with the mass water content:

$$m_l = \rho_l\,\phi\,s_l(p_c)$$

### 4.2 Darcy flux law

The liquid mass flux $\mathbf{W}_l$ is given by the generalized Darcy's law with gravity:

$$\mathbf{W}_l = -K_l\,\left(\nabla p_l - \rho_l\,\mathbf{g}\right)$$

where the effective hydraulic conductivity is:

$$K_l = \frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}$$

In 1D with $g$ along the $x$ axis (positive upward):

$$W_l = -K_l\,\left(\frac{\partial p_l}{\partial x} - \rho_l\,g\right) = -K_l\,\left(\frac{\partial p_l}{\partial x} + \rho_l\,|g|\right)$$

> **Hydrostatic equilibrium:** at equilibrium ($W_l = 0$): $\partial p_l/\partial x = -\rho_l\,|g| \approx -9810$ Pa/m. Pressure decreases linearly with height.

### 4.3 Retention curve and relative permeability

The functions $s_l(p_c)$ and $k_{rl}(p_c)$ are provided in **tabular form** in the `billes` file. This file contains 2000 points covering the range $p_c \in [500,\ 1000]$ Pa, with columns:

```
p_c [Pa]    s_l [-]    k_rl [-]
```

The key characteristics of glass beads are:

| Quantity | Value | Description |
|----------|-------|-------------|
| $p_{c,\text{entry}}$ | ≈ 500 Pa | Air-entry pressure |
| $s_{l,\text{max}}$ | 1.0 | Maximum saturation ($p_c < 500$ Pa) |
| $s_{l,\text{residual}}$ | ≈ 0.09 | Residual saturation ($p_c = 1000$ Pa) |
| $k_{rl}(s_l = 1)$ | 1.0 | Saturated relative permeability |
| $k_{rl}(s_l = 0.09)$ | ≈ $2.6 \times 10^{-8}$ | Residual relative permeability |

The curve is **very steep** (near-total desaturation over a 500 Pa interval), characteristic of a medium with uniform pore size.

### 4.4 Condensed form — Richards equation

Substituting the previous expressions and using $\partial m_l/\partial t = -\rho_l\,\phi\,(\partial s_l/\partial p_c)\,(\partial p_l/\partial t)$, the standard form of the **Richards equation** is obtained:

$$\boxed{-\rho_l\,\phi\,\frac{\partial s_l}{\partial p_c}\,\frac{\partial p_l}{\partial t} - \nabla \cdot \left[\frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}\left(\nabla p_l - \rho_l\,\mathbf{g}\right)\right] = 0}$$

The coefficient $C(p_c) = -\rho_l\,\phi\,\partial s_l/\partial p_c \geq 0$ is the **capillary capacity**; it appears in the mass matrix of the discrete problem.

---

## 5. Boundary and initial conditions

### Initial conditions

The initial pressure is given by a linear field (sub-hydrostatic, column initially near full saturation):

$$p_l(x,\,t=0) = p_{l0} + G\,x, \qquad p_{l0} = 10^5 \text{ Pa},\quad G = -3400 \text{ Pa/m}$$

| Boundary | $x$ [m] | $p_l(t=0)$ [Pa] | $p_c(t=0)$ [Pa] | $s_l(t=0)$ [-] |
|------|---------|-----------------|-----------------|----------------|
| Bottom | 0 | $10^5$ | 0 | 1.000 |
| Top | 0.2 | $9.932 \times 10^4$ | 680 | ≈ 0.9998 |

Initial capillary pressure is everywhere below the critical value (≈ 680 Pa at the top), and saturation is near-total ($s_l \approx 1$) throughout the column.

> **Note:** the initial gradient $G = -3400$ Pa/m is **smaller in absolute value** than the hydrostatic gradient $-9810$ Pa/m. The column is not in equilibrium; the initial flux is non-zero and directed downward.

### Boundary conditions

| Boundary | $x$ [m] | Type | Value |
|------|---------|------|-------|
| Bottom | 0 | Dirichlet | $p_l = 10^5$ Pa (atmospheric pressure, saturated base) |
| Top | 0.2 | Natural Neumann | $\mathbf{W}_l \cdot \mathbf{n} = 0$ (free drainage, no imposed flux) |

The bottom boundary is maintained at atmospheric pressure (submerged base or pressure-controlled drainage). The top boundary is free: this is the natural Neumann condition of the variational problem, which physically corresponds to an open top without incoming flux (water can only drain downward by gravity).

---

## 6. Test case: bead column under gravity drainage

### Simulation parameters

| Parameter | Value |
|-----------|-------|
| Column length $L$ | 0.2 m |
| Number of elements | 100 (non-uniform mesh, refined at $x=0$) |
| Total duration | 300 s |
| Output times | $t = 0$ s, $t = 120$ s, $t = 300$ s |
| Initial time step $\Delta t_\text{ini}$ | 0.01 s |
| Maximum time step $\Delta t_\text{max}$ | 1000 s |
| Objective variation (OBJE) | $\Delta p_l = 10$ Pa |
| Newton tolerance | $10^{-10}$ |
| Max iterations | 10 |

### Expected physics

The initial state (sub-hydrostatic gradient) creates a **disequilibrium**: gravitational force dominates the pressure gradient, and water flows downward. The system evolves toward **hydrostatic equilibrium** ($\nabla p_l = \rho_l\,\mathbf{g}$, i.e., $-9810$ Pa/m), which implies significant desaturation at the top of the column.

The physical sequence is:

1. **Immediate drainage**: from $t=0$, a downward flux is established ($W_l \approx -5.7 \times 10^{-2}$ kg/(m²·s) at the base).
2. **Desaturation front**: capillary pressure at the top increases beyond the entry pressure ($p_c > 500$ Pa); saturation rapidly drops from 1 to a residual value (~0.09).
3. **Front progression**: the desaturation front descends from $x = 0.2$ m toward $x = 0$ as pressure redistributes.
4. **Flux reduction**: the relative permeability $k_{rl}$ decreases strongly in the desaturated zone, slowing drainage.

![Pressure and saturation profiles during gravity drainage (M10-1)](M10-1_results.png)

---

## 7. Results

The three output times illustrate the different stages of drainage.

### Initial state ($t = 0$ s)

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] |
|---------|-----------|-------------------|-----------| 
| 0 | $1.000 \times 10^5$ | $-5.705 \times 10^{-2}$ | 1.000 |
| 0.1 | $\approx 9.966 \times 10^4$ | $-5.704 \times 10^{-2}$ | 1.000 |
| 0.2 | $9.932 \times 10^4$ | $-5.702 \times 10^{-2}$ | 0.9998 |

The flux is **nearly uniform** throughout the column: saturation is total and hydraulic conductivity is maximum.

### State at $t = 120$ s

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] |
|---------|-----------|-------------------|-----------| 
| 0 | $1.000 \times 10^5$ | $-4.782 \times 10^{-2}$ | 1.000 |
| 0.19 | $\approx 9.919 \times 10^4$ | $-2.86 \times 10^{-4}$ | 0.220 |
| 0.2 | $9.918 \times 10^4$ | $-2.86 \times 10^{-4}$ | 0.154 |

The **desaturation front** has reached $x \approx 0.15$–$0.19$ m. Flux is strongly reduced in the desaturated zone (ratio ~200 between bottom and top column flux).

### State at $t = 300$ s

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] |
|---------|-----------|-------------------|-----------| 
| 0 | $1.000 \times 10^5$ | $-3.955 \times 10^{-2}$ | 1.000 |
| 0.19 | $\approx 9.918 \times 10^4$ | $-5.48 \times 10^{-5}$ | 0.147 |
| 0.2 | $9.916 \times 10^4$ | $-5.48 \times 10^{-5}$ | 0.115 |

The front continues to move downward. The column top is strongly desaturated ($s_l \approx 0.11$–0.15) with near-zero flux. The pressure profile tends toward hydrostatic equilibrium ($dp_l/dx \to -9810$ Pa/m).

---

## 8. Numerical discretization

The model is discretized by the **finite element method (FEM)** as implemented in Bil via `FEM.h`. The linearized system matrix consists of:

- a **mass matrix** (accumulation terms), with coefficient $C(p_c) = -\rho_l\,\phi\,\partial s_l/\partial p_c$ evaluated at the current time step;
- a **conductance matrix** (flux terms), with tensor $K_l\,\mathbf{I}$ evaluated explicitly at the previous time step (semi-implicit scheme).

The nonlinear system is solved at each time step by the **Newton–Raphson method**.

The time step is adaptive, controlled by the maximum variation of the unknown $p_l$ (parameter **OBJE** = 10 Pa in Bil). The capillary capacity $C(p_c)$ can be zero in the saturated zone ($\partial s_l/\partial p_c = 0$ for $p_c < p_{c,\text{entry}}$): in this regime, the equation is **elliptic** (local steady state), which can cause numerical oscillations if $\Delta t$ is too large or the mesh too coarse.

> **Note on regularization:** unlike M5, M10 does not implement explicit saturation regularization at $p_c = 0$. The `billes` file directly provides $s_l = 1$ for $p_c \leq 500$ Pa (constant extension), which is sufficient to avoid degeneracy in this case.

---

## 9. Bibliographic references

### Richards equation and unsaturated flow

- **Richards, L. A.** (1931). Capillary conduction of liquids through porous mediums. *Physics*, 1(5), 318–333. — Founding article of the equation bearing his name, describing capillary flow in soils.
- **Bear, J.** (1972). *Dynamics of Fluids in Porous Media*. Elsevier, New York.
- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons, Chichester.

### Capillary retention curves

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898.
- **Brooks, R. H. & Corey, A. T.** (1964). *Hydraulic Properties of Porous Media*. Hydrology Paper 3, Colorado State University.

### Numerical methods for Richards equation

- **Celia, M. A., Bouloutas, E. T. & Zarba, R. L.** (1990). A general mass-conservative numerical solution for the unsaturated flow equation. *Water Resources Research*, 26(7), 1483–1496.
- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Source code: <https://github.com/Universite-Gustave-Eiffel/bil>
