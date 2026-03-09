#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Exception.h"
#include "Math_.h"
#include "Temperature.h"
#include "CementSolutionChemistry.h"
#include "CementSolutionDiffusion.h"
#include "DiffusionCoefficientOfMoleculeInWater.h"





static void  (CementSolutionDiffusion_AllocateMemory)     (CementSolutionDiffusion_t*) ;
static void  (CementSolutionDiffusion_UpdateElementFluxes)(CementSolutionDiffusion_t*) ;




CementSolutionDiffusion_t* (CementSolutionDiffusion_Create)(void)
{
  CementSolutionDiffusion_t* csd = (CementSolutionDiffusion_t*) Mry_New(CementSolutionDiffusion_t);
  
  if(!csd) arret("CementSolutionDiffusion_Create") ;
  
  {
    /* Memory allocation */
    CementSolutionDiffusion_AllocateMemory(csd) ;
  
    /* Initialize concentrations to zero and other related variables */
    CementSolutionDiffusion_Initialize(csd) ;
  }
  
  return(csd) ;
}



void (CementSolutionDiffusion_AllocateMemory)(CementSolutionDiffusion_t* csd)
{
  
  
  /* Allocation of space for the temperature */
  {
    Temperature_t* temp = Temperature_Create() ;
    
    CementSolutionDiffusion_GetTemperature(csd) = temp ;
  }
  
  /* Allocation of space for the diffusion coefficients */
  {
    double* d = (double*) Mry_New(double,CementSolutionDiffusion_NbOfSpecies) ;
    
    if(!d) arret("CementSolutionDiffusion_AllocateMemory(10)") ;
    
    CementSolutionDiffusion_GetDiffusionCoefficient(csd) = d ;
  }
  
  
  /* Allocation of space for the gradients */
  {
    double* grd = (double*) Mry_New(double,CementSolutionDiffusion_NbOfSpecies) ;
    
    if(!grd) arret("CementSolutionDiffusion_AllocateMemory(11)") ;
    
    CementSolutionDiffusion_GetGradient(csd) = grd ;
  }
  
  
  /* Allocation of space for the potentials */
  {
    size_t n = CementSolutionDiffusion_MaxNbOfPotentialVectors ;
    double* pot = (double*) Mry_New(double,n*CementSolutionDiffusion_NbOfSpecies) ;
    
    if(!pot) arret("CementSolutionDiffusion_AllocateMemory(11)") ;
    
    CementSolutionDiffusion_GetPotential(csd) = pot ;
  }
  
  
  /* Allocation of space for the pointer to potentials */
  {
    size_t n = CementSolutionDiffusion_MaxNbOfPotentialVectors ;
    double** ppot = (double**) Mry_New(double*,n) ;
    
    if(!ppot) arret("CementSolutionDiffusion_AllocateMemory(11)") ;
    
    CementSolutionDiffusion_GetPointerToPotentials(csd) = ppot ;
    
    {
      double* pot = CementSolutionDiffusion_GetPotential(csd) ;
      
      for(size_t i = 0 ; i < n ; i++) {
        ppot[i] = pot + i*CementSolutionDiffusion_NbOfSpecies ;
      }
    }
  }
  
  
  /* Allocation of space for the concentration fluxes */
  {
    double* flx = (double*) Mry_New(double,CementSolutionDiffusion_NbOfSpecies) ;
    
    if(!flx) arret("CementSolutionDiffusion_AllocateMemory(11)") ;
    
    CementSolutionDiffusion_GetFlux(csd) = flx ;
  }
  
  
  /* Allocation of space for the element fluxes */
  {
    double* efx = (double*) Mry_New(double,CementSolutionDiffusion_NbOfElementFluxes) ;
    
    if(!efx) arret("CementSolutionDiffusion_AllocateMemory(6)") ;
    
    CementSolutionDiffusion_GetElementFlux(csd) = efx ;
  }
}



void (CementSolutionDiffusion_Delete)(void* self)
{
  CementSolutionDiffusion_t* csd = (CementSolutionDiffusion_t*) self;

  {
    Temperature_t* temp = CementSolutionDiffusion_GetTemperature(csd);
    
    if(temp) {
      Temperature_Delete(temp);
      Mry_Free(temp);
    }
  }
  
  {
    double* d = CementSolutionDiffusion_GetDiffusionCoefficient(csd);
    
    if(d) {
      Mry_Free(d);
    }
  }

  {
    double* grd = CementSolutionDiffusion_GetGradient(csd);
    
    if(grd) {
      Mry_Free(grd);
    }
  }

  {
    double* pot = CementSolutionDiffusion_GetPotential(csd);
    
    if(pot) {
      Mry_Free(pot);
    }
  }

  {
    double** ppot = CementSolutionDiffusion_GetPointerToPotentials(csd);
    
    if(ppot) {
      Mry_Free(ppot);
    }
  }

  {
    double* flx = CementSolutionDiffusion_GetFlux(csd);
    
    if(flx) {
      Mry_Free(flx);
    }
  }
  
  {
    double* efx = CementSolutionDiffusion_GetElementFlux(csd);
    
    if(efx) {
      Mry_Free(efx);
    }
  }
}




/* Intern functions */

void (CementSolutionDiffusion_Initialize)(CementSolutionDiffusion_t* csd)
{
  /* Diffusion coefficients */
    
  {
    double TK = CementSolutionDiffusion_GetRoomTemperature(csd) ;
    double* z = CementSolutionDiffusion_GetDiffusionCoefficient(csd);
    
    #define AFFECT(A,B)  A=B
    #define D(A)  DiffusionCoefficientOfMoleculeInWater(A,TK)
    CementSolutionChemistry_SetProperties(AFFECT,z,D);
    #undef D
    #undef AFFECT
  }
  
  /* Gradients */
  {
    int     n = CementSolutionDiffusion_NbOfSpecies ;
    double* v = CementSolutionDiffusion_GetGradient(csd) ;
  
    {      
      for(int i = 0 ; i < n ; i++) {
        v[i] = 0;
      }
    }
  }
  
  /* Fluxes */
  {
    int     n = CementSolutionDiffusion_NbOfSpecies ;
    double* v = CementSolutionDiffusion_GetFlux(csd) ;
  
    {      
      for(int i = 0 ; i < n ; i++) {
        v[i] = 0;
      }
    }
  }
  
  /* Element fluxes */
  {
    int     n = CementSolutionDiffusion_NbOfElementFluxes ;
    double* v = CementSolutionDiffusion_GetElementFlux(csd) ;
  
    {      
      for(int i = 0 ; i < n ; i++) {
        v[i] = 0;
      }
    }
  }
}




void (CementSolutionDiffusion_ComputeFluxes)(CementSolutionDiffusion_t* csd)
{
  /* The fluxes */
  {
    double* grad = CementSolutionDiffusion_GetGradient(csd) ;
    double* flux = CementSolutionDiffusion_GetFlux(csd) ;
    double* diff = CementSolutionDiffusion_GetDiffusionCoefficient(csd) ;
    double* z    = CementSolutionChemistry_GetValence() ;
    int n = CementSolutionDiffusion_NbOfSpecies ;
    double  current = 0. ;
    int i ;

    for(i = 0 ; i < n ; i++) {
      flux[i]  = - diff[i] * grad[i] ;
      current += z[i]*flux[i] ;
    }
    
    CementSolutionDiffusion_GetIonCurrent(csd) = current ;
  }
  
  CementSolutionDiffusion_UpdateElementFluxes(csd) ;
}




void (CementSolutionDiffusion_UpdateElementFluxes)(CementSolutionDiffusion_t* csd)
/** Update the element fluxes **/
{
  CementSolutionDiffusion_SetElementFluxOf(csd,Al);
  CementSolutionDiffusion_SetElementFluxOf(csd,Ca);
  CementSolutionDiffusion_SetElementFluxOf(csd,C);
  CementSolutionDiffusion_SetElementFluxOf(csd,Cl);
  CementSolutionDiffusion_SetElementFluxOf(csd,Fe);
  CementSolutionDiffusion_SetElementFluxOf(csd,Mg);
  CementSolutionDiffusion_SetElementFluxOf(csd,N);
  CementSolutionDiffusion_SetElementFluxOf(csd,K);
  CementSolutionDiffusion_SetElementFluxOf(csd,Si);
  CementSolutionDiffusion_SetElementFluxOf(csd,Na);
  CementSolutionDiffusion_SetElementFluxOf(csd,Sr);
  CementSolutionDiffusion_SetElementFluxOf(csd,S);
}
