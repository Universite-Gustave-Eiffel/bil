# Modèle M5 — Transferts couplés eau–air en milieu poreux non saturé (isotherme 1D)

> **Fichiers sources :**
> `src/Models/ModelFiles/M5.c` · `examples/m5/m5` · `docs/M5_VoronoiFVM.jl`
>
> **Auteur du modèle Bil :** P. Dangla (Université Gustave Eiffel)

---

## Table des matières

1. [Contexte et objectif](#1-contexte-et-objectif)
2. [Hypothèses](#2-hypothèses)
3. [Variables et notation](#3-variables-et-notation)
4. [Modèle mathématique](#4-modèle-mathématique)
   - 4.1 [Équations de conservation](#41-équations-de-conservation)
   - 4.2 [Lois de flux](#42-lois-de-flux)
   - 4.3 [Relations constitutives thermodynamiques](#43-relations-constitutives-thermodynamiques)
   - 4.4 [Courbes de rétention et perméabilités relatives](#44-courbes-de-rétention-et-perméabilités-relatives)
   - 4.5 [Modèle de tortuosité et diffusion gazeuse](#45-modèle-de-tortuosité-et-diffusion-gazeuse)
5. [Conditions aux limites et initiales](#5-conditions-aux-limites-et-initiales)
6. [Cas test : barrière ouvragée sous séchage (`examples/m5/m5`)](#6-cas-test--barrière-ouvragée-sous-séchage)
7. [Discrétisation numérique](#7-discrétisation-numérique)
8. [Références bibliographiques](#8-références-bibliographiques)

---

## 1. Contexte et objectif

Le modèle M5 décrit les transferts **isothermes couplés eau–air** dans un milieu poreux partiellement saturé. Il est typiquement utilisé pour simuler le **séchage et la réhumidification de matériaux cimentaires ou de sols** soumis à des conditions hydriques variables.

Le cas test `examples/m5/m5` représente une **barrière ouvragée** (p. ex. argile compactée ou béton) en contact avec un sol naturel humide. La barrière, initialement très sèche, se réhumidifie progressivement depuis l'interface avec le sol sur une échelle de temps centennale. Ce scénario est représentatif de la problématique des **stockages de déchets radioactifs** ou des **barrières de confinement** en génie civil.

Le domaine est unidimensionnel de longueur $L = 1.225$ m, composé de deux matériaux :

| Zone | $x$ [m] | Matériau | $\phi$ [-] | $k_\text{int}$ [m²] |
|------|---------|----------|------------|---------------------|
| Barrière | $[0,\ 0.425]$ | Argile/béton compacté | 0.30 | $10^{-20}$ |
| Sol | $[0.425,\ 1.225]$ | Sol naturel | 0.05 | $10^{-19}$ |

---

## 2. Hypothèses

1. **Isotherme** : la température est uniforme et constante ($T = 293$ K). Les propriétés physico-chimiques (viscosités, diffusivité, pression de vapeur saturante) sont des constantes.
2. **Porosité rigide** : le squelette solide est indéformable ; $\phi = \text{const}$.
3. **Deux phases fluides** : phase liquide (eau) et phase gazeuse (mélange binaire vapeur d'eau + air sec). La phase gazeuse est considérée comme un **gaz idéal**.
4. **Trois constituants** : eau liquide, vapeur d'eau (H₂O gazeux), air sec.
5. **Équilibre local vapeur–liquide** (hypothèse de Kelvin) : la pression de vapeur est une fonction univoque de la pression liquide à travers la loi de Kelvin.
6. **Pas de gravité** dans le cas test (domaine horizontal, $g = 0$).
7. **Loi de Darcy** pour le transport advectif de chaque phase fluide.
8. **Loi de Fick** pour la diffusion de vapeur dans le gaz.
9. **Continuité des pressions capillaires** à l'interface entre matériaux.

---

## 3. Variables et notation

### Inconnues primaires

| Symbole | Signification | Unité |
|---------|---------------|-------|
| $p_l$ | Pression de la phase liquide | Pa |
| $p_a$ | Pression partielle de l'air sec | Pa |

La pression gazeuse totale et la pression capillaire en découlent :

$$p_g = p_v(p_l) + p_a, \qquad p_c = p_g - p_l$$

### Variables secondaires

| Symbole | Signification |
|---------|---------------|
| $s_l$ | Degré de saturation en eau liquide |
| $s_g = 1 - s_l$ | Degré de saturation en gaz |
| $p_v$ | Pression partielle de vapeur d'eau |
| $\rho_v,\ \rho_a,\ \rho_g$ | Masses volumiques : vapeur, air sec, gaz total |
| $k_{rl},\ k_{rg}$ | Perméabilités relatives liquide et gaz |
| $c_v = \rho_v/\rho_g$ | Fraction massique de vapeur dans le gaz |

### Constantes physico-chimiques

| Symbole | Valeur | Signification |
|---------|--------|---------------|
| $T$ | 293 K | Température |
| $R$ | 8.314 J/(mol·K) | Constante des gaz parfaits |
| $M_w$ | 18×10⁻³ kg/mol | Masse molaire de l'eau |
| $M_a$ | 28.8×10⁻³ kg/mol | Masse molaire de l'air sec |
| $V_w$ | 18×10⁻⁶ m³/mol | Volume molaire de l'eau liquide |
| $\rho_l$ | 1000 kg/m³ | Masse volumique de l'eau liquide |
| $\mu_l$ | 10⁻³ Pa·s | Viscosité dynamique de l'eau |
| $\mu_g$ | 1.8×10⁻⁵ Pa·s | Viscosité dynamique du gaz |
| $p_{v0}$ | 2460 Pa | Pression de vapeur saturante à $T$ |
| $p_{l0}$ | 10⁵ Pa | Pression de référence liquide |
| $D_{av0}$ | 2.56×10⁻⁵ m²/s | Diffusivité H₂O dans l'air à $(T,\ p_{g0})$ |

---

## 4. Modèle mathématique

### 4.1 Équations de conservation

Le système est composé de **deux équations de bilan de masse** intégrées sur le volume élémentaire représentatif (VER) :

**Conservation de la masse totale d'eau** (liquide + vapeur) :

$$\frac{\partial m_T}{\partial t} + \nabla \cdot \mathbf{W}_T = 0$$

avec

$$m_T = \rho_l\,\phi\,s_l + \rho_g\,\phi\,s_g$$

**Conservation de la masse d'air sec** :

$$\frac{\partial m_A}{\partial t} + \nabla \cdot \mathbf{W}_A = 0$$

avec

$$m_A = \rho_a\,\phi\,s_g$$

> **Remarque :** Le terme $\rho_g\,\phi\,s_g$ dans $m_T$ regroupe vapeur et air car le flux total gaz $\mathbf{W}_g$ est indivisible dans la loi de Darcy. La séparation liquide/vapeur est assurée par le bilan air.

### 4.2 Lois de flux

Le flux total eau $\mathbf{W}_T$ et le flux air $\mathbf{W}_A$ sont :

$$\mathbf{W}_T = \mathbf{W}_l + \mathbf{W}_g, \qquad \mathbf{W}_A = \mathbf{W}_g - \mathbf{W}_v$$

**Flux de Darcy** pour les phases liquide et gazeuse (sans gravité) :

$$\mathbf{W}_l = -K_{D,l}\,\nabla p_l, \qquad K_{D,l} = \frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}$$

$$\mathbf{W}_g = -K_{D,g}\,\nabla p_g, \qquad K_{D,g} = \frac{\rho_g\,k_\text{int}\,k_{rg}(p_c)}{\mu_g}$$

**Flux de vapeur** (advection dans le gaz + diffusion de Fick) :

$$\mathbf{W}_v = c_v\,\mathbf{W}_g - K_{F,v}\,\nabla\rho_v, \qquad K_{F,v} = \phi\,s_g\,\tau\,D_{av}$$

où $D_{av} = D_{av0}\,p_{g0}/p_g$ est la diffusivité de la vapeur dans l'air à la pression $p_g$ (loi de correction en pression), et $\tau$ est le facteur de tortuosité (voir §4.5).

> **Avec gravité** ($g \neq 0$), la loi de Darcy s'écrit en charge hydraulique : $\mathbf{W}_\alpha = -K_{D,\alpha}\,\nabla H_\alpha$ avec $H_\alpha = p_\alpha - \rho_\alpha\,g\,z$.

### 4.3 Relations constitutives thermodynamiques

#### Équilibre vapeur–liquide (loi de Kelvin)

La pression de vapeur à l'équilibre thermodynamique est donnée par la relation de Kelvin :

$$\boxed{p_v = p_{v0}\,\exp\!\left(\frac{V_w}{RT}\,(p_l - p_{l0})\right)}$$

On en déduit l'humidité relative :

$$h_r = \frac{p_v}{p_{v0}} = \exp\!\left(\frac{V_w}{RT}\,(p_l - p_{l0})\right)$$

Cette relation couple directement $p_l$ à la teneur en vapeur : une pression liquide très négative (milieu sec) se traduit par une faible humidité relative.

#### Masses volumiques des phases gazeuses (gaz idéal)

$$\rho_v = \frac{p_v\,M_w}{RT}, \qquad \rho_a = \frac{p_a\,M_a}{RT}, \qquad \rho_g = \rho_v + \rho_a$$

### 4.4 Courbes de rétention et perméabilités relatives

Les courbes sont fournies sous forme analytique (expressions Van Genuchten) dans le fichier `examples/m5/m5`.

#### Matériau 1 — Barrière ($p_{c3} = 10^6$ Pa)

**Saturation** (Van Genuchten) :

$$s_l(p_c) = \left[1 + \left(\frac{p_c}{1.5 \times 10^6}\right)^{1.064}\right]^{-0.06}$$

**Perméabilité relative liquide** :

$$k_{rl}(p_c) = \left[1 + \left(\frac{p_c}{3 \times 10^6}\right)^{2}\right]^{-0.5}$$

#### Matériau 2 — Sol ($p_{c3} = 2 \times 10^5$ Pa)

**Saturation** :

$$s_l(p_c) = \left[1 + \left(\frac{p_c}{10^7}\right)^{1.7}\right]^{-0.412}$$

**Perméabilité relative liquide** :

$$k_{rl}(p_c) = \left[1 + \left(\frac{p_c}{10^7}\right)^{2}\right]^{-1}$$

#### Perméabilité relative gaz (commune aux deux matériaux, modèle de Mualem)

$$k_{rg}(s_l) = (1 - s_l)^2\,\left(1 - s_l^{5/3}\right)$$

#### Régularisation de la saturation au voisinage de $s_l = 1$

Pour éviter la dégénérescence de la matrice de capacité lorsque $p_c \to 0$, la saturation est régularisée pour $p_c < p_{c3}$ par un raccordement exponentiel :

$$s_l(p_c) = 1 - (1 - s_l(p_{c3}))\,\exp\!\left(\frac{p_c - p_{c3}}{p_{c3}}\right), \quad p_c < p_{c3}$$

Cette formule assure la continuité de $s_l$ et de sa dérivée en $p_{c3}$, et vérifie $s_l(0) = 1$.

### 4.5 Modèle de tortuosité et diffusion gazeuse

La tortuosité du réseau poreux pour la diffusion gazeuse est modélisée par la loi de puissance de Thiery *et al.* (2007) :

$$\tau(\phi, s_g) = \phi^{\,a}\,s_g^{\,b}, \qquad a = 1.74,\quad b = 3.2$$

Le coefficient de diffusion effectif de la vapeur d'eau dans le gaz est :

$$D_\text{eff} = \phi\,s_g\,\tau\,D_{av} = \phi\,s_g\,\phi^{a}\,s_g^{b}\,D_{av}$$

> **Comparaison :** Le modèle de Millington–Quirk (1961) utilise $a = 1/3$, $b = 7/3$.

---

## 5. Conditions aux limites et initiales

### Conditions initiales

| Zone | $p_l$ [Pa] | $p_a$ [Pa] | $s_l$ [-] | $h_r$ [-] |
|------|-----------|-----------|----------|----------|
| Barrière | $-7.611 \times 10^7$ | $9.226 \times 10^4$ | ≈ 0.78 | ≈ 0.57 |
| Sol | $4.905 \times 10^6$ | $4.892 \times 10^6$ | ≈ 1.00 | > 1 |

> La barrière est initialement très sèche (forte pression capillaire $p_c \approx 7.6 \times 10^7$ Pa). Le sol est quasi-saturé ($p_l > 0$, en surpression hydrostatique).

### Conditions aux limites

| Bord | $x$ [m] | Type | Valeur |
|------|---------|------|--------|
| Gauche | 0 | Neumann nul | $\mathbf{W}_T \cdot \mathbf{n} = 0$, $\mathbf{W}_A \cdot \mathbf{n} = 0$ |
| Droit | 1.225 | Dirichlet | $p_l = 4.905 \times 10^6$ Pa, $p_a = 4.892 \times 10^6$ Pa |

Le bord gauche est **imperméable** (paroi ou plan de symétrie). Le bord droit maintient des conditions hydriques constantes représentant le sol naturel humide.

---

## 6. Cas test : barrière ouvragée sous séchage

### Paramètres de simulation

| Paramètre | Valeur |
|-----------|--------|
| Durée totale | 100 ans ($3.154 \times 10^9$ s) |
| Pas de temps initial $\Delta t_\text{ini}$ | 100 s |
| Pas de temps maximal $\Delta t_\text{max}$ | 1 an |
| Variation objective (OBJE) | $\Delta p_l = \Delta p_a = 10^4$ Pa |
| Tolérance Newton | $10^{-6}$ |
| Maillage | 25 éléments dans la barrière + 75 éléments dans le sol |

### Physique attendue

- La barrière initialement sèche ($h_r \approx 0.57$) se **réhumidifie** progressivement depuis le bord droit (sol humide). Le front d'humidification progresse de droite à gauche.
- La pression liquide $p_l$ dans la barrière **augmente** avec le temps (de $-7.6 \times 10^7$ Pa vers une valeur proche des conditions du sol).
- Le flux d'eau liquide est dominant pour les grandes pressions capillaires ; la diffusion gazeuse contribue significativement lorsque $p_c$ est modéré.
- Les deux matériaux ayant des perméabilités très différentes ($k_\text{int}$ diffère d'un facteur 10), l'évolution est beaucoup plus lente dans la barrière.

---

## 7. Discrétisation numérique

Le modèle est discrétisé par la méthode des **volumes finis centrés sur les cellules (cell-centered FVM)**, telle qu'implémentée dans Bil via `FVM.h`. La discrétisation spatiale utilise un schéma de différences centrées pour les flux (valeurs des conductances évaluées au pas de temps précédent — schéma semi-implicite).

Le système non-linéaire est résolu à chaque pas de temps par la **méthode de Newton–Raphson**, avec une matrice Jacobienne assemblée par différentiation numérique des flux et termes de stockage (voir `ComputeComponentDerivatives()` dans M5.c).

Le pas de temps est contrôlé par une stratégie adaptative basée sur la variation maximale des inconnues primaires (**OBJE** dans Bil, `Δu_opt` dans VoronoiFVM).

Le portage Julia utilise la bibliothèque **VoronoiFVM.jl**, qui implémente la même méthode FVM avec différentiation automatique (ForwardDiff.jl) pour le Jacobien.

---

## 8. Références bibliographiques

### Milieux poreux non saturés

- **Bear, J.** (1972). *Dynamics of Fluids in Porous Media*. Elsevier, New York. — Référence fondamentale pour les transferts en milieu poreux, lois de Darcy généralisées.

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons, Chichester. — Cadre thermodynamique pour les milieux poreux multi-phasiques, définition rigoureuse des pressions capillaires et des énergies de surface.

- **Whitaker, S.** (1977). Simultaneous heat, mass and momentum transfer in porous media: a theory of drying. *Advances in Heat Transfer*, 13, 119–203. — Fondements des équations de transfert couplé par prise de moyenne volumique.

### Courbes de rétention et perméabilités relatives

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898. — Expression analytique de $s_l(p_c)$ utilisée dans M5.

- **Mualem, Y.** (1976). A new model for predicting the hydraulic conductivity of unsaturated porous media. *Water Resources Research*, 12(3), 513–522. — Modèle de perméabilité relative à partir de la courbe de rétention, base de $k_{rg}(s_l)$.

### Équilibre vapeur–liquide

- **Kelvin, Lord (W. Thomson)** (1871). On the equilibrium of vapour at a curved surface of liquid. *Philosophical Magazine*, 42, 448–452. — Loi de Kelvin reliant la pression de vapeur à la courbure de l'interface liquide–gaz (et donc à $p_c$).

- **Defay, R., Prigogine, I., Bellemans, A. & Everett, D. H.** (1966). *Surface Tension and Adsorption*. Longmans, London. — Développement thermodynamique de la loi de Kelvin pour les milieux poreux.

### Diffusion gazeuse et tortuosité

- **Millington, R. J. & Quirk, J. P.** (1961). Permeability of porous solids. *Transactions of the Faraday Society*, 57, 1200–1207. — Modèle de tortuosité classique ($a=1/3$, $b=7/3$).

- **Thiery, M., Villain, G., Dangla, P. & Platret, G.** (2007). Investigation of the carbonation front shape on cementitious materials: Effects of the chemical kinetics. *Cement and Concrete Research*, 37(7), 1047–1058. — Paramètres de tortuosité recalibrés sur matériaux cimentaires ($a=1.74$, $b=3.2$) utilisés dans M5.

- **Reid, R. C., Prausnitz, J. M. & Poling, B. E.** (1987). *The Properties of Gases and Liquids* (4e éd.). McGraw-Hill, New York. — Propriétés physico-chimiques des gaz (diffusivité binaire H₂O–air, viscosités).

### Stockage de déchets et barrières de confinement

- **ANDRA** (2005). *Dossier 2005 Argile — Tome Architecture et gestion du stockage géologique*. Agence nationale pour la gestion des déchets radioactifs, Châtenay-Malabry. — Contexte applicatif des barrières ouvragées en argile compactée.

- **Cui, Y. J., Yahia-Aïssa, M. & Delage, P.** (2002). A model for the volume change behavior of heavily compacted swelling clays. *Engineering Geology*, 64(2–3), 233–250. — Comportement hydromécanique des argiles compactées utilisées comme barrières.

### Implémentation numérique

- **Dangla, P.** — *Bil : a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Code source : <https://github.com/Universite-Gustave-Eiffel/bil>

- **Fuhrmann, J.** (2021–). *VoronoiFVM.jl: Solver for coupled nonlinear PDEs based on Voronoi finite volume discretization*. Zenodo. <https://github.com/j-fu/VoronoiFVM.jl> — Bibliothèque Julia utilisée pour le portage numérique.
