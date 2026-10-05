#ifndef FIELD_H
#define FIELD_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Field_t;
struct FieldAffine_t;
struct FieldGrid_t;
struct FieldRandom_t;
struct DataFile_t;


extern Field_t*       (Field_New)                  (void) ;
extern void           (Field_Delete)               (void*) ;
extern void           (Field_Scan)                 (Field_t*,DataFile_t*) ;
//extern double         (Field_ComputeValueAtPoint)  (Field_t*,double*,int) ;


#define Field_MaxLengthOfKeyWord        (30)
#define Field_MaxLengthOfFileName       (200)
#define Field_MaxLengthOfTextLine       (500)


#define Field_GetType(FLD)           ((FLD)->GetType())
#define Field_GetFieldFormat(FLD)    ((FLD)->GetFieldFormat())

#define Field_SetType(FLD,A)         ((FLD)->SetType(A))
#define Field_SetFieldFormat(FLD,A)  ((FLD)->SetFieldFormat(A))

#define Field_Set(FLD,...)           ((FLD)->Set(__VA_ARGS__))
#define Field_Print(FLD)             ((FLD)->Print())
#define Field_ComputeValueAtPoint(FLD,...)  ((FLD)->ComputeValueAtPoint(__VA_ARGS__))



/* FieldAffine */
#define FieldAffine_GetValue(FLD)        ((FLD)->_v)
#define FieldAffine_GetGradient(FLD)     ((FLD)->_g)
#define FieldAffine_GetCoordinate(FLD)   ((FLD)->_x)

#define FieldAffine_SetValue(FLD,A)        ((FLD)->_v = (A))
#define FieldAffine_SetGradient(FLD,A)     ((FLD)->_g = (A))
#define FieldAffine_SetCoordinate(FLD,A)   ((FLD)->_x = (A))

#define FieldAffine_Set(FLD,A,B,C)           ((FLD)->Set(A,B,C))


/* FieldGrid */
extern FieldGrid_t*   (FieldGrid_Create)(char const*) ;
//extern void           (FieldGrid_Delete)(void*) ;


#define FieldGrid_GetFileName(FLD)                ((FLD)->_name)
#define FieldGrid_GetNbOfPointsAlongX(FLD)        ((FLD)->_n_x)
#define FieldGrid_GetNbOfPointsAlongY(FLD)        ((FLD)->_n_y)
#define FieldGrid_GetNbOfPointsAlongZ(FLD)        ((FLD)->_n_z)
#define FieldGrid_GetCoordinateAlongX(FLD)        ((FLD)->_x)
#define FieldGrid_GetCoordinateAlongY(FLD)        ((FLD)->_y)
#define FieldGrid_GetCoordinateAlongZ(FLD)        ((FLD)->_z)
#define FieldGrid_GetValue(FLD)                   ((FLD)->_v)

#define FieldGrid_SetFileName(FLD,A)          ((FLD)->_name = (A))
#define FieldGrid_SetNbOfPointsAlongX(FLD,A)  ((FLD)->_n_x = (A))
#define FieldGrid_SetNbOfPointsAlongY(FLD,A)  ((FLD)->_n_y = (A))
#define FieldGrid_SetNbOfPointsAlongZ(FLD,A)  ((FLD)->_n_z = (A))
#define FieldGrid_SetCoordinateAlongX(FLD,A)  ((FLD)->_x = (A))
#define FieldGrid_SetCoordinateAlongY(FLD,A)  ((FLD)->_y = (A))
#define FieldGrid_SetCoordinateAlongZ(FLD,A)  ((FLD)->_z = (A))
#define FieldGrid_SetValue(FLD,A)             ((FLD)->_v = (A))

#define FieldGrid_Set(FLD,A)       ((FLD)->Set(A))
#define FieldGrid_Delete(FLD)      (((FieldGrid_t*)FLD)->Delete())


/* 3. FieldConstant */
#define FieldConstant_GetValue(FLD)               ((FLD)->_v)
#define FieldConstant_GetRandomRangeLength(FLD)   ((FLD)->_ranlen)

#define FieldConstant_SetValue(FLD,A)               ((FLD)->_v = (A))
#define FieldConstant_SetRandomRangeLength(FLD,A)   ((FLD)->_ranlen = (A))

#define FieldConstant_Set(FLD,A,B)           ((FLD)->Set(A,B))

//#include<variant>
//using FieldFormat_t = std::variant<FieldAffine_t,FieldGrid_t,FieldRandom_t> ;

#include <vector>
#include <stdexcept>

struct FieldAffine_t {
  public:
  double _v ;                 /* value at A */
  double _g[3] ;              /* gradient at A */
  double _x[3] ;              /* coordinates of A */

  public:
  template<typename... Args>
  void Set(Args...) {
    throw std::runtime_error("FieldAffine_t::Set: unknown type") ;
  }
  void Set(double const& v,const std::vector<double>& g,const std::vector<double>& x) {
    Set(v,g.data(),x.data());
  }
  void Set(double const& v,double const* g,double const* x) {
    _v = v ;
    for(size_t i = 0 ; i < 3 ; i++) {
      _g[i] = g[i] ;
      _x[i] = x[i] ;
    }
  }
  double ComputeValueAtPoint(double const* x,const int& dim){
    double v = _v;

    for(int i = 0 ; i < dim ; i++) {
      v += _g[i]*(x[i] - _x[i]) ;
    }
  
    return(v) ;
  }
  void Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("\t Type: %s\n","affine") ;
    PRINT("\t Value    = %e\n",_v) ;
    PRINT("\t Gradient = ") ;
    PRINT("(%e,%e,%e)",_g[0],_g[1],_g[2]) ;
    PRINT("\n") ;
    PRINT("\t Point    = ") ;
    PRINT("(%e,%e,%e)",_x[0],_x[1],_x[2]) ;
    PRINT("\n") ;
    #undef PRINT
  }
} ;

struct FieldGrid_t {
  public:
  char*  _name ;               /* File name which grid is stored in */
  size_t _n_x ;                /* nb de points sur Ox */
  size_t _n_y ;                /* nb de points sur Oy */
  size_t _n_z ;                /* nb de points sur Oz */
  double* _x ;                 /* coordonnees sur Ox  */
  double* _y ;                 /* coordonnees sur Oy  */
  double* _z ;                 /* coordonnees sur Oz  */
  double* _v ;                 /* valeurs aux points de la grille */

  public:
  void Delete();

  template<typename... Args>
  void Set(Args...) {
    throw std::runtime_error("FieldGrid_t::Set: unknown type") ;
  }
  void Set(std::string const&);

  double ComputeValueAtPoint(double* p,const int& dim){
    #define V(i,j,k) (_v[(i) + (j)*_n_x + (k)*_n_x*_n_y])
    size_t n_x = _n_x ;
    size_t n_y = _n_y ;
    size_t n_z = _n_z ;
    double* x = _x ;
    double* y = _y ;
    double* z = _z ;
    double x0 = p[0],y0 = p[1],z0 = p[2] ;
    double v ;
    size_t ix1,iy1,iz1,ix2,iy2,iz2 ;

    if(dim > 0) {
      if(x0 <= x[0]) {
        ix1 = 0 ;
        ix2 = ix1 ;
      } else if(x0 >= x[n_x - 1]) {
        ix1 = n_x - 1 ;
        ix2 = ix1 ;
      } else {
        for(ix1 = 0 ; ix1 < n_x - 1 ; ix1++)  if(x0 < x[ix1+1]) break  ;
        ix2 = ix1 + 1 ;
      }
    }

    if(dim > 1) {
      if(y0 <= y[0]) {
        iy1 = 0 ;
        iy2 = iy1 ;
      } else if(y0 >= y[n_y - 1]) {
        iy1 = n_y - 1 ;
        iy2 = iy1 ;
      } else {
        for(iy1 = 0 ; iy1 < n_y - 1 ; iy1++)  if(y0 < y[iy1+1]) break  ;
        iy2 = iy1 + 1 ;
      }
    }

    if(dim > 2) {
      if(z0 <= z[0]) {
        iz1 = 0 ;
        iz2 = iz1 ;
      } else if(z0 >= z[n_z - 1]) {
        iz1 = n_z - 1 ;
        iz2 = iz1 ;
      } else {
        for(iz1 = 0 ; iz1 < n_z - 1 ; iz1++)  if(z0 < z[iz1+1]) break  ;
        iz2 = iz1 + 1 ;
      }
    }

    if(dim == 1) {
      double x1 = x[ix1],x2 = x[ix2] ;
      double v1 = V(ix1,0,0) ;
      double v2 = V(ix2,0,0) ;
    
      if(ix1 == ix2) v = v1 ;
      else v = v1 + (v2 - v1)*(x0 - x1)/(x2 - x1) ;

    } else if(dim == 2) {
      double x1 = x[ix1],x2 = x[ix2] ;
      double y1 = y[iy1],y2 = y[iy2] ;
      double d_x = x2 - x1,d_y = y2 - y1 ;
      double v11 = V(ix1,iy1,0) ;
      double v21 = V(ix2,iy1,0) ;
      double v12 = V(ix1,iy2,0) ;
      double v22 = V(ix2,iy2,0) ;
    
      if(ix1 == ix2 && iy1 == iy2) {
        v = v11 ;
      } else if(ix2 == ix1 + 1 && iy2 == iy1 + 1) {
        v = v11 + (v21 - v11)*(x0 - x1)/d_x + (v12 - v11)*(y0 - y1)/d_y \
        + (v22 + v11 - v21 - v12)*(x0 - x1)*(y0 - y1)/(d_x*d_y) ;
      } else if(ix1 == ix2) {
        v = v11 + (v12 - v11)*(y0 - y1)/d_y ;
      } else if(iy1 == iy2) {
        v = v11 + (v21 - v11)*(x0 - x1)/d_x ;
      } else {
        throw std::runtime_error("FieldGrid_t::ComputeValueAtPoint: non prevu") ;
      }
      throw std::runtime_error("FieldGrid_t::ComputeValueAtPoint: non prevu") ;
    } else if(dim == 3) {
      throw std::runtime_error("FieldGrid_t::ComputeValueAtPoint: non prevu") ;
    } else {
      throw std::runtime_error("FieldGrid_t::ComputeValueAtPoint: non prevu") ;
    }

    return(v) ;
    #undef V
  }
  void Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("\t Type: %s\n","grid") ;

    for(size_t u = 0 ; u < _n_x ; u++) {          
      for(size_t j = 0 ; j < _n_y ; j++) {            
        for(size_t k = 0 ; k < _n_z ; k++) {
          PRINT("\t v(%e,%e,%e) = %e\n",_x[u],_y[j],_z[k],_v[(u) + (j)*_n_x + (k)*_n_x*_n_y]) ;
        }
      }
    }
    #undef PRINT
  }
} ;

struct FieldRandom_t {      /* Random field */
  public:
  double _v ;                /* Value */
  double _ranlen ;           /* Random range length */

  public:
  template<typename... Args>
  void Set(Args...) {
    throw std::runtime_error("FieldRandom_t::Set: unknown type") ;
  }
  void Set(double const& v,double const& ranlen = 0) {
    _v = v ;
    _ranlen = ranlen ;
  }
  double ComputeValueAtPoint(double* x){
    double rmax = (double)RAND_MAX ;
    double r = (double)rand() / rmax - 0.5 ;
      
    return(_v + _ranlen*r) ;
  }
  void Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("\t Type: %s\n","random") ;
    PRINT("\t Value    = %e\n",_v) ;
    PRINT("\t RandomRange = %e\n",_ranlen) ;
    PRINT("\n") ;
    #undef PRINT
  }
} ;


#include <string>
#include <tuple>

struct Field_t {
  private:
  char*   _type ;
  void*   _store ;

  public:
  char* GetType(){return _type;}
  void* GetFieldFormat(){return _store;}

  void SetType(char* type){_type = type;}
  void SetFieldFormat(void* store){_store = store;}

  public:
  void DeleteStorage();

  template<typename... Args>
  void Set(const std::string&,const Args&...);

  double ComputeValueAtPoint(double* x,const int& dim = 3){
    std::string type(_type);
    double v = 0;

    if(type.compare("affine") == 0) {
      v = ((FieldAffine_t*)_store)->ComputeValueAtPoint(x,dim) ;
    } else if(type.compare("grid") == 0) {
      v = ((FieldGrid_t*)_store)->ComputeValueAtPoint(x,dim) ;
    } else if(type.compare("random") == 0) {
      v = ((FieldRandom_t*)_store)->ComputeValueAtPoint(x) ;
    } else {
      throw std::runtime_error("Field_t::ComputeValueAtPoint: unknown type") ;
    }

    return(v) ;
  }
  void Print() {
    std::string type(_type);

    if(type.compare("affine") == 0) {
    ((FieldAffine_t*)_store)->Print() ;
    } else if(type.compare("grid") == 0) {
      ((FieldGrid_t*)_store)->Print() ;
    } else if(type.compare("random") == 0) {
      ((FieldRandom_t*)_store)->Print() ;
    } else {
      throw std::runtime_error("Field_t::Print: unknown type") ;
    }
  }
} ;

#include "Mry.h"

  inline void FieldGrid_t::Delete(){
    if(_name) {
      Mry_Free(_name);
      _name = NULL;
    }

    if(_x) {
      Mry_Free(_x);
      _x = NULL;
      _y = NULL;
      _z = NULL;
    }

    if(_v) {
      Mry_Free(_v);
      _v = NULL;
    }
  }

  inline void FieldGrid_t::Set(std::string const& filestr) {
    FieldGrid_t* grid = FieldGrid_Create(filestr.c_str()) ;

    this[0] = grid[0];
    Mry_Free(grid);
  }

  inline void Field_t::DeleteStorage(){
    if(_store) {
      std::string type(_type);

      if(type.compare("grid") == 0) {
        ((FieldGrid_t*)_store)->Delete();
      }
    
      Mry_Free(_store);
      _store = nullptr;
    }
  }

  template<typename... Args>
  inline void Field_t::Set(const std::string& type,const Args&... args){
    DeleteStorage();

    strcpy(_type,type.c_str());

    if(type.compare("affine") == 0) {
      _store = (FieldAffine_t*) Mry_New(FieldAffine_t) ;
    ((FieldAffine_t*)_store)->Set(args...) ;

    } else if(type.compare("grid") == 0) {
      _store = (FieldGrid_t*) Mry_New(FieldGrid_t) ;
      ((FieldGrid_t*)_store)->Set(args...) ;

    } else if(type.compare("random") == 0) {
      _store = (FieldRandom_t*) Mry_New(FieldRandom_t) ;
      ((FieldRandom_t*)_store)->Set(args...) ;

    } else {
      throw std::runtime_error("Field_t::Set: unknown type") ;
    }
  }


/* Old notations which I try to eliminate little by little */
#define chmp_t          Field_t
#define chmpaffine_t    FieldAffine_t
#define chmpgrille_t    FieldGrid_t
#define champ(a,b,c)    Field_ComputeValueAtPoint(&(c),(a),(b))


#ifdef __CPLUSPLUS
}
#endif
#endif
