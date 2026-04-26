# Duracem Model — Durability of Cementitious Materials: Carbonation and Chloride Ingress (1D)

> **Bil model:** `src/Models/ModelFiles/Duracem.cpp`

> **Input file:** `doc/mkdocs/Durability/Duracem/Duracem`
>
> **Bil model author:** P. Dangla et al. (Université Gustave Eiffel)

---

## Table of Contents

1. [Context and Objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and Notation](#3-variables-and-notation)
4. [Mathematical Model](#4-mathematical-model)
   - 4.1 [Conservation Equations](#41-conservation-equations)
   - 4.2 [Flux Laws](#42-flux-laws)
   - 4.3 [Hardened Concrete Chemistry](#43-hardened-concrete-chemistry)
   - 4.4 [Dissolution/Precipitation Kinetics](#44-dissolutionprecipitation-kinetics)
   - 4.5 [Porosity and Permeability Evolution](#45-porosity-and-permeability-evolution)
   - 4.6 [Tortuosity and Ionic Diffusion in Solution](#46-tortuosity-and-ionic-diffusion-in-solution)
5. [Boundary and Initial Conditions](#5-boundary-and-initial-conditions)
6. [Test Case: Carbonation of a Cement Paste (`base/Duracem`)](#6-test-case-carbonation-of-a-cement-paste)
7. [Input Files: Step-by-Step Description](#7-input-files-step-by-step-description)
8. [Numerical Discretization](#8-numerical-discretization)
9. [References](#9-references)

---

## 1. Context and Objective

Duracem is the **parent model** for the durability of cementitious materials in Bil (2024). It is designed to simulate the long-term degradation of hardened cement paste subjected to chemical attack, in particular:

- **Carbonation**: penetration of atmospheric CO₂, dissolution of portlandite Ca(OH)₂ and C-S-H, precipitation of calcite CaCO₃;
- **Chloride ingress** (optional, via the `E_CHLORINE` macro): transport of Cl⁻ and formation of Friedel's salt.

Duracem serves as the basis for three more specialized derived models:

| Derived model | Target process |
|---|---|
| **Carbocem** | Carbonation only |
| **Chloricem** | Chlorides only |
| **Carbochloricem** | Coupled carbonation + chlorides |

The test case `base/Duracem/` represents a 1D column of ordinary Portland cement (OPC) paste exposed at the surface to air (CO₂ at 16% vol. after a transient) over a duration of 2 days.

---

## 2. Assumptions

1. **Isothermal**: $T = 298$ K (25 °C). All physicochemical constants are computed at this temperature.
2. **Deformable porosity**: the porosity $\phi$ evolves over time as a function of the molar volumes of solid phases (dissolution/precipitation).
3. **Mechanically non-deformable solid skeleton**: no poromechanical coupling.
4. **Single liquid phase** (aqueous pore solution) that is partially saturating; no explicit modeling of the gas phase as a dynamic constituent (the `E_AIR` option is disabled in the test case).
5. **Local vapor–liquid equilibrium** (Kelvin's law) for the relative water vapor pressure.
6. **Hardened concrete chemistry resolved locally** at each Gauss point by the `HardenedCementChemistry` module, which computes equilibrium concentrations of all ionic species from the primary unknowns (saturation indices, total concentrations, pH).
7. **Three main solid phases**: portlandite (CH), C-S-H, and calcite (CC).
8. **Ionic transport in solution** by diffusion (Nernst-Planck via `CementSolutionDiffusion`) + advection (Darcy).
9. **Gaseous CO₂ transport** by diffusion in partially desaturated pores (Fick's law).

---

## 3. Variables and Notation

### Primary Unknowns (`base/Duracem` test case)

| Symbol | Bil Unknown | Meaning | Unit |
|--------|------------|---------|------|
| $p_l$ | `p_l` | Liquid phase pressure | Pa |
| $\psi$ | `psi` | Electric potential $\times F/RT$ (dimensionless) | — |
| $\log c_{\text{CO}_2}$ | `logc_co2` | Log₁₀ of gaseous CO₂ concentration | mol/dm³ |
| $\log c_{\text{Na}}$ | `logc_na` | Log₁₀ of total Na concentration | mol/dm³ |
| $\log c_{\text{K}}$ | `logc_k` | Log₁₀ of total K concentration | mol/dm³ |
| $z_{\text{Ca}}$ | `z_ca` | Calcium zeta unknown (CH + CC) | — |
| $z_{\text{Si}}$ | `z_si` | Silicon zeta unknown (C-S-H) | — |
| $\log c_{\text{OH}}$ | `logc_oh` | Log₁₀ of OH⁻ concentration (electroneutrality) | mol/dm³ |

The zeta unknowns are defined by:

$$z_{\text{Ca}} = \frac{N_{\text{CcH}}}{N_0} + \log_{10}(S_{\text{CcH}})$$

$$z_{\text{Si}} = \frac{n_{\text{CSH}}}{n_{\text{CSH},0}} + \log_{10}(S_{\text{CSH}})$$

where $N_{\text{CcH}}$ is the molar content of solid calcium (CH + CC), $S_{\text{CcH}}$ the saturation index of the CaO-CO₂-H₂O system, and $n_{\text{CSH}}$ the silicon content in the C-S-H. These unknowns allow dissolution/precipitation of solid phases to be handled within the same equation system without variable changes.

### Computed Secondary Variables

| Symbol | Meaning |
|--------|---------|
| $s_l$ | Liquid water saturation degree |
| $\phi$ | Current porosity |
| $n_{\text{CH}}$ | Molar content of portlandite (mol/dm³) |
| $n_{\text{CSH}}$ | Molar content of C-S-H (mol/dm³) |
| $n_{\text{CC}}$ | Molar content of calcite (mol/dm³) |
| $x_{\text{CSH}}$ | Ca/Si ratio of C-S-H |
| pH | $-\log_{10}(c_{\text{H}^+})$ |
| $\rho_v$ | Water vapor density |

### Main Physicochemical Constants

| Symbol | Value | Meaning |
|--------|-------|---------|
| $T$ | 298 K | Temperature |
| $R$ | 8.314 J/(mol·K) | Ideal gas constant |
| $V_{\text{H}_2\text{O}}$ | 18 cm³/mol | Molar volume of liquid water |
| $\rho_l$ | 1000 kg/m³ | Water density |
| $V_{\text{CH}}$ | ≈ 33 cm³/mol | Molar volume of portlandite |
| $V_{\text{CC}}$ | 37 cm³/mol | Molar volume of calcite |
| $k_h$ | 0.9983 | Henry's constant CO₂(g)↔CO₂(aq) at 293 K |
| $D_{\text{CO}_2}$ | computed at $T$ | CO₂ diffusivity in air |
| $D_{\text{vap}}$ | computed at $T$ | H₂O diffusivity in air |

> **Solver units**: length in **dm** (decimeter), mass in **hg** (hectogram = 0.1 kg), time in **s**. Concentrations are in **mol/dm³ = mol/L**.

---

## 4. Mathematical Model

### 4.1 Conservation Equations

The system solves **8 balance equations** per node:

| # | Equation | Associated unknown |
|---|----------|--------------------|
| 1 | **Calcium** conservation: $\partial_t N_{\text{Ca}} + \nabla \cdot \mathbf{W}_{\text{Ca}} = 0$ | `z_ca` |
| 2 | **Silicon** conservation: $\partial_t N_{\text{Si}} + \nabla \cdot \mathbf{W}_{\text{Si}} = 0$ | `z_si` |
| 3 | **Sodium** conservation: $\partial_t N_{\text{Na}} + \nabla \cdot \mathbf{W}_{\text{Na}} = 0$ | `logc_na` |
| 4 | **Potassium** conservation: $\partial_t N_{\text{K}} + \nabla \cdot \mathbf{W}_{\text{K}} = 0$ | `logc_k` |
| 5 | **Charge** balance: $\nabla \cdot \mathbf{W}_q = 0$ | `psi` |
| 6 | **Total water mass** conservation: $\partial_t M_{\text{tot}} + \nabla \cdot \mathbf{W}_{\text{tot}} = 0$ | `p_l` |
| 7 | **Carbon** conservation: $\partial_t N_{\text{C}} + \nabla \cdot \mathbf{W}_{\text{C}} = 0$ | `logc_co2` |
| 8 | **Electroneutrality**: $\sum z_i c_i = 0$ | `logc_oh` |

Total molar contents integrate both liquid and solid contributions:

$$N_{\text{Ca}} = \phi s_l c_{\text{Ca},l} + n_{\text{CH}} + n_{\text{CC}} + x_{\text{CSH}}\,n_{\text{CSH}}$$

$$N_{\text{Si}} = \phi s_l c_{\text{Si},l} + n_{\text{CSH}}$$

$$N_{\text{C}} = \phi s_l c_{\text{C},l} + \phi(1-s_l)\,\rho_{\text{CO}_2} + n_{\text{CC}}$$

$$M_{\text{tot}} = \phi s_l \rho_l + \phi(1-s_l)\,\rho_v$$

### 4.2 Flux Laws

The total fluxes of each element combine **advection** (Darcy in solution) and **diffusion** (Nernst-Planck in solution + Fick in gas phase):

$$\mathbf{W}_\alpha = \underbrace{c_{\alpha,l}\,\mathbf{w}_l}_{\text{advection}} + \underbrace{\mathbf{j}_{\alpha,\text{diff}}}_{\text{ionic diffusion}}$$

#### Darcy Flux (liquid)

$$\mathbf{w}_l = -K_{D,l}\,\nabla p_l, \qquad K_{D,l} = \frac{\rho_l\,k_{\text{int}}\,k_{rl}(s_l)\,f(\phi)}{\mu_l}$$

where $f(\phi)$ is the permeability correction coefficient (Verma & Pruess model, see §4.5).

#### Ionic Diffusive Fluxes (Nernst-Planck)

$$\mathbf{j}_{\alpha,\text{diff}} = -\tau(\phi,s_l)\,c_{\alpha,l}\,\nabla \mu_\alpha$$

where $\mu_\alpha = \mu_\alpha^0 + RT\ln(c_\alpha/c_0) + z_\alpha F \psi$ is the electrochemical potential of species $\alpha$, and $\tau$ is the tortuosity factor of the porous network (see §4.6).

#### Gaseous CO₂ Flux (Fick)

$$\mathbf{w}_{\text{CO}_2,g} = -D_{\text{eff,CO}_2}\,\nabla c_{\text{CO}_2}$$

with the effective diffusion coefficient:

$$D_{\text{eff,CO}_2} = \phi\,(1-s_l)\,\tau_g\,D_{\text{CO}_2}$$

#### Water Vapor Flux (Fick)

$$\mathbf{w}_{\text{vap}} = -D_{\text{eff,vap}}\,\nabla \rho_v$$

$$\rho_v = \frac{p_{v0}\,M_{\text{H}_2\text{O}}}{RT}\,\exp\!\left(\frac{V_{\text{H}_2\text{O}}}{RT}(p_l - p_{l0})\right)$$

#### Total Ionic Current

$$\mathbf{W}_q = \sum_\alpha z_\alpha \mathbf{j}_{\alpha,\text{diff}}$$

The charge balance is quasi-stationary ($\partial_t \approx 0$), which gives $\nabla \cdot \mathbf{W}_q = 0$, an equation solved for $\psi$.

### 4.3 Hardened Concrete Chemistry

The solution chemistry is resolved locally by the `HardenedCementChemistry` module in the **CaO-SiO₂-Na₂O-K₂O-CO₂-H₂O** system. This module computes at thermodynamic equilibrium the concentrations of all ionic species from the primary unknowns (saturation indices, total concentrations, pH).

**Species in solution**: H⁺, OH⁻, Ca²⁺, CaOH⁺, Ca(OH)₂(aq), CaCO₃(aq), CaHCO₃⁺, H₂SiO₄²⁻, H₃SiO₄⁻, H₄SiO₄, CaH₂SiO₄, CaH₃SiO₄⁺, Na⁺, NaOH, NaHCO₃, NaCO₃⁻, K⁺, KOH, CO₂(aq), HCO₃⁻, CO₃²⁻, Cl⁻ (optional), etc.

**Solid phases** present:
- **Portlandite** Ca(OH)₂ (CH)
- **C-S-H** with variable Ca/Si ratio ($0 \le x \le 1.7$)
- **Calcite** CaCO₃ (CC)
- **Friedel's salt** 3CaO·Al₂O₃·CaCl₂·10H₂O (if E_CHLORINE)

### 4.4 Dissolution/Precipitation Kinetics

#### Portlandite (CH)

The dissolution of CH is controlled by a **first-order kinetic** law modified by the presence of a calcite film coating the spherical crystals:

$$\frac{dn_{\text{CH}}}{dt} = a_2\,f\!\left(1 - \frac{n_{\text{CH}}}{n_{\text{CH},0}}\right)\,\ln(S_{\text{CH}})$$

where $S_{\text{CH}}$ is the saturation index of portlandite ($S_{\text{CH}} < 1$ if the medium is undersaturated), and $f(\xi)$ is a function that depends on the dissolved fraction and the coefficient $c_2$. At the initial state $n_{\text{CH}} = n_{\text{CH},0}$.

The kinetics are based on the calcite-coated spherical crystal model developed in the thesis of Thiery (2005):

$$\frac{dn_{\text{CH}}}{dt} = -\frac{1}{\tau_\text{diss}}\,n_{\text{CH}}, \quad \tau_\text{diss} = \frac{R_{\text{CH}}}{3\,h\,V_{\text{CH}}}$$

Default parameters: $R_{\text{CH}} = 40\,\mu\text{m}$ (initial crystal radius), $h = 5.6 \times 10^{-6}$ mol/dm²/s (surface reaction rate).

#### C-S-H (Continuous Decalcification)

The decalcification of C-S-H is modeled by **equilibrium-driven dissolution/precipitation** controlled by the saturation index $S_{\text{CSH}}$. The Ca/Si ratio $x_{\text{CSH}}$ adjusts continuously as a function of pH and calcium concentration according to a curve defined in the `HardenedCementChemistry` module.

#### Calcite (CC)

Calcite precipitation (when $S_{\text{CC}} > 1$) can be either at equilibrium or kinetic:

$$\frac{dn_{\text{CC}}}{dt} = \text{rate\_calcite}\,(S_{\text{CC}} - 1)$$

In the active configuration of `base/Duracem` (`U_ZN_Ca_S`), calcite is at **equilibrium** with respect to CH: the sum $n_{\text{CH}} + n_{\text{CC}} = N_{\text{CcH}}(z_{\text{Ca}})$ is enforced.

### 4.5 Porosity and Permeability Evolution

Porosity evolves as a function of the molar volumes of solid phases:

$$\phi = \phi_0 - V_{\text{CH}}(n_{\text{CH}} - n_{\text{CH},0}) - V_{\text{CC}}\,n_{\text{CC}} - \Delta V_{\text{CSH}}$$

where $V_{\text{CH}}$, $V_{\text{CC}}$ are the molar volumes of portlandite and calcite, and $\Delta V_{\text{CSH}}$ accounts for the volume change of C-S-H during decalcification (curve `V_CSH`).

The **intrinsic permeability** is modified by the porosity evolution according to the **Verma & Pruess (1988)** model:

$$k = k_{\text{int},0}\,f_{\text{VP}}(\phi)$$

$$f_{\text{VP}}(\phi) = t^2\,\frac{1 - \text{frac} + \text{frac}/w^2}{1 - \text{frac} + \text{frac}\left(\frac{t}{t + w - 1}\right)^2}$$

with $t = (\phi - \phi_c)/(\phi_0 - \phi_c)$, $\phi_c = \phi_0 \times \phi_r$ (critical porosity), $w = 1 + (\phi_r/\text{frac})/(1-\phi_r)$.

This model accounts for **pore clogging** by calcite: when $\phi \to \phi_c$, $k \to 0$. The parameters `frac = 0.8` and `phi_r = 0.70` are calibrated for ordinary concrete.

### 4.6 Tortuosity and Ionic Diffusion in Solution

The tortuosity of the porous network for ionic diffusion in solution is modeled using the **Oh & Jang (2004)** model:

$$\tau = \tau_{\text{paste}} \times \tau_{\text{agg}}$$

$$\tau_{\text{paste}} = \left(m_\phi + \sqrt{m_\phi^2 + D_s^{1/n} \frac{\phi_c}{1 - \phi_c}}\right)^n$$

with $\phi_{\text{cap}} = \phi/2$ (capillary porosity), $\phi_c = 0.18$ (critical porosity), $n = 2.7$ (OPC), $D_s = 2 \times 10^{-4}$ (solid/liquid diffusivity ratio), and $m_\phi = 0.5 \frac{(\phi_{\text{cap}} - \phi_c) + D_s^{1/n}(1 - \phi_c - \phi_{\text{cap}})}{1 - \phi_c}$.

The saturation dependence is:

$$\tau(\phi, s_l) = \tau_{\text{sat}}(\phi) \times s_l^{4.5}$$

The aggregate factor is $\tau_{\text{agg}} = 0.27$ (concrete).

---

## 5. Boundary and Initial Conditions

### Initial Conditions

| Variable | Value | Physical meaning |
|----------|-------|-----------------|
| `logc_co2` | −15 | Initial CO₂ is zero in the paste ($c_{\text{CO}_2} \approx 10^{-15}$ mol/L) |
| `p_l` | grid `CN_satb` | Liquid pressure from the initial saturation profile |
| `psi` | −15 | Initial electric potential is zero |
| `z_si` | 1 | Intact C-S-H ($n_{\text{CSH}} = n_{\text{CSH},0}$, $S_{\text{CSH}} = 1$) |
| `z_ca` | 1 | Intact CH ($n_{\text{CH}} = n_{\text{CH},0}$, $S_{\text{CH}} = 1$) |

The initial pressure `p_l` is read from the `CN_satb` file, which provides a 2D mapping (porosity, capillary pressure) → saturation, allowing a consistent initial moisture state to be set in relation to material porosity.

### Boundary Conditions

The domain is a column of length $L = 0.6$ dm, exposed at the surface (right boundary, Region 1):

| Boundary | Unknown | Type | Value |
|----------|---------|------|-------|
| Left ($x=0$) | all | Zero Neumann | Flux = 0 (impermeable wall) |
| Right ($x=0.6$ dm) | `psi` | Dirichlet | Field 0 (−15, zero potential) |
| Right ($x=0.6$ dm) | `p_l` | Dirichlet | Field 3 (0 Pa, $p_l = p_{\text{atm}}$) |
| Right ($x=0.6$ dm) | `logc_co2` | Dirichlet + function | Field 1 × Function 1 (time profile) |

The CO₂ concentration imposed at the surface follows a **piecewise linear function**:

$$c_{\text{CO}_2}(t=0) = 1 \times 10^{-0.158} \approx 0.695 \text{ mol/dm}^3$$
$$c_{\text{CO}_2}(t=3600\,\text{s}) = 0.16 \times 10^{-0.158} \approx 0.111 \text{ mol/dm}^3$$

This transient represents an initial high-concentration phase followed by stabilization at the atmospheric level (CO₂ at approximately 0.04% vol.).

---

## 6. Test Case: Carbonation of a Cement Paste

### Simulation Parameters

| Parameter | Value |
|-----------|-------|
| Domain length | 0.6 dm (6 cm) |
| Number of elements | 100 (spacing = 0.001 dm = 1 mm) |
| Total duration | 172,800 s (2 days) |
| Output dates | 0, 86,400 s (1 day), 172,800 s (2 days) |
| Initial time step | 10 s |
| Maximum time step | 36,000 s (10 h) |
| Newton tolerance | $10^{-3}$ |
| Max iterations | 20 |

### Material Parameters

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `phi0` | 0.379 | Initial porosity (37.9%) |
| `kl_int` | $1.4 \times 10^{-17}$ dm² | Intrinsic liquid permeability |
| `n_ch0` | 3.9 mol/L | Initial portlandite content |
| `n_csh0` | 2.4 mol/L | Initial C-S-H content |
| `c_na0` | 0.019 mol/L | Initial Na concentration |
| `c_k0` | 0.012 mol/L | Initial K concentration |
| `frac` | 0.8 | Pore body fractional length (Verma-Pruess) |
| `phi_r` | 0.70 | Porosity fraction at zero permeability |

### Expected Physics

1. **Before carbonation**: the paste is at high pH (≈ 13) due to portlandite and alkalis. CO₂ is absent.
2. **CO₂ penetration** (by gaseous diffusion from the surface): the CO₂ front progresses from the surface inward.
3. **Carbonated zone** (at the surface):
   - Dissolution of portlandite: $\text{Ca(OH)}_2 + \text{CO}_2 \to \text{CaCO}_3 + \text{H}_2\text{O}$
   - Decalcification of C-S-H
   - Precipitation of calcite: solid mass gain, reduction in porosity
   - pH drop from 13 to ≈ 9
4. **Sound zone** (core): high pH maintained, intact portlandite, no carbonation.
5. **Carbonation front**: sharp interface between the sound and carbonated zones, propagating as $\sqrt{t}$ over the long term.
6. **Structural consequence**: the pH drop in the carbonated zone depassivates the reinforcement (if present), creating a risk of corrosion.

Over 2 days, the carbonation front remains close to the surface (< 1 mm depending on relative humidity).

![Duracem - Simulation results](Duracem_results.png)

---

## 7. Input Files: Step-by-Step Description

### 7.1 Main file `Duracem`

```
Units
Length = decimeter
Mass   = hectogram
```

**Solver unit system.** All numerical data that follow are in these units: lengths in dm, masses in hg, time in s, pressure in Pa.

---

```
Geometry
1 plan
```

**1D planar** geometry (one-dimensional).

---

```
Mesh
3 0. 0. 0.6
0.001
1 100
1 1
```

- **Line 1**: 3 coordinates of the domain boundary (x, y, z coordinates of the end node); the domain runs from 0 to 0.6 dm in x.
- **Line 2**: mesh spacing of 0.001 dm = 1 mm.
- **Line 3**: 1 group of 100 linear 1D elements.
- **Line 4**: 1×1 subdivision (no transverse refinement).

---

```
Material
Model = Duracem
InitialPorosity = 0.379
IntrinsicPermeability_liquid = 1.4e-17
InitialContent_portlandite = 3.9
InitialContent_csh = 2.4
InitialConcentration_sodium = 0.019
InitialConcentration_potassium = 0.012
FractionalLengthOfPoreBodies = 0.8
PorosityFractionAtVanishingPermeability = 0.70
```

Declaration of **material properties**:

| Key | Internal parameter | Role |
|-----|--------------------|------|
| `InitialPorosity` | `phi0` | Initial porosity, used as reference for $f_{\text{VP}}$ |
| `IntrinsicPermeability_liquid` | `kl_int` | Water permeability in dm² (at initial porosity) |
| `InitialContent_portlandite` | `n_ch0` | Initial CH content, mol/dm³ |
| `InitialContent_csh` | `n_csh0` | Initial C-S-H content, mol/dm³ |
| `InitialConcentration_sodium` | `c_na0` | Initial Na concentration, mol/dm³ |
| `InitialConcentration_potassium` | `c_k0` | Initial K concentration, mol/dm³ |
| `FractionalLengthOfPoreBodies` | `frac` | Verma-Pruess geometric parameter (= 0.8) |
| `PorosityFractionAtVanishingPermeability` | `phi_r` | Fraction of initial porosity at which $k \to 0$ |

---

```
Curves = desorbCN
Curves = relpermCN  s_l = Range{x1 = 0 , x2 = 1 , n = 101} kl_r = Mualem_liq(1){m = 0.45}
Curves = V_CSH
#Curves_log = cementpaste  p_c = ... (commented out)
```

Declaration of **material curves**:
- `desorbCN`: desorption curve $p_c(s_l)$ read from an external file (101 points).
- `relpermCN`: liquid relative permeability curve $k_{rl}(s_l)$, generated by the Mualem-Van Genuchten model with $m = 0.45$, over 101 points between 0 and 1.
- `V_CSH`: molar volume of C-S-H as a function of the Ca/Si ratio, read from an external file (3 points).
- The `cementpaste` line is **commented out** (preceded by `#`): the alternative retention curve is not used.

---

```
Fields
8
Value = -15         Gradient = 0. 0. 0.     Point = 0. 0. 0.
Value = -0.158      Gradient = 0. 0. 0.     Point = 0. 0. 0.
Value = -70.50000E+06 Gradient = 0. 0. 0.   Point = 0. 0. 0.
Value = 0.0         Gradient = 0. 0. 0.     Point = 0. 0. 0.
Value = 0.03        Gradient = 0. 0. 0.     Point = 0. 0. 0.
Value = 1           Gradient = 0. 0. 0.     Point = 0. 0. 0.
Value = 1           Gradient = 0. 0. 0.     Point = 0. 0. 0.
Type = grid         File = CN_satb
```

Declaration of **8 scalar fields** (uniform or grid-based):

| Index | Value | Usage |
|-------|-------|-------|
| 0 | −15 | Initial `logc_co2` (CO₂ ≈ 0); initial `psi` and BC |
| 1 | −0.158 | Surface `logc_co2` (CO₂ = $10^{-0.158}$ mol/dm³) |
| 2 | −70.5×10⁶ Pa | High initial capillary pressure (partially desaturated concrete) |
| 3 | 0.0 Pa | Liquid pressure at BC (exposed boundary, $p_l = 0$ = atmospheric reference) |
| 4 | 0.03 | Reserved value (not used in active BCs) |
| 5 | 1 | Initial `z_si` (intact C-S-H) |
| 6 | 1 | Initial `z_ca` (intact CH) |
| 7 | `CN_satb` grid | Initial `p_l` read by 2D interpolation from `CN_satb` |

Field 7 of type `grid` is a **2D interpolation** (in porosity and capillary pressure) that computes the initial liquid pressure consistent with the material porosity and initial moisture state.

---

```
Initialization
5
Region = 2  Unknown = logc_co2  Field = 1   Function = 0
Region = 2  Unknown = p_l       Field = 8   Function = 0
Region = 2  Unknown = psi       Field = 0   Function = 0
Region = 2  Unknown = z_si      Field = 6   Function = 0
Region = 2  Unknown = z_ca      Field = 7   Function = 0
```

**Initialization of unknowns** in Region 2 (interior domain) by field value. Field numbering starts at 1 here (Field = 8 → field at index 7 in the list above, i.e., the `CN_satb` grid).

The unknowns `logc_na`, `logc_k`, and `logc_oh` **are not initialized here** because they are automatically recomputed from the concrete chemistry at the first time step (via `concentrations_oh_na_k`).

---

```
Functions
1
N = 2 F(0) = 1. F(3600) = 0.16
```

Definition of **1 time function** (Function 1): linear interpolation between the points $(t=0\,\text{s},\, F=1.0)$ and $(t=3600\,\text{s},\, F=0.16)$. This function multiplies Field 1 to give the surface CO₂ concentration over time.

---

```
Boundary Conditions
3
Region = 1  Unknown = psi       Field = 0   Function = 0
Region = 1  Unknown = p_l       Field = 3   Function = 0
Region = 1  Unknown = logc_co2  Field = 1   Function = 1
```

**Three Dirichlet conditions** on Region 1 (exposed right boundary):
- `psi` = Field 0 × Function 0 (constant function = 1) → $\psi = -15$ (zero potential)
- `p_l` = Field 3 × Function 0 → $p_l = 0$ Pa
- `logc_co2` = Field 1 × Function 1 → $\log c_{\text{CO}_2}$ varies from −0.158 onward according to the time function

> The left boundary (Region 0) receives no condition: Bil applies a **zero flux** (homogeneous Neumann condition) by default to all unconstrained unknowns.

---

```
Objective Variations
logc_co2 = 0.1
p_l      = 1.e5
z_ca     = 0.1
psi      = 1.
logc_na  = 1e-1
logc_k   = 1e-1
z_si     = 1e-1
logc_oh  = 1.
```

**Target variations per time step** used for adaptive time step control. If an unknown varies by more than its target value, the time step is reduced. These values are calibrated to correctly capture the carbonation front.

---

```
Iterative Process
Iter        = 20
Tol         = 1.e-3
Repetitions = 0
```

**Newton-Raphson solver** parameters: maximum 20 iterations, convergence when the relative norm of the residual is below $10^{-3}$.

---

```
Dates
3 #27
0  86400  172800  # ...
```

**3 output times** (out of 27 possible, the others being commented out): $t = 0$ s, $t = 86400$ s (1 day), $t = 172800$ s (2 days).

---

```
Time Steps
Dtini = 10
Dtmax = 36000
```

Initial time step $\Delta t_{\text{ini}} = 10$ s (small to capture the fast CO₂ BC transient), maximum time step $\Delta t_{\text{max}} = 36000$ s (10 h).

---

### 7.2 File `desorbCN` — Desorption Curve

```
# Models:
# Labels: p_c(1)  s_l(2)
0      1       1
1000000 9.800265e-01 9.899629e-01
2000000 9.604322e-01 6.048733e-01
...
```

Tabulation of the **water retention curve** $s_l(p_c)$ for cement paste:

- Column 1: capillary pressure $p_c$ in Pa (from 0 to ~100 MPa)
- Column 2: saturation degree $s_l$ (from 1 to 0)
- Column 3: relative permeability $k_{rl}$ (not used here, replaced by `relpermCN`)

The data reflect a **desorption isotherm at 20 °C** typical of ordinary cement paste. Saturation decreases rapidly with capillary pressure due to the narrow pore size distribution (between 1 and 100 nm).

---

### 7.3 File `relpermCN` — Liquid Relative Permeability

```
# Models: X-axis(1) MualemVanGenuchten(2)
# Labels: s_l(1) kl_r(2)
0.000000e+00 0.000000e+00
1.000000e-02 2.615440e-11
...
1.000000e+00 1.000000e+00
```

Tabulation of the **liquid relative permeability** $k_{rl}(s_l)$ according to the Mualem-Van Genuchten model with parameter $m = 0.45$:

$$k_{rl}(s_l) = \sqrt{s_l}\,\left[1 - \left(1 - s_l^{1/m}\right)^m\right]^2$$

101 uniformly spaced points in $s_l$. The strong nonlinearity (variation over 11 decades between $s_l = 0$ and $s_l = 1$) is characteristic of cement pastes whose fine pores drastically reduce water permeability as soon as $s_l < 1$.

---

### 7.4 File `V_CSH` — Molar Volume of C-S-H

```
# Models:
# Labels: x_csh(1)  v_csh(2)
0    5.44e-02
0.85 8.14e-02
1.7  8.14e-02
```

Curve of the **molar volume of C-S-H** $V_{\text{CSH}}(x)$ (in dm³/mol) as a function of the Ca/Si ratio $x$:

| $x$ (Ca/Si) | $V_{\text{CSH}}$ (dm³/mol) | Corresponding phase |
|---|---|---|
| 0 | 0.0544 | Pure SH (hydrated silica) |
| 0.85 | 0.0814 | Intermediate Ca/Si C-S-H |
| 1.7 | 0.0814 | Calcium-rich C-S-H (jennite) |

This curve is used to compute the volume change of C-S-H during decalcification (transition from $x = 1.7$ to $x = 0$), which contributes to porosity modification.

---

### 7.5 File `CN_satb` — Initial Saturation Grid

```
12
0.04  0.07  0.10  0.13  0.16  0.19  0.22  0.25  0.30  0.35  0.40  0.50
-66e6 -56e6 -47e6 -40e6 -34e6 -30e6 -26e6 -23e6 -22e6 -20e6 -19e6 -15e6
```

2D grid of **(porosity, liquid pressure) correspondence** for initialization. The file contains:
- Line 1: number of columns (12 porosity values)
- Line 2: 12 porosity values from 0.04 to 0.50
- Line 3: 12 corresponding liquid pressure values (in Pa, all negative = capillary)

For a porosity of 0.379 (the material value), interpolation gives an initial liquid pressure $p_l \approx -19$ to $-20 \times 10^6$ Pa, corresponding to a relative humidity of approximately 87% (moderately dried concrete).

---

## 8. Numerical Discretization

The model is discretized using the **cell-centered finite volume method (FVM)**, implemented in Bil via `FVM.h`. The spatial discretization uses a centered difference scheme for fluxes.

The nonlinear system is solved by **Newton-Raphson** with a Jacobian matrix computed either by automatic differentiation (ForwardDiff, if `USE_AUTODIFF` is enabled at compile time) or by numerical differentiation.

The software architecture relies on:
- `MaterialPointMethod.h`: Gauss-point integration of constitutive laws
- `HardenedCementChemistry.h`: local resolution of hardened concrete chemistry
- `CementSolutionDiffusion.h`: computation of ionic diffusive fluxes (coupled Nernst-Planck)
- `CustomValues.h`: typed management of implicit/explicit/constant variables

---

## 9. References

### Durability and Chemistry of Cementitious Materials

- **Thiery, M.** (2005). *Modélisation de la carbonatation atmosphérique des matériaux cimentaires*. PhD thesis, École Nationale des Ponts et Chaussées, Paris. — Basis for the carbonation kinetic model (CH dissolution, calcite precipitation, carbonation front).

- **Dangla, P., et al.** (2024). *Mother model of durability of CBM (Duracem)*. Université Gustave Eiffel. Bil source code. — The Duracem model itself.

- **Glasser, F. P., Marchand, J. & Samson, E.** (2008). Durability of concrete — degradation phenomena involving detrimental chemical reactions. *Cement and Concrete Research*, 38(2), 226–246. — Overview of chemical degradation mechanisms.

### Retention Curves and Moisture Transport

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898.

- **Mualem, Y.** (1976). A new model for predicting the hydraulic conductivity of unsaturated porous media. *Water Resources Research*, 12(3), 513–522.

### Permeability as a Function of Porosity

- **Verma, A. & Pruess, K.** (1988). Thermohydrological conditions and silica redistribution near high-level nuclear wastes emplaced in saturated geological formations. *Journal of Geophysical Research*, 93(B2), 1159–1173. — Intrinsic permeability model as a function of porosity used in Duracem (`frac`, `phi_r`).

- **Kozeny, J.** (1927); **Carman, P.C.** (1937). Alternative Kozeny-Carman model, available in the source code.

### Tortuosity and Diffusion in Cementitious Media

- **Oh, B. H. & Jang, S. Y.** (2004). Prediction of diffusivity of concrete based on simple analytic equations. *Cement and Concrete Research*, 34(3), 463–480. — Oh-Jang tortuosity model (`phi_c = 0.18`, `n = 2.7`) used in Duracem.

- **Bažant, Z. P. & Najjar, L. J.** (1972). Nonlinear water diffusion in nonsaturated concrete. *Matériaux et Constructions*, 5(25), 3–20. — Alternative Bažant-Najjar model available in the code.

### Thermodynamic Equilibrium in Cementitious Solution

- **Lothenbach, B. & Winnefeld, F.** (2006). Thermodynamic modelling of the hydration of Portland cement. *Cement and Concrete Research*, 36(2), 209–226. — Thermodynamic database for hardened concrete chemistry (portlandite, C-S-H, calcite, etc.).

- **Morel, F. M. M. & Hering, J. G.** (1993). *Principles and Applications of Aquatic Chemistry*. John Wiley & Sons, New York. — General thermodynamic framework for aqueous solution chemistry and equilibrium constants.

### Numerical Implementation

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Source code: <https://github.com/Universite-Gustave-Eiffel/bil>
