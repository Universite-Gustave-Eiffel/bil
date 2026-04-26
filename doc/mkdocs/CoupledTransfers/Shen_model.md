# Shen Model — Concrete Carbonation by Supercritical CO₂

> **Bil model:** `src/Models/ModelFiles/Shen.c`

> **Input file:** `doc/mkdocs/CoupledTransfers/Shen/Shen`
>
> **Model author:** Shen (based on the Duguid, 2006 experiment)

> **BIL internal title:** `"Carbonation of CBM with scCO2 (2012)"`

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equations (elementary balances)](#41-conservation-equations-elementary-balances)
   - 4.2 [Chemistry of the aqueous solution](#42-chemistry-of-the-aqueous-solution)
   - 4.3 [Solid–liquid reactions: dissolution/precipitation](#43-solidliquid-reactions-dissolutionprecipitation)
   - 4.4 [Transport: advection + diffusion + electric migration](#44-transport-advection--diffusion--electric-migration)
   - 4.5 [Gas phase and CO₂ solubility](#45-gas-phase-and-co-solubility)
   - 4.6 [Porosity evolution](#46-porosity-evolution)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: Duguid experiment (`base/Shen/`)](#6-test-case-duguid-experiment)
7. [Model material parameterization](#7-model-material-parameterization)
8. [Step-by-step input file description](#8-step-by-step-input-file-description)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The **Shen** model simulates the **carbonation of a cement-based material (CBM) by supercritical CO₂ (scCO₂)**. This phenomenon is central to the assessment of long-term integrity of cemented boreholes used in **geological CO₂ storage** sites (CCS — Carbon Capture and Storage).

The physical scenario reproduces the laboratory experiment of Duguid (2006): a flow of scCO₂ is forced through a sandstone core with cement, triggering a cascade of chemical reactions that dissolve cement hydrates (portlandite CH, calcium silicate hydrates CSH) and precipitate calcite (CC). These transformations modify the porosity and, ultimately, the permeability of the cement.

---

## 2. Assumptions

1. **1D axial geometry**: The sandstone core is discretized as a 3.75 cm segment ($0$ to $0.0375$ m). CO₂ invasion propagates from the left face (injection) to the right.
2. **Biphasic porous medium**: The porosity simultaneously contains an aqueous liquid phase (ion-laden water) and a gas phase (pure supercritical CO₂ or CO₂-H₂O mixture). Liquid saturation degree $S_l$ depends on capillary pressure.
3. **Supercritical CO₂ treated as a pure gas**: CO₂ density and viscosity are computed via the Redlich-Kwong equation of state (density) and the Fenghour correlation (viscosity), tabulated as curves as a function of pressure $P_{CO_2}$.
4. **Local chemical equilibrium**: The aqueous solution is assumed at local thermodynamic equilibrium at each time step. Secondary species concentrations are algebraically derived from primary unknowns.
5. **Finite dissolution/precipitation kinetics**: CH dissolution and CC precipitation follow first-order kinetics controlled by characteristic times $T_{CH}$ and $T_{CC}$.
6. **Electroneutrality**: Total electric charge of the solution is zero, providing the additional equation to determine OH⁻ concentration (and hence pH).
7. **Constant temperature**: $T = 293\,\text{K}$ (20 °C). All equilibrium constants, diffusion coefficients, and viscosities are computed at this temperature.

---

## 3. Variables and notation

The model solves **7 equations** (NEQ = 7) with **7 primary unknowns** per node.

### Primary unknowns

| BIL Symbol | Meaning | Type |
|------------|---------|------|
| `logp_co2` | $\log_{10}(P_g)$ — (log) pressure of CO₂ gas | Continuous |
| `psi` | $\psi$ — electric potential of the solution | Continuous |
| `z_ca` | $z_{Ca,s}$ — quantity of Ca in solid phase (normalized) | Continuous |
| `z_si` | $z_{Si,s}$ — quantity of Si in solid phase (normalized) | Continuous |
| `c_k` | $c_K$ — K⁺ concentration | Continuous |
| `c_cl` | $c_{Cl}$ — Cl⁻ concentration | Continuous |
| `p_l` | $P_l$ — liquid phase pressure | Continuous |

> Gas pressure is solved in logarithmic space (`LOG_U`) to improve numerical convergence over several pressure decades. Liquid pressure is solved directly (`NOLOG_U`).

### Equations solved

| Index | Name | Physical meaning |
|-------|------|-----------------|
| `E_C` | `carbone` | Carbon (C) conservation |
| `E_q` | `charge` | Electric charge conservation |
| `E_Ca` | `calcium` | Calcium (Ca) conservation |
| `E_Si` | `silicium` | Silicon (Si) conservation |
| `E_K` | `potassium` | Potassium (K) conservation |
| `E_Cl` | `chlorine` | Chlorine (Cl) conservation |
| `E_mass` | `mass` | Total mass conservation (water + gas) |

### Computed secondary variables

From the 7 unknowns, the model algebraically derives:
- Aqueous concentrations of all species: $c_{OH}$, $c_H$, $c_{CO_2}$, $c_{HCO_3^-}$, $c_{CO_3^{2-}}$, $c_{Ca^{2+}}$, $c_{CaHCO_3^+}$, $c_{H_3SiO_4^-}$, $c_{H_4SiO_4}$, $c_{H_2SiO_4^{2-}}$, $c_{CaH_3SiO_4^+}$, $c_{CaH_2SiO_4}$, $c_{CaCO_3^{aq}}$, $c_{CaOH^+}$, $c_{H_2O}$
- Solid phase contents: $n_{CH}$ (portlandite), $n_{CC}$ (calcite), $n_{Si,s}$ (Si in CSH)
- Porosity $\phi$, saturations $S_l$, $S_g$
- Phase densities $\rho_l$, $\rho_g$

---

## 4. Mathematical model

### 4.1 Conservation equations (elementary balances)

For each chemical element $\alpha \in \{C, Ca, Si, K, Cl\}$, conservation is written as:

$$\frac{\partial N_\alpha}{\partial t} + \nabla \cdot \mathbf{W}_\alpha = 0$$

where $N_\alpha$ is the total quantity (liquid + gas + solid) per unit volume of porous medium and $\mathbf{W}_\alpha$ is the total flux of element $\alpha$.

**Charge conservation** (local electroneutrality):

$$\frac{\partial N_q}{\partial t} + \nabla \cdot \mathbf{W}_q = 0$$

**Total mass conservation** (water + CO₂ gas):

$$\frac{\partial M}{\partial t} + \nabla \cdot \mathbf{W}_M = 0$$

with $M = S_g \phi \rho_g + S_l \phi \rho_l + m_s$ (total mass of fluid and solid per unit porous volume).

### 4.2 Chemistry of the aqueous solution

Secondary aqueous species concentrations are computed from thermodynamic equilibrium constants (temperature-dependent):

$$K_{h_2o} = c_H \cdot c_{OH}$$
$$K_{hco_3} = \frac{c_{HCO_3^-} \cdot c_H}{c_{CO_2}}$$
$$K_{co_3} = \frac{c_{CO_3^{2-}} \cdot c_H}{c_{HCO_3^-}}$$
$$K_{h_3sio_4} = \frac{c_{H_3SiO_4^-} \cdot c_H}{c_{H_4SiO_4}}$$

**Electrical neutrality** provides $c_{OH}$:

$$\sum_i z_i \, c_i = 0$$

where $z_i$ is the charge of ion $i$. This is an algebraic equation solved numerically (4th-order polynomial in $c_{OH}$) at each time step.

### 4.3 Solid–liquid reactions: dissolution/precipitation

**Portlandite (CH = Ca(OH)₂)**: dissolves if $z_{CO_2} > 1$ (CO₂ supersaturation), precipitates if $z_{CO_2} < 1$. First-order kinetics:

$$n_{CH}(t + dt) = n_{CH}(t) \cdot (z_{CO_2})^{-dt/T_{CH}}$$

**Calcite (CC = CaCO₃)**: precipitates as CH dissolves. Kinetics:

$$n_{CC}(t + dt) = n_{CC}(t) \cdot (z_{CO_2})^{+dt/T_{CC}}$$

with $T_{CH}$ and $T_{CC}$ characteristic times for dissolution/precipitation (in seconds).

**CSH (Calcium Silicate Hydrates)**: their composition (C/S ratio = $x_{CSH}$, H/S ratio = $z_{CSH}$, molar volume $V_{CSH}$) depends on the CH saturation degree $S_{CH}$. These properties are read from the `csh4p` curve (CSH4Poles model).

**Amorphous silica gel (SH)**: forms during CSH decalcification. Its saturation degree $S_{SH}$ is computed from CSH4Poles curves.

### 4.4 Transport: advection + diffusion + electric migration

For each aqueous species $i$, the total flux combines three mechanisms:

$$\mathbf{W}_i = \underbrace{-D_i \, \tau_l(\phi, S_l) \, \nabla c_i}_{\text{Diffusion}} \underbrace{- D_i \, \tau_l \, \frac{F z_i}{RT} c_i \nabla \psi}_{\text{Electric migration}} \underbrace{+ c_i \, k_l(\phi, S_l) \, (-\nabla P_l + \rho_l \mathbf{g})}_{\text{Darcy advection}}$$

**Liquid phase permeability**:

$$k_l = \frac{k_{int,l}}{\mu_l} \cdot k_{rl}(P_c) \cdot \left(\frac{\phi}{\phi_0}\right)^3 \left(\frac{1-\phi_0}{1-\phi}\right)^2$$

**Gas permeability**:

$$k_g = \frac{k_{int,g}}{\mu_{CO_2}(P_g)} \cdot k_{rg}(P_c) \cdot \left(\frac{\phi}{\phi_0}\right)^3 \left(\frac{1-\phi_0}{1-\phi}\right)^2$$

**Tortuosity for liquid diffusion** (modified Millington-Quirk):

$$\tau_l(\phi, S_l) = \min\!\left(\frac{\phi}{0.25}, 1\right) \cdot \frac{0.00029 \, e^{9.95\phi}}{1 + 625\,(1-S_l)^4}$$

### 4.5 Gas phase and CO₂ solubility

The molar density of supercritical CO₂ is computed by the **Redlich-Kwong equation** (tabulated in the `density_co2` file). Viscosity is computed via the **Fenghour** correlation (tabulated in `viscosity_co2`).

**CO₂ solubility in water** at pressure $P_g$ and temperature $T$ (modified Henry's law with fugacity):

$$c_{CO_2}^{liq} = \text{solubilityCO2}(P_g, P_l, T)$$

**Molar fraction of H₂O in the gas phase**:

$$y_{H_2O} = \frac{P_{sat}(T) \cdot \phi_{H_2O}^{sat}}{P_g \cdot \phi_{H_2O}}$$

with $\phi_{H_2O}$ the fugacity coefficient of H₂O in the CO₂-H₂O mixture.

### 4.6 Porosity evolution

Porosity evolves dynamically through changes in solid volume:

$$\phi = \phi_0 + V_{s,0} - V_s$$

$$V_s = V_{CH} \cdot n_{CH} + V_{CC} \cdot n_{CC} + V_{CSH}(S_{CH}) \cdot n_{Si,s}$$

with $V_{CH} = 33 \times 10^{-3}$ dm³/mol and $V_{CC} = 37 \times 10^{-3}$ dm³/mol. CH dissolution (volume released) and CC precipitation (volume filled) partially compensate: **net dissolution creates porosity** while **calcite precipitation fills pores**.

---

## 5. Boundary and initial conditions

### Initial state (Initialization)

| Unknown | Initial value | BIL field | Meaning |
|---------|--------------|-----------|---------|
| `logp_co2` | `Field 1` → $10^1 = 10$ Pa | affine `Val = 1.` | Initial low CO₂ pressure |
| `psi` | `Field 0` → $0$ | affine `Val = 0.` | Zero electric potential |
| `z_si` | `Field 6` → $-2$ | affine `Val = -2.` | Partially decalcified CSH |
| `z_ca` | `Field 2` → $0$ | affine `Val = 0.` | Initial calcium-rich solid |
| `c_k` | `Field 5` → $0.5$ mol/dm³ | affine `Val = 0.5` | Dissolved potassium |
| `c_cl` | `Field 5` → $0.5$ mol/dm³ | affine `Val = 0.5` | Dissolved chloride |
| `p_l` | `Field 8` → $0.5$ Pa | affine `Val = 0.5` | Initial liquid pressure |

### Boundary conditions (`Boundary Conditions`)

| Boundary | Unknown | Condition | Meaning |
|----------|---------|-----------|---------|
| `Reg 3` (injection face, $x=0$) | `psi` | $\psi = 0$ (constant Func 0) | Electric potential reference |
| `Reg 3` (injection face) | `logp_co2` | Imposed via Func 1 | CO₂ pressure ramp |
| `Reg 3` (injection face) | `p_l` | Imposed via Func 0 | Liquid pressure at face |
| `Reg 2` (entire zone) | `c_k` | Imposed via Field 9 = $0.5$ mol/dm³, Func 0 | Constant K concentration |
| `Reg 2` (entire zone) | `c_cl` | Imposed via Field 9 = $0.5$ mol/dm³, Func 0 | Constant Cl concentration |
| `Reg 3` (injection face) | `z_ca` | Imposed via Field 4 = $1$, Func 4 (ramp from $-2$ to $1$) | Calcite front imposition |

---

## 6. Test case: Duguid experiment (`Shen/`)

### Geometry and mesh

The domain is a **1D segment of length $L = 0.0375$ m** (3.75 cm), discretized with **50 elements** of regular size $\Delta x = 7.5 \times 10^{-4}$ m.

```
x = 0 (CO₂ injection)  ←——————————————————→  x = 0.0375 m (free face)
Reg 3 (imposed BCs)                                Reg 2
```

### Simulated experimental protocol

The Duguid experiment injects supercritical CO₂ into a cemented sandstone core under confining pressure. CO₂ pressure is progressively raised at the injection face, forcing its invasion into the cement porosity.

- **$t = 0$ to $86\,400$ s (1 day)**: Transient phase of CO₂ front establishment.
- **$t = 86\,400$ to $864\,000$ s (10 days)**: Carbonation front progression.
- **$t = 864\,000$ to $2\,592\,000$ s (30 days)**: Stabilization and possible re-plugging by calcite.

### Expected results

The scenario produces a **carbonation front** advancing from the injection face toward the back:

1. **Carbonated zone** (near $x = 0$): Portlandite is completely dissolved ($n_{CH} \approx 0$), decalcified CSH are converted to silica gel. Precipitated calcite fills part of the porosity. pH drops from ~12 to ~6.
2. **Transition zone**: Coexistence of solid phases undergoing transformation, large gradient in $c_{CO_2}$ and pH.
3. **Sound zone** (near $x = L$): Intact cement ($n_{CH}$ high, pH > 12, no calcite).

**Porosity** oscillates according to the volumetric balance:
- CH dissolution releases volume → $\phi$ increases.
- CC precipitation occupies volume → $\phi$ decreases locally.
- The net result may lead to **partial clogging** (reduced permeability) if calcite precipitation dominates.

![Shen - Simulation Results](Shen/Shen_results.png)

---

## 7. Model material parameterization

| Parameter | Value in `Shen` | Physical role |
|-----------|--------------------------|---------------|
| `porosite` | 0.4 | Initial porosity of cemented sandstone |
| `N_CH` | 5.16 mol/dm³ | Initial portlandite content (reference for $n_{Ca,s}$) |
| `N_Si` | 3.9 mol/dm³ | Initial Si content in CSH (reference for $n_{Si,s}$) |
| `T_CH` | $4 \times 10^5$ s | Characteristic time for CH dissolution ($\approx 4.6$ days) |
| `k_intl` | $2 \times 10^{-17}$ m² | Intrinsic permeability to liquid phase |
| `k_intg` | $1.4 \times 10^{-16}$ m² | Intrinsic permeability to gas phase (CO₂) |
| `temperature` | 293 K | Experiment temperature ($\approx 20$ °C) |
| `Curves = CN_courbe` | 4-col tabulated file | Capillary curves: $S_l(P_c)$, $k_{rl}(P_c)$, $k_{rg}(P_c)$ |
| `Curves_log = csh4p` | 5-col tabulated file | CSH properties: C/S ratio, H/S, molar volume, $q_{SH}$, $X_S$ |
| `Curves = density_co2` | Tabulated (Redlich-Kwong) | CO₂ molar density: $\rho_{CO_2}(P, T=293\,\text{K})$ |
| `Curves = viscosity_co2` | Tabulated (Fenghour) | CO₂ dynamic viscosity: $\mu_{CO_2}(P, T=293\,\text{K})$ |

---

## 8. Step-by-step input file description

### 8.1 Control file `Shen`

#### `Geometry` block

```
Geometry
1 axis
```

One-dimensional (`1`) simulation in axial geometry. BIL interprets the problem as transport along a single axis.

#### `Mesh` block

```
Mesh
4 0 0 0.0375 0.0375
1.e-3
1 50 1
1 1 1
```

- `4 0 0 0.0375 0.0375`: Mesh type 4 (segment), from $x=0$ to $x=0.0375$ m.
- `1.e-3`: Geometric tolerance of the mesher.
- `1 50 1`: 1 element layer, 50 elements along length, 1 layer in thickness.
- `1 1 1`: Geometric region indices.

#### `Material` block

```
Model = Shen
porosite = 0.4
N_CH     = 5.16
N_Si     = 3.9
T_CH     = 4.e5
k_intl    = 2.e-17
k_intg    = 1.4e-16
temperature = 293.
Curves = CN_courbe
Curves_log = csh4p  q_CH = Range{...}  X_S = CSH4Poles(1){...}
Curves = density_co2    P_co2 = Range{...}  Rho_co2 = Redlich-Kwong_CO2(1){...}
Curves = viscosity_co2  P_co2 = Range{...}  Mu_co2 = Fenghour_CO2(1){...}
```

#### `Dates` block

```
Dates
4
0. 86400 864000 2592000
```

4 requested output times: $t = 0$, 1 day, 10 days, 30 days. Files `Shen.t0` to `Shen.t3` correspond to these 4 outputs.

#### `Objective Variations` block

```
Objective Variations
logp_co2 = 0.1
z_si     = 1.e-1
z_ca     = 1.e-1
psi      = 0.1
c_k      = 0.1
c_cl     = 0.1
p_l      = 10000.
```

Defines the maximum relative variation tolerated per unknown **for adaptive time step control**. BIL increases $\Delta t$ if all variations are below these thresholds, and decreases it otherwise.

#### `Time Steps` block

```
Time Steps
Dtini = 1.e1
Dtmax = 1.e5
```

- `Dtini = 10 s`: Very small initial time step, as chemical fronts are steep at startup.
- `Dtmax = 100 000 s` ($\approx 27.8$ hours): Maximum allowed time step.

---

## 9. Bibliographic references

- **Duguid, A.** (2006). *The effect of CO₂ on the integrity of well cements*. Princeton University PhD Thesis.
- **Shen, J., Dangla, P., Thiery, M.** (2013). Reactive transport modeling of CO₂ through cementitious materials under CO₂ geological storage conditions. *International Journal of Greenhouse Gas Control*, 18, 75–87.
- **Redlich, O., & Kwong, J. N. S.** (1949). On the thermodynamics of solutions. V. An equation of state. *Chemical Reviews*, 44(1), 233–244.
- **Fenghour, A., Wakeham, W. A., & Vesovic, V.** (1998). The viscosity of carbon dioxide. *Journal of Physical and Chemical Reference Data*, 27(1), 31–44.
- **Jennings, H. M.** (2000). A model for the microstructure of calcium silicate hydrate in cement paste. *Cement and Concrete Research*, 30(1), 101–116.
- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons.
