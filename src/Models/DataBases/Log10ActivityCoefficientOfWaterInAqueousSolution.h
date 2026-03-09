#ifndef LOG10ACTIVITYCOEFFICIENTOFWATERINAQUEOUSSOLUTION_H
#define LOG10ACTIVITYCOEFFICIENTOFWATERINAQUEOUSSOLUTION_H


#define Log10ActivityCoefficientOfWaterInAqueousSolution(M,I) \
        Log10ActivityCoefficientOfWaterInAqueousSolution_##M(I)


/*
 * M   = Model (Davies,...)
 * Z_k = electric charge of ion k
 * Im  = molal ionic strength = 0.5 * Sum{ m_k/m_0*Z_k*Z_k } (m_0 = 1 mol/kg)
 * Ic  = molar ionic strength = 0.5 * Sum{ c_k/c_0*Z_k*Z_k } (c_0 = 1 mol/L)
 * c_0*V_w = m_0*M_w = 0.018 (M_w = molar mass of water; V_w = molar volume of water)
 * n_k = molar content (mol)
 * c_k = molar concentration (n_k/V)
 * m_k = molal concentration (n_k/(M_w*n_w))
 * m_k/m_0 = c_k/(c_w*m_0*M_w) = c_k/(c_w*c_0*V_w)
 * Im  = Ic / (c_w*V_w) = Ic / (1 - Sum(V_k*c_k))
 * 
 * a_w = activity of water
 * g_w = activity coefficient of water: a_w = g_w*a_w^id
 * a_k = activity of solute or ion in solution
 * g_k = activity coefficient of solute or ion in solution (a_k = g_k*m_k)
 *
 * Definition of activity coefficient of water (g_w):
 * 
 * ln(a_w)    = ln(a_w^id) + ln(g_w)
 * ln(a_w^id) = - M_w * Sum_k m_k
 *
 * Note that the osmotic coefficient, phi, is defined by
 * ln(a_w)    = phi * ln(a_w^id)
 */

/* From Gibb-Duhem equation:
 *
 * n_w * d(ln(a_w)) = - Sum_k n_k * d(ln(a_k))
 * 
 * d(ln(a_w)) = - M_w * (Sum_k m_k * d(ln(a_k)))
 *            = - M_w * (Sum_k d(m_k) + Sum_k m_k * d(ln(g_k)))
 * i.e.
 *
 * ln(a_w) = - M_w * (Sum_k m_k + Int_0^{m_k} (Sum_k m_k * d(ln(g_k))))
 *
 * ln(a_w^id) = - M_w * (Sum_k m_k) = - Sum_k c_k/c_w
 * ln(g_w)    = - M_w * (Int_0^{m_k} (Sum_k m_k * d(ln(g_k))))
 *
 * log10(a_w^id) = - 1/ln(10) * M_w * (Sum_k m_k)
 * log10(g_w)    = - M_w * (Int_0^{m_k} (Sum_k m_k * d(log10(g_k))))
 * 
 * We also note that
 * log10(a_w^id) = - 1/ln(10) * (Sum_k c_k/c_w)
 */

/* Davies model (I=Im):
 * -------------------
 * 
 * Log10(g_k) = Z_k*Z_k*F(I)
 * F(I) = -A*sqrt(I) / (1 + B*sqrt(I)) + C*I
 * 
 * A = 0.5
 * B = 1
 * C = 0.15
 * 
 * Validity for I < 0.5
 *
 * log10(g_w) = - M_w * (Int_0^{m_k} (Sum_k m_k d(log10(g_k))))
 *            = - M_w * m_0 * (Int_0^{I} (2*I*F'(I)dI)
 *            = - M_w * m_0 * (2*I*F(I) - Int_0^{I} (2*F(I)*dI))
 *
 * 2*I*F(I) = -2*A*I*sqrt(I) / (1 + B*sqrt(I)) + 2*C*I*I
 * 
 * Int_0^{I} (2*F(I)*dI) = -2*A*(1/(B)*I - 2/(B*B)*sqrt(I) + 2/(B*B*B)*ln(1+B*sqrt(I))) + C*I*I
 *
 * log10(g_w) = - 0.018 * (2*A*(-I*sqrt(I)/(1 + B*sqrt(I)) + 1/(B)*I - 2/(B*B)*sqrt(I) + 2/(B*B*B)*ln(1+B*sqrt(I))) + C*I*I)
 *
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

#define Log10ActivityCoefficientOfWaterInAqueousSolution_DAVIES(I) \
        (-0.018*(-(I)*sqrt(I)/(1 + sqrt(I)) + (I) - 2*sqrt(I) + 2*log(1+sqrt(I)) + 0.15*(I)*(I)))

#endif
