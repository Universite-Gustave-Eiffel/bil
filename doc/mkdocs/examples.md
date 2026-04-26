# Examples

This chapter presents annotated examples for each of the models available in Bil, organized by application domain. Each section provides the governing equations and a commented input file. However the generic set of mathematical equations that every model refers to are systematically the conservation of mass for some elementary chemical species or substance and the static conservation of momentum if relevant. For example the conservation of mass of $A$ reads

$$\frac{\partial m_A}{\partial t} + \nabla \cdot \mathbf{w}_A = 0$$

where $m_A$ is the mass content of $A$ per unit volume of material and $\mathbf{w}_A$ is the mass flow vector of $A$. There is no source term in all these equations because it is implicitly assumed that each equation accounts for the content of $A$ in every phase of the material, whether solid, liquid, or gas. When $A$ refers to some atom (e.g. Ca, Si, Cl, etc...) the equation can refer equivalently to the conservation of the mole content instead of the mass content. There are two situations when $A$ doesn't refer to an atom or a species: the conservation of the total mass (in this case $A$ is either absent or is 'T') and the conservation of the total charge that ions carry (in this case $A$ is noted 'Q').

The static conservation of momemtum or the mechanical equilibrium equation reads

$$\nabla \cdot \boldsymbol{\sigma} + \mathbf{f} = \mathbf{0}$$

When relevant there is an additional equation expressing the electroneutrality of the material or more specifically the pore electrolyte solution:

$$\sum_i z_i \rho_i = 0$$

where $z_i$ stands for the electric charge of the ion $i$ and $\rho_i$ is the mole concentration of $i$ in the solution.

Finally some models need a thermal equation which can be expressed by an entropy balance:

$$\frac{\partial S_{\text{tot}}}{\partial t} + \nabla \cdot \left( \frac{\mathbf{q}}{T} + \sum_{\alpha} s_{\alpha} \mathbf{w}_{\alpha} \right) = 0$$

where $S_{\text{tot}}$ is the total entropy per unit volume of material and $\mathbf{q}$ is the heat flow vector. In addition $s_{\alpha}$ and $\mathbf{w}_{\alpha}$ refer to the specific entropy and the mass flow vector of the phase $\alpha$ either liquid or gas.

Each model presented in the following sections is based on these equations for a given set of defined $A$.

| Domain | Models |
|--------|--------|
| [Flow](Flow/flow_examples.md) | M1, M2, Fick, M10, Richards |
| [Mechanics](Mechanics/mechanics_examples.md) | Elast, Elasd, Plast, BBM |
| [Poromechanics](Poromechanics/poromechanics_examples.md) | M7, Poroplast |
| [Phase change](PhaseChange/phasechange_examples.md) | Frostaco, Frostaco3d |
| [Durability & chemistry](Durability/durability_examples.md) | Duracem, Chloricem, Sulfaco, Sulfuricem, Sulfaconew |
| [Coupled transfers](CoupledTransfers/coupledtransfers_examples.md) | DWS1, Yuan1, Shen, usoil |
| [Specialized](Specialized/specialized_examples.md) | MechaMic (FEM²) |
