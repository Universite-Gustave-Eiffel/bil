# Fick Model — Fick's Second Law (Solute Diffusion)

> **Bil model:** `src/Models/ModelFiles/Fick.c`

> **Input file:** `doc/mkdocs/Flow/Fick/Fick`

---

## Table of contents

1. [Context and objective](#1-context-and-objective)
2. [Assumptions](#2-assumptions)
3. [Variables and mathematical model](#3-variables-and-mathematical-model)
4. [Input file description](#4-input-file-description)
5. [Expected results](#5-expected-results)
6. [Bibliographic references](#6-bibliographic-references)

---

## 1. Context and objective

The **Fick** model simulates the "pure" diffusion of a solute (typically alkali ions such as Sodium $Na^+$ or Potassium $K^+$) dissolved in an inert, water-saturated porous medium. It is based on **Fick's Second Law**, which describes how a material concentration field fluctuates over time under the sole influence of its own gradients. There is no macroscopic movement of the carrier fluid (no convection, no advection).

The example `test_examples/Fick/Fick` is a one-dimensional demonstration (`1 Axis`) of the leaching of a block of material (a 3 cm rod). It represents a specimen initially saturated with salt (uniform initial concentration), one end of which is suddenly "flushed" upon contact with pure water, causing desorption/diffusion from the interior outward. The history is traced over a full year (31,536,000 seconds).

---

## 2. Assumptions

1. **Finite Volume Method (FVM)**: Unlike structural models such as *Elast* solved by Finite Elements, this model uses the FVM architecture (`#include "FVM.h"`) implemented in Bil. This formulation integrates fluxes at boundaries, inherently guaranteeing **strict conservation of chemical moles** from one cell to another.
2. **Diffusion without convection**: The velocity of the pore water is perfectly zero. The profile evolves like a Gaussian spreading or an error function of a chemical wave toward its boundary.
3. **Porous medium tortuosity**: The geometry complicates the transport of molecules. The transfer coefficient accounts for liquid tortuosity (e.g., `TortuosityOhJang`) which depends intimately on the sole physical input variable: porosity $\phi$.

---

## 3. Variables and mathematical model

### Unknowns
| Symbol | Meaning |
|---------|-----------------|
| $c_{\text{na}}$   | Molarity or primary solute concentration (state unknown) |

### Mass conservation
The nodal equation implicitly named `solute` expresses a balance of molar masses of Na. Its implementation follows the classical time scheme:

$$ \frac{\partial c_{\text{na}}}{\partial t} \phi  + \nabla \cdot \mathbf{w}_{\text{na}} = 0 $$

Where the discrete FVM balance implementation is: `(N_solute - N_soluten) + dt * div(W_solute) = 0`

### Fick's Flux (Constitutive law)
The relationship governing the exchange vector between cells:

$$ \mathbf{w}_{\text{na}} = - D_{\text{eff}} \nabla c_{\text{na}} - \dots $$

With an effective transfer coefficient computed in the code as: $D_{\text{eff}} = D_{\text{water}} \cdot \tau(\phi)$ where $\tau$ is the tortuosity, and $D_{\text{water}}$ is the diffusivity of $Na^+$ in water at $298^\circ K$.

---

## 4. Input file description (`Fick`)

The experiment targets maximum simplicity through minimalist algorithms and syntax:

1. **Geometry & Mesh**
   ```text
   Geometry
   1 Axis
   ```
   The system switches to one-dimensional mode along the central axis. The mesh does not use a `.msh` surface file from `gmsh` but is generated internally by numerical parameters (a continuous segment resulting in 3 cm of length, `3.e-2` segmented into sub-cells of length $3\cdot 10^{-4}$ meters, i.e., 100 aligned cells).

2. **Material**
   ```text
   Material
   Model = Fick
   porosity = 0.379 
   ```
   A single macroscopic tensor, parameterized by a porosity of $\approx 38\%$. The ion diffusivity in water is not specified; the code injects its pre-programmed constant (`DiffusionCoefficientOfMoleculeInWater(Na)`).

3. **Initial gradients and Boundary Conditions**
   ```text
   Fields
   1
   Value = 19.
   
   Initialization
   1
   Region = 1 	Unknown = c_na  	    Field = 1
   ```
   Through the initialization process, the entire porous domain (Region 1) is assigned a saturating initial value of $19$ mol for the primary field $c_{na}$.

   ```text
   Functions
   1
   N = 2   F(0) = 1.   F(3600) = 0.
   
   Boundary Conditions
   1
   Reg = 2 	Unknown = c_na      	Field = 1 	Function = 1
   ```
   The boundary node (Reg 2 — presumably the outer face) is chemically maintained. Via the `Function`, this node had a value of $19 \times 1$ at absolute time $t=0$, then drops sharply to reach a complete $0$ after one hour (3600 s), which it maintains indefinitely. This represents complete flushing by external soaking of the block.

4. **Solver**
   ```text
   Dates
   4
   0	86400	259200   31536000
   ```
   The simulation extracts values precisely at $t=0$, after **1 day** (86400 s), **3 days**, and **1 year** (31,536,000 s). The `Iterative Process` runs via Newton bounded to 20 iterations.

---

## 5. Expected results

This 1D desalination experiment, visualizable after computation via the `.t1` to `.t3` files, illustrates **depletion diffusion**:
- The temporal profile will reveal the progressive loss of salts along the rod.
- At 1 day (`.t1`), the face $x=0$ in contact will equal 0, while the other end of the sample at $x=3$ cm might still display its native retention plateau at 19 moles, as the chemical wave has not yet reached the limiting depth due to the high tortuosity of concrete/rock.
- At the end of one year (`.t3`), the concentration should approach asymptotic residuals near 0 along all axial nodes, with all the Sodium or Potassium having escaped by entropic gradient into the infinite washing water of the boundary condition (Reg 2).

![Fick - Simulation Results](Fick_results.png)

---

## 6. Bibliographic references

- **Dangla, P.** — *Bil: a FEM/FVM platform for multiphysics simulations*.
- Physical basis of capillary pore tortuosity (**Oh & Jang, 2004** or **Bažant & Najjar, 1972** laws implemented in the bil preprocessor to retard flux).
- **Fick's Law** — Fundamental phenomenological-macroscopic formulation of matter transport by diffusion.
