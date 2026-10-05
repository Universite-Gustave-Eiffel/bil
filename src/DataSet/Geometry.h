#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Geometry_t;
struct DataFile_t;
struct Periodicities_t;

#include <string>

extern Geometry_t*  (Geometry_New)   (void) ;
extern Geometry_t*  (Geometry_Create)(DataFile_t*) ;
extern void         (Geometry_Scan)  (Geometry_t*,DataFile_t*);
extern void         (Geometry_Delete)(void*) ;


#define Geometry_GetDimension(GEO)              ((GEO)->GetDimension())
#define Geometry_GetSymmetry(GEO)               ((GEO)->GetSymmetry())
#define Geometry_GetCoordinateSystem(GEO)       ((GEO)->GetCoordinateSystem())
#define Geometry_GetPeriodicities(GEO)          ((GEO)->GetPeriodicities())

#define Geometry_SetDimension(GEO,A)              ((GEO)->SetDimension(A))
#define Geometry_SetSymmetry(GEO,A)               ((GEO)->SetSymmetry(A))
#define Geometry_SetCoordinateSystem(GEO,A)       ((GEO)->SetCoordinateSystem(A))
#define Geometry_SetPeriodicities(GEO,A)          ((GEO)->SetPeriodicities(A))


/* Is it periodic? */
#define Geometry_IsPeriodic(GEO) ((GEO)->IsPeriodic())

/* Test the symmetry */
#define Geometry_HasCylindricalSymmetry(GEO) ((GEO)->HasCylindricalSymmetry())
#define Geometry_HasSphericalSymmetry(GEO) ((GEO)->HasSphericalSymmetry())
#define Geometry_HasPlaneSymmetry(GEO) ((GEO)->HasPlaneSymmetry())


/* Set the symmetry */
#define Geometry_SetNoSymmetry(GEO) (GEO)->SetNoSymmetry()
#define Geometry_SetPlaneSymmetry(GEO) (GEO)->SetPlaneSymmetry()
#define Geometry_SetCylindricalSymmetry(GEO) (GEO)->SetCylindricalSymmetry()
#define Geometry_SetSphericalSymmetry(GEO) (GEO)->SetSphericalSymmetry()

#define Geometry_Set(GEO,...) (GEO)->Set(__VA_ARGS__)

#define Geometry_Print(GEO) (GEO)->Print()

#include <algorithm>
#include "Symmetry.h"
#include "CoorSys.h"

struct Geometry_t {
  unsigned short int _dim ;    /* Dimension (1,2,3) */
  Symmetry_t _symmetry ;       /* Symmetry */
  CoorSys_t _coorsys ;         /* Coordinate system */
  Periodicities_t* _periodicities ;

  /* The getters */
  unsigned short int GetDimension(){return _dim;}
  Symmetry_t GetSymmetry(){return _symmetry;}
  CoorSys_t GetCoordinateSystem(){return _coorsys;}
  Periodicities_t* GetPeriodicities(){return _periodicities;}

  /* The setters */
  void SetDimension(unsigned short int A){_dim = A;}
  void SetSymmetry(Symmetry_t A){_symmetry = A;}
  void SetCoordinateSystem(CoorSys_t A){_coorsys = A;}
  void SetPeriodicities(Periodicities_t* A){_periodicities = A;}

  void SetNoSymmetry(){
    Symmetry_SetNoSymmetry(_symmetry);
    CoorSys_SetCartesian(_coorsys);
  }
  void SetPlaneSymmetry(){
    Symmetry_SetPlaneSymmetry(_symmetry);
    CoorSys_SetCartesian(_coorsys);
  }
  void SetCylindricalSymmetry(){
    Symmetry_SetCylindricalSymmetry(_symmetry);
    CoorSys_SetCylindrical(_coorsys);
  }
  void SetSphericalSymmetry(){
    Symmetry_SetSphericalSymmetry(_symmetry);
    CoorSys_SetSpherical(_coorsys);
  }
  void  Set(unsigned short int dim,const std::string& isym) {
    SetDimension(dim);
    SetNoSymmetry();
    SetPeriodicities(nullptr);

    /* The symmetry */
    if(dim > 0 && dim < 3) {
      std::string sym = isym;

      // Convert the string to lowercase in-place
      std::transform(sym.begin(), sym.end(), sym.begin(), [](unsigned char c) {
          return std::tolower(c);
      });

      if(!sym.compare("plane")){
        SetPlaneSymmetry();
      } else if(!sym.compare("axis")){
        SetCylindricalSymmetry();
      } else if(!sym.compare("sphe")){
        SetSphericalSymmetry();
      } else {
        SetPlaneSymmetry();
        printf("Geometry_t::Set: by default the symmetry is set to plane");
      }
    }
  }
  void Print(){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    PRINT("Geometry:\n") ;
    PRINT("\t Dimension = %dD\n",_dim) ;
    PRINT("\t Symmetry = ") ;
    
    if(0) {
    } else if(HasCylindricalSymmetry()) {
      PRINT("Axisymmetrical\n") ;
    } else if(HasSphericalSymmetry()) {
      PRINT("Spherical\n") ;
    } else if(HasPlaneSymmetry()) {
      PRINT("Plane\n") ;
    } else {
      PRINT("No symmetry\n") ;
    }
    #undef PRINT
  }
  
  /* The testers*/
  bool IsPeriodic(){return(_periodicities != NULL);}
  bool HasCylindricalSymmetry(){return Symmetry_IsCylindrical(_symmetry);}
  bool HasSphericalSymmetry(){return Symmetry_IsSpherical(_symmetry);}
  bool HasPlaneSymmetry(){return Symmetry_IsPlane(_symmetry);}
} ;


#ifdef __CPLUSPLUS
}
#endif

#endif
