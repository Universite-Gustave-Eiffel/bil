# Chloricem Model — Durability of Cementitious Materials: Chloride Ingress (1D)

> **Bil model:** `src/Models/ModelFiles/Chloricem.cpp`
>
> **Input file:** `doc/mkdocs/Durability/Chloricem/Chloricem`
>
> **Derived models:** Carbocem · Carbochloricem
>
> **Bil model authors:** P. Dangla et al. (Université Gustave Eiffel)

---

## Table of Contents

1. [Context and Objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and Notation](#3-variables-and-notation)
4. [Mathematical Model](#4-mathematical-model)
   - 4.1 [Conservation Equations](#41-conservation-equations)
   - 4.2 [Ionic Flux Laws](#42-ionic-flux-laws)
   - 4.3 [Pore Solution Chemistry](#43-pore-solution-chemistry)
   - 4.4 [Solid Phases: Dissolution, Precipitation, Decalcification](#44-solid-phases-dissolution-precipitation-decalcification)
   - 4.5 [Chloride Adsorption by C-S-H](#45-chloride-adsorption-by-c-s-h)
   - 4.6 [Hydraulic Transport — Darcy's Law](#46-hydraulic-transport--darcys-law)
   - 4.7 [Evolving Permeability and Tortuosity](#47-evolving-permeability-and-tortuosity)
5. [Boundary and Initial Conditions](#5-boundary-and-initial-conditions)
6. [Detailed Description of Input Files](#6-detailed-description-of-input-files)
   - 6.1 [Main file `Chloricem`](#61-main-file-chloricem)
   - 6.2 [Saturation curve `sat`](#62-saturation-curve-sat)
   - 6.3 [Relative permeability curve `relpermCN`](#63-relative-permeability-curve-relpermmcn)
   - 6.4 [C-S-H molar volume curve `V_CSH`](#64-c-s-h-molar-volume-curve-v_csh)
   - 6.5 [Chloride adsorption curves `Adscl`](#65-chloride-adsorption-curves-adscl)
   - 6.6 [Na/K adsorption curves — `Adsna`, `Adsk`](#66-nak-adsorption-curves--adsna-adsk)
   - 6.7 [Mesh file `Chloricem.msh`](#67-mesh-file-chloricemmsh)
   - 6.8 [Alternative file `m42` (simplified reference model)](#68-alternative-file-m42-simplified-reference-model)
7. [Expected Results and Physical Interpretation](#7-expected-results-and-physical-interpretation)
   - 7.1 [Structure of output files `.tN`](#71-structure-of-output-files-tn)
8. [Numerical Discretization](#8-numerical-discretization)
9. [References](#9-references)

---

## 1. Context and Objective

The Chloricem model is a **durability model for concrete and cementitious materials** (CBM — *Cementitious-Based Materials*). It couples **multi-ionic transport in the liquid phase** with the **chemistry of solid phases in hydrated cement** (portlandite, C-S-H, calcite, Friedel's salt). It is the so-called "parent" model from which the Carbocem (carbonation only) and Carbochloricem (carbonation + chlorides) models are derived.

The test case scenario represents the **penetration of chloride ions** into a saturated concrete or cement paste exposed to a saline solution (seawater, brine, deicing salts). This phenomenon is one of the main causes of reinforcement corrosion in reinforced concrete structures: chlorides exceed a critical threshold at the steel surface and initiate depassivation.

**Domain:** 1D planar, $L = 0.5$ dm = 5 cm.

| Parameter | Value |
|-----------|-------|
| Initial porosity $\phi_0$ | 12.1% |
| Intrinsic permeability $k_l^\text{int}$ | $1.4 \times 10^{-17}$ dm² |
| Initial portlandite content $n_\text{CH}^0$ | 1.64 mol/dm³ |
| Initial C-S-H content $n_\text{CSH}^0$ | 0.635 mol/dm³ |
| Simulation duration | 1 year ($3.1536 \times 10^7$ s) |

The material is initially **sound** (intact portlandite and C-S-H, alkaline pore solution, no chlorides). The left boundary ($x = 0$) is subjected to saline conditions that evolve over time (increasing Cl⁻ concentration interpolated over 24 h). The right boundary ($x = 0.5$ dm) is impermeable (zero Neumann condition on all species except `p_l` and `z_si`, which are held as Dirichlet conditions).

---

## 2. Assumptions

1. **Isothermal**: $T = 298$ K (25 °C). Equilibrium constants, viscosities, and diffusivities are fixed at this temperature.
2. **Saturated medium** ($s_l = 1$, no active gas phase): the saturation curve is constant and equal to 1 over the pressure range considered. The `E_AIR` (air) equation is disabled in this configuration.
3. **Evolving porosity**: dissolution of portlandite and precipitation of Friedel's salt modify the solid volume, and thus the porosity.
4. **Local electroneutrality**: an additional equation constrains the net charge of the solution (via the unknown $\log c_\text{OH}$).
5. **Ideal gases** for computing partial pressures of vapor and gaseous CO₂ (useful in the derived coupled models).
6. **Darcy's law** for advective total mass transport (pressure $p_l$).
7. **Nernst-Planck law** for ionic transport (diffusion + electrical migration).
8. **Local thermodynamic equilibria** for solution chemistry, resolved by the `HardenedCementChemistry` library.
9. **Precipitation kinetics** for calcite and Friedel's salt; **kinetic dissolution** of portlandite coated with a calcite layer (Thiery model).

---

## 3. Variables and Notation

### Primary Unknowns (8 in the Chloricem test case)

| Bil Symbol | Physical meaning | Unit |
|------------|-----------------|------|
| `p_l` | Liquid phase pressure | Pa |
| `z_ca` | Calcium zeta unknown: $z_\text{ca} = n_\text{CcH}/n_\text{ch}^0 + \log_{10} S_\text{CcH}$ | — |
| `z_si` | Silicon zeta unknown: $z_\text{si} = n_\text{Si,CSH}/n_\text{csh}^0 + \log_{10} S_\text{CSH}$ | — |
| `logc_na` | $\log_{10}(c_\text{Na})$, total dissolved Na | mol/dm³ |
| `logc_k` | $\log_{10}(c_\text{K})$, total dissolved K | mol/dm³ |
| `psi` | Dimensionless electric potential $\psi = F\varphi/(RT)$ | — |
| `logc_oh` | $\log_{10}(c_\text{OH})$ (electroneutrality) | mol/dm³ |
| `logc_cl` | $\log_{10}(c_\text{Cl})$, total dissolved Cl⁻ | mol/dm³ |

> **Note on zeta unknowns:** The variable $z_\text{ca}$ simultaneously encodes the quantity of solid (CH + CC) and its saturation state. When $z_\text{ca} > 0$, the solid is present and $S_\text{CcH} = 1$ (equilibrium); when $z_\text{ca} < 0$, the solid is exhausted and $S_\text{CcH} = 10^{z_\text{ca}} < 1$ (undersaturation). The same principle applies to $z_\text{si}$ for C-S-H.

### Computed Secondary Variables

| Symbol | Meaning |
|--------|---------|
| $s_l$ | Liquid saturation degree |
| $\phi$ | Current porosity |
| $n_\text{CH}$ | Molar content of portlandite (mol/dm³) |
| $n_\text{CSH}$ | Molar content of C-S-H (mol/dm³) |
| $n_\text{CC}$ | Molar content of calcite (mol/dm³) |
| $n_\text{FS}$ | Molar content of Friedel's salt (mol/dm³) |
| $x_\text{csh}$ | Ca/Si ratio in C-S-H |
| $c_\text{Cl}^\text{ads}$ | Chlorides adsorbed on C-S-H (mol/dm³) |
| $\tau_l$ | Effective tortuosity for ionic transport |
| $k_\text{perm}$ | Permeability coefficient relative to $\phi_0$ |

### Thermodynamic Constants

| Symbol | Value | Meaning |
|--------|-------|---------|
| $T$ | 298 K | Temperature |
| $R$ | 8.314 J/(mol·K) | Ideal gas constant |
| $K_w$ | $10^{-14}$ mol²/dm⁶ | Ionic product of water |
| $\rho_l^0$ | 1 kg/dm³ | Water density |
| $\mu_l$ | $\approx 10^{-3}$ Pa·s | Dynamic viscosity of water at 298 K |

---

## 4. Mathematical Model

### 4.1 Conservation Equations

The system solves **8 coupled partial differential equations** (in the active Chloricem configuration):

**Total mass (water) conservation:**

$$\frac{\partial M_\text{tot}}{\partial t} + \nabla \cdot \mathbf{W}_\text{tot} = 0$$

with $M_\text{tot} = \rho_l\,\phi\,s_l$ (liquid water mass per unit total volume; vapor is negligible in a saturated medium).

**Molar calcium conservation:**

$$\frac{\partial N_\text{Ca}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Ca} = 0$$

$$N_\text{Ca} = \phi s_l\,c_\text{Ca,l} + n_\text{CH} + x_\text{csh}\,n_\text{CSH} + n_\text{CC} + n_\text{FS,Ca}$$

**Molar silicon conservation:**

$$\frac{\partial N_\text{Si}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Si} = 0$$

$$N_\text{Si} = \phi s_l\,c_\text{Si,l} + n_\text{CSH}$$

**Molar sodium conservation:**

$$\frac{\partial N_\text{Na}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Na} = 0$$

$$N_\text{Na} = \phi s_l\,c_\text{Na,l} + c_\text{Na,ads}^\text{CSH}\,n_\text{CSH}$$

**Molar potassium conservation:**

$$\frac{\partial N_\text{K}}{\partial t} + \nabla \cdot \mathbf{W}_\text{K} = 0$$

$$N_\text{K} = \phi s_l\,c_\text{K,l} + c_\text{K,ads}^\text{CSH}\,n_\text{CSH}$$

**Molar chlorine conservation:**

$$\frac{\partial N_\text{Cl}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Cl} = 0$$

$$N_\text{Cl} = \phi s_l\,c_\text{Cl,l} + c_\text{Cl,ads}^\text{CSH}\,n_\text{CSH} + 2\,n_\text{FS}$$

**Electric charge balance (zero current divergence):**

$$\nabla \cdot \mathbf{W}_q = 0$$

**Local electroneutrality:**

$$\sum_i z_i\,c_i = 0$$

i.e., $2c_{\text{Ca}^{2+}} + c_{\text{Na}^+} + c_{\text{K}^+} + c_{\text{H}^+} - c_{\text{OH}^-} - c_{\text{Cl}^-} - 2c_{\text{CO}_3^{2-}} - \ldots = 0$

This relation, resolved within `HardenedCementChemistry`, yields $c_\text{OH}$ (unknown `logc_oh`).

---

### 4.2 Ionic Flux Laws

The molar flux of each ion $i$ in the liquid phase is given by the **Nernst-Planck** equation:

$$\mathbf{W}_i = -\phi\,s_l\,\tau_l\,D_i^\text{eff}\,\left(\nabla c_i + z_i\,c_i\,\nabla\psi\right) + c_i\,\mathbf{W}_l/\rho_l$$

where:
- $D_i^\text{eff}$ is the effective diffusion coefficient of ion $i$ in water (computed by `CementSolutionDiffusion`);
- $z_i$ is the charge number of the ion;
- $\psi = F\varphi/(RT)$ is the dimensionless electric potential;
- $\tau_l$ is the tortuosity of the liquid phase (see §4.7);
- $\mathbf{W}_l$ is the liquid mass flux (Darcy, see §4.6).

The **electric current** $\mathbf{W}_q$ is the charge-weighted sum:

$$\mathbf{W}_q = \sum_i z_i\,\mathbf{W}_i$$

The condition $\nabla \cdot \mathbf{W}_q = 0$ (charge conservation) determines the electric potential gradient $\nabla\psi$ that ensures local flux neutrality.

---

### 4.3 Pore Solution Chemistry

The chemistry is resolved at each integration point by the **`HardenedCementChemistry`** library, which computes all ionic concentrations at thermodynamic equilibrium from the primary variables. The aqueous species taken into account include notably:

| Species | Comment |
|---------|---------|
| Ca²⁺, CaOH⁺, CaOH₂(aq) | Calcium |
| H₂SiO₄²⁻, H₃SiO₄⁻, H₄SiO₄ | Silicate |
| Na⁺, NaOH(aq) | Sodium |
| K⁺, KOH(aq) | Potassium |
| H⁺, OH⁻ | Water |
| CO₃²⁻, HCO₃⁻, CO₂(aq) | Carbon |
| Cl⁻ | Chloride |

The `CementSolutionDiffusion` library provides the effective diffusion coefficients for each species as a function of solution composition and temperature.

---

### 4.4 Solid Phases: Dissolution, Precipitation, Decalcification

#### Portlandite Ca(OH)₂ — CH

Portlandite dissolves according to a kinetic law controlled by CO₂ diffusion through a calcite layer that forms on the crystal surface (Thiery, 2005 model). In the absence of carbonation (CO₂ = 0), CH is at equilibrium:

$$S_\text{CH} = \frac{[\text{Ca}^{2+}][\text{OH}^-]^2}{K_\text{sp,CH}}$$

The CH content is governed by the unknown $z_\text{ca}$:

$$n_\text{CH} = n_\text{ch}^0 \cdot \max(z_\text{ca},\, 0), \qquad \log_{10} S_\text{CH} = \min(z_\text{ca},\, 0)$$

#### C-S-H (Calcium Silicate Hydrates)

C-S-H form a solid solution of variable composition, characterized by the Ca/Si ratio = $x_\text{csh}$. The progressive decalcification of C-S-H is modeled by a decrease in $x_\text{csh}$ as the solution becomes depleted in calcium. The molar volume of C-S-H varies linearly with $x_\text{csh}$ (curve `V_CSH`):

$$V_\text{CSH}(x) = \frac{x_1 - x}{x_1 - x_0}\,V_0 + \frac{x - x_0}{x_1 - x_0}\,V_1$$

with $x_0 = 0$, $V_0 = 5.44 \times 10^{-2}$ dm³/mol (tobermorite) and $x_1 = 0.85$, $V_1 = 8.14 \times 10^{-2}$ dm³/mol.

#### Calcite CaCO₃ — CC

Calcite precipitation/dissolution follows **first-order kinetics**:

$$\frac{d n_\text{CC}}{d t} = r_\text{CC} \cdot (S_\text{CC} - 1), \qquad r_\text{CC} = 10^{-6}\ \text{mol/(dm³·s)}$$

Calcite only forms if the solution is supersaturated with respect to CaCO₃ ($S_\text{CC} > 1$), which requires the presence of dissolved CO₂. In the pure chloride test case (no external CO₂), calcite remains absent.

#### Friedel's Salt Ca₄Al₂O₆Cl₂·10H₂O

Friedel's salt is the solid phase that chemically binds chlorides. Its precipitation follows an analogous kinetic law:

$$\frac{d n_\text{FS}}{d t} = r_\text{FS} \cdot (S_\text{FS} - 1), \qquad r_\text{FS} = 10^{-6}\ \text{mol/(dm³·s)}$$

Friedel's salt contains 2 moles of Cl per mole, contributing significantly to chloride binding in concrete.

#### Current Porosity

Porosity evolves as a function of the solid phase volumes:

$$\phi = \phi_0 - \Delta V_s^\text{CSH} - \Delta V_s^\text{CH} - \Delta V_s^\text{CC} - \Delta V_s^\text{FS}$$

$$\phi = \phi_0 - V_\text{CSH}(x)\,(n_\text{CSH} - n_\text{csh}^0) - V_\text{CH}\,(n_\text{CH} - n_\text{ch}^0) - V_\text{CC}\,n_\text{CC} - V_\text{FS}\,n_\text{FS}$$

---

### 4.5 Chloride Adsorption by C-S-H

C-S-H physically adsorb Cl⁻ ions according to a **Langmuir isotherm** as a function of the free chloride concentration $c_\text{Cl}$ and the current Ca/Si ratio $x_\text{csh}$:

$$c_\text{Cl,ads}^\text{CSH} = \frac{\alpha(x_\text{csh})\,c_\text{Cl}}{1 + \beta(x_\text{csh})\,c_\text{Cl}}$$

The coefficients $\alpha$ and $\beta$ are tabulated as a function of $x_\text{csh}$ in the `Adscl` curve. In the test case:

$$\alpha = 3.192, \qquad \beta = 26.6 \text{ dm³/mol}$$

These values are **independent of** $x_\text{csh}$ in this example (constant curve over $[0.5,\, 1.5]$).

Sodium and potassium adsorption is modeled by a **linear isotherm** truncated at $c = 0.3$ mol/dm³:

$$c_\text{Na,ads}^\text{CSH} = R_\text{Na}(x_\text{csh})\,\min(c_\text{Na},\, 0.3\ \text{mol/dm³})$$

In the test case, $R_\text{Na} = R_\text{K} = 0$ (curves `Adsna`, `Adsk`), so alkalis are not adsorbed.

---

### 4.6 Hydraulic Transport — Darcy's Law

The total liquid water mass flux (in a saturated medium, without gravity) is:

$$\mathbf{W}_\text{tot} = -\frac{\rho_l\,k_l^\text{int}\,k_{rl}(s_l)\,k_\text{perm}(\phi)}{\mu_l}\,\nabla p_l$$

The coefficient $k_\text{perm}(\phi)$ models the permeability reduction due to clogging by reaction products (see §4.7). In the saturated test case, $k_{rl} = 1$.

---

### 4.7 Evolving Permeability and Tortuosity

#### Tortuosity — Oh & Jang (2004) model

The tortuosity of the liquid phase for ionic transport is computed according to the empirical Oh & Jang model:

$$\tau_l(\phi,\, s_l) = f(\phi,\, s_l)$$

This model is calibrated on experimental diffusivity data for concretes and cement pastes, and accounts for the combined effect of porosity and saturation.

#### Permeability — Verma & Pruess (1988) model

The permeability reduction due to clogging is modeled by the Verma & Pruess model:

$$k_\text{perm}(\phi) = \left(\frac{\phi - \phi_r}{\phi_0 - \phi_r}\right)^n \cdot f_\text{pore}$$

where $\phi_r = 0.70\,\phi_0$ is the threshold porosity below which permeability vanishes, and the shape factor $f_\text{pore}$ depends on the fractional length of pore bodies `frac` = 0.8.

---

## 5. Boundary and Initial Conditions

### Initial Conditions

The concrete is **initially sound**, in equilibrium with the alkaline pore solution:

| Unknown | Initial value | Comment |
|---------|--------------|---------|
| `p_l` | 0 Pa | Reference pressure |
| `logc_cl` | $-4$ (i.e., $10^{-4}$ mol/dm³) | Trace chlorides |
| `z_ca` | 1 | Portlandite present ($n_\text{CH} = n_\text{ch}^0$) |
| `z_si` | $-0.92$ | C-S-H present at initial composition |
| `logc_na` | $-0.64$ (i.e., ≈ 0.23 mol/dm³) | Initial sodium |
| `logc_k` | $-4$ (i.e., $10^{-4}$ mol/dm³) | Low initial potassium |
| `psi` | 0 | Zero reference potential |

> These values correspond to a strongly alkaline pore solution (pH ≈ 13.5, portlandite at equilibrium) characteristic of hydrated OPC concrete.

### Boundary Conditions

| Boundary | $x$ [dm] | Variable | Type | Value |
|----------|----------|----------|------|-------|
| Left (exposure) | 0 | `logc_cl` | Dirichlet | $-4$ at $t=0$ → $-0.28$ at $t=86400$ s |
| Left | 0 | `z_ca` | Dirichlet | $1 \to -0.208$ (progressive decalcification) |
| Left | 0 | `logc_na` | Dirichlet | $-0.64 \to -2.49$ |
| Left | 0 | `logc_k` | Dirichlet | $-4$ (constant) |
| Left | 0 | `psi` | Dirichlet | 0 |
| Right (symmetry) | 0.5 | `p_l` | Dirichlet | 0 Pa |
| Right | 0.5 | `z_si` | Dirichlet | $-0.92$ (imposed C-S-H) |

The left boundary conditions evolve **linearly in time** between $t = 0$ and $t = 86400$ s (1 day), simulating the progressive establishment of contact with the saline solution. Beyond one day, the values remain constant (the function is interpolated from the 2 points provided, which implies constant extrapolation beyond that point).

> **Physical interpretation:** The chloride front penetrates from the left boundary (surface exposed to seawater or a saline solution) toward the right boundary (material core, impermeable or symmetry plane). Over 1 year, the front reaches a few millimeters to a few centimeters depending on the effective diffusivity.

---

## 6. Detailed Description of Input Files

### 6.1 Main File `Chloricem`

```
Units
Length = decimeter
Mass   = hectogram
```

**Unit system:** The model operates in **decimeters** (length) and **hectograms** (mass). All inputs must be consistent with this system:
- Lengths in dm (1 dm = 10 cm)
- Concentrations in mol/dm³ (= mol/L)
- Pressures in Pa
- Time in seconds

---

```
Geometry
1 plan
```

**Geometry:** 1D planar problem (x-axis, no cylindrical or spherical symmetry).

---

```
Mesh
4 0. 0. 0.5 0.5
5.e-3
1 100 1
1 1 1
```

**Mesh:** Defines a 1D domain from 0 to 0.5 dm with 100 elements of size $5 \times 10^{-3}$ dm = 0.5 mm. The mesh generates 103 nodes (including boundary nodes). The first 4 values `4 0. 0. 0.5 0.5` define the corner coordinates of the domain; the format is read by Bil to build the structured mesh.

> **Tip:** The `Chloricem.msh` file (Gmsh v2.0 format) is the exported version of this mesh, used when Bil is run with an external `.msh` file rather than internal generation.

---

```
Material
Model = Chloricem
InitialPorosity = 0.121
IntrinsicPermeability = 1.4e-17
InitialContent_portlandite = 1.64
InitialContent_csh = 0.635
InitialConcentration_sodium = -1
InitialConcentration_potassium = -1
FractionalLengthOfPoreBodies = 0.8
PorosityFractionAtVanishingPermeability = 0.70
MinimumPorosity = 0.01
PrecipitationRate_calcite = 1.e-6
PrecipitationRate_friedelsalt = 1.e-6
Curves = sat       p_c = ...   s_l = ...
Curves = relpermCN s_l = ...   kl_r = ...
Curves = V_CSH     x = ...     v_csh = ...
Curves = Adscl     x = ...     alpha = ...  beta = ...
Curves = Adsna     x = ...     adsna = ...
Curves = Adsk      x = ...     adsk = ...
```

Material parameters read by the `ReadMatProp` function:

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `InitialPorosity` | 0.121 | $\phi_0$ = 12.1% |
| `IntrinsicPermeability` | $1.4 \times 10^{-17}$ dm² | $k_l^\text{int}$ (in dm², not m²) |
| `InitialContent_portlandite` | 1.64 mol/dm³ | $n_\text{CH}^0$ |
| `InitialContent_csh` | 0.635 mol/dm³ | $n_\text{CSH}^0$ |
| `InitialConcentration_sodium` | $-1$ | Negative value = automatic computation via `HardenedCementChemistry` |
| `InitialConcentration_potassium` | $-1$ | Same |
| `FractionalLengthOfPoreBodies` | 0.8 | `frac` parameter of the Verma-Pruess model |
| `PorosityFractionAtVanishingPermeability` | 0.70 | $\phi_r/\phi_0$: clogging threshold |
| `MinimumPorosity` | 0.01 | $\phi_\text{min}$: numerical porosity floor |
| `PrecipitationRate_calcite` | $10^{-6}$ mol/(dm³·s) | Calcite kinetic rate $r_\text{CC}$ |
| `PrecipitationRate_friedelsalt` | $10^{-6}$ mol/(dm³·s) | Friedel's salt kinetic rate $r_\text{FS}$ |

---

```
Fields
1
Value = 1.      Gradient = 0 0 0      Point = 0 0 0
```

**Spatial fields:** A single uniform field of value 1, used as a multiplier in the initial and boundary conditions (a neutral scaling factor here).

---

```
Initialization
7
Region = 2    Unknown = p_l       Field = 0    Function = 0
Region = 2    Unknown = logc_cl   Field = 1    Function = 1
Region = 2    Unknown = z_ca      Field = 1    Function = 2
Region = 2    Unknown = z_si      Field = 1    Function = 0
Region = 2    Unknown = logc_na   Field = 1    Function = 3
Region = 2    Unknown = logc_k    Field = 1    Function = 4
Region = 2    Unknown = psi       Field = 0    Function = 0
```

**Initialization:** The 7 unknowns are initialized in region 2 (the entire volumetric domain). The syntax `Field = i  Function = j` means that the initial value is `Field[i] × Function[j](t=0)`:

| Unknown | Field | Function | Initial value | Interpretation |
|---------|-------|----------|--------------|----------------|
| `p_l` | 0 (=0) | 0 (=0) | 0 Pa | Reference pressure |
| `logc_cl` | 1 (=1) | 1: F(0)=−4 | −4 | $c_\text{Cl} = 10^{-4}$ mol/dm³ |
| `z_ca` | 1 (=1) | 2: F(0)=1 | 1 | Intact portlandite |
| `z_si` | 1 (=1) | 0 (=0) | 0 | **Note**: Z_si=0 means $n_\text{CSH} = n_\text{csh}^0$, $S_\text{CSH}=1$ |
| `logc_na` | 1 (=1) | 3: F(0)=−0.639 | −0.639 | $c_\text{Na} \approx 0.23$ mol/dm³ |
| `logc_k` | 1 (=1) | 4: F(0)=−4 | −4 | $c_\text{K} = 10^{-4}$ mol/dm³ |
| `psi` | 0 (=0) | 0 (=0) | 0 | Zero potential |

---

```
Functions
5
N = 2     F(0) = -4         F(86400) = -0.28133
N = 2     F(0) = 1          F(86400) = -2.7766
N = 2     F(0) = -0.92082   F(86400) = -0.20761
N = 2     F(0) = -0.63927   F(86400) = -2.4948
N = 1     F(0) = -4
```

**Time functions:** 5 piecewise linear functions used for initial and boundary conditions:

| Function | $t=0$ | $t=86400$ s (1 day) | Usage |
|----------|-------|---------------------|-------|
| 1 | $-4$ | $-0.281$ | `logc_cl`: from $10^{-4}$ mol/dm³ to $0.52$ mol/dm³ |
| 2 | $1$ | $-2.777$ | `z_ca`: progressive portlandite dissolution (decalcification) |
| 3 | $-0.921$ | $-0.208$ | `z_si`: C-S-H decalcification |
| 4 | $-0.639$ | $-2.495$ | `logc_na`: sodium dilution by seawater |
| 5 | $-4$ | (1 point) | `logc_k`: constant |

> These values correspond to boundary conditions at the left face representing contact with seawater (approximately 0.5 mol/dm³ NaCl, neutral to slightly basic pH, diluted calcium).

---

```
Boundary Conditions
7
Region = 2    Unknown = p_l       Field = 0    Function = 0
Region = 1    Unknown = logc_cl   Field = 1    Function = 1
Region = 1    Unknown = z_ca      Field = 1    Function = 2
Region = 1    Unknown = logc_na   Field = 1    Function = 3
Region = 1    Unknown = logc_k    Field = 1    Function = 4
Region = 1    Unknown = psi       Field = 0    Function = 0
Region = 2    Unknown = z_si      Field = 1    Function = 0
```

**Boundary conditions:**

- **Region 1** (left boundary, $x = 0$): Dirichlet conditions on ionic species. These values evolve over time via functions 1–4.
- **Region 2** (right boundary, $x = 0.5$ dm): Dirichlet on `p_l` (reference pressure) and `z_si` (C-S-H at initial state, material core).
- **`logc_oh`** is not listed because it is determined implicitly by the electroneutrality equation.

> The condition `psi = 0` at the exposure boundary sets the reference for the electric potential.

---

```
Dates
13
0.  2.628e6 5.256e6 7.884e6 1.0512e7 1.314e7 1.5768e7 1.8396e7 2.1024e7 2.3652e7 2.628e7 2.8908e7 3.1536e7
```

**Output times:** 13 output times regularly spaced approximately 1 month apart ($\approx 2.628 \times 10^6$ s), covering 1 year. Each time generates a file `Chloricem.tN`.

---

```
Objective Variations
logc_cl    = 1.e-1
p_l        = 1.e5
z_ca       = 1.
z_si       = 1.
logc_na    = 1.
logc_k     = 1.
psi        = 1.e3
logc_oh    = 1.e-2
```

**Objective variations (adaptive time step control):** These values define the maximum allowable variation of each unknown per time step. If an unknown varies more than its objective value, the time step is reduced. `logc_oh = 1e-2` is the strictest constraint because pH is highly sensitive.

---

```
Iterative Process
Iterations = 20
Tolerance = 1.e-6
Repetition = 2
```

**Newton-Raphson iterative process:** Maximum 20 Newton iterations, tolerance $10^{-6}$ on the residual, 2 repetition attempts in case of convergence failure (with time step reduction).

---

```
Time Steps
Dtini = 1.e-1
Dtmax = 1.e6
```

**Time steps:** Very small initial step (0.1 s) to handle the large initial discontinuities in the boundary conditions, then adaptive growth up to $10^6$ s ≈ 11.6 days.

---

### 6.2 Saturation Curve `sat`

```
Curves = sat  p_c = Range{x1 = 0, x2 = 1, n = 2}  s_l = Expressions(1){s_l = 1}
```

The medium is considered **fully saturated** ($s_l = 1$) regardless of the capillary pressure. This simplification is valid for concrete that is submerged or in permanent contact with free water. The curve is defined by 2 points with $s_l = 1$ constant.

---

### 6.3 Relative Permeability Curve `relpermCN`

```
Curves = relpermCN  s_l = Range{x1 = 0, x2 = 1, n = 101}
                    kl_r = Mualem_liq(1){m = 0.45}
```

Liquid relative permeability computed by the **Mualem-van Genuchten model** with shape parameter $m = 0.45$:

$$k_{rl}(s_l) = \sqrt{s_l}\,\left[1 - \left(1 - s_l^{1/m}\right)^m\right]^2$$

In a saturated medium ($s_l = 1$), $k_{rl} = 1$. This curve is defined over 101 points from $s_l = 0$ to $s_l = 1$.

---

### 6.4 C-S-H Molar Volume Curve `V_CSH`

```
Curves = V_CSH  x = Range{x1 = 0, x2 = 0.85, n=2}
                v_csh = Expressions(1){x0=0 ; v0=5.44e-2 ; x1=0.85 ; v1=8.14e-2 ;
                                       v_csh = (x1-x)/(x1-x0)*v0 + (x-x0)/(x1-x0)*v1}
```

**Linear** interpolation of the C-S-H molar volume as a function of the Ca/Si ratio $x$:

| $x$ (Ca/Si) | $V_\text{CSH}$ [dm³/mol] | Phase |
|-------------|--------------------------|-------|
| 0 | 0.0544 | Tobermorite (Si-rich C-S-H) |
| 0.85 | 0.0814 | Typical OPC C-S-H |

This volume enters directly into the calculation of the current porosity $\phi$.

---

### 6.5 Chloride Adsorption Curves `Adscl`

```
Curves = Adscl  x = Range{x1 = 0.5, x2 = 1.5, n = 100}
                alpha = Expressions(1){alpha = 3.192 ;}
                beta  = Expressions(1){beta = 26.6 ;}
```

Langmuir isotherm for chloride adsorption. The coefficients are constant over $x_\text{csh} \in [0.5,\, 1.5]$:

$$c_\text{Cl,ads} = \frac{3.192\,c_\text{Cl}}{1 + 26.6\,c_\text{Cl}} \quad [\text{mol Cl/mol CSH}]$$

The maximum adsorption capacity (Langmuir plateau) is $\alpha/\beta = 3.192/26.6 \approx 0.12$ mol Cl/mol CSH.

---

### 6.6 Na/K Adsorption Curves — `Adsna`, `Adsk`

```
Curves = Adsna  x = Range{x1 = 0, x2 = 1, n = 2}  adsna = Expressions(1){adsna = 0;}
Curves = Adsk   x = Range{x1 = 0, x2 = 1, n = 2}  adsk  = Expressions(1){adsk  = 0;}
```

Zero adsorption for Na and K in this test case. These curves are required by the model but are inactive here.

---

### 6.7 Mesh File `Chloricem.msh`

File in **Gmsh v2.0 format**, generated automatically. It contains:
- 103 nodes on the segment $[0,\, 0.5]$ dm;
- 102 interior linear elements (LINE2);
- 2 point elements (POINT) at the ends (regions 1 and 2, corresponding to the left and right boundary nodes).

Physical regions `1` (left boundary) and `2` (right boundary / volumetric domain) are used to assign boundary conditions.

---

### 6.8 Alternative File `m42` (Simplified Reference Model)

The `m42` file uses reference model **42** (a simplified, purely diffusive chloride transport model) on a shorter domain ($L = 0.05$ dm = 5 mm). It serves as a **reference solution** to validate the behavior of Chloricem: the two models should produce consistent chloride profiles within the limits of the simplifications.

Model 42 uses 8 algebraically different unknowns (`c_cl`, `z_caoh2`, `z_c3a`, `z_sf`, `c_na`, `c_k`, `z_aloh3`, `psi`) and material parameters directly in SI units (not dm/hg): `D_Cl = 2.032e-13 m²/s`.

---

## 7. Expected Results and Physical Interpretation

### 7.1 Structure of Output Files `.tN`

Each output time generates a text file `<n>.tN` (e.g., `Carbochloricem.t1`). The file begins with a comment header (`#`) followed by one line of data per mesh node.

```
# Version 2.8.3, ...
# Time = 8.603352e+04
# Model = Carbochloricem
# Number of views = 70
# Numbers of components per view = 1 1 1 ...
# Coordinates(1)  p_l(4)  saturation(5)  ...
```

> Columns 2 and 3 (y and z coordinates) are present in the file but not named in the header; they equal 0 in 1D planar geometry.

The table below lists the 73 named columns of the **Carbochloricem** model (a model derived from Chloricem that includes carbonation). The unit system is that of the input file: lengths in **dm**, mass in **hg** (hectogram = 100 g), time in **s**.

| Col. | Bil name | Description | Unit |
|-----:|---------|-------------|------|
| 1 | `Coordinates` | Spatial coordinate $x$ | dm |
| 4 | `p_l` | Liquid phase pressure | Pa |
| 5 | `saturation` | Saturation degree $s_l$ | — |
| 6 | `porosity` | Porosity $\phi$ | — |
| 7 | `c_co2` | Total dissolved CO₂ concentration | mol/dm³ |
| 8 | `c_ca_l` | Total Ca concentration in liquid phase | mol/dm³ |
| 9 | `c_si_l` | Total Si concentration in liquid phase | mol/dm³ |
| 10 | `c_na_l` | Total Na concentration in liquid phase | mol/dm³ |
| 11 | `c_k_l` | Total K concentration in liquid phase | mol/dm³ |
| 12 | `c_c_l` | Total dissolved inorganic carbon concentration | mol/dm³ |
| 13 | `c_cl_l` | Total Cl concentration in liquid phase | mol/dm³ |
| 14 | `n_CH` | Portlandite Ca(OH)₂ content | mol/dm³ |
| 15 | `s_ch` | Portlandite saturation index | — |
| 16 | `n_CSH` | C-S-H content | mol/dm³ |
| 17 | `x_csh` | Ca/Si ratio in C-S-H | — |
| 18 | `s_sh` | C-S-H saturation index | — |
| 19 | `n_CC` | Calcite CaCO₃ content | mol/dm³ |
| 20 | `s_cc` | Calcite saturation index | — |
| 21 | `n_Friedel's salt` | Friedel's salt content | mol/dm³ |
| 22 | `s_friedelsalt` | Friedel's salt saturation index | — |
| 23 | `c_h` | H⁺ concentration | mol/dm³ |
| 24 | `c_oh` | OH⁻ concentration | mol/dm³ |
| 25 | `ph` | Solution pH | — |
| 26 | `c_ca` | Ca²⁺ concentration | mol/dm³ |
| 27 | `c_caoh` | CaOH⁺ concentration | mol/dm³ |
| 28 | `c_h2sio4` | H₂SiO₄²⁻ concentration | mol/dm³ |
| 29 | `c_h3sio4` | H₃SiO₄⁻ concentration | mol/dm³ |
| 30 | `c_h4sio4` | H₄SiO₄ (neutral silicic acid) concentration | mol/dm³ |
| 31 | `c_na` | Na⁺ concentration | mol/dm³ |
| 32 | `c_naoh` | NaOH concentration | mol/dm³ |
| 33 | `c_k` | K⁺ concentration | mol/dm³ |
| 34 | `c_koh` | KOH concentration | mol/dm³ |
| 35 | `c_co3` | CO₃²⁻ concentration | mol/dm³ |
| 36 | `c_hco3` | HCO₃⁻ concentration | mol/dm³ |
| 37 | `c_cah2sio4` | CaH₂SiO₄ concentration | mol/dm³ |
| 38 | `c_cah3sio4` | CaH₃SiO₄⁺ concentration | mol/dm³ |
| 39 | `c_cahco3` | CaHCO₃⁺ concentration | mol/dm³ |
| 40 | `c_caco3aq` | Aqueous CaCO₃ concentration | mol/dm³ |
| 41 | `c_caoh2aq` | Aqueous Ca(OH)₂ concentration | mol/dm³ |
| 42 | `c_nahco3` | NaHCO₃ concentration | mol/dm³ |
| 43 | `c_naco3` | NaCO₃⁻ concentration | mol/dm³ |
| 44 | `c_cl` | Cl⁻ concentration | mol/dm³ |
| 45 | `n_Ca` | Total Ca content (all phases) | mol/dm³ |
| 46 | `n_Si` | Total Si content (all phases) | mol/dm³ |
| 47 | `n_Na` | Total Na content (all phases) | mol/dm³ |
| 48 | `n_K` | Total K content (all phases) | mol/dm³ |
| 49 | `n_C` | Total C content (all phases) | mol/dm³ |
| 50 | `n_Cl` | Total Cl content (liquid + adsorbed + Friedel's salt) | mol/dm³ |
| 51 | `total mass` | Liquid mass per unit material volume | hg/dm³ |
| 52 | `total mass flow` | Total mass flux (Darcy) | hg/(dm²·s) |
| 53 | `carbon mass flow` | Carbon mass flux | hg/(dm²·s) |
| 54 | `calcium mass flow` | Calcium mass flux | hg/(dm²·s) |
| 55 | `silicon mass flow` | Silicon mass flux | hg/(dm²·s) |
| 56 | `sodium mass flow` | Sodium mass flux | hg/(dm²·s) |
| 57 | `potassium mass flow` | Potassium mass flux | hg/(dm²·s) |
| 58 | `chlorine mass flow` | Chlorine mass flux | hg/(dm²·s) |
| 59 | `Ca/Si ratio` | Global Ca/Si ratio (solid phases) | — |
| 60 | `Electric potential` | Dimensionless electric potential $\psi = F\varphi/(RT)$ | — |
| 61 | `charge` | Ionic charge density of the solution | mol/dm³ |
| 62 | `I` | Ionic strength of the solution | mol/dm³ |
| 63 | `v_csh` | Molar volume of C-S-H | dm³/mol |
| 64 | `v_ch` | Molar volume of portlandite | dm³/mol |
| 65 | `v_cc` | Molar volume of calcite | dm³/mol |
| 66 | `air pressure` | Partial pressure of air (gas phase) | Pa |
| 67 | `humidity` | Relative humidity | — |
| 68 | `CO2 ppm` | Gaseous CO₂ content | ppm |
| 69 | `gas pressure` | Total gas phase pressure | Pa |
| 70 | `tortuosity to liquid` | Tortuosity factor for liquid phase transport | — |
| 71 | `permeability coef` | Relative permeability coefficient ($k_\text{perm}/k_0$) | — |
| 72 | `adsorbed chloride` | Chlorides adsorbed on C-S-H | mol/dm³ |
| 73 | `liquid mass density` | Liquid phase mass density | hg/dm³ |

> **Unit conversion reminders:** 1 hg/dm³ = 100 g/L = 100 kg/m³; 1 mol/dm³ = 1 mol/L = 1 M.

---

### Initial State ($t=0$)

At $t = 0$, the material is uniform and sound:

| Variable | Uniform value |
|----------|--------------|
| $p_l$ | 0 Pa |
| $s_l$ | 1.0 |
| $\phi$ | 12.1% |
| $c_\text{Cl}$ | $10^{-4}$ mol/dm³ (traces) |
| $n_\text{CH}$ | 1.64 mol/dm³ |
| $n_\text{CSH}$ | 0.635 mol/dm³ |
| $x_\text{CSH}$ (Ca/Si) | 1.79 |
| pH | ≈ 13.5 (strongly alkaline) |
| $c_\text{Ca,l}$ | ≈ 5.8×10⁻⁴ mol/dm³ |

### Evolution Over 1 Year ($t = 3.1536 \times 10^7$ s)

Analysis of the output files `Chloricem.t12` at node $x = 0$ (exposed boundary) reveals after 1 year:

| Variable | $t=0$ | $t=1$ year (boundary) | Evolution |
|----------|-------|----------------------|-----------|
| $n_\text{CH}$ | 1.64 mol/dm³ | ≈ 0 mol/dm³ | Portlandite **fully dissolved** at the surface |
| $n_\text{CSH}$ | 0.635 mol/dm³ | ≈ 0.0017 mol/dm³ | C-S-H **nearly exhausted** at the surface |
| $n_\text{CC}$ | 0 | ≈ 0 | No calcite (no CO₂) |
| $c_\text{Cl}$ | $10^{-4}$ | ≈ 0.52 mol/dm³ | Chloride front established at the surface |
| pH | 13.5 | ≈ 9.6 | **Depassivation** (pH < 11.5) |
| $\phi$ | 12.1% | ≈ 17.5% | Increase (dissolution without re-precipitation) |

### Physics of the Penetration Front

1. **Chloride front**: Cl⁻ ions penetrate from the left boundary primarily by diffusion (secondary migration). The profile is of erfc type (complementary error function) for a constant diffusion coefficient.

2. **Reinforcement depassivation**: the standard depassivation criterion is reached when the free chloride content exceeds $0.4\ \text{kg/m³}$ of concrete, i.e., approximately 0.01 mol/dm³. Over 1 year in this configuration, this threshold is reached in the first few millimeters.

3. **Portlandite dissolution**: the pH decrease induced by boundary dilution causes CH to dissolve, releasing Ca²⁺ that partially buffers the solution.

4. **Binding by adsorption and Friedel's salt**: a fraction of the chlorides is trapped in adsorbed form on C-S-H and precipitated as Friedel's salt. This binding retards the advance of the free chloride front.

5. **Porosity evolution**: dissolution of CH and C-S-H without re-precipitation creates a degraded zone of increased porosity at the surface, progressively accelerating transport.

![Chloricem - Simulation results](Chloricem_results.png)

---

## 8. Numerical Discretization

The model is discretized using the **cell-centered finite volume method** (FVM), implemented in Bil via `FVM.h`. The spatial discretization uses a centered scheme for diffusive fluxes and the electric flux.

The nonlinear system (8 coupled equations × 103 nodes = 824 degrees of freedom) is solved at each time step by the **Newton-Raphson method**, with a Jacobian matrix assembled by `SetTangentMatrix()`.

The **adaptive time step** is controlled by the maximum variation of the unknowns (OBJE in Bil): the step is reduced if an unknown varies more than its objective value, and increased otherwise, up to the maximum `Dtmax = 1e6` s.

The internal chemistry (resolution of thermodynamic equilibria and solid phase saturations) is performed at each integration point by the **`HardenedCementChemistry`** library, via the `Integrate()` method of the `MaterialPointMethod`.

---

## 9. References

### Ionic Transport Models in Porous Media

- **Nernst, W.** (1888). Zur Kinetik der in Lösung befindlichen Körper. *Zeitschrift für Physikalische Chemie*, 2, 613–637. — Nernst-Planck equation for ionic diffusion coupled with electrical migration.

- **Planck, M.** (1890). Ueber die Potentialdifferenz zwischen zwei verdünnten Lösungen binärer Elektrolyte. *Annalen der Physik*, 276(8), 561–576. — Nernst-Planck formalism.

- **Samson, E. & Marchand, J.** (1999). Numerical solution of the extended Nernst-Planck model. *Journal of Colloid and Interface Science*, 215(1), 1–8. — Numerical implementation of multi-ionic transport in concrete.

### Concrete Durability — Chloride Ingress

- **Tuutti, K.** (1982). *Corrosion of Steel in Concrete*. Swedish Cement and Concrete Research Institute, Stockholm. — Classic two-phase service life model: initiation + corrosion propagation.

- **Andrade, C., Alonso, C. & Molina, F.J.** (1993). Cover cracking as a function of bar corrosion: Part I — Experimental test. *Materials and Structures*, 26(8), 453–464. — Reinforcement depassivation criteria.

- **Tang, L. & Nilsson, L.O.** (1992). Rapid determination of the chloride diffusivity in concrete by applying an electric field. *ACI Materials Journal*, 89(1), 49–53. — RCPT method and chloride diffusion coefficients.

### Chemistry of Hydrated Cement

- **Taylor, H.F.W.** (1997). *Cement Chemistry* (2nd ed.). Thomas Telford, London. — Fundamental reference on the composition and chemistry of cement hydrates.

- **Lothenbach, B. & Winnefeld, F.** (2006). Thermodynamic modelling of the hydration of Portland cement. *Cement and Concrete Research*, 36(2), 209–226. — Thermodynamic modeling of solid phases in hydrated cement.

### Chloride Adsorption by C-S-H

- **Barbarulo, R., Marchand, J., Snyder, K.A. & Prené, S.** (2000). Dimensional analysis of ionic transport problems in hydrated cement systems. *Cement and Concrete Research*, 30(12), 1955–1960. — Modeling of chloride binding.

- **Zibara, H., Hooton, R.D., Thomas, M.D.A. & Stanish, K.** (2008). Influence of the C/S and C/A ratios of hydration products on the chloride ion binding capacity of lime-SF and lime-MK mixtures. *Cement and Concrete Research*, 38(3), 422–426. — Dependence of chloride adsorption on C-S-H composition.

### Friedel's Salt

- **Birnin-Yauri, U.A. & Glasser, F.P.** (1998). Friedel's salt, Ca₂Al(OH)₆(Cl,OH)·2H₂O: its solid solutions and their role in chloride binding. *Cement and Concrete Research*, 28(12), 1713–1723. — Thermodynamic properties of Friedel's salt.

### Effective Tortuosity and Diffusivity

- **Oh, B.H. & Jang, S.Y.** (2004). Prediction of diffusivity of concrete based on simple analytic equations. *Cement and Concrete Research*, 34(3), 463–480. — Oh-Jang tortuosity model for concrete.

- **Millington, R.J. & Quirk, J.P.** (1961). Permeability of porous solids. *Transactions of the Faraday Society*, 57, 1200–1207. — Classical tortuosity model.

### Permeability and Clogging

- **Verma, A. & Pruess, K.** (1988). Thermohydrological conditions and silica redistribution near high-level nuclear wastes emplaced in saturated geological formations. *Journal of Geophysical Research*, 93(B2), 1159–1173. — Permeability reduction model due to precipitation.

### Portlandite Dissolution and Kinetics

- **Thiery, M.** (2005). *Modélisation de la carbonatation atmosphérique des matériaux cimentaires*. PhD thesis, École Nationale des Ponts et Chaussées, Paris. — Kinetics of portlandite dissolution coated with calcite; parameters $a_2$, $c_2$, $D$.

- **Dangla, P., Thiery, M. & Villain, G.** (2004). Modélisation couplée des transferts et des réactions chimiques dans les matériaux cimentaires exposés à la carbonatation atmosphérique. *Revue Française de Génie Civil*, 8(5), 571–589. — Theoretical framework of the Carbocem/Chloricem model.

### Numerical Implementation

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Source code: <https://github.com/Universite-Gustave-Eiffel/bil>
