#ifndef LOG10ACTIVITYCOEFFICIENTOFAQUEOUSSPECIES_H
#define LOG10ACTIVITYCOEFFICIENTOFAQUEOUSSPECIES_H


#define Log10ActivityCoefficientOfAqueousSpecies(M,Z,I) \
        Log10ActivityCoefficientOfAqueousSpecies_##M(Z,I)

/*
 * M   = Model (Davies,...)
 * Z   = electric charge of ion
 * Im  = molal ionic strength = 0.5 * Sum{ m_k/m_0*Z_k*Z_k } (m_0 = 1 mol/kg)
 * Ic  = molar ionic strength = 0.5 * Sum{ c_k/c_0*Z_k*Z_k } (c_0 = 1 mol/L)
 * c_0*V_w = m_0*M_w = 0.018 (M_w = molar mass of water; V_w = molar volume of water)
 * n_k = molar content (mol)
 * c_k = molar concentration (n_k/V)
 * m_k = molal concentration (n_k/(M_w*n_w))
 * m_k/m_0 = c_k/(c_w*m_0*M_w) = c_k/(c_w*c_0*V_w)
 * Im  = Ic / (c_w*V_w) = Ic / (1 - Sum(V_k*c_k))
 * 
 */

/* Davies equation (I=Im):
 * ----------------------
 * 
 * Log10(g) = Z*Z*(-A*sqrt(I) / (1 + B*sqrt(I)) + C*I)
 * 
 * A = 0.5
 * B = 1
 * C = 0.15
 * 
 * Validity for I < 0.5
 * 
 * Source:
 * Davies, C.W. (1962). Ion Association. London: Butterworths. pp. 37-53
 */
 
/* Expanded extended Debye-Huckel model:
 * ------------------------------------
 *
 * Log10(g_i) = Z_i*Z_i*(-A*sqrt(I) / (1 + B*a_i*sqrt(I)) + b_i*I
 *
 * A = 0.51            at T = 298 K and P = 0.1 MPa
 * B = 0.328e10 (1/m)  at T = 298 K and P = 0.1 MPa
 * a_i = ion-size parameter or effective diameter of ion i (m)
 * b_i = ion fitting parameter
 */

#define Log10ActivityCoefficientOfAqueousSpecies_DAVIES(Z,I) \
        ((Z)*(Z)*(-0.5*sqrt(I)/(1 + sqrt(I)) + 0.15*(I)))

#endif
