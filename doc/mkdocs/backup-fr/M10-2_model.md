# Modèle M10 — Équation de Richards : drainage d'une colonne de billes sous gravité (2D)

> **Fichiers sources :**
> `src/Models/ModelFiles/M10.c` · `test_examples/M10-2/M10-2` · `test_examples/M10-2/M10-2.msh` · `test_examples/M10-2/billes`
>
> **Auteur du modèle Bil :** P. Dangla (Université Gustave Eiffel)

---

## Table des matières

1. [Contexte et objectif](#1-contexte-et-objectif)
2. [Hypothèses](#2-hypothèses)
3. [Variables et notation](#3-variables-et-notation)
4. [Modèle mathématique](#4-modèle-mathématique)
   - 4.1 [Équation de conservation](#41-équation-de-conservation)
   - 4.2 [Loi de flux de Darcy vectorielle](#42-loi-de-flux-de-darcy-vectorielle)
   - 4.3 [Courbe de rétention et perméabilité relative](#43-courbe-de-rétention-et-perméabilité-relative)
   - 4.4 [Forme condensée — équation de Richards](#44-forme-condensée--équation-de-richards)
5. [Conditions aux limites et initiales](#5-conditions-aux-limites-et-initiales)
6. [Cas test : colonne de billes bidimensionnelle (`test_examples/M10-2`)](#6-cas-test--colonne-de-billes-bidimensionnelle)
7. [Résultats](#7-résultats)
8. [Discrétisation numérique Éléments Finis 2D](#8-discrétisation-numérique-éléments-finis-2d)
9. [Références bibliographiques](#9-références-bibliographiques)

---

## 1. Contexte et objectif

Le modèle M10 résout l'**équation de Richards** pour l'écoulement monophasique de l'eau liquide dans un milieu poreux partiellement saturé. La phase gazeuse est supposée à pression constante ; seule la **pression liquide** $p_l$ est l'inconnue.

Dans ce cas spécifique `test_examples/M10-2`, la modélisation s'attache à reproduire le **drainage gravitaire d'une colonne de billes de verre**, mais cette fois sur **un plan bidimensionnel (2D)**. Le but de ce test est de valider le comportement de l'intégration purement locale de la méthode des Éléments Finis (FEM) de M10 sur un **maillage non structuré importé** (`M10-2.msh`), contrairement au cas `M10-1` qui n'était qu'un découpage monodimensionnel généré à la volée.

Bien que le maillage possède une largeur (coordonnée $x \in [0, 0.01]$ m), l'absence de charges ou de gradients transversaux garantit un écoulement strictement vertical le long de l'axe $y \in [0, 0.2]$ m.

| Paramètre de base | Valeur |
|-----------|--------|
| Porosité $\phi$ | 0.38 |
| Perméabilité intrinsèque $k_\text{int}$ | $8.9 \times 10^{-12}$ m² |

---

## 2. Hypothèses

1. **Monophasique liquide 2D** : La résolution vectorielle se fait sur un plan $(x,y)$. La pression gazeuse est imposée à un plafond constant $p_g = 10^5$ Pa.
2. **Isotherme** : Viscosité et densités constantes.
3. **Porosité rigide** : $\phi = \text{constante}$.
4. **Gravité vectorielle** : $\mathbf{g} = (0, -9.81)$ m/s² orientée vers le bas.

---

## 3. Variables et notation

### Inconnue primaire nodale

| Symbole | Signification | Unité |
|---------|---------------|-------|
| $p_l$ | Pression de la phase liquide 2D $p_l(x,y)$ | Pa |

La dynamique locale (points de Gauss) se gère à l'aide de la pression capillaire $p_c = p_g - p_l$.

### Variables secondaires

| Symbole | Signification |
|---------|---------------|
| $s_l(p_c)$ | Degré de saturation en eau liquide |
| $k_{rl}(p_c)$ | Perméabilité relative pondératrice |
| $m_l$ | Teneur en eau locale |
| $\mathbf{W}_l$ | Vecteur de flux de liquide $(W_{lx}, W_{ly})$ |

---

## 4. Modèle mathématique

### 4.1 Équation de conservation

Bilan de masse sur le contrôle de volume bidimensionnel des éléments finis :

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{W}_l = 0 \quad \text{avec } m_l = \rho_l\,\phi\,s_l(p_c)$$

### 4.2 Loi de flux de Darcy vectorielle

Le flux est propulsé par la charge motrice générale, en tenant compte des deux degrés de gradients continus :

$$\mathbf{W}_l = -K_l\,\left( \begin{pmatrix} \frac{\partial p_l}{\partial x} \\ \frac{\partial p_l}{\partial y} \end{pmatrix} - \rho_l \begin{pmatrix} 0 \\ g_y \end{pmatrix} \right)$$
*(Où $g_y = -9.81$)*

La dépendance du coefficient effectif $K_l = \frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}$ au champ local $p_c$ pilote directement la progression frontale non linéaire.

### 4.3 Courbe de rétention et perméabilité relative

Le milieu de billes en verre présente une désaturation extrêmement soudaine. Sa courbe d'essai est retranscrite au sein de `test_examples/M10-2/billes`.

| Grandeur | Valeur | Description |
|----------|--------|-------------|
| $p_{c,\text{entrée}}$ | ≈ 500 Pa | Pression d'entrée d'air caractéristique |
| $s_{l,\text{max}}$ | 1.0 | Saturation complète en dessous de 500 Pa |
| $s_{l,\text{résiduel}}$ | ≈ 0.09 | Vide hydraulique total à ~ 1000 Pa |

### 4.4 Forme condensée — équation de Richards

La substitution des variables conduit au système Jacobien classique pour obtenir le système de Newton :

$$ -C(p_c) \frac{\partial p_l}{\partial t} - \nabla \cdot \left[ K_l \left(\nabla p_l - \rho_l\,\mathbf{g}\right) \right] = 0 $$

La variable $C(p_c)$ (Capacité de matrice) est le produit $-\rho_l\,\phi\,\partial s_l/\partial p_c$.

---

## 5. Conditions aux limites et initiales

### Conditions initiales : l'appel du `Champ` M10-2

La colonne n'est pas remplie en régime hydrostatique stabilisé avec la gravité, mais selon une simple dévaluation linéaire sur Y pour imposer un lourd flux initial drainant :
`(Fichier M10-2 : Val = 1.e5 Grad = 0 -3400 Poin = 0 0)`

$$p_l(x,\,y,\,t=0) = p_{l0} + G_{y}\,y$$
Avec $p_{l0} = 10^5 \text{ Pa}$ et $G_y = -3400 \text{ Pa/m}$.

| Bord géométrique | $y$ [m] | $p_l(t=0)$ [Pa] | $S_l(t=0)$ [-] |
|------|---------|-----------------|----------------|
| Base (Reg 1) | 0 | $10^5$ | 1.000 |
| Sommet libre | 0.2 | $9.932 \times 10^4$ | ≈ 0.9998 |

*Note sur $X$ : Tout $x$ à la même hauteur $y$ reçoit strictement l'exacte même valeur de pression initiale.*

### Conditions aux limites (FEM - Vectorielles)

- `Boundary Conditions` impose `Reg = 1` à `Fonc = 0` sur `Champ = 1`. 
  L'assise 2D ($y = 0$) est maintenue bloquée à l'état de pression du `Champ 1`, c'est-à-dire la pression atmosphérique saturée pure de $10^5$ Pa.
- Les frontières extérieures (parois latérales en $X$ gauche et droite) comme le toit extérieur en $Y$ libre sont implicitement paramétrés sous conditions de Newman Naturelle, se comportant en **frontières imperméables exactes** : aucun flux ne rentre.

---

## 6. Cas test : colonne de billes bidimensionnelle (`test_examples/M10-2`)

### Paramètres de simulation
Ce cas reprend exactement les exécutions de M10-1 mais à un coût vectoriel supérieur car `M10-2.msh` possède un nombre de nœuds bien plus importants disséminés horizontalement et verticalement.

| Paramètre | Valeur |
|-----------|--------|
| Type de maillage | GMSH bidimensionnel (`2 plan`) |
| Durée totale | 300 s |
| Sorties demandées | $t = 0$, $t = 120$, $t = 300$ s |
| Tolérance d'erreur Newton | $1.0\times 10^{-10}$ |

### Physique de drainage en surface

La décrue des saturations doit suivre l'onde de front verticale pure. Puisque tous les gradients locaux en $\partial p_l / \partial x$ sont rigoureusement vides, le solveur Éléments Finis 2D n'assiste à aucun écoulement transversal.

---

## 7. Résultats

La représentation suivante est générée selon un graphe de dispersion (`scatter`) superposant individuellement chaque nœud du plan 2D selon son étagement `y` vis-à-vis des valeurs de la pression $p_l$ et de saturation $S_l$. 

Bien que des nœuds différents existent sur la même base $y$, aucune déviation d'axe croisé de $p_l$ n'est constatée, soulignant visuellement les courbes lisses, justifiant la qualité d'homogénéité isotrope du solveur d'intégration de M10.

![Résultats du Modèle M10-2](M10-2_results.png)

### Les phases :
- En $t=0$ : Entraînement hydraulique puissant par poussée de la masse ($W_l \approx -5.7 \times 10^{-2}$ kg/m²·s) et le retard isobarique initial.
- En $t=120$ et $t=300$ s : Disparition de la saturation sur le sommet. Cette frontière passe au résiduel total ($S_l \approx 0.11$). L'arrêt des flux et le retour vers The equilibrium hydrostatique strict est à 100% garanti par FEM.

---

## 8. Discrétisation numérique Éléments Finis 2D

Dans ce mode, l'algorithme `ComputeVariables` injecte le format $x$ et $y$ au cœur de la matrice conductivité du nœud : 
- La matrice de masse déploie ses fonctions de forme intégrales pleines ($N_i(x,y)\,N_j(x,y)$) aux points de Gauss du volume.
- La construction de la Jacobienne de conductivité régit à la fois les gradients $\frac{\partial p_l}{\partial x}$ et $\frac{\partial p_l}{\partial y}$.

Si le maillage `M10-2.msh` était distordu géométriquement, l'approche continue standard FEM appliquée garantirait la mass-conservation globale du drainage final, l'un des piliers du standard de Celia et al.

---

## 9. Références bibliographiques

- **Celia, M. A., Bouloutas, E. T. & Zarba, R. L.** (1990). A general mass-conservative numerical solution for the unsaturated flow equation. *Water Resources Research*, 26(7), 1483–1496. — Rôle sur l'exigence d'une méthode variationnelle propre (utilisée dans ce module bil 2D/3D).
- **Zienkiewicz, O. C., & Taylor, R. L.** (2000). *The Finite Element Method*. — Principes de quadrature en 2D spatial.
- **Dangla, P.** — *Bil : a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Code source : <https://github.com/Universite-Gustave-Eiffel/bil>
