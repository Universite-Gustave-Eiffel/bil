# Sulfaco Model — Internal/External Sulfate Attack of Concrete

> **Bil model:** `src/Models/ModelFiles/Sulfaco.c`

> **Input file:** `doc/mkdocs/Durability/Sulfaco/Sulfaco`
>
> **Model authors:** Gu, Dangla (Université Gustave Eiffel)
> **Internal title:** *"Internal/External sulfate attack of concrete (2017)"*

---

## Table of Contents

1. [Context and Objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and Notation](#3-variables-and-notation)
4. [Mathematical Model](#4-mathematical-model)
   - 4.1 [Conservation Equations](#41-conservation-equations)
   - 4.2 [Pore Solution Chemistry](#42-pore-solution-chemistry)
   - 4.3 [Reactions with Solid Phases](#43-reactions-with-solid-phases)
   - 4.4 [Crystallization Pressure and Damage](#44-crystallization-pressure-and-damage)
   - 4.5 [Transport Law (Ionic Diffusion)](#45-transport-law-ionic-diffusion)
5. [Boundary and Initial Conditions](#5-boundary-and-initial-conditions)
6. [Test Case: Sulfate Attack on a Concrete Sample (`base/Sulfaco/`)](#6-test-case-sulfate-attack-on-a-concrete-sample-basesulfaco)
7. [Model Material Parameters](#7-model-material-parameters)
8. [Step-by-Step Description of Input Files](#8-step-by-step-description-of-input-files)
9. [References](#9-references)

---

## 1. Context and Objective

The **Sulfaco** model simulates the **chemical degradation of reinforced concrete by sulfate attack** (internal or external). This durability phenomenon is one of the main causes of long-term deterioration in concrete infrastructure (bridges, foundations, tunnels in gypsiferous ground).

Sulfate attack occurs when sulfate ions ($\text{SO}_4^{2-}$) penetrate the hardened cement paste (external attack) or are released by sulfate-bearing aggregates (internal attack). These ions react with the aluminous solid phases of hydrated cement to form **secondary ettringite** ($\text{AFt} = \text{C}_6\text{AS}_3\text{H}_{32}$). The formation of ettringite in a confined environment generates a **crystallization pressure** that exceeds the tensile strength of the concrete, causing cracking and spalling.

---

## 2. Assumptions

1. **Isothermal**: Temperature is fixed at $T = 293\,\text{K}$ ($20°\text{C}$). Thermodynamic equilibrium constants are evaluated at this temperature.
2. **Saturated porous medium**: The pore solution occupies the entire porosity (no gas phase). Saturation $S_r$ is described by a tabulated curve as a function of pore radius $r$.
3. **Dissolution/precipitation kinetics**: Reactions of solid phases (CSH₂, AFt, AFm, C₃AH₆) are governed by first-order kinetic laws in terms of departure from supersaturation, with rates $R_x$ (mol/L/s). Portlandite (CH) and ettringite (AFt at equilibrium) follow a direct mass balance.
4. **Electroneutrality**: The total electric charge of the solution is zero. This constraint closes the system: it defines the $\text{OH}^-$ concentration from the other ions.
5. **Crystallization pressure**: The pressure exerted by a growing AFt crystal in a pore of radius $r$ is given by Laplace thermodynamics (Kelvin-Thomson equation).
6. **Uniaxial damage**: A swelling strain $\varepsilon$ is computed from porosity changes. When $\varepsilon$ exceeds the threshold $\varepsilon_0$, an exponential damage law reduces the effective stiffness.
7. **Nernst-Planck diffusion**: Ionic transport follows the electrochemical coupled diffusion law, including migration under the internal electric field (Donnan/diffusion potential $\psi$).

---

## 3. Variables and Notation

### Primary Unknowns (6 equations)

| Symbol | Meaning | BIL Internal | BIL Unknown |
|--------|---------|--------------|-------------|
| $\log c_{\text{SO}_4}$ | Log₁₀ of sulfate concentration | `E_Sulfur` | `logc_so4` |
| $\psi$ | Diffuse electric potential (V) | `E_charge` | `psi` |
| $z_\text{Ca}$ | Normalized quantity of solid calcium (CH) | `E_Calcium` | `z_ca` |
| $\log c_\text{K}$ | Log₁₀ of potassium concentration | `E_Potassium` | `logc_k` |
| $z_\text{Al}$ | Normalized quantity of solid aluminum (AH₃) | `E_Aluminium` | `z_al` |
| $\log c_\text{OH}$ | Log₁₀ of hydroxyl concentration | `E_eneutral` | `logc_oh` |

### Main Computed Secondary Variables

| Symbol | Meaning |
|--------|---------|
| $n_\text{CH}$ | Molar content of portlandite per liter of concrete |
| $n_\text{CSH2}$ | Molar content of gypsum (CaSO₄·2H₂O) |
| $n_\text{AFt}$ | Molar content of ettringite ($\text{C}_6\text{AS}_3\text{H}_{32}$) |
| $n_\text{AFm}$ | Molar content of monosulfoaluminate ($\text{C}_4\text{ASH}_{12}$) |
| $n_\text{C3AH6}$ | Molar content of hydrogarnet ($\text{C}_3\text{AH}_6$) |
| $\phi$ | Instantaneous porosity (modified by solid reactions) |
| $P_c$ | Crystallization pressure of AFt under confinement (Pa) |
| $\varepsilon$ | Free swelling strain |
| $D$ | Damage variable ($0$ = intact, $1$ = fully damaged) |

---

## 4. Mathematical Model

### 4.1 Conservation Equations

The model solves six coupled conservation balances (Finite Volumes, `FVM.h`):

1. **Sulfur conservation** (S, as $\text{SO}_4^{2-}$ and complexes):
$\frac{\partial N_S}{\partial t} + \nabla \cdot \mathbf{W}_S = 0$

2. **Electric charge conservation** (global electroneutrality):
$\frac{\partial N_q}{\partial t} + \nabla \cdot \mathbf{W}_q = 0$

3. **Calcium conservation** (dissolved + solid CH + CSH + CSH₂ + AFt + AFm + C₃AH₆):
$\frac{\partial N_\text{Ca}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Ca} = 0$

4. **Potassium conservation**:
$\frac{\partial N_K}{\partial t} + \nabla \cdot \mathbf{W}_K = 0$

5. **Aluminum conservation** (dissolved + solid AH₃ + AFm + AFt + C₃AH₆):
$\frac{\partial N_\text{Al}}{\partial t} + \nabla \cdot \mathbf{W}_\text{Al} = 0$

6. **Local electroneutrality** (system closure):
$\sum_i z_i c_i = 0$

where $N_x = \phi \, c_x^{\text{total}} + n_x^{\text{solid}}$ is the total content (liquid + solid) per unit volume of concrete.

### 4.2 Pore Solution Chemistry

The `HardenedCementChemistry.h` module solves the complete chemical equilibrium of the concrete pore solution. The species considered are:

| Species | Symbol |
|---------|--------|
| Hydroxyl | $\text{OH}^-$ |
| Proton | $\text{H}^+$ |
| Free calcium | $\text{Ca}^{2+}$ |
| Calcium complexes | $\text{CaOH}^+$ |
| Dissolved silicates | $\text{H}_2\text{SiO}_4^{2-}$, $\text{H}_3\text{SiO}_4^-$, $\text{H}_4\text{SiO}_4$ |
| Ca-Si complexes | $\text{CaH}_2\text{SiO}_4$, $\text{CaH}_3\text{SiO}_4^+$ |
| Sulfates | $\text{SO}_4^{2-}$, $\text{HSO}_4^-$, $\text{H}_2\text{SO}_4$ |
| Ca-sulfate complexes | $\text{CaSO}_4^\text{aq}$, $\text{CaHSO}_4^+$ |
| Potassium | $\text{K}^+$, $\text{KOH}^\text{aq}$ |
| Aluminates | $\text{Al}(\text{OH})_4^-$ |

All these concentrations are derived from the 6 primary unknowns via thermodynamic equilibrium constants at $T = 293\,\text{K}$.

### 4.3 Reactions with Solid Phases

Solid phases dissolve or precipitate according to their saturation state:

#### Portlandite — $\text{CH} = \text{Ca(OH)}_2$

Dissolution controlled by the solid calcium mass balance:
$n_\text{CH} = N_\text{CH}^{\text{ref}} \cdot \max(z_\text{Ca}, 0)$

The variable $z_\text{Ca}$ decreases as solid calcium is consumed by sulfates or leached out.

#### Gypsum — $\text{CSH}_2 = \text{CaSO}_4 \cdot 2\text{H}_2\text{O}$ (Kinetic)

$$n_\text{CSH2}(t+\Delta t) = \max\!\left(n_\text{CSH2}(t) + \Delta t \cdot R_\text{CSH2} \cdot (S_\text{CSH2} - 1),\, 0\right)$$

where $S_\text{CSH2}$ is the saturation index (activity-to-solubility product ratio) and $R_\text{CSH2}$ is the kinetic rate.

#### Ettringite — $\text{AFt} = \text{C}_6\text{AS}_3\text{H}_{32}$ (Kinetic with Confinement)

The AFt precipitation kinetics are more elaborate: they distinguish **growth at a free interface** (pore wall) from **growth under confinement** (filled pore), the latter generating pressure:

$$n_\text{AFt}(t+\Delta t) = \max\!\left(n_\text{AFt}(t) + \Delta t \cdot R_\text{AFt} \cdot f(\beta, \beta_p),\, 0\right)$$

where $\beta = S_\text{AFt}$ (global supersaturation index) and $\beta_p$ is the equilibrium index at the confined crystal-liquid interface.

#### Monosulfoaluminate — $\text{AFm} = \text{C}_4\text{ASH}_{12}$ and Hydrogarnet — $\text{C}_3\text{AH}_6$ (Kinetic)

$$n_x(t+\Delta t) = \max\!\left(n_x(t) + \Delta t \cdot R_x \cdot (S_x - 1),\, 0\right)$$

### 4.4 Crystallization Pressure and Damage

When AFt precipitates in a pore of radius $r$, it generates an overpressure given by the **Kelvin-Thomson** relation:

$$P_c = \frac{RT}{V_\text{AFt}} \ln(\beta)$$

where $\beta$ is the supersaturation index of AFt relative to the confined equilibrium and $V_\text{AFt} = 710.32\,\text{cm}^3/\text{mol}$ is the molar volume of ettringite.

The thermodynamic equilibrium at the crystal-liquid interface in a pore of radius $r$ is:
$\beta_\text{eq}(r) = \exp\!\left(\frac{2\,\Gamma_\text{AFt}\,V_\text{AFt}}{RT \cdot r}\right)$

with $\Gamma_\text{AFt} = 0.1\,\text{N/m}$ the surface tension.

The volumetrically integrated crystallization pressure produces a **free swelling strain** $\varepsilon$. When $\varepsilon$ exceeds the threshold $\varepsilon_0$, the concrete undergoes damage according to an exponential law:

$$D(\varepsilon) = 1 - \frac{\varepsilon_0}{\varepsilon} \exp\!\left(-\frac{\varepsilon - \varepsilon_0}{\varepsilon_f}\right)$$

where $\varepsilon_0$ is the threshold strain and $\varepsilon_f$ is the characteristic failure strain. The damage $D$ reduces the effective stress and increases ionic permeability.

### 4.5 Transport Law (Ionic Diffusion)

The flux of each ion $i$ (with charge $z_i$ and diffusion coefficient $D_i$) in the pore solution is given by the **Nernst-Planck** equation:

$$\mathbf{W}_i = -\phi \, \tau(\phi) \left( D_i \nabla c_i + z_i \frac{F D_i}{RT} c_i \nabla \psi \right)$$

where $\tau(\phi)$ is the effective tortuosity according to the Bazant-Najjar law:

$$\tau(\phi) = 0.001 + 0.07\phi^2 + H(\phi - 0.18) \cdot 1.8(\phi - 0.18)^2$$

The `CementSolutionDiffusion.h` module handles the coefficients $D_i$ for each ion.

---

## 5. Boundary and Initial Conditions

### Initial Conditions

The concrete is initially in a chemically "intact" equilibrium state defined by the initial contents of solid phases ($N_\text{CH}$, $N_\text{CSH}$, $N_\text{C3AH6}$...) and a very low sulfate concentration ($\log c_{\text{SO}_4} = -6$, i.e., $c_{\text{SO}_4} = 10^{-6}\,\text{mol/L}$).

### Boundary Conditions

The system is **exposed on one side** (region 1, $x=0$) to a sulfate solution whose $\text{SO}_4^{2-}$ concentration increases over time according to the functions defined in the input file. The other face (region 3, $x=L$) is left with no flux (implicit zero Neumann condition).

---

## 6. Test Case: Sulfate Attack on a Concrete Sample (`Sulfaco/`)

### Geometry

The sample is a **1D domain of length $L = 1\,\text{dm}$**, discretized into a single finite volume element with two nodes (representing a homogeneous "material point" with no internal spatial gradient).

### Simulated Scenario

This is a simulation of a **single material point** (no spatial gradient) exposed to an increasing sulfate supply. The concentrations imposed at $x=0$ vary according to time functions defined in the input file:

| Imposed variable | $t=0$ | $t=8640\,\text{s}$ | $t=864000\,\text{s}$ |
|-----------------|-------|--------|---------|
| $\log c_{\text{SO}_4}$ | $-6$ | $-2.556$ | $-0.456$ |
| $\log c_K$ | $-5.7$ | $-2.256$ | $-0.156$ |
| $\log c_\text{OH}$ | $-2$ (pH 12) | $-2$ | $-2$ |

The total simulated duration is $3.5 \times 10^6\,\text{s} \approx 40\,\text{days}$.

### Expected Results (`.gp` outputs)

The Gnuplot file `Sulfaco.gp` plots nine post-processing figures:

| Figure | File | Physical content |
|--------|------|-----------------|
| 1 | `Strain.eps` | Swelling strain $\varepsilon(t)$ |
| 2 | `EttringiteSaturationIndex.eps` | Saturation index $S_\text{AFt}(t)$ (log-log) |
| 3 | `EttringiteContent.eps` | Ettringite content $n_\text{AFt}(t)$ (mol/L of concrete) |
| 4 | `CrystallizationPressure.eps` | Crystallization pressure $P_c(t)$ (Pa) |
| 5 | `pH.eps` | Evolution of pore solution pH |
| 6 | `SaturationIndexes.eps` | Saturation indices: AFt and gypsum (CSH₂) |
| 7 | `SolidContents.eps` | Evolution of solid phases: CH, AFt, C₃AH₆ |
| 8 | `IonConcentrations.eps` | $\text{SO}_4^{2-}$ concentration (mol/L) |
| 9 | `StressStrain.eps` | Effective stress $S_c \cdot P_c$ vs. strain |

The expected physical sequence is:
1. Incoming sulfates first react with **C₃AH₆** (hydrogarnet) and **AFm** (monosulfoaluminate), which convert to **AFt** (ettringite).
2. The AFt saturation index grows, and the **crystallization pressure** increases.
3. When $P_c$ exceeds the tensile strength (strain $\varepsilon > \varepsilon_0$), **damage** $D$ activates and the free strain $\varepsilon$ rises sharply.
4. **Portlandite** (CH) dissolves progressively, buffering the pH.

---

## 7. Model Material Parameters

| Parameter | Value in `Sulfaco` | Physical role |
|-----------|-------------------|---------------|
| `porosity` | $0.23$ | Initial porosity of sound concrete |
| `N_CH` | $1.53\,\text{mol/L}$ | Initial portlandite content |
| `N_CSH` | $1.393\,\text{mol/L}$ | Initial C-S-H content (Si source) |
| `N_AH3` | $10^{-6}\,\text{mol/L}$ | Gibbsite (near-zero, trace amount) |
| `N_C3AH6` | $0.2\,\text{mol/L}$ | Initial hydrogarnet (aluminum precursor) |
| `N_CSH2`, `N_AFm`, `N_AFt` | $0$ | No initial gypsum or ettringite |
| `R_CSH2` | $10^{-12}\,\text{mol/L/s}$ | Gypsum precipitation rate (very slow) |
| `R_AFm` | $10^{-6}\,\text{mol/L/s}$ | Monosulfoaluminate precipitation rate |
| `R_C3AH6` | $10^{-6}\,\text{mol/L/s}$ | Hydrogarnet dissolution rate |
| `A_i` | $8.4 \times 10^{-8}$ | AFt growth rate at free interface ($a_r$) |
| `A_p` | $4.4 \times 10^{-9}$ | AFt growth rate in confined pore ($a_p$) |
| `K_bulk` | $30.1\,\text{GPa}$ | Bulk modulus of sound concrete |
| `Biot` | $0.54$ | Biot coefficient (pressure-strain coupling) |
| `Strain0` | $4 \times 10^{-4}$ | Damage threshold strain $\varepsilon_0$ |
| `Strainf` | $6.4 \times 10^{-3}$ | Characteristic failure strain $\varepsilon_f$ |
| `AlphaCoef` | $0$ | Sulfate adsorption coefficient on C-S-H (disabled) |
| `BetaCoef` | $0.87$ | Saturation coefficient in the adsorption isotherm |
| `Curves_log = Sat` | Van Genuchten ($m=0.2516$, $r_0=155.82\,\text{nm}$) | Liquid saturation curve $S_r(r)$ in pores |

---

## 8. Step-by-Step Description of Input Files

### 8.1 Unit System (`Units`)

```
Units
Length = decimeter     # 1 length unit = 1 dm
Time   = second        # 1 time unit = 1 s
Mass   = hectogram     # 1 mass unit = 100 g
```

Choosing the decimeter as the length unit is consistent with typical concrete specimen sizes (a few centimeters to decimeters). Concentrations are therefore in **mol/dm³ = mol/L** and diffusivities in **dm²/s**.

### 8.2 Geometry and Mesh (`Geometry`, `Mesh`)

```
Geometry
Dimension = 1  plan     # 1D problem in planar (Cartesian) coordinates

Mesh
2                        # 2 nodes
0. 1                     # Node 1: x=0 (exposed surface)
1.                       # Node 2: x=1 dm (core)
1                        # 1 element
1                        # Linear element connecting nodes 1-2 (region 1)
```

The mesh file `Sulfaco.msh` (Gmsh format) explicitly defines 4 nodes including 2 boundary nodes (type 15 = points) and 1 segment (type 1) between $x=0$ and $x=1\,\text{dm}$.

### 8.3 Material (`Material`)

```
Model = Sulfaco
porosity = 0.23
N_CH     = 1.53
N_CSH    = 1.393
N_AH3    = 1.e-6
N_C3AH6  = 0.2
```

These lines define the **initial microstructure** of the concrete: portlandite and C-S-H are the calcium reservoirs that control the pH buffer and the calcium source for sulfate reactions.

```
R_CSH2   = 1.e-12
R_AFm    = 1.e-6
R_C3AH6  = 1.e-6
```

These kinetic constants (`R_x`, in mol/L/s) control the rate of solid transformations. A very small `R_CSH2` means gypsum precipitates very slowly; a moderate `R_C3AH6` allows rapid dissolution of hydrogarnet into ettringite.

```
A_i = 8.4e-8
A_p = 4.4e-9
```

AFt crystal growth kinetic coefficients: `A_i` (free interface, fast) and `A_p` (confined pore, 20 times slower).

```
K_bulk = 30.1e9
Biot = 0.54
Strain0 = 4.e-4
Strainf = 6.4e-3
```

Mechanical parameters: the bulk modulus $K_\text{bulk}$ and the Biot coefficient are used to compute the effective stress. `Strain0` and `Strainf` define the damage curve: concrete starts cracking at a strain of $4 \times 10^{-4}$ and is fully damaged at $\sim 7 \times 10^{-3}$.

```
Curves_log = Sat  r = Range{r1 = 1.e-8 , r2 = 1.e-3 , n = 1000}
             S_r = Expressions(1){r0=155.82e-8; m = 0.2516 ; S_r = (1 + (r0/r)**(1/(1-m)))**(-m)}
```

Van Genuchten saturation curve: for a pore radius $r$ (in m), $S_r(r)$ gives the fraction of pores smaller than $r$ that are saturated with solution. It is tabulated over 1000 points between $r=10\,\text{nm}$ and $r=1\,\text{mm}$.

### 8.4 Fields (`Fields`)

```
Fields
1
Value = 1   Gradient = 0 0 0   Point = 0 0 0
```

A single uniform scalar field (value 1, zero gradient) is defined. It serves as a **multiplier** in the initialization and boundary condition functions.

### 8.5 Initialization (`Initialization`)

```
Initialization
6
Region = 1 Unknown = logc_so4   Field = 1 Function = 1
Region = 1 Unknown = psi        Field = 0 Function = 0
Region = 1 Unknown = z_ca       Field = 1 Function = 4
Region = 1 Unknown = z_al       Field = 1 Function = 3
Region = 1 Unknown = logc_k     Field = 1 Function = 2
Region = 1 Unknown = logc_oh    Field = 1 Function = 5
```

Each line initializes an unknown in region 1 (the entire domain) to the value `Field × Function(t=0)`:

| Unknown | Initial value | Meaning |
|---------|--------------|---------|
| `logc_so4` | $1 \times (-6) = -6$ | $c_{\text{SO}_4} = 10^{-6}\,\text{mol/L}$ (sound concrete, low sulfate) |
| `psi` | $0 \times 0 = 0$ | Zero electric potential at initial state |
| `z_ca` | $1 \times 1 = 1$ | Intact portlandite: $n_\text{CH} = N_\text{CH} \times 1$ |
| `z_al` | $1 \times 1 = 1$ | Intact solid AH₃: $n_\text{AH3} = N_\text{AH3} \times 1$ |
| `logc_k` | $1 \times (-5.7) = -5.7$ | $c_K \approx 2 \times 10^{-6}\,\text{mol/L}$ |
| `logc_oh` | $1 \times (-2) = -2$ | pH $= 12$ ($c_{\text{OH}^-} = 10^{-2}\,\text{mol/L}$) |

### 8.6 Time Functions (`Functions`)

```
Functions
5
N = 5 F(0) = -6     F(8640) = -2.556  F(86400) = -1.556  F(345600) = -0.954  F(864000) = -0.456
N = 5 F(0) = -5.7   F(8640) = -2.256  F(86400) = -1.256  F(345600) = -0.654  F(864000) = -0.156
N = 2 F(0) = -7.32  F(86400) = -6.32
N = 2 F(0) = 1      F(86400) = 1
N = 2 F(0) = -2     F(86400) = -2
```

Five piecewise linear time interpolation functions (with `N` points each):

| Function | Usage | Description |
|----------|-------|-------------|
| 1 | `logc_so4` in BC | $\log c_{\text{SO}_4}$: increases from $-6$ to $-0.456$ (from $10^{-6}$ to $0.35\,\text{mol/L}$) over $10\,\text{days}$ |
| 2 | `logc_k` in BC | $\log c_K$: increases from $-5.7$ to $-0.156$ (from $2\times10^{-6}$ to $0.7\,\text{mol/L}$) over $10\,\text{days}$ |
| 3 | `z_al` initial | Constant $= 1$ (stable solid aluminum) |
| 4 | `z_ca` initial | Slight slope (slow CH dissolution over the first day) |
| 5 | `logc_oh` in BC | pH held constant at 12 ($c_{\text{OH}^-} = 10^{-2}\,\text{mol/L}$) |

### 8.7 Boundary Conditions (`Boundary Conditions`)

```
Boundary Conditions
4
Region = 1 Unknown = logc_so4   Field = 1 Function = 1
Region = 1 Unknown = psi        Field = 0 Function = 0
Region = 1 Unknown = logc_k     Field = 1 Function = 2
Region = 1 Unknown = logc_oh    Field = 1 Function = 5
```

Boundary conditions are applied to **region 1** (exposed surface at $x=0$):
- Sulfate and potassium concentrations are **prescribed** and increase over time (functions 1 and 2).
- The electric potential is held at **zero** (reference).
- The pH is **held at 12** throughout the simulation (function 5, constant).
- $z_\text{Ca}$ and $z_\text{Al}$ are **not prescribed** as BCs (commented-out lines): the solid calcium evolves freely through dissolution/precipitation.

### 8.8 Observation Points (`Points`)

```
Points
1
0.5
```

A single observation point is defined at $x = 0.5\,\text{dm}$ (center of the sample). This is the point at which secondary variables (columns of the `.p1` file) are recorded over time.

### 8.9 Time Window and Convergence Criteria

```
Dates
2
0.  3.5e6       # t=0 s to t=3.5×10⁶ s ≈ 40 days

Objective Variations
logc_so4  = 1.e-3   # Allowable relative variation for the sulfate unknown
z_ca      = 1.e-3
psi       = 1.e-1
logc_k    = 1.e-1
z_al      = 1.e-1
logc_oh   = 1.e-1

Iterative Process
Iter = 20       # Max number of Newton-Raphson iterations per time step
Tol  = 1.e-4    # Tolerance on the global residual

Time Steps
Dtini = 1.e3    # Initial time step: 1000 s ≈ 17 min
Dtmax = 1.e3    # Maximum time step: 1000 s (fixed)
```

The fixed time step of $1000\,\text{s}$ is suited to the fastest chemical kinetics (AFm, C₃AH₆ with $R_x = 10^{-6}\,\text{mol/L/s}$). The `Objective Variations` tolerance controls the adaptive time stepping in BIL: if the variation of an unknown exceeds its threshold, the step is subdivided.

---

## 9. References

- **Biot, M. A.** (1941). General theory of three-dimensional consolidation. *Journal of applied physics*, 12(2), 155–164. — Establishes the pore pressure / deformation coupling (Biot coefficient $b = 0.54$ used in the model).

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons. — Rigorous treatment of crystallization pressure in porous media (Kelvin-Thomson equation applied to crystals growing in pores).

- **Flatt, R. J. & Scherer, G. W.** (2008). Thermodynamics of crystallization stresses in DEF. *Cement and Concrete Research*, 38(3), 325–336. — Thermodynamic basis for computing the delayed ettringite crystallization pressure $P_c = \frac{RT}{V_\text{AFt}}\ln\beta$.

- **Montoya, A. G., et al.** (2016). A chemo-mechanical model for sulfate attack in concrete — Provides the chemistry-mechanics coupling framework used in Sulfaco (damage, swelling, porosity alteration).

- **Nernst, W. & Planck, M.** (1890). Theory of ionic diffusion coupled to electric potential. — Nernst-Planck equation for ion transport in porous solution ($\mathbf{W}_i = -\phi\tau(D_i \nabla c_i + z_i \frac{FD_i}{RT} c_i \nabla\psi)$).

- **Bazant, Z. P. & Najjar, L. J.** (1972). Nonlinear water diffusion in nonsaturated concrete. *Matériaux et Construction*, 5(25), 3–20. — Tortuosity law $\tau(\phi)$ used in the computation of diffusive fluxes.

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898. — Saturation curve model $S_r(r)$ used for the pore size distribution (parameters $m=0.2516$, $r_0 = 155.82\,\text{nm}$).
