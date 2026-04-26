# M5 Model — Coupled Water–Air Transfer in Unsaturated Porous Media (Isothermal 1D)

> **Source files:**
> `src/Models/ModelFiles/M5.c` · `examples/m5/m5` · `docs/M5_VoronoiFVM.jl`
>
> **Bil model author:** P. Dangla (Université Gustave Eiffel)

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equations](#41-conservation-equations)
   - 4.2 [Flux laws](#42-flux-laws)
   - 4.3 [Thermodynamic constitutive relations](#43-thermodynamic-constitutive-relations)
   - 4.4 [Retention curves and relative permeabilities](#44-retention-curves-and-relative-permeabilities)
   - 4.5 [Tortuosity model and gas diffusion](#45-tortuosity-model-and-gas-diffusion)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: engineered barrier under drying (`examples/m5/m5`)](#6-test-case-engineered-barrier-under-drying)
7. [Numerical discretization](#7-numerical-discretization)
8. [Bibliographic references](#8-bibliographic-references)

---

## 1. Context and objective

The M5 model describes **isothermal coupled water–air transfers** in a partially saturated porous medium. It is typically used to simulate **drying and re-wetting of cementitious materials or soils** subjected to variable hydraulic conditions.

The test case `examples/m5/m5` represents an **engineered barrier** (e.g., compacted clay or concrete) in contact with a moist natural soil. The barrier, initially very dry, progressively re-wets from the soil interface over a centennial time scale. This scenario is representative of **radioactive waste storage** or **containment barriers** in civil engineering.

The domain is one-dimensional with length $L = 1.225$ m, composed of two materials:

| Zone | $x$ [m] | Material | $\phi$ [-] | $k_\text{int}$ [m²] |
|------|---------|---------|------------|---------------------|
| Barrier | $[0,\ 0.425]$ | Compacted clay/concrete | 0.30 | $10^{-20}$ |
| Soil | $[0.425,\ 1.225]$ | Natural soil | 0.05 | $10^{-19}$ |

---

## 2. Assumptions

1. **Isothermal**: temperature is uniform and constant ($T = 293$ K). Physico-chemical properties (viscosities, diffusivity, saturated vapor pressure) are constants.
2. **Rigid porosity**: the solid skeleton is non-deformable; $\phi = \text{const}$.
3. **Two fluid phases**: liquid phase (water) and gas phase (binary mixture of water vapor + dry air). The gas phase is treated as an **ideal gas**.
4. **Three constituents**: liquid water, water vapor (gaseous H₂O), dry air.
5. **Local vapor–liquid equilibrium** (Kelvin assumption): vapor pressure is a unique function of liquid pressure via the Kelvin law.
6. **No gravity** in the test case (horizontal domain, $g = 0$).
7. **Darcy's law** for advective transport of each fluid phase.
8. **Fick's law** for vapor diffusion in the gas.
9. **Continuity of capillary pressures** at the interface between materials.

---

## 3. Variables and notation

### Primary unknowns

| Symbol | Meaning | Unit |
|---------|---------|------|
| $p_l$ | Liquid phase pressure | Pa |
| $p_a$ | Dry air partial pressure | Pa |

Total gas pressure and capillary pressure follow:

$$p_g = p_v(p_l) + p_a, \qquad p_c = p_g - p_l$$

### Secondary variables

| Symbol | Meaning |
|---------|---------|
| $s_l$ | Liquid water saturation degree |
| $s_g = 1 - s_l$ | Gas saturation degree |
| $p_v$ | Water vapor partial pressure |
| $\rho_v,\ \rho_a,\ \rho_g$ | Densities: vapor, dry air, total gas |
| $k_{rl},\ k_{rg}$ | Liquid and gas relative permeabilities |
| $c_v = \rho_v/\rho_g$ | Vapor mass fraction in gas |

### Physico-chemical constants

| Symbol | Value | Meaning |
|---------|-------|---------|
| $T$ | 293 K | Temperature |
| $R$ | 8.314 J/(mol·K) | Ideal gas constant |
| $M_w$ | 18×10⁻³ kg/mol | Water molar mass |
| $M_a$ | 28.8×10⁻³ kg/mol | Dry air molar mass |
| $V_w$ | 18×10⁻⁶ m³/mol | Liquid water molar volume |
| $\rho_l$ | 1000 kg/m³ | Liquid water density |
| $\mu_l$ | 10⁻³ Pa·s | Water dynamic viscosity |
| $\mu_g$ | 1.8×10⁻⁵ Pa·s | Gas dynamic viscosity |
| $p_{v0}$ | 2460 Pa | Saturated vapor pressure at $T$ |
| $p_{l0}$ | 10⁵ Pa | Liquid reference pressure |
| $D_{av0}$ | 2.56×10⁻⁵ m²/s | H₂O diffusivity in air at $(T,\ p_{g0})$ |

---

## 4. Mathematical model

### 4.1 Conservation equations

The system comprises **two mass balance equations** integrated over the representative elementary volume (REV):

**Total water mass conservation** (liquid + vapor):

$$\frac{\partial m_T}{\partial t} + \nabla \cdot \mathbf{W}_T = 0$$

with

$$m_T = \rho_l\,\phi\,s_l + \rho_g\,\phi\,s_g$$

**Dry air mass conservation**:

$$\frac{\partial m_A}{\partial t} + \nabla \cdot \mathbf{W}_A = 0$$

with

$$m_A = \rho_a\,\phi\,s_g$$

> **Note:** The term $\rho_g\,\phi\,s_g$ in $m_T$ groups vapor and air because the total gas flux $\mathbf{W}_g$ is indivisible in Darcy's law. Liquid/vapor separation is ensured by the air balance.

### 4.2 Flux laws

Total water flux $\mathbf{W}_T$ and air flux $\mathbf{W}_A$ are:

$$\mathbf{W}_T = \mathbf{W}_l + \mathbf{W}_g, \qquad \mathbf{W}_A = \mathbf{W}_g - \mathbf{W}_v$$

**Darcy fluxes** for liquid and gas phases (without gravity):

$$\mathbf{W}_l = -K_{D,l}\,\nabla p_l, \qquad K_{D,l} = \frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}$$

$$\mathbf{W}_g = -K_{D,g}\,\nabla p_g, \qquad K_{D,g} = \frac{\rho_g\,k_\text{int}\,k_{rg}(p_c)}{\mu_g}$$

**Vapor flux** (advection in gas + Fick diffusion):

$$\mathbf{W}_v = c_v\,\mathbf{W}_g - K_{F,v}\,\nabla\rho_v, \qquad K_{F,v} = \phi\,s_g\,\tau\,D_{av}$$

where $D_{av} = D_{av0}\,p_{g0}/p_g$ is the vapor diffusivity in air at pressure $p_g$ (pressure correction law), and $\tau$ is the tortuosity factor (see §4.5).

> **With gravity** ($g \neq 0$), Darcy's law is written in hydraulic head: $\mathbf{W}_\alpha = -K_{D,\alpha}\,\nabla H_\alpha$ with $H_\alpha = p_\alpha - \rho_\alpha\,g\,z$.

### 4.3 Thermodynamic constitutive relations

#### Vapor–liquid equilibrium (Kelvin's law)

The thermodynamic equilibrium vapor pressure is given by the Kelvin relation:

$$\boxed{p_v = p_{v0}\,\exp\!\left(\frac{V_w}{RT}\,(p_l - p_{l0})\right)}$$

The relative humidity follows:

$$h_r = \frac{p_v}{p_{v0}} = \exp\!\left(\frac{V_w}{RT}\,(p_l - p_{l0})\right)$$

This relation directly couples $p_l$ to vapor content: a very negative liquid pressure (dry medium) translates to low relative humidity.

#### Gas phase densities (ideal gas)

$$\rho_v = \frac{p_v\,M_w}{RT}, \qquad \rho_a = \frac{p_a\,M_a}{RT}, \qquad \rho_g = \rho_v + \rho_a$$

### 4.4 Retention curves and relative permeabilities

Curves are provided analytically (Van Genuchten expressions) in the file `examples/m5/m5`.

#### Material 1 — Barrier ($p_{c3} = 10^6$ Pa)

**Saturation** (Van Genuchten):

$$s_l(p_c) = \left[1 + \left(\frac{p_c}{1.5 \times 10^6}\right)^{1.064}\right]^{-0.06}$$

**Liquid relative permeability**:

$$k_{rl}(p_c) = \left[1 + \left(\frac{p_c}{3 \times 10^6}\right)^{2}\right]^{-0.5}$$

#### Material 2 — Soil ($p_{c3} = 2 \times 10^5$ Pa)

**Saturation**:

$$s_l(p_c) = \left[1 + \left(\frac{p_c}{10^7}\right)^{1.7}\right]^{-0.412}$$

**Liquid relative permeability**:

$$k_{rl}(p_c) = \left[1 + \left(\frac{p_c}{10^7}\right)^{2}\right]^{-1}$$

#### Gas relative permeability (common to both materials, Mualem model)

$$k_{rg}(s_l) = (1 - s_l)^2\,\left(1 - s_l^{5/3}\right)$$

#### Saturation regularization near $s_l = 1$

To avoid degeneration of the capacity matrix when $p_c \to 0$, saturation is regularized for $p_c < p_{c3}$ by an exponential blending:

$$s_l(p_c) = 1 - (1 - s_l(p_{c3}))\,\exp\!\left(\frac{p_c - p_{c3}}{p_{c3}}\right), \quad p_c < p_{c3}$$

This formula ensures continuity of $s_l$ and its derivative at $p_{c3}$, and satisfies $s_l(0) = 1$.

### 4.5 Tortuosity model and gas diffusion

Pore network tortuosity for gas diffusion is modeled by the power law of Thiery *et al.* (2007):

$$\tau(\phi, s_g) = \phi^{\,a}\,s_g^{\,b}, \qquad a = 1.74,\quad b = 3.2$$

The effective diffusion coefficient of water vapor in gas is:

$$D_\text{eff} = \phi\,s_g\,\tau\,D_{av} = \phi\,s_g\,\phi^{a}\,s_g^{b}\,D_{av}$$

> **Comparison:** The Millington–Quirk (1961) model uses $a = 1/3$, $b = 7/3$.

---

## 5. Boundary and initial conditions

### Initial conditions

| Zone | $p_l$ [Pa] | $p_a$ [Pa] | $s_l$ [-] | $h_r$ [-] |
|------|-----------|-----------|----------|----------|
| Barrier | $-7.611 \times 10^7$ | $9.226 \times 10^4$ | ≈ 0.78 | ≈ 0.57 |
| Soil | $4.905 \times 10^6$ | $4.892 \times 10^6$ | ≈ 1.00 | > 1 |

> The barrier is initially very dry (high capillary pressure $p_c \approx 7.6 \times 10^7$ Pa). Soil is nearly saturated ($p_l > 0$, hydrostatic overpressure).

### Boundary conditions

| Edge | $x$ [m] | Type | Value |
|------|---------|------|-------|
| Left | 0 | Zero Neumann | $\mathbf{W}_T \cdot \mathbf{n} = 0$, $\mathbf{W}_A \cdot \mathbf{n} = 0$ |
| Right | 1.225 | Dirichlet | $p_l = 4.905 \times 10^6$ Pa, $p_a = 4.892 \times 10^6$ Pa |

The left edge is **impermeable** (wall or symmetry plane). The right edge maintains constant hydraulic conditions representing the moist natural soil.

---

## 6. Test case: engineered barrier under drying

### Simulation parameters

| Parameter | Value |
|-----------|-------|
| Total duration | 100 years ($3.154 \times 10^9$ s) |
| Initial time step $\Delta t_\text{ini}$ | 100 s |
| Maximum time step $\Delta t_\text{max}$ | 1 year |
| Objective variation (OBJE) | $\Delta p_l = \Delta p_a = 10^4$ Pa |
| Newton tolerance | $10^{-6}$ |
| Mesh | 25 elements in barrier + 75 elements in soil |

### Expected physics

- The initially dry barrier ($h_r \approx 0.57$) progressively **re-wets** from the right edge (moist soil). The wetting front advances from right to left.
- Liquid pressure $p_l$ in the barrier **increases** over time (from $-7.6 \times 10^7$ Pa toward soil-like conditions).
- Liquid water flux dominates at large capillary pressures; gas diffusion contributes significantly when $p_c$ is moderate.
- Since both materials have very different permeabilities ($k_\text{int}$ differs by a factor of 10), evolution is much slower in the barrier.

---

## 7. Numerical discretization

The model is discretized by the **cell-centered Finite Volume Method (FVM)**, as implemented in Bil via `FVM.h`. Spatial discretization uses a centered difference scheme for fluxes (conductance values evaluated at the previous time step — semi-implicit scheme).

The nonlinear system is solved at each time step by **Newton–Raphson**, with a Jacobian matrix assembled by numerical differentiation of fluxes and storage terms (see `ComputeComponentDerivatives()` in M5.c).

The time step is controlled by an adaptive strategy based on the maximum variation of primary unknowns (**OBJE** in Bil, `Δu_opt` in VoronoiFVM).

The Julia port uses the **VoronoiFVM.jl** library, which implements the same FVM method with automatic differentiation (ForwardDiff.jl) for the Jacobian.

---

## 8. Bibliographic references

### Unsaturated porous media

- **Bear, J.** (1972). *Dynamics of Fluids in Porous Media*. Elsevier, New York.
- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons, Chichester.
- **Whitaker, S.** (1977). Simultaneous heat, mass and momentum transfer in porous media: a theory of drying. *Advances in Heat Transfer*, 13, 119–203.

### Retention curves and relative permeabilities

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898.
- **Mualem, Y.** (1976). A new model for predicting the hydraulic conductivity of unsaturated porous media. *Water Resources Research*, 12(3), 513–522.

### Vapor–liquid equilibrium

- **Kelvin, Lord (W. Thomson)** (1871). On the equilibrium of vapour at a curved surface of liquid. *Philosophical Magazine*, 42, 448–452.

### Gas diffusion and tortuosity

- **Millington, R. J. & Quirk, J. P.** (1961). Permeability of porous solids. *Transactions of the Faraday Society*, 57, 1200–1207.
- **Thiery, M., Villain, G., Dangla, P. & Platret, G.** (2007). Investigation of the carbonation front shape on cementitious materials: Effects of the chemical kinetics. *Cement and Concrete Research*, 37(7), 1047–1058.

### Numerical implementation

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel.
- **Fuhrmann, J.** (2021–). *VoronoiFVM.jl: Solver for coupled nonlinear PDEs based on Voronoi finite volume discretization*. Zenodo.
