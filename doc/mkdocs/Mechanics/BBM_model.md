# BBM Model — Barcelona Basic Model for Unsaturated Soils (2D Axisymmetric)

> **Bil model:** `src/Models/ModelFiles/BBM.c` · `src/Models/ConstitutiveLaws/PlasticityModels/PlasticityBBM.c`
>
> **Input file:** `doc/mkdocs/Mechanics/BBM/BBM` · `doc/mkdocs/Mechanics/BBM/BBM2` · `doc/mkdocs/Mechanics/BBM/BBM_pcst`
>
> **Bil model authors:** Eizaguirre, Dangla (Université Gustave Eiffel)

---

## Table of Contents

1. [Context and Objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and Notation](#3-variables-and-notation)
4. [Mathematical Model](#4-mathematical-model)
   - 4.1 [Conservation Equations](#41-conservation-equations)
   - 4.2 [Nonlinear Elastic Constitutive Law](#42-nonlinear-elastic-constitutive-law)
   - 4.3 [Yield Surface and LC Curve](#43-yield-surface-and-lc-curve)
   - 4.4 [Flow Rule and Hardening](#44-flow-rule-and-hardening)
   - 4.5 [Hydromechanical Coupling — Darcy Flow](#45-hydromechanical-coupling--darcy-flow)
   - 4.6 [Capillary Retention Curves](#46-capillary-retention-curves)
5. [Boundary and Initial Conditions](#5-boundary-and-initial-conditions)
6. [Detailed Description of Input Files](#6-detailed-description-of-input-files)
   - 6.1 [Main File `BBM` — Isotropic Compression with Suction Cycles](#61-main-file-bbm--isotropic-compression-with-suction-cycles)
   - 6.2 [File `BBM2` — p-q Stress Path with Variable Suction](#62-file-bbm2--p-q-stress-path-with-variable-suction)
   - 6.3 [File `BBM_pcst` — Constant Suction](#63-file-bbm_pcst--constant-suction)
   - 6.4 [Material Curves: `wrc2`, `krc2`, `lc`](#64-material-curves-wrc2-krc2-lc)
   - 6.5 [Mesh `carre.msh` and Geometry `carre.geo`](#65-mesh-carremsh-and-geometry-carregeo)
7. [Results](#7-results)
8. [Numerical Discretization](#8-numerical-discretization)
9. [References](#9-references)

---

## 1. Context and Objective

The BBM model implements the **Barcelona Basic Model** (BBM), proposed by Alonso, Gens, and Josa (1990), which is the reference model for unsaturated soil mechanics. It extends the Modified Cam-Clay model to unsaturated states by introducing **suction** $s = p_g - p_l$ as an additional state variable, which modulates the plastic envelope through the Loading Collapse (LC) curve.

The model is **hydromechanically coupled**: deformations of the solid skeleton modify the porosity and thus the water storage, while suction affects stiffness and strength. Water flow follows the generalized Darcy's law.

The three test cases illustrate **drained condition tests** (suction is imposed directly) on an axisymmetric cubic representative volume element (RVE):

| File | Test | Objective |
|------|------|-----------|
| `BBM` | Isotropic compression with loading/unloading cycles, step-wise increasing suction | Verification of the LC curve and hardening |
| `BBM2` | p-q stress path (triaxial compression) with increasing suction | Verification of the yield surface in p-q space |
| `BBM_pcst` | Compression with shear, constant zero suction | Saturated behavior, comparison with Cam-Clay |

---

## 2. Assumptions

1. **Deformable porous medium**: elastoplastic solid skeleton, variable porosity.
2. **Two fluid phases**: liquid phase (water) and gas phase (at constant pressure $p_g = 0$, taken as reference).
3. **Small strains**: continuum mechanics framework with small strains (linearized strain tensor).
4. **Net stresses**: the mechanical behavior is formulated in terms of **net stresses** $\bar{\boldsymbol{\sigma}} = \boldsymbol{\sigma} + p_g\,\mathbf{I}$ and suction $s = p_g - p_l \geq 0$.
5. **Transverse isotropy** (axisymmetry): 2D problem with rotational symmetry.
6. **Drained conditions**: suction is imposed throughout the domain; intrinsic permeability is very low ($k_\text{int} = 10^{-20}$ m²) but the drained state is achieved through the boundary conditions.
7. **No gravity**: $g = 0$ in all three test cases.

---

## 3. Variables and Notation

### Primary Unknowns

| Symbol | Meaning | Unit |
|--------|---------|------|
| $p_l$ | Liquid phase pressure | Pa |
| $\mathbf{u} = (u_1,\,u_2)$ | Displacement vector | m |

Suction $s$ and net stress $\bar{\boldsymbol{\sigma}}$ follow from these:

$$s = p_g - p_l = -p_l \quad (\text{since } p_g = 0), \qquad \bar{\boldsymbol{\sigma}} = \boldsymbol{\sigma} + p_g\,\mathbf{I} = \boldsymbol{\sigma}$$

### Internal Variables (Stored at Integration Points)

| Symbol | Meaning |
|--------|---------|
| $\boldsymbol{\sigma}$ | Total stress tensor (9 components) |
| $\boldsymbol{\varepsilon}^p$ | Plastic strain tensor (9 components) |
| $m_l$ | Liquid mass content |
| $\mathbf{W}_l$ | Liquid mass flux |
| $p_{c0}$ | Pre-consolidation pressure at zero suction (hardening variable) |
| $\phi$ | Porosity |

### Material Parameters (test case `BBM`)

| Parameter | Symbol | Value | Meaning |
|-----------|--------|-------|---------|
| `slope_of_swelling_line` | $\kappa$ | 0.011 | Slope of the swelling line ($\ln p$-$e$ space) |
| `slope_of_virgin_consolidation_line` | $\lambda_0$ | 0.065 | Slope of the NCL at zero suction |
| `slope_of_critical_state_line` | $M$ | 1.2 | Slope of the CSL in $p$-$q$ space |
| `initial_pre-consolidation_pressure` | $p_{c0}$ | 40 kPa | Initial pre-consolidation pressure |
| `reference_consolidation_pressure` | $p_r$ | 10 kPa | Reference pressure for the LC curve |
| `kappa_s` | $\kappa_s$ | 0.005 | Elastic swelling index with respect to suction |
| `suction_cohesion_coefficient` | $k$ | 0.8 | Suction-induced cohesion increase coefficient |
| `poisson` | $\nu$ | 0.15 | Poisson's ratio |
| `initial_porosity` | $\phi_0$ | 0.25 | Initial porosity |
| `initial_stress` | $\boldsymbol{\sigma}_0$ | $-1000$ Pa (isotropic) | Initial stress state |
| `k_int` | $k_\text{int}$ | $10^{-20}$ m² | Intrinsic permeability |
| `mu_l` | $\mu_l$ | $10^{-3}$ Pa·s | Liquid viscosity |
| `rho_l` | $\rho_l$ | 1000 kg/m³ | Liquid density |
| `rho_s` | $\rho_s$ | 2000 kg/m³ | Solid particle density |

---

## 4. Mathematical Model

The BBM model solves a system of $1 + \dim$ coupled equations: 1 water mass conservation equation and $\dim$ mechanical equilibrium equations.

### 4.1 Conservation Equations

#### Liquid Water Mass Balance

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{W}_l = 0, \qquad m_l = \rho_l\,\phi\,s_l(s)$$

where $\phi = \phi_0 + \text{tr}\,\boldsymbol{\varepsilon}$ is the current porosity (Biot), $s_l(s)$ is the degree of saturation as a function of suction, and $\mathbf{W}_l$ is the Darcy flux (§4.5).

#### Mechanical Equilibrium (without gravity)

$$\nabla \cdot \boldsymbol{\sigma} = \mathbf{0}$$

i.e., in 2D axisymmetry:

$$\frac{\partial \sigma_{rr}}{\partial r} + \frac{\partial \sigma_{rz}}{\partial z} + \frac{\sigma_{rr} - \sigma_{\theta\theta}}{r} = 0$$

$$\frac{\partial \sigma_{rz}}{\partial r} + \frac{\partial \sigma_{zz}}{\partial z} + \frac{\sigma_{rz}}{r} = 0$$

### 4.2 Nonlinear Elastic Constitutive Law

The BBM elasticity is **nonlinear**: the bulk modulus depends on the stress state and suction.

**Bulk modulus:**

$$K = -\frac{(1 + e_0)\,\bar{p}}{\kappa + \kappa_s\,\Delta\ln(s + p_\text{atm})}$$

In the Bil implementation (in practice, for the current time steps), the simplified formulation is used:

$$K = -\frac{(1 + e_0)\,\bar{p}_n}{\kappa}, \qquad E = 3K(1 - 2\nu)$$

where $\bar{p}_n = (\sigma_{11}^n + \sigma_{22}^n + \sigma_{33}^n)/3 + p_g$ is the mean net pressure at the previous time step. Young's modulus $E$ and the shear modulus $\mu = E/[2(1+\nu)]$ are updated at each time step.

**Elastoplastic strain decomposition:**

$$\boldsymbol{\varepsilon} = \boldsymbol{\varepsilon}^e + \boldsymbol{\varepsilon}^p$$

**Elastic trial net stresses:**

$$\bar{\boldsymbol{\sigma}}^* = \bar{\boldsymbol{\sigma}}_n + \mathbb{C}^e : \Delta\boldsymbol{\varepsilon}$$

with the additional suction contribution:

$$\Delta\bar{\sigma}_m^s = -\bar{p}_n\,\frac{\kappa_s}{\kappa}\,\Delta\ln\!\left(\frac{s + p_\text{atm}}{s_n + p_\text{atm}}\right)$$

### 4.3 Yield Surface and LC Curve

The **BBM yield surface** (plasticity criterion) in net stress space is:

$$\boxed{f(\bar{p},\,q,\,s) = \frac{q^2}{M^2} + (\bar{p} - p_s)(\bar{p} + p_c(s)) = 0}$$

where:

| Term | Expression | Meaning |
|------|-----------|---------|
| $\bar{p}$ | $(\bar{\sigma}_{11} + \bar{\sigma}_{22} + \bar{\sigma}_{33})/3$ | Mean net pressure (negative in compression) |
| $q$ | $\sqrt{3J_2}$ | Deviatoric stress |
| $M$ | 1.2 | Slope of the CSL |
| $p_s(s)$ | $k \cdot s$ | Suction-induced cohesion increase |
| $p_c(s)$ | LC curve | Pre-consolidation pressure at suction $s$ |

**Loading Collapse (LC) Curve:**

The LC curve relates the pre-consolidation pressure at suction $s$ to that at zero suction $p_{c0}$ via:

$$\ln\frac{p_c(s)}{p_r} = \text{lc}(s)\,\ln\frac{p_{c0}}{p_r}$$

where the LC factor $\text{lc}(s)$ is:

$$\text{lc}(s) = \frac{\lambda_0 - \kappa}{\lambda(s) - \kappa}, \qquad \lambda(s) = \lambda_0\left[(1-r)\,e^{-\beta s} + r\right]$$

With the test case parameters: $\lambda_0 = 0.065$, $\kappa = 0.011$, $r = 0.75$, $\beta = 20 \times 10^{-6}$ Pa$^{-1}$.

This formula shows that:
- At $s = 0$: $\lambda(0) = \lambda_0$, $\text{lc}(0) = 1$, $p_c(0) = p_{c0}$ (saturated soil, standard Cam-Clay).
- At $s > 0$: $\lambda(s) < \lambda_0$, $\text{lc}(s) > 1$, $p_c(s) > p_{c0}$ — **suction strengthens the soil**, the plastic envelope expands.
- As $s \to \infty$: $\lambda(\infty) = \lambda_0 r = 0.049$, $\text{lc}(\infty) = (\lambda_0 - \kappa)/(\lambda_0 r - \kappa) \approx 1.43$ — asymptotic limit.

The **hardening variable** stored is $a = \ln(p_{c0})$, such that:

$$p_{c0} = e^a, \qquad a_0 = \ln(p_{c0,\text{init}}) = \ln(40000) \approx 10.597 \quad \text{(initial value observed in results)}$$

### 4.4 Flow Rule and Hardening

**Associated flow rule** (the plastic potential is identical to the yield surface):

$$\dot{\boldsymbol{\varepsilon}}^p = \dot{\lambda}\,\frac{\partial f}{\partial \bar{\boldsymbol{\sigma}}} = \dot{\lambda}\left[\frac{2\bar{p} + p_c - p_s}{3}\,\mathbf{I} + \frac{3}{M^2}\,\mathbf{s}\right]$$

**Isotropic hardening law** (isotropic consolidation):

The volumetric plastic strain $\dot{\varepsilon}^p_v = \text{tr}\,\dot{\boldsymbol{\varepsilon}}^p$ drives the evolution of pre-consolidation:

$$\dot{a} = \frac{\mathrm{d}\ln p_{c0}}{\mathrm{d}t} = h(\bar{p}, a)\,\dot{\lambda}, \qquad h = -\frac{1 + e_0}{\lambda_0 - \kappa}\,(2\bar{p} + p_c - p_s)$$

**Hardening modulus** (defined by $\mathrm{d}f = \frac{\partial f}{\partial \bar{\boldsymbol{\sigma}}}:\mathrm{d}\bar{\boldsymbol{\sigma}} - H\,\mathrm{d}\lambda = 0$):

$$H = \frac{1 + e_0}{\lambda_0 - \kappa}\,(\bar{p} - p_s)\,(2\bar{p} + p_c - p_s)\,p_c\,\text{lc}(s)$$

A positive modulus ($H > 0$) indicates **hardening** behavior (consolidation), typical of normally consolidated compression states.

### 4.5 Hydromechanical Coupling — Darcy Flow

The liquid mass flux is given by Darcy's law (without gravity in these test cases):

$$\mathbf{W}_l = -K_l\,\nabla p_l, \qquad K_l = \frac{\rho_l\,k_\text{int}\,k_{rl}(s)}{\mu_l}$$

The current porosity follows the volumetric strain evolution:

$$\phi = \phi_0 + \text{tr}\,\boldsymbol{\varepsilon}$$

### 4.6 Capillary Retention Curves

Two curves are used (defined analytically and tabulated in `wrc2`):

**Degree of saturation** (van Genuchten model):

$$s_l(s) = \left(1 + \left(\frac{s}{p_0}\right)^{\frac{1}{1-m}}\right)^{-m}, \qquad p_0 = 10^6 \text{ Pa},\quad m = 0.6$$

**Relative permeability** (constant in this test case):

$$k_{rl}(s) = 1 \quad \forall s$$

This simplification ($k_{rl} = 1$ and $k_g = 1$ in `krc2`) reflects the fact that the test cases are **suction-controlled** (drained conditions): flow plays no dynamic role; only the mechanical stresses and imposed suction govern the response.

---

## 5. Boundary and Initial Conditions

### Initial State (Common to All Three Test Cases)

The RVE is initially in a light isotropic state ($\sigma_0 = -1000$ Pa, well below the pre-consolidation pressure $p_{c0} = 40000$ Pa) and saturated ($s = 0$, $s_l = 1$, $p_l = 0$):

| Variable | Initial Value |
|----------|--------------|
| $\boldsymbol{\sigma}$ | $-1000$ Pa (isotropic) |
| $\mathbf{u}$ | $\mathbf{0}$ |
| $p_l$ | 0 Pa |
| $s = -p_l$ | 0 Pa |
| $s_l$ | 1.0 |
| $\phi$ | 0.25 |
| $p_{c0}$ | 40 kPa |
| $a = \ln(p_{c0})$ | $\ln(40000) \approx 10.597$ |

### Geometric Boundary Conditions (Common)

| Region | Line | Condition | Meaning |
|--------|------|-----------|---------|
| 14 | left ($r = 0$) | $u_1 = 0$ | Axis of symmetry (zero radial displacement) |
| 11 | bottom ($z = 0$) | $u_2 = 0$ | Fixed base (zero vertical displacement) |
| 100 | entire domain | $p_l$ imposed | Suction controlled via $p_l = -s$ |

---

## 6. Detailed Description of Input Files

### 6.1 Main File `BBM` — Isotropic Compression with Suction Cycles

```
Geometry
2 axis
```

**2D axisymmetric** problem (`axis`): $x$ is the radius $r$, $y$ is the height $z$. The geometry is rotationally symmetric about the $r = 0$ axis.

---

```
Mesh
carre.msh
```

Reference to the external mesh file `carre.msh` (1 m × 1 m square, single quadrilateral element). See §6.5.

---

```
Material
Model = BBM
gravity = 0
rho_s = 2000
slope_of_swelling_line = 0.011
slope_of_virgin_consolidation_line = 0.065
poisson = 0.15
slope_of_critical_state_line = 1.2
initial_pre-consolidation_pressure = 0.04e6
reference_consolidation_pressure = 0.01e6
kappa_s = 0.005
initial_stress_11 = -1000
initial_stress_22 = -1000
initial_stress_33 = -1000
initial_porosity = 0.25
rho_l = 1000
k_int = 1e-20
mu_l = 0.001
suction_cohesion_coefficient = 0.8
```

The BBM plasticity parameters are:

| Parameter | Value | Role |
|-----------|-------|------|
| `slope_of_swelling_line = 0.011` | $\kappa = 0.011$ | Elastic compressibility (swelling slope) |
| `slope_of_virgin_consolidation_line = 0.065` | $\lambda_0 = 0.065$ | Plastic compressibility at saturation |
| `slope_of_critical_state_line = 1.2` | $M = 1.2$ | Friction angle: $\sin\phi' = 3M/(6+M)$ |
| `initial_pre-consolidation_pressure = 0.04e6` | $p_{c0} = 40$ kPa | Initial pre-consolidation |
| `reference_consolidation_pressure = 0.01e6` | $p_r = 10$ kPa | Reference for the LC curve |
| `kappa_s = 0.005` | $\kappa_s$ | Elastic compressibility with respect to suction |
| `suction_cohesion_coefficient = 0.8` | $k$ | Cohesion: $p_s = k \cdot s$ |

The curves are defined using **analytical command-line syntax** (Bil curve generation syntax):

```
Curves = wrc2   pc = Range{x1 = 0, x2 = 30.e6, n = 171}
                sl = Expressions(1){p0 = 1.e6 ; m = 0.6 ;
                                    sl = (1 + (pc/p0)**(1/(1-m)))**(-m)}
```

**`Range{...}` syntax**: generates a table of 171 uniformly spaced values of $p_c$ between 0 and $30 \times 10^6$ Pa.

**`Expressions(1){...}` syntax**: defines the `sl` column by the van Genuchten formula:
$$s_l = \left(1 + \left(\frac{p_c}{10^6}\right)^{1/(1-0.6)}\right)^{-0.6} = \left(1 + \left(\frac{p_c}{10^6}\right)^{2.5}\right)^{-0.6}$$

```
Curves = krc2   pc = Range{x1 = 0, x2 = 30.e6, n = 171}
                kl = Expressions(1){kl = 1}
                kg = Expressions(1){kg = 1}
```

Relative permeabilities constant at 1 (perfectly drained condition).

```
Curves = lc     pc = Range{x1 = 0, x2 = 1.e6, n = 200}
                lc = Expressions(1){l0 = 0.065 ; k = 0.011 ; beta = 20.e-6 ;
                                    r = 0.75 ;
                                    lc = (l0 - k)/(l0*((1-r)*exp(-beta*pc) + r) - k)}
```

The `lc` curve tabulates the LC factor:
$$\text{lc}(p_c) = \frac{\lambda_0 - \kappa}{\lambda_0\left[(1-r)e^{-\beta p_c} + r\right] - \kappa}$$

| $p_c$ [Pa] | $\lambda(p_c)$ | $\text{lc}(p_c)$ |
|------------|----------------|-------------------|
| 0 | 0.065 | 1.000 |
| $10^5$ | 0.057 | 1.150 |
| $10^6$ | 0.049 | 1.430 (asymptote) |

---

```
Fields
2
Value = -1.e3 Gradient = 0 0 0 Point = 0 0 0
Value =  1.e3 Gradient = 0 0 0 Point = 0 0 0
```

Two uniform spatial fields:
- **Field 1** = $-1000$ Pa: used to impose $p_l = -s$ (suction in Pa). Since $p_l = \text{Field 1} \times \text{Function 2}$, one obtains $p_l = -1000 \times F_2(t) = -s(t)$ in Pa.
- **Field 2** = $+1000$ Pa: used as the amplitude of the applied mechanical pressure ($1\,\text{kPa}$ per unit of function).

---

```
Functions
2
N = 7 F(0) = 1   F(1) = 40  F(2) = 1   F(3) = 80   F(4) = 1   F(5) = 160  F(6) = 1
N = 7 F(0) = 0   F(1.999) = 0  F(2) = 40  F(3.999) = 40  F(4) = 80  F(5.999) = 80  F(6) = 160
```

**Bil function syntax:** `N = n` gives the number of $(t_i,\,F_i)$ pairs; intermediate values are linearly interpolated.

- **Function 1** — mechanical pressure (kPa):

```
  Value (×1000 Pa)
   160│                           ╮ ╮
    80│             ╮ ╮            │  │
    40│   ╮ ╮        │  │            │  │
     1│───╯   ╰───╯    ╰───╯    ╰───
       0   1   2   3   4   5   6  t
```

Three loading-unloading cycles: 1→40→1 kPa, 1→80→1 kPa, 1→160→1 kPa.

- **Function 2** — suction (kPa), step-wise:

```
  Suction (kPa)
   160│                                     ╭───
    80│                   ╭─────────────╯
    40│       ╭───────────╯
     0│───────╯
       0   1   2   3   4   5   6  t
```

Suction is zero during the first cycle, then increases in steps (40 kPa at $t = 2$, 80 kPa at $t = 4$, 160 kPa at $t = 6$).

> **Reading note:** `F(1.999) = 0` followed by `F(2) = 40` creates a near-instantaneous suction ramp at $t = 2$ (over $\Delta t = 10^{-3}$ time units). This simulates an **instantaneous suction imposition** (drying is considered infinitely fast compared to mechanical loading).

---

```
Boundary Conditions
3
Region = 14 Unknown = u_1 Field = 0 Function = 0
Region = 11 Unknown = u_2 Field = 0 Function = 0
Region = 100 Unknown = p_l Field = 1 Function = 2
```

| Condition | Interpretation |
|-----------|---------------|
| `Region = 14 Unknown = u_1 Field = 0` | $u_1 = 0$ on the axis ($r = 0$) — axial symmetry |
| `Region = 11 Unknown = u_2 Field = 0` | $u_2 = 0$ at the bottom ($z = 0$) — fixed base |
| `Region = 100 Unknown = p_l Field = 1 Function = 2` | $p_l = (-1000) \times F_2(t) = -s(t)$ over the entire domain |

**`Field = 0`** means the value is directly zero.

The condition on `Region = 100` (entire surface of the square) imposes $p_l$ at **all nodes** simultaneously. Combined with `Field = 1` ($= -1000$ Pa) and `Function = 2` (step-wise ramp), it creates the imposed suction $s(t) = 1000 \times F_2(t)$ Pa.

---

```
Loads
2
Region = 13 Equation = meca_1 Type = pressure Field = 2 Function = 1
Region = 12 Equation = meca_1 Type = pressure Field = 2 Function = 1
```

The `Loads` impose Neumann conditions (prescribed traction flux on faces):

| Load | Interpretation |
|------|---------------|
| `Region = 13` (top boundary) | Vertical pressure $= 1000 \times F_1(t)$ Pa |
| `Region = 12` (right boundary) | Radial pressure $= 1000 \times F_1(t)$ Pa |

Since both faces undergo the **same pressure**, the loading is **isotropic**: $\sigma_{rr} = \sigma_{zz} = -1000 \times F_1(t)$ Pa. The negative sign follows the Bil convention (a pressure is a negative traction).

---

```
Points
1
0.5 0.5
```

Output at point $(r, z) = (0.5, 0.5)$ m (center of the square), recorded at every time step in `BBM.p1`.

---

```
Dates
7
0 1 2 3 4 5 6
```

Full-field snapshots saved in `BBM.t0` through `BBM.t6`.

---

```
Objective Variations
u_1 = 1.e-4
u_2 = 1.e-4
p_l = 1000
```

Target variations per time step:
- Displacements: $\Delta u \leq 10^{-4}$ m (very small to capture plastic transitions)
- Pressure: $\Delta p_l \leq 1000$ Pa (fine control of suction during steps)

---

```
Iterative Process
Iteration = 100
Tolerance = 1e-06
Repetition = 0

Time Steps
Dtini = 1.e-4
Dtmax = 1e-3
```

Very small time steps ($\Delta t_\text{ini} = 10^{-4}$, $\Delta t_\text{max} = 10^{-3}$) to track the near-instantaneous suction jumps (rising over $10^{-3}$ time units). Newton–Raphson with up to 100 iterations and a tolerance of $10^{-6}$.

---

### 6.2 File `BBM2` — p-q Stress Path with Variable Suction

This test case follows a stress path in $(p, q)$ space by separately applying an isotropic stress (p) and a deviatoric stress (q). It requires **4 fields** and **3 functions**:

```
Fields
4
Type = affine  Value =  1.e3    Point = 0 0 0   ← Field 1: isotropic amplitude
Type = affine  Value = -0.33e3  Point = 0 0 0   ← Field 2: radial correction (-1/3)
Type = affine  Value =  0.66e3  Point = 0 0 0   ← Field 3: axial correction (+2/3)
Type = affine  Value =  -1.e3   Point = 0 0 0   ← Field 4: suction (negative)
```

**Construction of the p-q path:**

For an axisymmetric stress state $(\sigma_r, \sigma_z)$:
$$p = \frac{2\sigma_r + \sigma_z}{3}, \qquad q = |\sigma_z - \sigma_r|$$

By applying a stress $\sigma_r = \text{F1} \times F_1 + \text{F2} \times F_2$ and $\sigma_z = \text{F1} \times F_1 + \text{F3} \times F_2$:

$$\sigma_r = 1000\,F_1 - 330\,F_2, \qquad \sigma_z = 1000\,F_1 + 660\,F_2$$

$$\Rightarrow \quad p \approx 1000\,F_1, \qquad q \approx 990\,F_2 \approx F_2 \text{ kPa}$$

The functions:
```
N = 6  F(0) = 1   F(1) = 40   F(2) = 1   F(3) = 80   F(4) = 1   F(5) = 160  ← p in kPa
N = 6  F(0) = 1   F(1) = 20   F(2) = 1   F(3) = 40   F(4) = 1   F(5) = 80   ← q in kPa
N = 6  F(0) = 0   F(1.999) = 0  F(2) = 40  F(3.999) = 40  F(4) = 80  F(5) = 80  ← suction
```

The stress path traces cycles in $p$-$q$ space: isotropic compression ($F_1$) and superimposed deviatoric stress ($F_2$) combine to approach or exceed the BBM yield surface.

---

### 6.3 File `BBM_pcst` — Constant Suction

Identical to `BBM2` but with constant zero suction:

```
N = 6  F(0) = 0  F(1) = 0  F(2) = 0  F(3) = 0  F(4) = 0  F(5) = 0  ← suction = 0
```

This case reproduces **Modified Cam-Clay behavior at full saturation**, serving as a reference to validate the saturated limit of the BBM.

---

### 6.4 Material Curves: `wrc2`, `krc2`, `lc`

These files are **automatically generated by Bil** from the analytical expressions in the `Curves` section. They are in ASCII format:

**`wrc2`** (3 columns: $p_c$, $s_l$, $k_l$, $k_g$) — van Genuchten retention curve:

```
# Models: X-axis(1) Expressions(2) Expressions(3) Expressions(4)
# Labels: pc(1) sl(2) kl(3) kg(4)
0.000000e+00   1.000000e+00   1.000000e+00   1.000000e+00
1.764706e+05   9.922319e-01   1.000000e+00   1.000000e+00
...
3.000000e+07   6.085066e-03   1.000000e+00   1.000000e+00
```

| $p_c$ [Pa] | $s_l$ [-] | Note |
|------------|-----------|------|
| 0 | 1.000 | Saturated soil |
| $10^5$ | 0.979 | Slightly unsaturated |
| $10^6$ | 0.758 | Notable desaturation |
| $10^7$ | 0.302 | Strongly desaturated |
| $3 \times 10^7$ | 0.006 | Near-residual |

**`lc`** (2 columns: $p_c$, lc) — LC factor:

```
# Labels: pc(1) lc(2)
0.000000e+00   1.000000e+00    ← saturated soil: p_c(s) = p_c(0)
5.025126e+03   1.029626e+00    ← suction 5 kPa: +3% pre-consolidation
1.005025e+04   1.057973e+00    ← suction 10 kPa: +6%
1.000000e+06   1.430464e+00    ← suction 1 MPa: +43% (asymptote)
```

---

### 6.5 Mesh `carre.msh` and Geometry `carre.geo`

The geometry is a **unit square** $[0,1] \times [0,1]$ m generated with Gmsh.

**`carre.geo`** — Gmsh source file:

```gmsh
cl1 = 1;                          // characteristic size = 1 m (single element)
Point(1) = {0, 0, 0, cl1};        // (r=0, z=0) — bottom-left corner (axis + base)
Point(2) = {1, 0, 0, cl1};        // (r=1, z=0) — bottom-right corner
Point(3) = {1, 1, 0, cl1};        // (r=1, z=1) — top-right corner
Point(4) = {0, 1, 0, cl1};        // (r=0, z=1) — top-left corner (axis + top)
Line(11) = {1, 2};                 // bottom edge (z=0)  → Region 11
Line(12) = {2, 3};                 // right edge (r=1)   → Region 12
Line(13) = {3, 4};                 // top edge (z=1)     → Region 13
Line(14) = {4, 1};                 // axis (r=0)         → Region 14
Plane Surface(100) = {99};         // interior surface → Region 100
Recombine Surface {100};           // force quadrilaterals
```

**`carre.msh`** — compiled mesh (Gmsh v1 format):

```
$NOD
4                           ← 4 nodes
1  0  0  0                  ← node 1: (r=0, z=0)
2  1  0  0                  ← node 2: (r=1, z=0)
3  1  1  0                  ← node 3: (r=1, z=1)
4  0  1  0                  ← node 4: (r=0, z=1)
$ELM
5
1  1  1  11  2  1  2        ← linear element, Region 11 (bottom), nodes 1-2
2  1  1  12  2  2  3        ← linear element, Region 12 (right), nodes 2-3
3  1  1  13  2  3  4        ← linear element, Region 13 (top), nodes 3-4
4  1  1  14  2  4  1        ← linear element, Region 14 (axis), nodes 4-1
5  3  1  100  4  1  2  3  4 ← quadrilateral, Region 100, nodes 1-2-3-4
```

The mesh is intentionally **minimal** (a single 4-node quadrilateral element): these test cases represent the behavior of a **Gauss point** (RVE), and spatial convergence is not the objective.

---

## 7. Results

The results of the `BBM` test case are stored in `BBM.t0`–`BBM.t6` (spatial fields) and `BBM.p1` (time evolution at the central point $(0.5,\,0.5)$).

### Output File Format

Each line of `BBM.p1` contains 29 columns:

```
# Time(1) Liquid_pore_pressure(2) Displacements(3) Fluid_mass_flow(6) 
# Stresses(9) Saturation_degree(18) Void_ratio_variation(19) 
# Plastic_strains(20) Hardening_variable(29)
```

Key columns:

| Column | Quantity | Unit |
|--------|----------|------|
| 1 | Time $t$ | — (dimensionless, $t_\text{max} = 6$) |
| 2 | $p_l$ | Pa |
| 9, 13, 17 | $\sigma_{11}$, $\sigma_{22}$, $\sigma_{33}$ | Pa |
| 18 | $s_l$ | — |
| 19 | $\Delta e = (1+e_0)\,\text{tr}\,\boldsymbol{\varepsilon}$ | — |
| 29 | $a = \ln(p_{c0})$ | — |

### Evolution at Key Time Steps

| Time step | $t$ | $p_l$ [Pa] | $\sigma$ [Pa] | $s$ [kPa] | $s_l$ [-] | $\Delta e$ | $a = \ln p_{c0}$ |
|-----------|-----|-----------|---------------|-----------|-----------|------------|-----------------|
| Initial | 0 | 0 | −1000 | 0 | 1.000 | 0 | **10.597** |
| After 1st loading (40 kPa) | 1 | 0 | −40000 | 0 | 1.000 | −0.0408 | 10.597 |
| After 1st unloading | 2 | 0 | −1000 | 0 | 1.000 | −0.0054 | 10.597 |
| After 2nd loading (80 kPa, s=40 kPa) | 3 | −40000 | −80000 | 40 | 0.998 | −0.0694 | **10.906** |
| After 2nd unloading | 4 | −40000 | −5000 | 40 | 0.998 | −0.0394 | 10.945 |
| After 3rd loading (160 kPa, s=80 kPa) | 5 | −80000 | −143000 | 80 | 0.996 | −0.0935 | **11.234** |
| Final state (s=80 kPa, unloaded) | 6 | −160000 | −1000 | 160 | 0.993 | −0.0457 | **11.317** |

> **Note:** the variable $a = \ln(p_{c0})$ remains constant during elastic phases (cycle 1) and increases during **plastic** phases (cycles 2 and 3), reflecting hardening of the pre-consolidation pressure.

### Physical Interpretation

**1st cycle (t = 0 → 2) — saturated compression to 40 kPa:**
- The material is loaded up to its initial pre-consolidation $p_{c0} = 40$ kPa: the deformation is **purely elastic** (hardv constant = 10.597).
- Elastic volumetric strain during loading: $\Delta e \approx -0.041$, computed as $\Delta e = -(1+e_0)\kappa\,\Delta\ln p \approx -(1+0.333) \times 0.011 \times \ln(40000/1000) = -0.039$ ✓
- Upon unloading, a fraction of the deformation is recovered.

**2nd cycle (t = 2 → 4) — application of suction (40 kPa) then compression to 80 kPa:**
- Suction increases pre-consolidation via the LC curve: $p_c(40\,\text{kPa}) = p_{c0} \times \text{lc}(40000) \approx 40000 \times 1.15 = 46$ kPa.
- By compressing to 80 kPa > 46 kPa, the material **yields** → hardv increases from 10.597 to 10.906 ($p_{c0}$ goes from 40 to 54 kPa).
- The degree of saturation drops slightly to $s_l = 0.998$ (suction = 40 kPa < 100 kPa — mild desaturation).

**3rd cycle (t = 4 → 6) — suction 80 kPa then compression to 160 kPa:**
- New increase in pre-consolidation. The LC at $s = 80$ kPa gives $p_c(80\,\text{kPa}) \approx 40000 \times 1.22 = 49$ kPa.
- Loading to 160 kPa >> 49 kPa → **strong yielding**, hardv rises to 11.317 ($p_{c0} \approx 82$ kPa).

**$\Delta e$ — $\ln p$ diagram (generated by `BBM.gp` → `E-P.eps`):**

```
 Δe
  0 │───╮
     │    ╰─────╮
−0.04│           ╰──╮ ← elastic cycle (saturation)
     │               ╰────╮
−0.07│    ← yielding       ╰╮ ← slope λ(40kPa)
     │                       ╰────╮
−0.09│         ← slope λ(80kPa)   ╰──
     └─────────────────────────────────── ln p
      1kPa      40kPa   80kPa    160kPa
```

The slope of the compression curves changes with suction, reflecting the variation of $\lambda(s)$.

![BBM - Simulation Results](BBM_results.png)

---

## 8. Numerical Discretization

The BBM model uses the **Finite Element Method (FEM)** via `FEM.h`. The coupled hydromechanical system with $1 + \dim$ equations per node is solved simultaneously (monolithic formulation).

**Element tangent stiffness matrix (block decomposition):**

$$\mathbf{K} = \begin{pmatrix}
K_{pp} & K_{pu} \\
K_{up} & K_{uu}
\end{pmatrix}$$

| Block | Expression | Meaning |
|-------|-----------|---------|
| $K_{uu}$ | $\int \mathbf{B}^T \mathbb{C}^{ep} \mathbf{B}\,dV$ | Tangent elastoplastic stiffness |
| $K_{up}$ | $\int \mathbf{B}^T \frac{\partial\bar{\boldsymbol{\sigma}}}{\partial p_l}\,\mathbf{N}\,dV$ | Mechanical-hydraulic coupling |
| $K_{pu}$ | $\int \mathbf{N}^T \rho_l\,s_l\,\text{div}\,\mathbf{N}_u\,dV$ | Biot coupling (strain → storage) |
| $K_{pp}$ | $\int \mathbf{N}^T C(p_l)\,\mathbf{N}\,dV + \Delta t\,\int \mathbf{B}^T K_l\,\mathbf{B}\,dV$ | Capillary capacity + diffusion |

The elastoplastic stiffness matrix $\mathbb{C}^{ep}$ is computed by the `PlasticityBBM.c` plasticity module via **return mapping**. The algorithm uses **AutoDiff** (automatic differentiation) to compute the yield criterion gradient and hardening modulus exactly.

---

## 9. References

### Barcelona Basic Model — BBM

- **Alonso, E. E., Gens, A. & Josa, A.** (1990). A constitutive model for partially saturated soils. *Géotechnique*, 40(3), 405–430. — Founding paper of the BBM: LC yield surface, retention curve, suction-mechanics coupling.

- **Gens, A. & Alonso, E. E.** (1992). A framework for the behaviour of unsaturated expansive clays. *Canadian Geotechnical Journal*, 29(6), 1013–1032. — Extension to expansive clays (BExM).

- **Gens, A.** (1996). Constitutive modelling: Application to compacted soils. In *Unsaturated Soils*, E. Alonso & P. Delage (Eds.), Vol. 3, pp. 1179–1200. — Overview of models for unsaturated soils.

### Unsaturated Soil Mechanics

- **Fredlund, D. G. & Morgenstern, N. R.** (1977). Stress state variables for unsaturated soils. *Journal of Geotechnical Engineering*, 103(GT5), 447–466. — Definition of net stresses and suction as independent state variables.

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons, Chichester. — Rigorous thermodynamic framework for multiphase porous media (generalized effective stresses, Helmholtz potential).

- **Wheeler, S. J. & Sivakumar, V.** (1995). An elasto-plastic critical state framework for unsaturated soil. *Géotechnique*, 45(1), 35–53. — Experimental validation of the BBM on compacted soils.

### LC Curve and Suction Behavior

- **Cui, Y. J. & Delage, P.** (1996). Yielding and plastic behaviour of an unsaturated compacted silt. *Géotechnique*, 46(2), 291–311. — Experimental determination of the LC curve for compacted silt.

- **Romero, E., Gens, A. & Lloret, A.** (1999). Water permeability, water retention and microstructure of unsaturated compacted Boom clay. *Engineering Geology*, 54(1–2), 117–127. — Retention curves and hydromechanical behavior of unsaturated clays.

### Plasticity and Return Mapping

- **Simo, J. C. & Hughes, T. J. R.** (1998). *Computational Inelasticity*. Springer, New York. — Return mapping algorithms and computation of the consistent tangent matrix.

- **Borja, R. I.** (1991). Cam-Clay plasticity, Part II: Implicit integration of constitutive equation based on a nonlinear elastic stress predictor. *Computer Methods in Applied Mechanics and Engineering*, 88(2), 225–240. — Implicit integration of Modified Cam-Clay, basis of the BBM algorithm.

### Coupled Numerical Implementation

- **Lewis, R. W. & Schrefler, B. A.** (1998). *The Finite Element Method in the Deformation and Consolidation of Porous Media* (2nd ed.). John Wiley & Sons, Chichester. — Coupled FEM formulation for consolidation of porous media.

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Source code: <https://github.com/Universite-Gustave-Eiffel/bil>
