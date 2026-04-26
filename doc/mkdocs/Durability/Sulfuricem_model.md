# Sulfuricem Model — Sulfuric Acid Attack on Cementitious Materials

> **Bil model:** `src/Models/ModelFiles/Sulfuricem.c`

> **Input file:** `doc/mkdocs/Durability/Sulfuricem/Sulfuricem`
>
> **Model authors:** O. Grandclerc, B. Yuan, P. Dangla

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and notation](#3-variables-and-notation)
4. [Mathematical model](#4-mathematical-model)
   - 4.1 [Conservation equations](#41-conservation-equations)
   - 4.2 [Dissolution and precipitation reactions](#42-dissolution-and-precipitation-reactions)
5. [Boundary and initial conditions](#5-boundary-and-initial-conditions)
6. [Test case: 1D acid attack on a cement specimen (`test_examples/Sulfuricem`)](#6-test-case-1d-acid-attack-on-a-cement-specimen)
7. [Modelling results](#7-modelling-results)
8. [Step-by-step input file description](#8-step-by-step-input-file-description)
9. [Bibliographic references](#9-bibliographic-references)

---

## 1. Context and objective

The **Sulfuricem** model simulates the chemical degradation (and reactive transport response) of cementitious materials (OPC and CAC — *Portland* and *Aluminous*) exposed to **sulfuric acid** ($\text{H}_2\text{SO}_4$), typical of biogenic corrosion in sewage networks or extreme industrial attacks.

This model integrates a strong coupling between multi-species ionic transport in the pore solution and the chemical thermodynamics of cement hydrates. Sulfuric attack translates into a sudden pH drop, dissolution of protective cement phases (Portlandite CH, C-S-H), releasing the matrix, followed by expansive secondary re-precipitation as **Gypsum** (CSH2) and **Ettringite** (AFt).

---

## 2. Assumptions

1. **Full coupled chemistry**: Explicitly integrates transport under concentration and electric potential gradients.
2. **Mass action law**: Chemical equilibrium in the pore solution is based on ion pair equilibrium expressions.
3. **Heterogeneous solid mixtures**: Solids are quantified via dissolution/precipitation kinetics with saturation thresholds (expressed in moles).
4. **Conservation of conservative atoms ("Zeta" $\zeta_{x}$)**: Bulk chemical balances are conducted around neutral element markers (computing Z_Ca for all dissolved and solid calcium instead of splitting the equation).
5. **Cross-charge electroneutrality**: A Poisson field ensures electrical neutrality of the solution via an artificial electromotive potential variable $\psi$.

---

## 3. Variables and notation

The model defines **8 coupled equations** at nodes by finite volumes.

### Primary unknowns (nodal vectors)

| Symbol | BIL variable label | Meaning |
|---------|--------------------|---------|
| $c_{\text{H}_2\text{SO}_4}$ | `logc_h2so4` | Total sulfuric acid concentration (imposed logarithmic scale) |
| $\psi$ | `psi` | Local electric potential (Nernst-Planck ionic diffusion) |
| $Z_{\text{Ca}}$ | `z_ca` | Global conservative unknown for Calcium ion |
| $Z_{\text{Si}}$ | `z_si` | Global conservative unknown for Silica |
| $c_{\text{K}}$ | `logc_k` | Alkali Potassium ion concentration |
| $c_{\text{Cl}}$ | `logc_cl` | Chloride concentration |
| $Z_{\text{Al}}$ | `z_al` | Conservative unknown for Aluminum |
| $c_{\text{OH}}$ | `logc_oh` | Hydroxide ion concentration (defining electroneutrality and pH) |

### Behavior variables

| Symbol | Meaning |
|---------|---------|
| `N_CH`, `N_CSH` | Local molarities of Portlandite (CH) and Calcium Silicate Hydrates (C-S-H) |
| `N_CSH2` | Local molarity of precipitated Gypsum (hydrated calcium sulfate) |
| $\phi$ | Instantaneous porosity. Increases with dissolution, drops with gypsum/ettringite precipitation |
| $S_{\text{CH}}$, $S_{\text{CSH2}}$ | Physico-chemical saturation indices driving the kinetics |

---

## 4. Mathematical model

### 4.1 Conservation equations

The system is based on strict mole conservation per constituent atom (S, Ca, Si, K, Cl, Al). For example, the Sulfur $S$ balance:

$$\frac{\partial}{\partial t}\left(\phi \cdot c_S^{tot} + N_{\text{gypsum}} + N_{\text{ettringite}} \right) + \text{div}(\mathbf{W}_S) = 0$$

Fluxes $\mathbf{W}_i$ are given by the *Nernst-Planck* equation, governing diffusion under tortuosity (Oh-Jang or Bazant-Najjar models) and electric migration.

### 4.2 Dissolution and precipitation reactions

Local reaction rates are based on saturation. For gypsum $CSH_2$:

$$ \frac{\partial N_{\text{gypsum}}}{\partial t} = \frac{1}{\tau_{\text{CSH2}}} (S_{CSH2} - 1) $$

*(with lower bound at 0 if gypsum does not exist under undersaturation.)*

The assembly of chemical components of the C-S-H phase depends on the intrinsic multi-pole curves of the `C/S` ratio coded in the `csh3p` database.

---

## 5. Boundary and initial conditions

Boundary conditions generally correspond to a laboratory setup (acid bath):

- **Exposed region (boundary)**: Dirichlet imposition via `Boundary Conditions` macro on `logc_h2so4` ($-\text{pH}_{\text{acid}}$) and `logc_cl`. The potential is also anchored (typically $\psi = 0$).
- **Original mass (initials)**: Variables `z_ca` and `z_si` are dimensioned to initialize Portlandite (CH) and C-S-H at initial contents equivalent to those of a healthy, non-degraded cement paste (pH ~13).

---

## 6. Test case: 1D acid attack on a cement specimen (`test_examples/Sulfuricem`)

The provided case simulates prolonged exposure of a concrete plane to a highly aggressive solution.

1. Sulfuric acid penetrates through the exposed surface (right side of mesh). The front advances by first decomposing the portlandite, releasing dissolved Calcium which reacts violently with the incoming sulfate SO₄²⁻.
2. Formation of a massive transient Gypsum (CSH2) gangue.
3. Total modification of the porous network.

---

## 7. Modelling results

After execution, nodal outputs `.p1` at the attacked boundary (or by deep integration) allow tracking of degradation over time.

![Sulfuricem Test Results](Sulfuricem/Sulfuricem_results.png)

1. **pH impact**: Sharp pH drop toward harmful acidity (carbonation/sulfation front).
2. **Dissolution**: The front consumes all native CH of the matrix.
3. **Gypsum precipitation**: Spectacular development of confined Gypsum (CSH2) is observed, abruptly swelling the solid volume fraction.
4. **Retroactive porosity evolution**: Initially, CH loss increases porosity, but this is immediately countered (then completely congested) by the expansive formation of calcium sulfate (gypsum and ettringite).

---

## 8. Step-by-step input file description

### File `Sulfuricem`

1. **Mesh `Mesh`**: Creates a 1D discretized volume on a thin strip via internal command: `4 \n  0.8 0.8 1. 1. \n 2.e-3 \n 1 100 1...` (100 cells on one axis).
2. **Material properties `Material`**: Defines a theoretical healthy, initially undegraded saturated "paste":
   - `porosite = 0.3`: Moderate initial porosity.
   - `N_CH = 5.2`, `N_CSH = 5.`: High typical hydrate binder content.
   - `N_CSH2 = 0.`, `N_AFt = 0.`: Degraded phases not yet formed.
   - `Curves_log = csh3p ...`: Incorporates a link on CSH heterogeneity from the companion text file `csh3p`.
3. **Fields and `Initialization`**: Standard log values (-35, -3, -1...) representing a healthy initial ultra-trace acid concentration, and potentials through `z_ca`, `z_si`.
4. **Boundary Conditions**: `Reg 3 Unknown = logc_h2so4 Field = 1 Function = 1` → At contact plane 3, a concentrated acid with functional factor 1 is imposed (time evolution for a gradual shock, parameterized in the `Functions` block).
5. **Time control (`Iterative Process` & `Time Steps`)**: A pseudo-logarithmic step starts at `Dtini = 100 s.` spread over several hundred days (2.89e7 s). The algorithm operates by heavy implicit method.

---

## 9. Bibliographic references

- **Grandclerc, O.** (2018). *Modelling of reactive transport in saturated porous media and mechano-chemical couplings induced by dissolution-precipitation reactions: Application to acid attack on cementitious materials*. Université Paris-Est.
- **Yuan, B., Dangla, P., & Chatellier, P.** (2013). *Numerical modeling of sulfuric acid attack on concrete via a reactive transport approach*.
- **Samson, E., Marchand, J.** (2007). *Modeling the effect of temperature on ionic transport in cementitious materials.*
