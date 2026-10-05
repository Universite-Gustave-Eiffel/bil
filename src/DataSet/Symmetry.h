#ifndef SYMMETRY_H
#define SYMMETRY_H

enum Symmetry_e {             /* symmetry of the problem */
  Symmetry_None,
  Symmetry_Plane,
  Symmetry_Cylindrical,
  Symmetry_Spherical,
} ;




/* Test and set the symmetry */
#if 1
typedef enum Symmetry_e     Symmetry_t ;

#define Symmetry_IsCylindrical(SYM) (SYM == Symmetry_Cylindrical)
#define Symmetry_IsSpherical(SYM) (SYM == Symmetry_Spherical)
#define Symmetry_IsPlane(SYM) (SYM == Symmetry_Plane)

#define Symmetry_SetNoSymmetry(SYM) (SYM = Symmetry_None)
#define Symmetry_SetPlaneSymmetry(SYM) (SYM = Symmetry_Plane)
#define Symmetry_SetCylindricalSymmetry(SYM) (SYM = Symmetry_Cylindrical) 
#define Symmetry_SetSphericalSymmetry(SYM) (SYM = Symmetry_Spherical)
#else
//#define Symmetry_GetType(SYM) ((SYM).GetType())
//#define Symmetry_SetType(SYM,A) ((SYM).SetType(A))

#define Symmetry_IsCylindrical(SYM) ((SYM).IsCylindrical())
#define Symmetry_IsSpherical(SYM) ((SYM).IsSpherical())
#define Symmetry_IsPlane(SYM) ((SYM).IsPlane())

#define Symmetry_SetNoSymmetry(SYM) (SYM).SetNoSymmetry()
#define Symmetry_SetPlaneSymmetry(SYM) (SYM).SetPlaneSymmetry()
#define Symmetry_SetCylindricalSymmetry(SYM) (SYM).SetCylindricalSymmetry()
#define Symmetry_SetSphericalSymmetry(SYM) (SYM).SetSphericalSymmetry()


struct Symmetry_t {
  Symmetry_e _type ;       /* symmetry of the problem */
  
  /* The getters */
  //Symmetry_e GetType(){return _type;}
  
  /* The setters */
  //void SetType(Symmetry_e A){_type = A;}

  void SetNoSymmetry(){_type = Symmetry_None;}
  void SetPlaneSymmetry(){_type = Symmetry_Plane;}
  void SetCylindricalSymmetry(){_type = Symmetry_Cylindrical;}
  void SetSphericalSymmetry(){_type = Symmetry_Spherical;}

  bool IsCylindrical(){return (_type == Symmetry_Cylindrical);}
  bool IsSpherical(){return (_type == Symmetry_Spherical);}
  bool IsPlane(){return (_type == Symmetry_Plane);}
} ;
#endif


/* Old notations which I try to eliminate little by little */
//#define geom_t    Symmetry_t

#endif
