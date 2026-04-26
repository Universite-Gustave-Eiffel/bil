# Modèle usoil — Mécanique des Sols Non-Saturés (2019)

> **Fichiers sources :**
> `src/Models/ModelFiles/usoil.c` · `test_examples/usoil/usoil`
>
> **Auteur du modèle :** P. Dangla

---

## Table des matières

1. [Contexte et objectif](#1-contexte-et-objectif)
2. [Hypothèses](#2-hypothèses)
3. [Variables et notation](#3-variables-et-notation)
4. [Modèle mathématique](#4-modèle-mathématique)
   - 4.1 [Équations d'équilibre et de conservation](#41-équations-déquilibre-et-de-conservation)
   - 4.2 [Modèle Constitutif Multi-Couplé (Cam-Clay)](#42-modèle-constitutif-multi-couplé-cam-clay)
5. [Conditions aux limites et initiales](#5-conditions-aux-limites-et-initiales)
6. [Cas test : Compression Cyclique 2D (`test_examples/usoil`)](#6-cas-test--compression-cyclique-2d)
7. [Résultats de la modélisation](#7-résultats-de-la-modélisation)
8. [Description pas-à-pas des fichiers d'entrée](#8-description-pas-à-pas-des-fichiers-dentrée)
9. [Références bibliographiques](#9-références-bibliographiques)

---

## 1. Contexte et objectif

Le modèle **usoil** résout le problème couplé hydro-mécanique pour les **sols non-saturés**. Contrairement au modèle `M7` qui utilise la poro-élasticité solide linéaire de Biot classique, ce modèle intègre des effets **élasto-plastiques stricts** gérés avec des lois rhéologiques avancées des sols à grain fin, telles que le comportement d'écrouissage visco-plastique de type **Cam-Clay Modifié**.

L'objectif de ce solveur est de prédire des phénomènes critiques comme l'effondrement (*collapse*) dû à une ré-imbibition sous charge, la compressibilité d'un sol sous chargement lourd, ainsi que la progression complexe des flux d'eau liés à la rétention capillaire non-linéaire.

---

## 2. Hypothèses

1. **Généralisation Hydro-Mécanique Poro-Plastique** : Intègre entièrement le module `Plasticity.h` (incluant l'algorithme "Return Mapping" pour calculer la contrainte effective plastique).
2. **Phase Gazeuse Passive** : La pression de gaz $p_g$ est supposée constante, purement atmosphérique ($p_g = 0$) partout. L'écoulement de l'air n'affecte donc le modèle qu'en tant que contrainte hydraulique relative à la saturation (Pression capillaire $p_c = -p_l$).
3. **Pression Équivalente** : Les contraintes totales et les gradients plastiques reposent sur l'utilisation du concept phénoménologique d'une Pression Pores Équivalente qui pondère la contribution capillaire à l'effort des grains solides.

---

## 3. Variables et notation

Le modèle est défini par la résolution de $1 + \dim$ équations.

### Inconnues primaires

| Symbole | Intitulé BIL | Signification |
|---------|--------------|-------------|
| $p_l$ | `p_l` | Pression du fluide (eau interstitielle) retenu |
| $\mathbf{u}$ | `u_1, u_2, u_3` | Déplacements matériels du squelette solide (2 directions selon le plan/axe) |

### Variables Explicites (Calculées Secondairement)

- $e$ : Indice des vides.
- $S_l$ : Degré de saturation en liquide.
- Paramètres de Cam-Clay : $\kappa$ (*slope of swelling line*), $\lambda$ (*consolidation line*), $M$ (*critical state line*).

---

## 4. Modèle mathématique

### 4.1 Équations d'équilibre et de conservation

1. **Diffusion et accumulation de l'eau (Hydraulique)** :
   $$\frac{\partial}{\partial t}\left(m_l\right) + \text{div}\left( \mathbf{W}_l \right) = 0$$
   Avec $m_l = \rho_l \, \phi \, S_l(p_c)$. La loi de **Van Genuchten** ou une formule exponentielle est utilisée pour définir $S_l$ ainsi que la perméabilité non satureé $k_{rl}$.

2. **Équilibre mécanique Macroscopique** :
   $$\text{div}(\boldsymbol{\sigma}) + (\rho_s + m_l)\mathbf{g} = \mathbf{0}$$
   Pour former la matrice, la contribution hydrique sur le jacobien d'assemblage transmet les efforts au bloc `K(E_mec, U_p_l)`.

### 4.2 Modèle Constitutif Multi-Couplé (Cam-Clay)

La relation contraintes-déformations dépasse le cadre de Hooke :
$$ \boldsymbol{\sigma} = \boldsymbol{\sigma}' - p_{\text{equivalent}} \cdot \mathbf{I} $$

La contrainte effective $\boldsymbol{\sigma}'$ avance grâce au solveur elasto-plastique interne de l'élément fini, utilisant les surfaces de charge ellipsoïdes de **Cam-Clay** dans l'espace invariante $(p', q)$.

---

## 5. Conditions aux limites et initiales

Les paramètres intègrent des charges par pressions macroscopiques ainsi que des potentiels capillaires locaux imposés.
Les conditions courantes impliquent notamment le retrait des déplacements latéraux ou verticaux selon la contrainte de la nappe (`Boundary Conditions Region X Unknown = u_X`).

---

## 6. Cas test : Compression Cyclique 2D (`test_examples/usoil`)

Le cas test sollicite un maillage axisymétrique (`carre.msh`) soumis via ses frontières (Region 2 et 3) à un pic de chargement en **pression radiale et verticale cyclique**.

La fonction de charge pilote (`N = 3 F(1) = 1 F(256) = 256 F(512) = 1`) va comprimer l'échantillon intensément de la seconde 1 à 256, puis le décharger de facon progressive de 256 à 512.
Le drainage permet d'illustrer la différence entre le comportement inélastique absolu (les déformations plastiques qui ne se recouvrent pas) et la réaction hydrique réversible.

---

## 7. Résultats de la modélisation

![Résultats USOIL](usoil_results.png)

1. **Variations Volumiques et Vides (e)** :
   L'indice des vides baisse violemment au cours de la phase de compression (temps < 256 s), à mesure que les grains minéraux s'interbloquent plastiquement ("Consolidation Virgin"). 
2. **Recouvrement vs Permanence** :
   Au moment de la décharge mécanique totale (t=256s vers 512s), la restauration de l'indice des vides n'est que minimale, pilotée uniquement par la ligne de recompression/élasticité $\kappa$. L'écrasement global est irrémédiable, et c'est ce que prouve le modèle de *Cam-Clay*.
3. **Comportement Hydraulique** :
   La pression capillaire générée lors du compactement expulse instantanément l'eau, limitant à terme la re-saturation passive pure. 

---

## 8. Description pas-à-pas des fichiers d'entrée

### Fichier `test_examples/usoil/usoil`

1. **Géométrie & Maillage** : `2 axis` (Révolution 2D sur GMSH via `carre.msh`). 
2. **Propriétés Physiques Multi-États `Material`** :
   - `slope_of_swelling_line = 0.004` (kappa du rebond visco-élastique).
   - `slope_of_virgin_consolidation_line = 0.037` (lambda, chemin plastique en $V-\ln p'$).
   - `slope_of_critical_state_line = 1.2` (Coeff de pente du plateau de critère de cisaillement critique $M$).
   - `initial_pre-consolidation_pressure = 18000` ($p_{c0}$ marquant la mémoire des charges géologiques anciennes).
   - `Curves = sterrebeek0 ...` : Ici, plutôt que de fournir un simple tableau, le texte utilise l'interpréteur de BIL `Expressions(1)` impliquant directement des fonctions arithmétiques imbriquées (ex. opérateurs conditionnels `? : ` en cascade) pour le calcul des fonctions $S_l$, $k_l$, $l_c$ (durcissement capillaire croisé).
3. **Génération de charge** :
   `Loads` applique une `pression` pilotée par le Champ 2  (`Val = 1.e3`) modulée par une fonction triangulaire F : $[0 \rightarrow 1 \rightarrow 256 \rightarrow 1]$.
4. **Tolérances Strictes** :
   `Tol = 1e-06` : Exigé rigoureusement car le solveur de point-de-retour plastique ("Return Mapping") demande des incréments fins pour converger la dérive des contraintes tangentielles.

---

## 9. Références bibliographiques

- **Coussy, O.** (2004). *Poromechanics*. John Wiley & Sons. - Généralisation énergétique de la poromécanique englobant les milieux non saturés à plusieurs fluides.
- **Roscoe, K.H., Burland, J.B.** (1968). *On the generalized stress-strain behavior of "wet" clay*. In Engineering plasticity (pp. 535-609). - Cadre originel phénoménologique de la Modified Cam-Clay Model implémentée dans `Plasticity.h`.
- **Dangla, P.** - *Bil : a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. (Implémentation interne).
