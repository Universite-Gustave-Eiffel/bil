# Running Bil

## Synopsis

```
bil [options] [my_file]
```

If `my_file` is provided, Bil computes the solution of the problem described in that input file.
Without any argument, the list of available options is displayed:

```bash
bil
```

---

## Input file

The file `my_file` provides the input data for the problem to be solved. A list of **reserved keywords** (with an upper-case first letter) organizes the data into several groups: mesh, material properties, boundary conditions, etc.

> Any content of a line after `#` is considered a comment and skipped.

### Required keywords

| Keyword | Description |
|---------|-------------|
| `Geometry` | Dimension and symmetry of the problem |
| `Mesh` | Mesh definition |
| `Material` | Material properties (repeated for each material) |
| `Fields` | Field definitions (spatial functions) |
| `Initialization` | Initial conditions |
| `Functions` | Time functions |
| `Boundary Conditions` | Boundary conditions |
| `Loads` | Loads |
| `Points` | Output points |
| `Dates` | Output dates |
| `Objective Variations` | Objective variations of the main unknowns |
| `Iterative Process` | Convergence criteria of the iterative process |
| `Time Steps` | Time step calculation parameters |

### Optional keywords

| Keyword | Description |
|---------|-------------|
| `Units` | Change units |
| `Periodicities` | Mesh periodicities for periodic problems |
| `Model` | Rename the unknowns of the model |

On-line help is available via:

```bash
bil -help
```

### Example input file

The following example illustrates a drainage problem for a 1 m sand column governed by Richards' equation. The column is initially saturated with liquid pressure $p_l(x) = p_{atm} - 9.81(x-1)$. At $t = 0$, the column is drained from the bottom by imposing $p_l = p_{atm}$.

```
# Drainage of a sand column

Geometry
1 Plan                          # 1D problem, plane symmetry

Mesh
col.msh                         # Gmsh mesh file (20 elements, 0 to 1 m)

Material
Model = M1                      # model code name
gravite = -9.81                 # gravity (m/s²)
phi = 0.3                       # porosity
rho_l = 1000                    # fluid mass density (kg/m³)
k_int = 4.4e-13                 # intrinsic permeability (m²)
mu_l = 0.001                    # fluid viscosity (Pa·s)
p_g = 100000                    # gas pressure (Pa)
Curves = tab                    # retention curve file: columns p_c, S_l, k_rl

Fields
2
Type = affine Value = 1.e5 Gradient = -9.81 0 0 Point = 1 0 0  # p_l = 1e5 - 9.81*(x-1)
Type = affine Value = 1.e5 Gradient = 0 0 0     Point = 0 0 0  # constant p_l = 1e5

Initialization
1
Region = 2 Unknown = p_l Field = 1   # initial p_l in region 2 (i.e. between 0 and 1)

Functions
0                               # no time function

Boundary Conditions
1
Region = 1 Unknown = p_l Field = 2 Function = 0   # p_l = 1e5 in region 1 (i.e. at bottom x = 0)

Loads
0

Points
0

Dates
2
0. 1800000                      # t0 = 0 s, t1 = 1800000 s

Objective Variations
p_l = 1000                      # objective variation Δp_l = 1000 Pa

Iterative Process
Iterations = 20                 # max nb of iterations
Tolerance = 1e-10               # convergence criterion
Repetitions = 0                 # no repetitions

Time Steps
Dtini = 1                       # initial time step = 1 s
Dtmax = 3600                    # maximum time step = 3600 s
```

---

## Output files

Each run produces **two sets of output files**.

### Point files `.pi` — results at points

`my_file.pi` where `i` ranges from 1 to the number of points defined by `Points`.
These files give the results at the specified points. The first column contains the times; other columns contain the model quantities.

### Date files `.ti` — results at dates

`my_file.ti` where `i` ranges from 0 to the number of dates defined by `Dates`.
The first three columns contain the node coordinates. The following columns contain the same quantities as in the `.pi` files.

> Lines commented with `#` in the first column indicate the nature of the computed quantities.

---

## Other files

Bil produces some files and sometimes needs other files to run properly. The name of these files are formed with the name of the input data file and suffixes:

| File | Description |
|------|-------------|
| `my_file.ti` | Results at date index `i` |
| `my_file.pi` | Results at point index `i` |
| `my_file.posi` | View `i` readable by Gmsh |
| `my_file.msh` | Gmsh mesh file |
| `my_file.graph` | Mesh graph |
| `my_file.graph.iperm` | Inverse permutations file |
| `my_file.sto` | Storage file |
| `my_file.cont` | Continuation file (without re-initialization) |
| `my_file.conti` | Continuation file (with re-initialization) |

### Resuming a calculation

- **`.cont`**: restart without executing the initialization step — the calculation continues as if there had been no interruption.
- **`.conti`**: restart with re-initialization — some variables can be reset (e.g. strain variables).

**Procedure:**

1. Copy `old_file.sto` to `new_file.cont` (or `.conti`)
2. Create `new_file` with additional dates beyond the last date of the previous run
3. Run `bil new_file`

---

## Calculation options

### Node renumbering

```bash
bil -iperm my_file
```

Generates `my_file.graph.iperm` with the inverse node permutation (HSL_MC40 method).

!!! warning "Important for 2D/3D problems"
    Run renumbering **before** the calculation for any 2D or 3D problem:

    ```bash
    bil -iperm my_file    # generates my_file.graph.iperm
    bil my_file           # calculation with optimized mesh
    ```

    For multi-frontal solvers (SuperLU, MA38) or Krylov space methods (PETSc), this file is not needed.

### Solver selection

```bash
bil -solver method my_file
```

| Method | Description |
|--------|-------------|
| `crout` | Crout method (default) |
| `superlu` | Sequential SuperLU (requires SUPERLU to be enabled) |
| `ma38` | HSL MA38 (requires BLAS and LAPACK to be enabled) |
| `petscksp` | PETSc KSP solver (requires PETSC to be enabled) |
<!--
| `superlumt` | Multi-threaded SuperLU (option `-nthreads N`) |
| `superludist` | Distributed SuperLU |
-->

**SuperLU:**

```bash
bil -solver superlu my_file
```

<!--
**Multi-threaded SuperLU:**

```bash
bil -solver superlumt -nthreads 4 my_file
```
-->

**PETSc KSP:**

```bash
bil -solver petscksp -ksp_type gmres -pc_type ilu my_file
```

### Resolution module

```bash
bil -with Monolithic N my_file   # N = number of time solutions kept in memory (default 2)
bil -with SNIA N my_file         # Sequential non-iterative approach
```

---

## Post-processing options

```bash
bil -post GmshParsed my_file   # generates .posi files in Gmsh parsed format
bil -post GmshASCII  my_file   # generates .posi files in Gmsh ASCII format
```

---

## Other options

| Option | Description |
|--------|-------------|
| `bil -help` | On-line help |
| `bil -model` | List available models |
| `bil -model mymodel` | Show example properties for `mymodel` |
| `bil -readonly my_file` | Parse input file, then quit |
| `bil -debug all my_file` | Display all parsed data structures |
