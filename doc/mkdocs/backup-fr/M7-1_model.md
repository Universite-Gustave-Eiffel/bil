# Modèle M7 — Poroélasticité non saturée de Biot · Cas test M7-1 : Barrage de Ternay

> **Fichiers sources :**
> `src/Models/ModelFiles/M7.c` · `examples/M7-1/M7-1` · `examples/M7-1/ternay.geo`
>
> **Auteur du modèle Bil :** P. Dangla (Université Gustave Eiffel)

---

## Table des matières

1. [Contexte et objectif](#1-contexte-et-objectif)
2. [Géométrie et maillage](#2-géométrie-et-maillage)
3. [Hypothèses du modèle](#3-hypothèses-du-modèle)
4. [Variables et notation](#4-variables-et-notation)
5. [Modèle mathématique](#5-modèle-mathématique)
   - 5.1 [Loi de comportement poroélastique (Biot)](#51-loi-de-comportement-poroélastique-biot)
   - 5.2 [Équation d'équilibre mécanique](#52-équation-déquilibre-mécanique)
   - 5.3 [Équation de conservation de la masse d'eau](#53-équation-de-conservation-de-la-masse-deau)
   - 5.4 [Loi de Darcy](#54-loi-de-darcy)
   - 5.5 [Pression équivalente π (pression de Bishop généralisée)](#55-pression-équivalente-π-pression-de-bishop-généralisée)
6. [Paramètres matériaux](#6-paramètres-matériaux)
7. [Conditions aux limites et chargements](#7-conditions-aux-limites-et-chargements)
8. [Paramètres de simulation](#8-paramètres-de-simulation)
9. [Sorties calculées](#9-sorties-calculées)
10. [Physique attendue](#10-physique-attendue)
11. [Décryptage ligne à ligne du fichier d'entrée](#11-décryptage-ligne-à-ligne-du-fichier-dentrée)
12. [Références bibliographiques](#12-références-bibliographiques)

---

## 1. Contexte et objectif

Le cas test M7-1 simule en **2D plan** le comportement hydromécanique couplé d'un **barrage-poids en béton posé sur un massif rocheux**, vraisemblablement inspiré du **barrage de Ternay** (isère, France). Le barrage est soumis à la mise en eau du réservoir.

Le modèle M7 implémente la **poroélasticité non saturée de Biot**. Dans ce cas test particulier, les courbes de rétention capillaire imposent la saturation complète ($S_l = 1$) pour toute pression capillaire, ce qui ramène le problème à une **poroélasticité saturée de Biot** classique.

Le couplage hydromécanique est à double sens :

- La **surpression interstitielle** réduit les contraintes effectives dans le béton et le rocher (soulèvement).
- La **déformation volumique** du squelette solide modifie la porosité et donc la capacité de stockage hydraulique.

---

## 2. Géométrie et maillage

Le domaine est défini dans `examples/M7-1/ternay.geo` (format GMSH). Il comprend **deux surfaces** :

```
Surface 1 (béton)  : contour fermé par les lignes 101–113
Surface 2 (roche)  : contour fermé par les lignes 121–125 + 113
```

### Coordonnées clés (en mètres, axes X horizontal et Y altitude)

| Point | X (m) | Y (m, NGF) | Rôle |
|-------|--------|------------|------|
| 1 | 0 | 476 | Pied amont gauche |
| 2–5 | 0–4.9 | 476–494 | Parement amont |
| 6 | 4.9 | 517 | Cote de la retenue (crête) |
| 7–9 | 8.1–8.8 | 512–517 | Déversoir / sommet |
| 9→10 | — | — | Arc de cercle (parement aval courbe) |
| 10–13 | 18.6–27 | 478–476 | Pied aval |
| 14–17 | −16 à 43 | 455–476 | Emprise du massif rocheux |

### Schéma de principe

```
              Retenue (réservoir)
   y=517  ─────────┬──────┐
                   │ béton│  parement aval (arc)
                   │      ╰──────────────────╮
   y=476  ─────────┴──────────────────────────┴────────── x
          ← roche (fondation, y : 455→476) →
   y=455  ─────────────────────────────────────────────── x
         x=-16                                        x=43
```

- **Parement amont** (lignes 101–105) : face en contact avec l'eau du réservoir.
- **Crête et parement aval** (lignes 106–112) : surface libre, drainée.
- **Base du barrage** (ligne 113, $y = 476$ m) : interface béton/rocher.
- **Massif rocheux** : rectangle sous le barrage, encastré en pied et sur les côtés.

Le maillage `ternay.msh` (format GMSH 2.2) comprend **479 nœuds** avec une taille de maille locale $l_{c1} = 1.5$ m dans le béton et $l_{c2} = 4$ m dans la roche.

---

## 3. Hypothèses du modèle

1. **Poroélasticité linéaire de Biot** : le squelette solide se déforme élastiquement ; les paramètres de Biot $b$ (coefficient de Biot) et $N$ (module de stockage à volume constant) sont constants.
2. **Milieu saturé** (dans ce cas test) : les courbes de rétention imposent $S_l = 1$ et $k_{rl} = 1$ pour toute pression capillaire ; la pression de gaz est $p_g = 0$.
3. **Gravité nulle** ($g = 0$) : le poids propre n'est pas pris en compte (les conditions initiales comprennent une contrainte lithostatique nulle).
4. **Déformations planes** (2 plan) : problème 2D.
5. **Loi de Darcy isotrope** : perméabilité intrinsèque scalaire $k_\text{int}$.
6. **Linéarité des équations** : pas de non-linéarité géométrique ni matérielle.

---

## 4. Variables et notation

### Inconnues primaires

| Symbole | Signification | Unité |
|---------|---------------|-------|
| $u_1, u_2$ | Composantes du déplacement du squelette | m |
| $p_l$ | Pression de la phase liquide (interstitielle) | Pa |

### Variables secondaires (calculées)

| Symbole | Définition | Signification |
|---------|-----------|---------------|
| $\boldsymbol{\varepsilon}$ | $\frac{1}{2}(\nabla \mathbf{u} + \nabla^T \mathbf{u})$ | Tenseur des déformations |
| $\varepsilon_v = \text{tr}\,\boldsymbol{\varepsilon}$ | $\varepsilon_{11} + \varepsilon_{22} + \varepsilon_{33}$ | Déformation volumique |
| $\boldsymbol{\sigma}$ | — | Tenseur des contraintes totales |
| $m_l$ | $\rho_l \phi S_l$ | Masse volumique de liquide dans les pores |
| $\phi$ | $\phi_0 + \Delta\phi$ | Porosité courante |
| $\mathbf{w}_l$ | — | Flux massique de liquide |
| $p_c = p_g - p_l$ | — | Pression capillaire |
| $S_l(p_c)$ | courbe de rétention | Degré de saturation |
| $\pi(p_l, p_g)$ | — | Pression équivalente de Bishop généralisée |

### Paramètres matériaux

| Symbole | Signification |
|---------|---------------|
| $E$ | Module de Young |
| $\nu$ | Coefficient de Poisson |
| $\mu = E / 2(1+\nu)$ | Module de cisaillement |
| $\lambda = 2\mu\nu/(1-2\nu)$ | Coefficient de Lamé |
| $b$ | Coefficient de Biot |
| $N$ | Module de stockage à déformation nulle |
| $k_\text{int}$ | Perméabilité intrinsèque |
| $\mu_l$ | Viscosité dynamique du fluide |
| $\rho_l$ | Masse volumique du fluide |
| $\phi_0$ | Porosité initiale |

---

## 5. Modèle mathématique

### 5.1 Loi de comportement poroélastique (Biot)

La loi de Biot étendue au cas non saturé est :

$$\boxed{\boldsymbol{\sigma} = \boldsymbol{\sigma}_0 + \mathbf{C}:\boldsymbol{\varepsilon} - b\,(\pi - \pi_0)\,\mathbf{1}}$$

avec $\mathbf{C}$ le tenseur élastique isotrope :

$$C_{ijkl} = \lambda\,\delta_{ij}\delta_{kl} + \mu\,(\delta_{ik}\delta_{jl} + \delta_{il}\delta_{jk})$$

La **variation de porosité** est donnée par :

$$\Delta\phi = b\,\varepsilon_v + N\,(\pi - \pi_0)$$

où $\pi_0 = \pi(p_{l0}, p_g)$ est la pression de référence et $\pi$ la pression équivalente de Bishop généralisée (voir §5.5).

En milieu saturé ($S_l = 1$), $\pi = p_l$ et l'on retrouve la formulation standard de Biot :

$$\boldsymbol{\sigma} = \boldsymbol{\sigma}_0 + \lambda\,\varepsilon_v\,\mathbf{1} + 2\mu\,\boldsymbol{\varepsilon} - b\,(p_l - p_{l0})\,\mathbf{1}$$

### 5.2 Équation d'équilibre mécanique

$$\nabla \cdot \boldsymbol{\sigma} + \rho_\text{tot}\,\mathbf{g} = \mathbf{0}$$

Avec $g = 0$ dans ce cas test :

$$\nabla \cdot \left[\lambda\,\varepsilon_v\,\mathbf{1} + 2\mu\,\boldsymbol{\varepsilon} - b\,(\pi - \pi_0)\,\mathbf{1}\right] = \mathbf{0}$$

En décomposant :

$$(\lambda + \mu)\,\nabla(\nabla\cdot\mathbf{u}) + \mu\,\Delta\mathbf{u} = b\,\nabla\pi$$

Le terme de droite est la **force volumique due à la surpression interstitielle** (force de soulèvement hydrostatique).

### 5.3 Équation de conservation de la masse d'eau

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{w}_l = 0$$

avec la masse de liquide par unité de volume :

$$m_l = \rho_l\,\phi\,S_l$$

En développant $\dot{m}_l$ :

$$\rho_l\,\frac{\partial}{\partial t}\!\left(\phi_0 + b\,\varepsilon_v + N(\pi - \pi_0)\right) S_l + \rho_l\,\phi\,\dot{S}_l + \nabla \cdot \mathbf{w}_l = 0$$

Le premier terme couple la variation de volume (consolidation) à l'évolution hydraulique. Le terme $\dot{S}_l$ est nul en milieu saturé.

### 5.4 Loi de Darcy

Le flux massique de liquide obéit à la loi de Darcy généralisée :

$$\mathbf{w}_l = -\frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}\,\nabla p_l + \frac{\rho_l^2\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}\,\mathbf{g}$$

Avec $g = 0$ et $k_{rl} = 1$ (saturation complète) :

$$\mathbf{w}_l = -\frac{\rho_l\,k_\text{int}}{\mu_l}\,\nabla p_l$$

Le coefficient de conductivité hydraulique effectif est $K_l = \rho_l\,k_\text{int}/\mu_l$ (calculé en tant que variable explicite `va[0]` dans le code).

### 5.5 Pression équivalente π (pression de Bishop généralisée)

La pression de Bishop généralisée $\pi$ est définie par l'intégrale thermodynamique :

$$\pi(p_l, p_g) = S_l(p_c)\,p_l + S_g(p_c)\,p_g - \frac{2}{3}\,U(p_c)$$

où :

$$U(p_c) = \int_0^{p_c} S_l(p'_c)\,dp'_c - S_l(p_c)\,p_c$$

est l'énergie de surface associée à l'interface capillaire (terme de Suction, négatif de l'intégrale de Gibbs-Duhem).

Dans le code, $U(p_c)$ est calculée par quadrature de Gauss–Legendre à 3 points (fonction `pie()`). En milieu saturé ($S_l = 1$, $p_g = 0$), on retrouve simplement $\pi = p_l$.

---

## 6. Paramètres matériaux

| Paramètre | Béton (Surface 1) | Roche (Surface 2) | Unité |
|-----------|-------------------|-------------------|-------|
| $\rho_s$ | 2300 | 2500 | kg/m³ |
| $E$ (Young) | $1.4 \times 10^{10}$ | $1.8 \times 10^{10}$ | Pa |
| $\nu$ (Poisson) | 0.15 | 0.15 | — |
| $\phi_0$ | 0.05 | 0.40 | — |
| $\rho_l$ | 1000 | 1000 | kg/m³ |
| $p_{l0}$ | 0 | 0 | Pa |
| $k_\text{int}$ | $10^{-14}$ | $10^{-11}$ | m² |
| $\mu_l$ | $10^{-3}$ | $10^{-3}$ | Pa·s |
| $p_g$ | 0 | 0 | Pa |
| $b$ (Biot) | 0.4 | 0.2 | — |
| $N$ (stockage) | $10^{-10}$ | $10^{-10}$ | Pa⁻¹ |
| Courbes | `beton` ($S_l=1$) | `roche` ($S_l=1$) | — |

**Conductivités hydrauliques effectives** ($K = k_\text{int}/\mu_l$) :

| Matériau | $K$ (m²/Pa·s) | $K_h = K\,\rho_l\,g$ (m/s) |
|----------|---------------|--------------------------|
| Béton | $10^{-11}$ | $\approx 10^{-10}$ m/s |
| Roche | $10^{-8}$ | $\approx 10^{-7}$ m/s |

La roche est **1000 fois plus perméable** que le béton : la dissipation de pression se fera principalement dans le béton.

---

## 7. Conditions aux limites et chargements

### Champ de pression de référence (Field 1)

Champ affine représentant la pression hydrostatique du réservoir (niveau = 517 m NGF, $\rho_l\,g = 10^4$ Pa/m) :

$$p_l^\text{ref}(y) = 10000 \times (517 - y) \quad [\text{Pa}]$$

| Altitude | $p_l^\text{ref}$ |
|----------|-----------------|
| $y = 517$ m (cote retenue) | 0 Pa |
| $y = 476$ m (base barrage) | 410 000 Pa |
| $y = 455$ m (pied de fondation) | 620 000 Pa |

### Conditions hydrauliques

| Région | Localisation | Condition |
|--------|-------------|-----------|
| 121 | Interface roche/amont, $y=476$, $x \in [-16;0]$ | $p_l = p_l^\text{ref}(y)$ (amont) |
| 101–105 | Parement amont béton | $p_l = p_l^\text{ref}(y)$ (amont) |
| 106–112 | Crête + parement aval béton | $p_l = 0$ (drainé) |
| 125 | Interface roche/aval, $y=476$, $x \in [27;43]$ | $p_l = 0$ (drainé) |

### Conditions mécaniques

| Région | Localisation | Condition |
|--------|-------------|-----------|
| 122 | Côté gauche roche ($x = -16$) | $u_1 = 0$ (déplacement horizontal bloqué) |
| 123 | Base roche ($y = 455$) | $u_1 = 0$, $u_2 = 0$ (encastrement) |
| 124 | Côté droit roche ($x = 43$) | $u_1 = 0$ |

### Chargements mécaniques (pression d'eau sur le parement amont)

Les régions 101–105 et 121 reçoivent une **pression normale** égale à $p_l^\text{ref}(y)$ (poussée hydrostatique du réservoir sur le parement) :

$$\mathbf{t} = -p_l^\text{ref}(y)\,\mathbf{n}$$

---

## 8. Paramètres de simulation

| Paramètre | Valeur |
|-----------|--------|
| Géométrie | 2D plan |
| Durée | 0 → 1000 (jours) |
| Nombre de pas | 10 |
| $\Delta t_\text{ini}$ | 100 |
| $\Delta t_\text{max}$ | 100 |
| Variation objective $p_l$ | $10^4$ Pa |
| Variation objective $u_1, u_2$ | $10^{-3}$ m |
| Tolérance Newton | $10^{-4}$ |
| Itérations max | 3 |

---

## 9. Sorties calculées

Le modèle M7 exporte 7 quantités par point de calcul (`so7`) :

| Indice | Nom | Dimension | Description |
|--------|-----|-----------|-------------|
| 0 | `pression-liquide` | scalaire | $p_l$ [Pa] |
| 1 | `deplacements` | vecteur (3) | $(u_1, u_2, 0)$ [m] |
| 2 | `flux-liquide` | vecteur (3) | $\mathbf{w}_l$ [kg/m²/s] |
| 3 | `contraintes` | tenseur (9) | $\sigma_{ij}$ [Pa] |
| 4 | `porosite` | scalaire | $\phi$ [-] |
| 5 | `saturation` | scalaire | $S_l$ [-] |
| 6 | `pression-pi` | scalaire | $\pi$ [Pa] |

---

## 10. Physique attendue

### Mise en eau et dissipation des pressions interstitielles

Lors de la mise en eau, la pression hydrostatique du réservoir est appliquée instantanément sur le parement amont. La **surpression interstitielle** se diffuse ensuite à travers le béton et le rocher selon la loi de consolidation de Biot :

$$c_v \,\Delta p_l = \dot{p}_l, \qquad c_v = \frac{k_\text{int}/\mu_l}{b^2/(\lambda+2\mu) + N\,S_l^2}$$

Les temps de consolidation caractéristiques sont :

$$T_c \approx \frac{L^2}{c_v}$$

Pour $L \sim 10$ m (épaisseur béton) :
- **Béton** : $c_v \approx \frac{10^{-11}}{(0.4)^2/(4 \times 10^9)} \approx 2.5 \times 10^{-4}$ m²/s → $T_c \approx 4 \times 10^5$ s $\approx$ **5 jours**
- **Roche** : 1000× plus rapide → $T_c \approx$ quelques minutes

Le béton est le **composant limitant** de la consolidation. À $t = 1000$ jours, la distribution de pression doit être proche du régime permanent (écoulement en régime établi).

### Déformation et soulèvement

La pression interstitielle réduit les contraintes effectives :

$$\boldsymbol{\sigma}' = \boldsymbol{\sigma} + b\,\pi\,\mathbf{1}$$

La poussée amont génère un **moment de renversement** partiellement compensé par le poids propre. La surpression interstitielle dans la fondation crée une **force de soulèvement** (uplift) qui réduit la stabilité au glissement à la base.

En régime établi, le champ de pression dans la fondation suit le potentiel hydraulique (solution de l'équation de Laplace $\Delta p_l = 0$ avec les CL amont/aval), et le déplacement atteint son état d'équilibre.

---

## 11. Décryptage ligne à ligne du fichier d'entrée

Le fichier `examples/M7-1/M7-1` est le fichier de données principal de la simulation. Il est lu par le parseur Flex/Bison de Bil. Chaque **bloc** commence par un mot-clé et se termine par une ligne vide. Voici son contenu annoté intégralement.

---

### Bloc `Geometry` — Dimension et type de problème

```
Geometry
2 plan
```

| Mot-clé | Valeur | Signification |
|---------|--------|---------------|
| `Geometry` | — | Début du bloc géométrie |
| `2` | dimension | Problème **2D** |
| `plan` | mode | Hypothèse des **déformations planes** (`plan` = plane strain). L'alternative serait `axi` (axisymétrique) ou `3` (3D). |

---

### Bloc `Mesh` — Maillage

```
Mesh
 ternay.msh
```

| Mot-clé | Valeur | Signification |
|---------|--------|---------------|
| `Mesh` | — | Début du bloc maillage |
| `ternay.msh` | nom de fichier | Fichier de maillage au format **GMSH 2.2**, situé dans le même répertoire. Il contient 479 nœuds et les éléments triangulaires/quadrangulaires des deux surfaces. |

---

### Blocs `Material` — Propriétés des matériaux (×2)

Il y a **deux blocs Material**, un par zone physique du maillage (Surface 1 = béton, Surface 2 = roche). Bil les associe aux surfaces dans l'ordre de déclaration.

#### Matériau 1 — Béton (Surface physique 1)

```
Material
Model = M7
gravite = 0
rho_s = 2300
young = 1.4e+10
poisson = 0.15
phi = 0.05
rho_l = 1000
p_l0 = 0
k_int = 1e-14
mu_l = 0.001
p_g = 0.
b = 0.4
N = 1e-10
Curves = beton
```

| Ligne | Mot-clé | Valeur | Signification |
|-------|---------|--------|---------------|
| 1 | `Material` | — | Début du bloc matériau |
| 2 | `Model` | `M7` | Identifiant du modèle physique : appelle les fonctions `dm7`, `ct7`, `mx7`, `rs7`, `so7` de `M7.c` |
| 3 | `gravite` | `0` | Accélération de la pesanteur [m/s²]. Mis à 0 : le poids propre est négligé. |
| 4 | `rho_s` | `2300` | Masse volumique du squelette sec [kg/m³]. Utilisé uniquement pour le terme de gravité dans le résidu mécanique. |
| 5 | `young` | `1.4e+10` | Module de Young $E$ [Pa] = 14 GPa. Rigidité du béton. |
| 6 | `poisson` | `0.15` | Coefficient de Poisson $\nu$ [-]. Béton peu compressible latéralement. |
| 7 | `phi` | `0.05` | Porosité initiale $\phi_0$ [-] = 5%. Faible porosité du béton de barrage. |
| 8 | `rho_l` | `1000` | Masse volumique du fluide $\rho_l$ [kg/m³] = eau à 20°C. |
| 9 | `p_l0` | `0` | Pression liquide de référence $p_{l0}$ [Pa]. Définit l'état initial sans contrainte capillaire. |
| 10 | `k_int` | `1e-14` | Perméabilité intrinsèque $k_\text{int}$ [m²] = $10^{-14}$ m², équivalent à un béton très peu perméable. |
| 11 | `mu_l` | `0.001` | Viscosité dynamique de l'eau $\mu_l$ [Pa·s] = $10^{-3}$ Pa·s à 20°C. |
| 12 | `p_g` | `0.` | Pression de gaz $p_g$ [Pa]. Nulle : le gaz est à pression atmosphérique de référence. Implique $p_c = -p_l$. |
| 13 | `b` | `0.4` | Coefficient de Biot $b$ [-]. Représente la fraction de la variation de pression interstitielle transmise aux contraintes totales ($b=1$ = milieu totalement saturé avec grains incompressibles). |
| 14 | `N` | `1e-10` | Coefficient de stockage $N$ [Pa⁻¹] à déformation nulle. Lié à la compressibilité des pores et du fluide. |
| 15 | `Curves` | `beton` | Nom du fichier de courbes de rétention capillaire. Contient deux colonnes ($p_c$, $S_l$, $k_{rl}$). Ici, le fichier `beton` impose $S_l = 1$ et $k_{rl} = 1$ : milieu **saturé**. |

#### Matériau 2 — Roche (Surface physique 2)

```
Material
Model = M7
gravite = 0
rho_s = 2500
young = 1.8e+10
poisson = 0.15
phi = 0.4
rho_l = 1000
p_l0 = 0
k_int = 1e-11
mu_l = 0.001
p_g = 0
b = 0.2
N = 1e-10
Curves = roche
```

Mêmes mots-clés que le béton. Différences notables :

| Paramètre | Béton | Roche | Interprétation |
|-----------|-------|-------|----------------|
| `rho_s` | 2300 | 2500 | Rocher plus dense |
| `young` | 1.4e10 | 1.8e10 | Rocher plus rigide |
| `phi` | 0.05 | 0.40 | Rocher beaucoup plus poreux (fractures ?) |
| `k_int` | 1e-14 | 1e-11 | Rocher 1000× plus perméable |
| `b` | 0.4 | 0.2 | Couplage hydromécanique plus faible dans le rocher (grains plus rigides) |
| `Curves` | `beton` | `roche` | Deux fichiers de courbes distincts, mais même contenu ($S_l=1$) |

---

### Bloc `Fields` — Champs spatiaux

```
Fields
1
Type = affine Value = 0 Gradient = 0 -10000 0 Point = 0 517 0
```

| Ligne | Mot-clé | Valeur | Signification |
|-------|---------|--------|---------------|
| 1 | `Fields` | — | Début du bloc champs |
| 2 | `1` | — | Nombre de champs définis |
| 3 | `Type` | `affine` | Champ **linéaire** (affine) dans l'espace : $f(\mathbf{x}) = f_0 + \mathbf{G} \cdot (\mathbf{x} - \mathbf{x}_0)$ |
| | `Value` | `0` | Valeur $f_0$ au point de référence $\mathbf{x}_0$ |
| | `Gradient` | `0 -10000 0` | Gradient $\mathbf{G} = (G_x, G_y, G_z)$ [Pa/m]. $G_y = -10000$ Pa/m = $-\rho_l g$ : pression qui **diminue** quand $y$ augmente (gradient hydrostatique). |
| | `Point` | `0 517 0` | Point de référence $\mathbf{x}_0 = (0, 517, 0)$ m. C'est la **cote de la retenue** (plan d'eau à $y = 517$ m), où la pression est nulle. |

Ce champ définit la pression hydrostatique du réservoir :
$$p_l^\text{ref}(y) = 0 + (0)(x-0) + (-10000)(y - 517) + 0 = 10000\,(517 - y) \quad [\text{Pa}]$$

---

### Bloc `Initialization` — Conditions initiales

```
Initialization
0
```

| Ligne | Signification |
|-------|---------------|
| `Initialization` | Début du bloc d'initialisation |
| `0` | **Aucune** initialisation particulière : toutes les inconnues sont mises à zéro ($u_i = 0$, $p_l = 0$). L'état initial correspond au barrage sans réservoir, sans contrainte initiale. |

---

### Bloc `Functions` — Fonctions temporelles

```
Functions
0
```

| Ligne | Signification |
|-------|---------------|
| `Functions` | Début du bloc fonctions |
| `0` | **Aucune** fonction temporelle définie. Les chargements sont donc **statiques** (les conditions aux limites ne varient pas dans le temps). La mise en charge est appliquée à $t=0$ et maintenue constante. |

---

### Bloc `Boundary Conditions` — Conditions aux limites

```
Boundary Conditions
18
Region = 122 Unk = u_1   Field = 0 Function = 0
Region = 123 Unk = u_1   Field = 0 Function = 0
Region = 123 Unk = u_2   Field = 0 Function = 0
Region = 124 Unk = u_1   Field = 0 Function = 0
Region = 121 Unk = p_l   Field = 1 Function = 0
Region = 101 Unk = p_l   Field = 1 Function = 0
Region = 102 Unk = p_l   Field = 1 Function = 0
Region = 103 Unk = p_l   Field = 1 Function = 0
Region = 104 Unk = p_l   Field = 1 Function = 0
Region = 105 Unk = p_l   Field = 1 Function = 0
Region = 106 Unk = p_l   Field = 0 Function = 0
Region = 107 Unk = p_l   Field = 0 Function = 0
Region = 108 Unk = p_l   Field = 0 Function = 0
Region = 109 Unk = p_l   Field = 0 Function = 0
Region = 110 Unk = p_l   Field = 0 Function = 0
Region = 111 Unk = p_l   Field = 0 Function = 0
Region = 112 Unk = p_l   Field = 0 Function = 0
Region = 125 Unk = p_l   Field = 0 Function = 0
```

**Syntaxe générale d'une ligne :**
```
Region = <tag> Unk = <inconnue> Field = <n> Function = <m>
```

- `Region` : numéro de la **Physical Line** (ou Physical Surface) dans le fichier GMSH auquel s'applique la condition.
- `Unk` : nom de l'inconnue contrainte (défini dans `dm7` : `u_1`, `u_2`, `p_l`).
- `Field = n` : impose la valeur du champ n°n comme condition de Dirichlet. `Field = 0` impose la valeur 0. `Field = 1` impose le champ affine défini plus haut (pression hydrostatique).
- `Function = m` : multiplie la valeur par la fonction temporelle n°m. `Function = 0` → facteur 1 (aucune modulation temporelle).

**Détail de chaque condition :**

| # | Région | Unk | Field | Localisation physique | Interprétation |
|---|--------|-----|-------|-----------------------|----------------|
| 1 | 122 | $u_1$ | 0 | Côté gauche roche ($x=-16$) | Déplacement horizontal **bloqué** |
| 2 | 123 | $u_1$ | 0 | Base roche ($y=455$) | Déplacement horizontal bloqué |
| 3 | 123 | $u_2$ | 0 | Base roche ($y=455$) | Déplacement vertical **bloqué** : encastrement complet en base |
| 4 | 124 | $u_1$ | 0 | Côté droit roche ($x=43$) | Déplacement horizontal bloqué |
| 5 | 121 | $p_l$ | 1 | Tête amont roche ($y=476$, $x \in [-16;0]$) | Pression hydrostatique du réservoir (sous la base du barrage, côté amont) |
| 6–10 | 101–105 | $p_l$ | 1 | Parement amont béton | Pression hydrostatique : réservoir plein, $p_l = 10000(517-y)$ |
| 11–17 | 106–112 | $p_l$ | 0 | Crête + parement aval béton | **Drainage libre** : $p_l = 0$ (pression atmosphérique de référence) |
| 18 | 125 | $p_l$ | 0 | Tête aval roche ($y=476$, $x \in [27;43]$) | Drainage libre côté aval |

> **Note :** Il n'y a pas de condition de déplacement sur le parement du barrage (lignes 101–112) : la surface du béton est **libre** mécaniquement (sauf via les chargements de pression ci-dessous).

---

### Bloc `Loads` — Chargements mécaniques

```
Loads
6
Region = 101 Equ = meca_1 Type = pressure Field = 1 Function = 0
Region = 102 Equ = meca_1 Type = pressure Field = 1 Function = 0
Region = 103 Equ = meca_1 Type = pressure Field = 1 Function = 0
Region = 104 Equ = meca_1 Type = pressure Field = 1 Function = 0
Region = 105 Equ = meca_1 Type = pressure Field = 1 Function = 0
Region = 121 Equ = meca_1 Type = pressure Field = 1 Function = 0
```

**Syntaxe générale :**
```
Region = <tag> Equ = <équation> Type = <type> Field = <n> Function = <m>
```

- `Equ` : équation dans laquelle le chargement est injecté. `meca_1` = équation d'équilibre mécanique selon $x_1$ (direction horizontale). En 2D, cela génère les composantes normales et tangentielles par projection sur la normale sortante de la région.
- `Type = pressure` : charge de type **pression normale**. La force surfacique appliquée est $\mathbf{t} = -p\,\mathbf{n}$ (signe négatif : pression compressive sur la surface).
- `Field = 1` : valeur de la pression = champ affine n°1 = $10000\,(517 - y)$ Pa.

**Interprétation :**

Les régions 101–105 et 121 constituent le **parement amont** du barrage et la face amont du massif rocheux. Le réservoir y exerce une **poussée hydrostatique** :

$$\mathbf{t}(y) = -p_l^\text{ref}(y)\,\mathbf{n} = -10000\,(517-y)\,\mathbf{n}$$

Cette force augmente linéairement avec la profondeur. Elle tend à faire basculer le barrage vers l'aval et à le comprimer horizontalement.

> **Cohérence hydraulique/mécanique :** Les mêmes régions reçoivent à la fois la condition de pression interstitielle (`Boundary Conditions`) et le chargement mécanique (`Loads`). C'est la formulation correcte en poroélasticité : le réservoir impose simultanément la **pression dans les pores** de la surface en contact et la **pression totale** comme chargement mécanique.

---

### Bloc `Points` — Points de sortie

```
Points
0
```

Aucun point de sortie spécifique demandé. Les résultats sont calculés sur l'ensemble du maillage.

---

### Bloc `Dates` — Instants de sortie

```
Dates
11
0 100 200 300 400 500 600 700 800 900 1000
```

| Ligne | Signification |
|-------|---------------|
| `Dates` | Début du bloc |
| `11` | Nombre d'instants de sortie |
| `0 100 … 1000` | Liste des instants [unité de temps du problème]. Avec $k_\text{int}/\mu_l$ en m²/Pa·s et $E$ en Pa, l'unité de temps cohérente avec les paramètres est la **seconde**. Les pas de 100 s et le maximum à 1000 s correspondent à ~17 minutes, ce qui est adapté aux temps de consolidation du béton ($T_c \sim 10^5$ s). Ces instants sont aussi les instants auxquels Bil écrit les fichiers de résultats (`.t0`, `.t1`, …). |

---

### Bloc `Objective Variations` — Contrôle adaptatif du pas de temps

```
Objective Variations
u_1 = 0.001
u_2 = 0.001
p_l = 10000
```

Ce bloc définit les **variations maximales admissibles** des inconnues entre deux pas de temps (critère OBJE de Bil). Bil ajuste $\Delta t$ pour ne pas dépasser ces seuils :

| Inconnue | Variation objectif | Interprétation |
|----------|--------------------|----------------|
| `u_1` | $10^{-3}$ m | Déplacement horizontal : variation max de 1 mm par pas |
| `u_2` | $10^{-3}$ m | Déplacement vertical : variation max de 1 mm par pas |
| `p_l` | $10^4$ Pa | Pression interstitielle : variation max de 10 kPa par pas |

Si la variation calculée dépasse l'objectif, Bil réduit le pas de temps (et peut le rallonger si la variation est très faible, jusqu'à `Dtmax`).

---

### Bloc `Iterative Variations` — Paramètres de Newton-Raphson

```
Iterative Variations
Iter  = 3
Tol   = 0.0001
Recom = 0
```

| Mot-clé | Valeur | Signification |
|---------|--------|---------------|
| `Iter` | `3` | Nombre maximal d'**itérations de Newton** par pas de temps. Faible car le problème est linéaire (poroélasticité sans non-linéarité) : 1 à 2 itérations suffisent. |
| `Tol` | `0.0001` | Tolérance de convergence (norme relative du résidu). Si la norme du résidu est inférieure à $10^{-4}$ fois la norme initiale, la convergence est acceptée. |
| `Recom` | `0` | `0` = pas de recommencement automatique si la convergence échoue. Bil continuera avec le résultat non convergé. |

---

### Bloc `Time Steps` — Contrôle du pas de temps

```
Time Steps
Dtini = 100
Dtmax = 100
```

| Mot-clé | Valeur | Signification |
|---------|--------|---------------|
| `Dtini` | `100` | Pas de temps **initial** $\Delta t_0$ [unité de temps]. |
| `Dtmax` | `100` | Pas de temps **maximum** $\Delta t_\text{max}$. Ici, $\Delta t_\text{ini} = \Delta t_\text{max}$ : le pas de temps ne pourra pas dépasser 100 s. Combiné aux 11 instants de sortie (de 0 à 1000 par pas de 100), chaque intervalle entre deux sorties est parcouru en **exactement un pas de temps** (sauf si le critère OBJE force une subdivision). |

---

### Récapitulatif structurel du fichier

```
examples/M7-1/M7-1
│
├── Geometry          → 2D déformations planes
├── Mesh              → ternay.msh (GMSH, 479 nœuds, 2 surfaces)
│
├── Material (×2)
│   ├── [1] béton    → M7, E=14 GPa, k=1e-14 m², b=0.4, Curves=beton
│   └── [2] roche    → M7, E=18 GPa, k=1e-11 m², b=0.2, Curves=roche
│
├── Fields            → 1 champ affine : pression hydrostatique p(y)=10000(517-y)
├── Initialization    → état initial nul
├── Functions         → aucune fonction temporelle
│
├── Boundary Conditions (18)
│   ├── Mécaniques    → u₁=0 sur côtés et base roche (4 CL)
│   └── Hydrauliques  → p_l = hydrostatique (amont, 6 CL) | p_l = 0 (aval, 8 CL)
│
├── Loads (6)         → poussée hydrostatique sur parement amont (meca_1, pressure)
│
├── Points            → 0 (pas de point de sortie ponctuel)
├── Dates             → 11 instants : 0, 100, ..., 1000
│
├── Objective Variations → Δu≤1 mm, Δp_l≤10 kPa (contrôle Δt adaptatif)
├── Iterative Variations → max 3 iter Newton, tol=1e-4
└── Time Steps        → Dtini=Dtmax=100 (pas fixe de 100 s)
```

---

## 12. Références bibliographiques

### Poroélasticité de Biot

- **Biot, M. A.** (1941). General theory of three-dimensional consolidation. *Journal of Applied Physics*, 12(2), 155–164. — Article fondateur couplant déformation élastique et diffusion de fluide en milieu poreux saturé.

- **Biot, M. A.** (1955). Theory of elasticity and consolidation for a porous anisotropic solid. *Journal of Applied Physics*, 26(2), 182–185. — Extension aux milieux anisotropes.

- **Coussy, O.** (1995). *Mechanics of Porous Continua*. John Wiley & Sons, Chichester. — Cadre thermodynamique complet ; définition rigoureuse du coefficient de Biot $b$ et du module de stockage $N$.

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons, Chichester. — Extension au cas non saturé ; définition de la pression de Bishop généralisée $\pi$.

### Poroélasticité non saturée et pression de Bishop

- **Bishop, A. W.** (1959). The principle of effective stress. *Teknisk Ukeblad*, 106(39), 859–863. — Introduction du concept de contrainte effective en milieu non saturé via un paramètre de pondération $\chi$.

- **Dangla, P., Coussy, O. & Olchitzky, E.** (1997). Nonlinear thermo-mechanical couplings in unsaturated porous media. *IUTAM Symposium on Mechanics of Granular and Porous Materials*, 369–380. — Définition thermodynamique de la pression équivalente $\pi$ (implémentée dans `pie()` de M7.c).

### Consolidation de barrages

- **Terzaghi, K.** (1943). *Theoretical Soil Mechanics*. John Wiley & Sons, New York. — Théorie 1D de la consolidation ; base conceptuelle du calcul de dissipation des pressions interstitielles.

- **Uplift pressure in dams** — La pression interstitielle de soulèvement à la base des barrages-poids est une préoccupation classique en génie hydraulique depuis les travaux de Levy (1895) et des premières études sur les barrages en béton.

### Implémentation numérique

- **Dangla, P.** — *Bil : a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Code source : <https://github.com/Universite-Gustave-Eiffel/bil>

- **Geuzaine, C. & Remacle, J.-F.** (2009). GMSH: A three-dimensional finite element mesh generator with built-in pre- and post-processing facilities. *International Journal for Numerical Methods in Engineering*, 79(11), 1309–1331. — Logiciel utilisé pour générer `ternay.msh` à partir de `ternay.geo`.
