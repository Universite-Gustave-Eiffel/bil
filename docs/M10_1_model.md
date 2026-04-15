# Modèle M10 — Équation de Richards : drainage d'une colonne de billes sous gravité (1D)

> **Fichiers sources :**
> `src/Models/ModelFiles/M10.c`
>
> **Exemple :**
> `test_examples/M10-1/M10-1` · `test_examples/M10-1/billes` · `test_examples/M10-1/M10-1.msh`
>
> **Auteur du modèle Bil :** P. Dangla (Université Gustave Eiffel)

---

## Table des matières

1. [Contexte et objectif](#1-contexte-et-objectif)
2. [Hypothèses](#2-hypothèses)
3. [Variables et notation](#3-variables-et-notation)
4. [Modèle mathématique](#4-modèle-mathématique)
   - 4.1 [Équation de conservation](#41-équation-de-conservation)
   - 4.2 [Loi de flux de Darcy avec gravité](#42-loi-de-flux-de-darcy-avec-gravité)
   - 4.3 [Courbe de rétention et perméabilité relative](#43-courbe-de-rétention-et-perméabilité-relative)
   - 4.4 [Équation de Richards — forme complète](#44-équation-de-richards--forme-complète)
5. [Conditions aux limites et initiales](#5-conditions-aux-limites-et-initiales)
6. [Explication détaillée des fichiers d'entrée](#6-explication-détaillée-des-fichiers-dentrée)
   - 6.1 [Fichier principal `M10-1`](#61-fichier-principal-m10-1)
   - 6.2 [Fichier de courbes `billes`](#62-fichier-de-courbes-billes)
   - 6.3 [Fichier de maillage `M10-1.msh`](#63-fichier-de-maillage-m10-1msh)
   - 6.4 [Fichier de stockage `M10-1.sto`](#64-fichier-de-stockage-m10-1sto)
7. [Résultats](#7-résultats)
8. [Discrétisation numérique](#8-discrétisation-numérique)
9. [Références bibliographiques](#9-références-bibliographiques)

---

## 1. Contexte et objectif

Le modèle M10 résout l'**équation de Richards** pour l'écoulement monophasique de l'eau liquide dans un milieu poreux partiellement saturé. La pression gazeuse est supposée uniforme et constante ; seule la **pression de la phase liquide** $p_l$ est inconnue.

Le cas test `M10-1` simule le **drainage gravitaire d'une colonne verticale de billes de verre** (hauteur 20 cm). Ce scénario est un classique de la mécanique des fluides en milieu poreux, utilisé depuis les expériences fondatrices de Richards (1931) pour valider les codes d'écoulement non saturé. Les billes de verre constituent un matériau modèle : leur courbe de rétention est bien caractérisée, leur porosité est élevée ($\phi = 0.38$) et leur perméabilité intrinsèque est grande ($k_\text{int} = 8.9 \times 10^{-12}$ m²), ce qui rend les temps de drainage accessibles à l'expérience.

**Physique simulée :** la colonne, initialement quasi-saturée mais hors équilibre hydrostatique, se draine sous l'effet de la gravité. L'eau s'écoule vers le fond (maintenu à pression atmosphérique), la pression capillaire augmente en sommet de colonne, et dès qu'elle dépasse la pression d'entrée d'air des billes (~680 Pa), une désaturation rapide se produit. Un front de désaturation progresse de haut en bas sur une échelle de temps de quelques centaines de secondes.

---

## 2. Hypothèses

1. **Monophasique liquide** : seule la phase liquide (eau) est mobile. La phase gazeuse est supposée à pression constante $p_g = 10^5$ Pa (atmosphère ouverte).
2. **Isotherme** : la température est uniforme ; viscosité et masse volumique sont des constantes.
3. **Porosité rigide** : le squelette solide est indéformable, $\phi = \text{const}$.
4. **Loi de Darcy généralisée** avec gravité pour le transport de la phase liquide.
5. **Équilibre capillaire local** : la saturation $s_l$ est une fonction univoque de la pression capillaire $p_c = p_g - p_l$.
6. **Gravité** : $g = -9.81$ m/s², orientée dans la direction décroissante de $x$ (axe $x$ vertical vers le haut).
7. **Pas de source/puits** internes.

---

## 3. Variables et notation

### Inconnue primaire

| Symbole | Signification | Unité |
|---------|---------------|-------|
| $p_l$ | Pression de la phase liquide | Pa |

La pression capillaire en découle directement (pression gazeuse constante) :

$$p_c = p_g - p_l = 10^5 - p_l$$

### Variables secondaires calculées

| Symbole | Définition | Signification |
|---------|------------|---------------|
| $s_l(p_c)$ | tabulée dans `billes` | Degré de saturation en eau |
| $k_{rl}(p_c)$ | tabulée dans `billes` | Perméabilité relative liquide |
| $K_l$ | $\rho_l k_\text{int} k_{rl} / \mu_l$ | Conductivité hydraulique effective |
| $m_l$ | $\rho_l \phi s_l$ | Teneur en eau massique par unité de volume total |
| $W_l$ | cf. §4.2 | Flux massique de liquide |

### Paramètres du cas test

| Symbole | Valeur | Signification |
|---------|--------|---------------|
| $\rho_l$ | 1000 kg/m³ | Masse volumique de l'eau liquide |
| $\mu_l$ | $10^{-3}$ Pa·s | Viscosité dynamique de l'eau |
| $k_\text{int}$ | $8.9 \times 10^{-12}$ m² | Perméabilité intrinsèque des billes de verre |
| $\phi$ | 0.38 | Porosité |
| $p_g$ | $10^5$ Pa | Pression gazeuse (constante, atmosphérique) |
| $g$ | $-9.81$ m/s² | Accélération de la pesanteur |

La conductivité hydraulique à saturation complète vaut :

$$K_{l,\text{sat}} = \frac{\rho_l\,k_\text{int}}{\mu_l} = \frac{1000 \times 8.9 \times 10^{-12}}{10^{-3}} = 8.9 \times 10^{-6} \text{ kg/(m²·Pa·s)}$$

---

## 4. Modèle mathématique

### 4.1 Équation de conservation

Le système est composé d'une **unique équation de bilan de masse de l'eau liquide** intégrée sur le volume élémentaire représentatif (VER) :

$$\frac{\partial m_l}{\partial t} + \nabla \cdot \mathbf{W}_l = 0$$

avec la teneur en eau massique volumique :

$$m_l = \rho_l\,\phi\,s_l(p_c)$$

### 4.2 Loi de flux de Darcy avec gravité

Le flux massique de liquide est donné par la loi de Darcy généralisée :

$$\mathbf{W}_l = -K_l\,\left(\nabla p_l - \rho_l\,\mathbf{g}\right), \qquad K_l = \frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}$$

Dans l'implémentation M10.c (1D, axe $x$ vertical vers le haut, $g_x = g = -9.81$ m/s²) :

$$W_l = \underbrace{-K_l\,\frac{\partial p_l}{\partial x}}_{\text{terme de pression}} + \underbrace{K_l\,\rho_l\,g}_{\text{terme gravitaire}}$$

La condition d'équilibre hydrostatique ($W_l = 0$) impose :

$$\frac{\partial p_l}{\partial x}\bigg|_\text{hydro} = \rho_l\,g = 1000 \times (-9.81) = -9810 \text{ Pa/m}$$

La pression liquide décroît de 9810 Pa par mètre de hauteur à l'équilibre.

> **Interprétation physique :** si le gradient de pression mesuré $\partial p_l/\partial x > \rho_l g$ (moins négatif que $-9810$ Pa/m), la force gravitaire domine et $W_l < 0$ : l'eau s'écoule vers le bas. C'est le cas initial du problème ($\partial p_l/\partial x = -3400$ Pa/m au lieu de $-9810$ Pa/m).

### 4.3 Courbe de rétention et perméabilité relative

Les fonctions $s_l(p_c)$ et $k_{rl}(p_c)$ sont fournies **tabulées** dans le fichier `billes`. Les caractéristiques clés des billes de verre sont :

| Grandeur | Valeur | Description |
|----------|--------|-------------|
| Plage de $p_c$ | $[500,\ 1000]$ Pa | Domaine de la courbe tabulée |
| $s_l(p_c < 500\ \text{Pa})$ | 1.0 (extrapolé) | Saturation totale en dessous de la pression d'entrée |
| $p_{c,\text{entrée}}$ | ≈ 680 Pa | Pression d'entrée d'air (décrochage de $s_l$) |
| $s_l(680\ \text{Pa})$ | ≈ 0.9998 | Saturation à la pression d'entrée — quasiment 1 |
| $s_l(750\ \text{Pa})$ | ≈ 0.94 | Désaturation rapide juste au-dessus de l'entrée |
| $s_l(1000\ \text{Pa})$ | ≈ 0.09 | Saturation résiduelle |
| $k_{rl}(s_l = 1)$ | 1.0 | Perméabilité relative à saturation |
| $k_{rl}(s_l = 0.09)$ | ≈ $2.6 \times 10^{-8}$ | Perméabilité résiduelle (chute de 8 décades) |

La courbe montre deux régimes :
- **$p_c < 680$ Pa** : milieu quasi-saturé, $s_l \approx 1$, conductivité maximale.
- **$680 < p_c < 750$ Pa** : transition abrupte (genou de la courbe), $s_l$ passe de 0.9998 à 0.94 sur seulement 70 Pa.
- **$p_c > 750$ Pa** : désaturation progressive jusqu'à la saturation résiduelle à 1000 Pa.

### 4.4 Équation de Richards — forme complète

En substituant les expressions précédentes et en développant $\partial m_l/\partial t$ :

$$\frac{\partial m_l}{\partial t} = \rho_l\,\phi\,\frac{\partial s_l}{\partial p_c}\,\frac{\partial p_c}{\partial t} = -\rho_l\,\phi\,\frac{\partial s_l}{\partial p_c}\,\frac{\partial p_l}{\partial t}$$

On définit la **capacité capillaire** :

$$C(p_c) = -\rho_l\,\phi\,\frac{\partial s_l}{\partial p_c} \geq 0$$

L'équation de Richards s'écrit alors :

$$\boxed{C(p_c)\,\frac{\partial p_l}{\partial t} - \nabla \cdot \left[\frac{\rho_l\,k_\text{int}\,k_{rl}(p_c)}{\mu_l}\left(\nabla p_l - \rho_l\,\mathbf{g}\right)\right] = 0}$$

Cette équation est **parabolique non-linéaire** (le coefficient de diffusion $K_l$ dépend de $p_l$ via $p_c$ et $k_{rl}$). Elle dégénère vers une équation **elliptique** dans les zones entièrement saturées où $C(p_c) = 0$.

---

## 5. Conditions aux limites et initiales

### Conditions initiales

La pression initiale est un champ linéaire en $x$ (sous-hydrostatique) :

$$p_l(x,\,t=0) = p_{l0} + G\,x = 10^5 - 3400\,x \quad \text{[Pa]}$$

| Bord | $x$ [m] | $p_l(t=0)$ [Pa] | $p_c(t=0)$ [Pa] | $s_l(t=0)$ [-] | Remarque |
|------|---------|-----------------|-----------------|----------------|----------|
| Bas | 0 | $10^5$ | 0 | 1.000 | Saturé |
| Haut | 0.2 | $9.932 \times 10^4$ | 680 | ≈ 0.9998 | Au seuil d'entrée d'air |

Le gradient initial $-3400$ Pa/m est **moins négatif** que le gradient hydrostatique $-9810$ Pa/m : la colonne est hors équilibre, ce qui génère un flux vers le bas dès $t = 0$.

### Conditions aux limites

| Bord | $x$ [m] | Type | Valeur | Physique |
|------|---------|------|--------|---------|
| Bas | 0 | Dirichlet | $p_l = 10^5$ Pa | Fond ouvert à l'atmosphère (saturé en permanence) |
| Haut | 0.2 | Neumann naturel | $W_l \cdot \mathbf{n} = 0$ | Sommet libre (drainage purement par gravité) |

Aucune condition de flux n'est imposée en sommet : c'est la **condition de Neumann naturelle** de la formulation variationnelle. L'eau peut s'évacuer uniquement par le fond. Le sommet s'assèche progressivement sous l'effet de la gravité qui entraîne l'eau vers le bas.

---

## 6. Explication détaillée des fichiers d'entrée

### 6.1 Fichier principal `M10-1`

Le fichier d'entrée utilise la **syntaxe Bil moderne** (mots-clés complets, introduite dans Bil v2.x), par opposition à la syntaxe abrégée des anciens exemples (`DIME`, `MAIL`, `MATE`, etc.). Chaque section commence par un mot-clé de section et est suivie de ses paramètres.

```
# Drainage d'une colonne de billes
# 
```
**Commentaires** : toute ligne commençant par `#` est ignorée.

---

#### Section `Geometry`

```
Geometry
1 plan
```

| Champ | Valeur | Signification |
|-------|--------|---------------|
| Nombre de dimensions | `1` | Problème 1D |
| Type de symétrie | `plan` | Géométrie plane (pas de symétrie cylindrique ou sphérique) |

Les types de symétrie disponibles sont `plan`, `cylindrique` (axisymétrique) et `spherique`.

---

#### Section `Mesh`

```
Mesh
3 0. 0. 0.2
0.001
1 100
1 1
```

La section `Mesh` peut recevoir soit un nom de fichier `.msh` (maillage externe), soit une description inline de maillage 1D. Ici, c'est une description inline :

| Ligne | Valeur | Signification |
|-------|--------|---------------|
| `3 0. 0. 0.2` | 3 points de contrôle, abscisses : $x_1 = 0$, $x_2 = 0$, $x_3 = 0.2$ m | Définit l'intervalle $[0,\ 0.2]$ m. Le premier point (abscisse $x_2 = 0$) définit l'origine du nœud de bord gauche, $x_3 = 0.2$ m est le bord droit |
| `0.001` | Taille de maille de départ | Premier élément de longueur $h_1 = 10^{-3}$ m |
| `1 100` | 1 groupe d'éléments, 100 éléments au total | 100 éléments dans la colonne |
| `1 1` | 1 groupe de nœuds, 1 nœud de bord | Définit le nœud de bord (utilisé pour les conditions aux limites) |

Le maillage est **non uniforme** : les éléments sont plus petits en $x = 0$ (fond, là où le gradient est le plus fort au début) et s'élargissent progressivement. Ce raffinement est calculé automatiquement par Bil à partir de la taille initiale `0.001` m et du nombre total d'éléments.

> **Résultat :** 102 nœuds (100 éléments = 102 nœuds pour les éléments linéaires à 2 nœuds, dont 2 nœuds de bord confondus), coordonnées stockées dans `M10-1.msh`.

---

#### Section `Material`

```
Material
Model = M10
gravite = -9.81
phi = 0.38
rho_l = 1000
k_int = 8.9e-12
mu_l = 0.001
p_g = 100000 
Curves = billes
```

| Paramètre | Valeur | Rôle dans M10.c |
|-----------|--------|-----------------|
| `Model = M10` | — | Désigne le fichier physique `M10.c` à utiliser |
| `gravite` | $-9.81$ m/s² | Composante de la gravité selon $x$ (signe négatif : $x$ vers le haut) |
| `phi` | 0.38 | Porosité $\phi$ |
| `rho_l` | 1000 kg/m³ | Masse volumique de l'eau |
| `k_int` | $8.9 \times 10^{-12}$ m² | Perméabilité intrinsèque |
| `mu_l` | $10^{-3}$ Pa·s | Viscosité dynamique de l'eau |
| `p_g` | $10^5$ Pa | Pression gazeuse constante |
| `Curves = billes` | — | Nom du fichier contenant les courbes tabulées $s_l(p_c)$ et $k_{rl}(p_c)$ |

Le fichier `billes` doit être présent dans le même répertoire que le fichier d'entrée principal.

---

#### Section `Fields`

```
Fields
1
Value = 1.e5 Gradient = -3400 Point = 0
```

Les champs (*fields*) sont des fonctions spatiales prédéfinies utilisées pour les conditions initiales et les conditions aux limites. Bil évalue leur valeur en tout point $x$ du domaine.

| Ligne | Signification |
|-------|---------------|
| `1` | Nombre de champs définis : ici, 1 seul champ |
| `Value = 1.e5` | Valeur de référence à l'origine : $p_{l,\text{ref}} = 10^5$ Pa |
| `Gradient = -3400` | Gradient spatial : $G = -3400$ Pa/m |
| `Point = 0` | Point de référence : $x_0 = 0$ m |

**Formule d'évaluation :**

$$\text{Champ 1}(x) = p_{l,\text{ref}} + G\,(x - x_0) = 10^5 - 3400\,x \quad \text{[Pa]}$$

Ainsi, pour un point $x$ quelconque du maillage, `Champ 1` donne la pression initiale linéaire sous-hydrostatique.

> **Remarque :** le même champ sert à la fois pour la **condition initiale** (§`Initialization`) et pour la **condition aux limites** Dirichlet en $x = 0$ (§`Boundary Conditions`). En $x = 0$, le champ vaut $10^5$ Pa.

---

#### Section `Initialization`

```
Initialization
1
Region = 2 Unknown = p_l Field = 1
```

| Champ | Valeur | Signification |
|-------|--------|---------------|
| `1` | Nombre de conditions initiales | 1 condition initiale |
| `Region = 2` | Région 2 | S'applique aux éléments intérieurs (région 2 = tout le domaine sauf les nœuds de bord) |
| `Unknown = p_l` | Inconnue `p_l` | Initialise la pression liquide |
| `Field = 1` | Champ 1 | Utilise le champ défini ci-dessus : $p_l(x,0) = 10^5 - 3400\,x$ |

Dans la numérotation Bil, **Region 1** désigne les nœuds de bord (points aux extrémités du maillage), et **Region 2** désigne les nœuds intérieurs et leurs éléments adjacents.

---

#### Section `Functions`

```
Functions
0
```

Aucune fonction temporelle n'est définie (0 fonctions). Les fonctions Bil permettent de modéliser des sollicitations variant dans le temps (chargement cyclique, rampe, créneau, etc.). Dans ce cas test, toutes les conditions sont stationnaires.

Lorsque `Function = 0` est référencé dans une condition aux limites, Bil interprète cela comme une **fonction unité constante** (valeur 1), ce qui multiplie le champ par 1 — c'est une convention Bil.

---

#### Section `Boundary Conditions`

```
Boundary Conditions
1
Region = 1 Unknown = p_l Field = 1 Function = 0
```

| Champ | Valeur | Signification |
|-------|--------|---------------|
| `1` | Nombre de conditions | 1 condition aux limites Dirichlet |
| `Region = 1` | Région 1 | S'applique aux nœuds de bord (ici, $x = 0$, le fond de la colonne) |
| `Unknown = p_l` | Inconnue | Condition portant sur la pression liquide |
| `Field = 1` | Champ 1 | Valeur cible : $\text{Champ 1}(x=0) = 10^5$ Pa |
| `Function = 0` | Fonction constante | Multiplicateur temporel = 1 (condition stationnaire) |

**Condition imposée :** $p_l(x=0,\,t) = 10^5$ Pa pour tout $t > 0$.

En $x = 0.2$ m (sommet), aucune condition n'est spécifiée : c'est la **condition de Neumann homogène naturelle**, qui correspond à un flux nul au travers du bord supérieur. L'eau peut s'écouler librement vers le bas mais rien n'entre ni ne sort par le haut.

---

#### Section `Loads`

```
Loads
0
```

Aucun chargement surfacique n'est appliqué. Les `Loads` en Bil permettent d'imposer des flux ou des forces réparties sur des surfaces. Non utilisé ici.

---

#### Section `Points`

```
Points
0
```

Aucun point de sortie particulier n'est défini. Les points permettraient d'extraire les résultats en des localisations précises du maillage en plus des sorties volumiques. Non utilisé ici.

---

#### Section `Dates`

```
Dates
3
0 120 300
```

| Ligne | Valeur | Signification |
|-------|--------|---------------|
| `3` | Nombre d'instants de sortie | 3 instantanés |
| `0 120 300` | Instants [s] | Sauvegarde à $t = 0$ s, $t = 120$ s et $t = 300$ s |

Ces instants correspondent aux fichiers de résultats `M10-1.t0`, `M10-1.t1` et `M10-1.t2`.

---

#### Section `Objective Variations`

```
Objective Variations
p_l = 10
```

Ce paramètre contrôle le **pas de temps adaptatif**. Bil ajuste $\Delta t$ à chaque itération temporelle pour que la variation maximale de l'inconnue $p_l$ entre deux pas de temps reste proche de la valeur cible :

$$\Delta p_l^\text{max} \approx 10 \text{ Pa}$$

- Si $\Delta p_l^\text{max} > 10$ Pa lors d'un pas : Bil réduit $\Delta t$ et recommence.
- Si $\Delta p_l^\text{max} \ll 10$ Pa : Bil augmente $\Delta t$ pour accélérer la simulation.

Ce contrôle garantit la précision temporelle tout en maximisant l'efficacité.

---

#### Section `Iterative Process`

```
Iterative Process
Iter = 10 
Tol = 1e-10 
Repetition = 0
```

Paramètres de la **méthode de Newton–Raphson** utilisée pour résoudre le système non-linéaire à chaque pas de temps :

| Paramètre | Valeur | Signification |
|-----------|--------|---------------|
| `Iter = 10` | 10 | Nombre maximal d'itérations Newton par pas de temps |
| `Tol = 1e-10` | $10^{-10}$ | Tolérance de convergence (norme relative du résidu) |
| `Repetition = 0` | 0 | Nombre de répétitions en cas de non-convergence (0 = arrêt immédiat si non-convergé) |

---

#### Section `Time Steps`

```
Time Steps
Dtini = 0.01 
Dtmax = 1000
```

| Paramètre | Valeur | Signification |
|-----------|--------|---------------|
| `Dtini = 0.01` | 0.01 s | Pas de temps initial |
| `Dtmax = 1000` | 1000 s | Pas de temps maximal autorisé |

Le pas de temps démarre à 0.01 s (suffisamment petit pour capturer la physique initiale rapide) et peut croître jusqu'à 1000 s selon la stratégie adaptative pilotée par `Objective Variations`.

---

### 6.2 Fichier de courbes `billes`

Le fichier `billes` est un tableau ASCII à **3 colonnes** et **2000 lignes**, sans en-tête :

```
p_c [Pa]        s_l [-]         k_rl [-]
5.000000e+02    1.000000e+00    1.000000e+00
5.002501e+02    1.000000e+00    1.000000e+00
...
6.798399e+02    9.997772e-01    9.993311e-01
...
7.498749e+02    9.413010e-01    8.339165e-01
...
1.000000e+03    9.000106e-02    2.579971e-08
```

**Structure :**

| Colonne | Signification | Plage |
|---------|---------------|-------|
| 1 | Pression capillaire $p_c$ [Pa] | $[500,\ 1000]$ Pa |
| 2 | Saturation $s_l$ [-] | $[0.09,\ 1.0]$ |
| 3 | Perméabilité relative $k_{rl}$ [-] | $[2.6 \times 10^{-8},\ 1.0]$ |

**Pas de discrétisation :** $\Delta p_c = (1000 - 500) / (2000 - 1) \approx 0.25$ Pa — résolution très fine.

**Physique de la courbe des billes de verre :**

```
s_l
1.0 │▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓─╮
    │                         ╰───╮
    │                              ╰────────╮
0.5 │                                        ╰────────╮
    │                                                   ╰────╮
0.1 │                                                         ╰──────────
    └──────────────────────────────────────────────────────────────────── p_c
   500             600            700            800             1000 [Pa]
```

- **$p_c < 500$ Pa** : la courbe n'est pas tabulée ; Bil extrapole en conservant $s_l = 1$ et $k_{rl} = 1$ (saturation totale).
- **$500 < p_c < 680$ Pa** : plateau de saturation totale ($s_l \approx 1$) — l'air n'a pas encore pénétré.
- **$p_c \approx 680$ Pa** : **pression d'entrée d'air** — début de la désaturation.
- **$680 < p_c < 750$ Pa** : chute abrupte de $s_l$ (genou de la courbe, caractéristique d'un matériau bien trié comme les billes).
- **$750 < p_c < 1000$ Pa** : désaturation progressive vers la saturation résiduelle $s_{l,r} \approx 0.09$.
- **$k_{rl}$ chute de 8 décades** entre saturation totale et résiduelle, rendant l'écoulement résiduel pratiquement nul.

**Utilisation par M10.c :** Bil interpole linéairement dans ce tableau pour obtenir $s_l(p_c)$, $\partial s_l/\partial p_c$ (pour la capacité capillaire) et $k_{rl}(p_c)$ en tout point d'intégration.

---

### 6.3 Fichier de maillage `M10-1.msh`

Le fichier `M10-1.msh` est généré automatiquement par Bil à partir de la description inline dans la section `Mesh`. Il est au **format Gmsh** (version 1) :

```
$NOD
102
1  0.000000e+00  0  0    ← nœud 1 : x = 0 (fond, bord)
2  0.000000e+00  0  0    ← nœud 2 : x = 0 (fond, nœud intérieur adjacent)
3  1.000000e-03  0  0    ← nœud 3 : x = 1 mm (premier élément)
4  2.219931e-03  0  0    ← nœud 4 : x ≈ 2.2 mm
...
102 2.000000e-01  0  0   ← nœud 102 : x = 0.2 m (sommet)
$ENDNOD
$ELM
101
1  15 1 1 1  1           ← élément ponctuel (nœud de bord) : nœud 1
2   1 1 2 2  1  2        ← élément linéaire : nœuds 1–2 (longueur nulle, bord)
3   1 1 2 2  2  3        ← élément linéaire : nœuds 2–3 (longueur 1 mm)
...
101 1 1 2 2  101 102     ← dernier élément : nœuds 101–102
$ENDELM
```

**Structure des nœuds :**
- 102 nœuds au total : 2 nœuds superposés en $x = 0$ (nœud de bord + premier nœud intérieur), puis 100 nœuds régulièrement croissants jusqu'à $x = 0.2$ m.
- Le maillage est **non uniforme** : les éléments sont plus petits près du fond ($h \approx 1$ mm) et s'élargissent progressivement vers le sommet.

**Structure des éléments :**
- 1 élément ponctuel (type 15) en $x = 0$ : correspond à la **région de bord 1** (où la condition de Dirichlet est appliquée).
- 100 éléments linéaires à 2 nœuds (type 1) : constituent le domaine de calcul (**région 2**).

---

### 6.4 Fichier de stockage `M10-1.sto`

Le fichier `M10-1.sto` est un **fichier de redémarrage** (*restart file*) généré par Bil à la fin de la simulation. Il contient :

1. **Le dernier état de l'inconnue $p_l$** pour chaque nœud du maillage (101 valeurs numériques après le temps final $t = 300$ s).
2. **Les termes implicites** (flux $W_l$ par élément et par point d'intégration).
3. **Les termes explicites** (conductivités $K_l$ par élément).

Ce fichier permet de **reprendre la simulation** depuis l'état final sans recalculer depuis $t = 0$. Il est chargé automatiquement si Bil détecte sa présence lors d'un nouveau lancement.

---

## 7. Résultats

Les sorties sont stockées dans les fichiers `M10-1.t0` (t=0 s), `M10-1.t1` (t=120 s), `M10-1.t2` (t=300 s). Chaque fichier contient, pour chaque point d'intégration du maillage, 5 quantités :

```
# Coordinates(1)  pressure(4)  flow(5)  saturation(8)
x [m]   y   z   p_l [Pa]   W_l [kg/m²/s]   0   0   s_l [-]
```

### État initial — $t = 0$ s

La pression suit exactement le champ linéaire initial. La saturation est quasi-totale dans toute la colonne.

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] |
|---------|-----------|-------------------|-----------|
| 0 | $1.000 \times 10^5$ | $-5.705 \times 10^{-2}$ | 1.0000 |
| 0.05 | $9.983 \times 10^4$ | $-5.705 \times 10^{-2}$ | 1.0000 |
| 0.10 | $9.966 \times 10^4$ | $-5.705 \times 10^{-2}$ | 1.0000 |
| 0.15 | $9.949 \times 10^4$ | $-5.705 \times 10^{-2}$ | 1.0000 |
| 0.20 | $9.932 \times 10^4$ | $-5.702 \times 10^{-2}$ | 0.9998 |

**Vérification analytique du flux :**

Le flux initial est uniforme, calculable analytiquement depuis le gradient de pression et la force gravitaire :

$$W_l = -K_{l,\text{sat}}\,\left(\frac{\partial p_l}{\partial x} - \rho_l g\right) = -8.9 \times 10^{-6} \times (-3400 - (-9810)) = -8.9 \times 10^{-6} \times 6410 \approx -5.70 \times 10^{-2} \text{ kg/(m²·s)}$$

Ce résultat est en accord parfait avec les valeurs du fichier `M10-1.t0`.

### État à $t = 120$ s

Un **front de désaturation** a progressé de haut en bas, atteignant $x \approx 0.15$–$0.19$ m. Le flux est réduit d'un facteur ~200 dans la zone désaturée.

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] | Remarque |
|---------|-----------|-------------------|-----------|----------|
| 0 | $1.000 \times 10^5$ | $-4.78 \times 10^{-2}$ | 1.000 | Fond saturé |
| 0.10 | $\approx 9.96 \times 10^4$ | $-4.78 \times 10^{-2}$ | 1.000 | Zone saturée |
| 0.185 | $\approx 9.920 \times 10^4$ | $-5.97 \times 10^{-3}$ | 0.418 | Zone de transition |
| 0.193 | $\approx 9.920 \times 10^4$ | $-3.31 \times 10^{-3}$ | 0.318 | |
| 0.200 | $9.918 \times 10^4$ | $-2.86 \times 10^{-4}$ | 0.154 | Sommet fortement désaturé |

### État à $t = 300$ s

Le front de désaturation a progressé plus loin vers le bas. La perméabilité résiduelle quasi-nulle gèle progressivement l'écoulement dans la zone désaturée.

| $x$ [m] | $p_l$ [Pa] | $W_l$ [kg/(m²·s)] | $s_l$ [-] | Remarque |
|---------|-----------|-------------------|-----------|----------|
| 0 | $1.000 \times 10^5$ | $-3.96 \times 10^{-2}$ | 1.000 | Fond saturé |
| 0.10 | $\approx 9.96 \times 10^4$ | $-3.96 \times 10^{-2}$ | 1.000 | Zone saturée |
| 0.186 | $\approx 9.919 \times 10^4$ | $-1.37 \times 10^{-3}$ | 0.272 | Front de désaturation |
| 0.200 | $9.916 \times 10^4$ | $-5.48 \times 10^{-5}$ | 0.115 | Sommet encore plus sec |

### Synthèse de l'évolution temporelle

```
Profil de saturation s_l(x)  [schématique]

s_l
1.0 │████████████████████████████╮  ← t = 0  (quasi-uniforme)
    │                             ╰─╮
    │████████████████████████╮       ╰──╮  ← t = 120 s
    │                         ╰──────╮   ╰╮
    │████████████████████╮            ╰──╮ ╰╮  ← t = 300 s
    │                     ╰─────────────╯  ╰─╯
0.1 │
    └──────────────────────────────────────────── x [m]
    0                   0.15              0.20
```

**Physique observée :**

1. **Flux décroissant au fond** : le flux en $x = 0$ passe de $-5.7 \times 10^{-2}$ à $-3.96 \times 10^{-2}$ kg/(m²·s) entre $t=0$ et $t=300$ s, signe que le profil de pression s'approche progressivement de l'équilibre hydrostatique.
2. **Désaturation asymétrique** : la désaturation se concentre en sommet de colonne et le front descend ; le fond reste saturé grâce à la condition de Dirichlet.
3. **Chute dramatique de la perméabilité** dans la zone désaturée ($k_{rl}$ chute de 8 décades) : le flux devient négligeable au-dessus du front.
4. **Convergence lente vers l'état d'équilibre** : à l'équilibre ($dp_l/dx = -9810$ Pa/m), la pression capillaire en sommet atteindrait $p_c(0.2) = 1962$ Pa, bien au-delà de la saturation résiduelle. La simulation à 300 s est donc encore loin de l'état final.

---

## 8. Discrétisation numérique

Le modèle est discrétisé par la méthode des **éléments finis (FEM)** via `FEM.h`. La matrice élémentaire globale est composée de deux contributions :

**Matrice de masse** (termes d'accumulation) :

$$K^\text{masse}_{ij} = \int_\Omega C(p_c)\,N_i\,N_j\,\mathrm{d}V, \qquad C(p_c) = -\rho_l\,\phi\,\frac{\partial s_l}{\partial p_c}$$

Évaluée au pas de temps courant (implicite), avec dérivée de la courbe tabulée interpolée.

**Matrice de conduction** (termes de flux, schéma semi-implicite) :

$$K^\text{cond}_{ij} = \int_\Omega K_l\,\nabla N_i \cdot \nabla N_j\,\mathrm{d}V$$

Évaluée **explicitement** au pas de temps précédent (conductivité $K_l$ stockée dans les termes explicites `vex`).

**Système à résoudre à chaque pas de temps $\Delta t$ :**

$$\left(K^\text{masse} + \Delta t\,K^\text{cond}\right)\,\Delta\mathbf{p}_l = -\mathbf{R}$$

résolu par Newton–Raphson jusqu'à convergence ($\|\mathbf{R}\| < 10^{-10}$).

**Cas particuliers :**

- **Zone saturée** : $C(p_c) = 0$ (courbe plate) → la matrice de masse est nulle localement → équation **elliptique** (état stationnaire instantané). Aucune instabilité n'est observée ici grâce au pas de temps adaptatif.
- **Front de désaturation** : $C(p_c)$ est grand (forte pente de la courbe $s_l$) → l'équation est parabolique et le front est bien résolu.

---

## 9. Références bibliographiques

### Équation de Richards

- **Richards, L. A.** (1931). Capillary conduction of liquids through porous mediums. *Physics*, 1(5), 318–333. — Article fondateur décrivant l'écoulement capillaire dans les sols partiellement saturés.

- **Bear, J.** (1972). *Dynamics of Fluids in Porous Media*. Elsevier, New York. — Traitement rigoureux des équations de Darcy et Richards en milieu poreux.

- **Celia, M. A., Bouloutas, E. T. & Zarba, R. L.** (1990). A general mass-conservative numerical solution for the unsaturated flow equation. *Water Resources Research*, 26(7), 1483–1496. — Formulation en pression de l'équation de Richards permettant la conservation exacte de la masse.

### Propriétés hydrauliques des billes de verre

- **Touma, J. & Vauclin, M.** (1986). Experimental and numerical analysis of two-phase infiltration in a partially saturated soil. *Transport in Porous Media*, 1(1), 27–55. — Expériences de référence sur les colonnes de billes de verre.

- **Haverkamp, R., Vauclin, M., Touma, J., Wierenga, P. J. & Vachaud, G.** (1977). A comparison of numerical simulation models for one-dimensional infiltration. *Soil Science Society of America Journal*, 41(2), 285–294. — Comparaison de codes sur colonnes de billes et sables.

### Courbes de rétention capillaire

- **Van Genuchten, M. Th.** (1980). A closed-form equation for predicting the hydraulic conductivity of unsaturated soils. *Soil Science Society of America Journal*, 44(5), 892–898. — Modèle analytique de référence pour $s_l(p_c)$ et $k_{rl}(p_c)$.

- **Brooks, R. H. & Corey, A. T.** (1964). *Hydraulic Properties of Porous Media*. Hydrology Paper 3, Colorado State University. — Modèle alternatif adapté aux matériaux à courbe abrupte (type billes de verre).

### Méthodes numériques pour l'équation de Richards

- **Haverkamp, R. & Vauclin, M.** (1979). A note on estimating finite difference interblock hydraulic conductivity values for transient unsaturated flow problems. *Water Resources Research*, 15(1), 181–187. — Stratégies d'évaluation des conductivités aux interfaces.

- **Paniconi, C. & Putti, M.** (1994). A comparison of Picard and Newton iteration in the numerical solution of multidimensional variably saturated flow problems. *Water Resources Research*, 30(12), 3357–3374. — Analyse de la convergence Newton vs Picard pour l'équation de Richards.

### Implémentation numérique

- **Dangla, P.** — *Bil : a FEM/FVM platform for multiphysics simulations*. Université Gustave Eiffel. Code source : <https://github.com/Universite-Gustave-Eiffel/bil>
