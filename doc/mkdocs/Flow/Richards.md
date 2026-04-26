# Richards-2d — Drainage of a bead column

**Bil Model:** `Richards` ([src/Models/ModelFiles/Richards.cpp](../../../src/Models/ModelFiles/Richards.cpp))

**Input file:** `doc/mkdocs/Flow/Richards/Richards-2d`

---

## Physical context

This example simulates the **gravity drainage of a heterogeneous porous medium column** composed of two zones with different permeabilities. It is a canonical problem in unsaturated hydrogeology: we observe how water drains from a granular material (beads) under gravity when the pressure imposed at the base suddenly shifts from saturated to unsaturated state.

---

## Mathematical equations: the Richards equation

The model solves the **Richards equation** (1931), which governs the flow of liquid in a partially saturated porous medium under the assumption that gas pressure remains constant (continuous gas phase).

### Liquid mass balance

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{w}_l = 0$$

with the liquid mass content:

$$m_l = \rho_l \, \phi \, S_l(p_c)$$

### Generalized Darcy's law

$$\mathbf{w}_l = -k_l \left( \nabla p_l - \rho_l \, \mathbf{g} \right)$$

with the **effective liquid permeability**:

$$k_l = \rho_l \, \frac{k_\text{int}}{\mu_l} \, k_{rl}(p_c)$$

### Variables and closure relations

| Symbol | Meaning | Value |
|---|---|---|
| $p_l$ | Liquid pressure (primary unknown) | — |
| $p_c = p_g - p_l$ | Capillary pressure | $p_g = 0$ Pa |
| $S_l(p_c)$ | Degree of saturation | `billes` curve |
| $k_{rl}(p_c)$ | Liquid relative permeability | `billes` curve |
| $\phi$ | Porosity | 0.38 |
| $\rho_l$ | Liquid mass density | 1000 kg/m³ |
| $k_\text{int}$ | Intrinsic permeability | 8.9×10⁻¹² m² (outer zone) / 8.9×10⁻¹³ m² (inner zone) |
| $\mu_l$ | Dynamic viscosity | 0.001 Pa·s (water at 20 °C) |
| $g$ | Gravitational acceleration | −9.81 m/s² |

### Time scheme

The flux $\mathbf{w}_l$ is computed with the permeability $k_l$ from the **previous time step** (Picard lagging), while the mass content $m_l$ is implicit in the current $p_l$. This is the classical IMPES scheme for the Richards equation.

### Bibliographic reference

> **Richards, L.A. (1931).** *Capillary conduction of liquids through porous mediums.*
> Physics, 1(5), 318–333.

---

## Geometry and mesh

The file `columncomposite.geo` defines a **composite 2D rectangular column**:

```
W = 0.02 m  (total width)
H = 0.20 m  (total height)
w = W/2     (inclusion width)
h = H/2     (inclusion height)
```

The column is composed of two material zones:

| GMSH Region | Physical surface | Role | Intrinsic permeability |
|---|---|---|---|
| Surface(100) | Outer ring (domain – inclusion) | more permeable zone | 8.9×10⁻¹² m² |
| Surface(101) | Centered inner rectangle | less permeable inclusion | 8.9×10⁻¹³ m² |

```
┌──────────────────────┐
│   Zone 100           │  k_int = 8.9e-12 m²
│   ┌────────────┐     │
│   │  Zone 101  │     │  k_int = 8.9e-13 m²
│   │ (inclusion)│     │
│   └────────────┘     │
│                      │
└──────────────────────┘
    Line 11 (base, region 11)
```

---

## Retention curves and relative permeability: file `billes`

The `billes` file contains a 3-column table: $p_c$ [Pa], $S_l$ [−], $k_{rl}$ [−].

| $p_c$ range | Behavior |
|---|---|
| 500 – 577 Pa | $S_l = 1$, $k_{rl} = 1$ → fully saturated medium |
| 577 – 1000 Pa | $S_l$ and $k_{rl}$ decrease rapidly |
| $p_c = 1000$ Pa | $S_l \approx 0.09$, $k_{rl} \approx 2.6 \times 10^{-8}$ → nearly dry medium |

The **air-entry pressure** is around 575 Pa, and the **residual saturation** is approximately 9%. These curves are typical of an assembly of calibrated spherical beads (Brooks-Corey or van Genuchten type curve with experimentally fitted parameters).

---

## Line-by-line description of the `Richards-2d` file

```
# Drainage d'une colonne de billes
```
Descriptive comment of the test case.

---

### `Geometry`

```
2 plan
```

2D planar space. The Richards equation is solved in a vertical plane.

---

### `Mesh`

```
columncomposite.msh
```

GMSH mesh of the composite column (generated from `columncomposite.geo`).

---

### `Material` — block 1 (outer zone, Surface 100)

```
Model = Richards
Gravity = -9.81
Porosity = 0.38
LiquidMassDensity = 1000
IntrinsicPermeability = 8.9e-12
LiquidViscosity = 0.001
ReferenceGasPressure = 0
Curves = billes
```

| Parameter | Meaning |
|---|---|
| `Model = Richards` | Selects the `Richards.cpp` module |
| `Gravity = -9.81` | Gravity along negative y-axis (downward), in m/s² |
| `Porosity = 0.38` | 38% voids |
| `LiquidMassDensity = 1000` | Water, in kg/m³ |
| `IntrinsicPermeability = 8.9e-12` | Intrinsic permeability in m² (outer zone) |
| `LiquidViscosity = 0.001` | Dynamic viscosity of water at 20 °C, in Pa·s |
| `ReferenceGasPressure = 0` | Reference gas pressure $p_g = 0$ Pa |
| `Curves = billes` | Curve file for $S_l(p_c)$ and $k_{rl}(p_c)$ |

---

### `Material` — block 2 (inner zone, Surface 101)

Identical to block 1 except:

```
IntrinsicPermeability = 8.9e-13
```

The inclusion is **10 times less permeable** than the outer zone, locally slowing drainage.

---

### `Fields`

```
1
Value = 0  Gradient = 0 -9810 0  Point = 0 0.2 0
```

Defines the **linear hydrostatic pressure field**:

$$p_l(x, y) = 0 + (-9810) \times (y - 0.2) = 9810 \times (0.2 - y) \text{ Pa}$$

anchored at point $(0, 0.2, 0)$ (column top) where $p_l = 0$ Pa. This field corresponds to the hydrostatic equilibrium of a saturated column.

---

### `Initialization`

```
2
Region = 100  Unknown = p_l  Field = 1  Function = 0
Region = 101  Unknown = p_l  Field = 1  Function = 0
```

Initializes $p_l$ in both regions with the hydrostatic field (Field 1, factor = 1). The column starts **fully saturated at hydrostatic equilibrium**.

---

### `Functions`

```
1
N = 2  F(0) = 1  F(360) = 0
```

Linear time function equal to 1 at $t = 0$ s and decreasing to 0 at $t = 360$ s. It modulates the amplitude of the base boundary condition.

---

### `Boundary Conditions`

```
1
Region = 11  Unknown = p_l  Field = 1  Function = 1
```

The condition is applied on the **column base** (line 11, lower boundary):

$$p_l^{\text{base}}(t) = \underbrace{9810 \times 0.2}_{\text{Field 1 at } y=0} \times \underbrace{\text{Function}_1(t)}_{\in [0,1]}$$

- At $t = 0$: $p_l = 1962$ Pa (saturated state)
- At $t = 360$ s: $p_l = 0$ Pa (atmospheric pressure → suction)

In practice: **zero pressure is progressively imposed at the base**, triggering downward drainage of the column.

---

### `Loads`

```
0
```

No external volumetric liquid mass source.

---

### `Points`

```
0
```

No particular output points.

---

### `Dates`

```
16
0 200 400 600 800 1000 1200 1400 1600 1800 2000 2200 2400 2600 2800 3000
```

16 output times, from $t = 0$ to $t = 3000$ s (50 minutes), every 200 s.

---

### `Objective Variations`

```
p_l = 1.e3
```

Admissible variation of $p_l$ between two adaptive time step iterations: **1000 Pa**. Controls the automatic time step size.

---

### `Iterative Process`

```
Iter = 20
Tol = 1.e-6
Recom = 2
```

| Parameter | Meaning |
|---|---|
| `Iter = 20` | Maximum 20 Newton-Raphson iterations per time step |
| `Tol = 1.e-6` | Relative convergence tolerance |
| `Recom = 2` | On non-convergence: time step subdivision, up to 2 times |

---

### `Time Steps`

```
Dtini = 1
Dtmax = 1000
```

| Parameter | Meaning |
|---|---|
| `Dtini = 1` | Initial time step: 1 s |
| `Dtmax = 1000` | Maximum time step: 1000 s |

The solver automatically adapts the time step between these bounds based on convergence and the objective variation of $p_l$.

---

## Computed outputs

The model computes and exports (`Richards.cpp`, function `ComputeOutputs`):

| Variable | Output name | Unit |
|---|---|---|
| Liquid pressure | `pressure` | Pa |
| Liquid mass flux | `flow` | kg/m²/s |
| Degree of saturation | `saturation` | — |

---

## Summary of the simulated scenario

1. **$t = 0$ s**: composite column fully saturated at hydrostatic equilibrium.
2. **$0 < t \leq 360$ s**: pressure at the base is progressively reduced to 0 Pa → the bottom of the column begins to drain.
3. **$t > 360$ s**: zero pressure maintained at the base → the desaturation front moves up through the column under gravity.
4. **Effect of heterogeneity**: the central inclusion (10× less permeable) locally slows drainage → a **desaturation delay** is observed in the inner zone, a characteristic phenomenon of heterogeneous porous media under unsaturated conditions.

## Simulation results

![Richards-2d - Simulation Results](Richards/Richards-2d_results.png)
*(Automatically generated graphs for the Richards-2d example)*
