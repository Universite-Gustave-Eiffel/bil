# Introduction

**Bil** is an open-source software framework based on finite element/volume methods, designed for the simulation of coupled phenomena in continuum mechanics, geomechanics, environmental engineering, and materials science. Bil is distributed under the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html) (GPL v3).

Bil is intended to be used by students, engineers or researchers to work out problems or develop their own models. Bil is written in C/C++ and can run on any OS with a C++ compiler installed (Linux, macOS, Windows).

Bil doesn’t include a mesh generator or a built-in post-processing tools. It relies on **[Gmsh](https://gmsh.info)** for mesh generation (`.msh` files) and post-processing (`.posi` files). The output files created by Bil for 1D problems can be used easily by some plotting programs such as **[Gnuplot](http://www.gnuplot.info)**.

## General overview

Bil adopts a **plugin architecture in C** that strictly separates the generic computation core (solvers, assembly, I/O) from physical constitutive models. Each physical model is an independent plugin implementing the interface contract defined by the `Model_t` structure.

This design makes it possible to:

- develop new models without modifying the framework
- combine different physics (hydro-mechanical, thermo-chemical couplings, …)
- work with 1D, 2D, and 3D meshes using finite element (FEM) and finite volume (FVM) discretizations

## Application domains

| Domain | Available models |
|--------|-----------------|
| **Flow** | Richards equation, Fick's second law, two-phase flow |
| **Mechanics** | Elasticity, elasticity with damage, hardening plasticity (Drucker-Prager, Cam-Clay, BBM) |
| **Poromechanics** | Biot's poroelasticity, poroplasticity |
| **Phase change** | Freeze-thaw in concrete and soils |
| **Durability & chemistry** | Carbonation, chloride ingress, sulfate attack, acid attack in concrete |
| **Coupled transfers** | Drying-wetting-salt, unsaturated soils |
| **Specialized** | Multi-scale mechanics by FEM² |

## Quick installation

```bash
# 1. Download Bil from the repository
wget https://github.com/Universite-Gustave-Eiffel/bil/archive/refs/heads/master.zip

# 2. Create a build directory at the Bil's source directory
cd bil-master
mkdir build

# 3. Run cmake from within the build directory pointing to the Bil's source directory
cd build
cmake ..

# 4. To build Bil, simply type
make

# 5. To install Bil type (this may require root permission)
make install

# To add more features in Bil's capabilities turn to ON options in the file OPIONS.
# To add private libraries to Bil, indicate their full path in the file EXTRALIBS. 
# Then repeat from step 3.

# Check if Bil is correctly installed
bil -info

# Run a calculation
bil -iperm my_file     # node renumbering (bandwidth optimization)
bil my_file            # run the calculation
```

!!! tip
    The `-iperm` option (Cuthill-McKee renumbering) should be run **before** the calculation
    to optimize matrix bandwidth when using Crout factorization. Not needed for multi-frontal
    (SuperLU, MA38) or Krylov space (PETSc) solvers.

## Documentation structure

| Chapter | Content |
|---------|---------|
| **Running Bil** | Command-line options, input file format, output files, solvers |
| **Examples** | Equations and annotated input files for each model |
| **Internal Architecture** | Plugin architecture, `Model_t` interface, computation loop |
| **How to Add a New Model** | Step-by-step guide, the 11 model functions, `MaterialPointMethod.h`, databases |

## Citation

> Dangla, P. (2024). *Bil — A modeling platform based on finite element/volume methods*. Université Gustave Eiffel.
