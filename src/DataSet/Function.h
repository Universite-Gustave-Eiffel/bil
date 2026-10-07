#ifndef FUNCTION_H
#define FUNCTION_H


/* Forward declarations */
struct Function_t;
struct FunctionPiecewiseAffine_t;
struct DataFile_t;


extern Function_t*  (Function_New)          (void) ;
extern void         (Function_Delete)       (void*) ;
extern void         (Function_Scan)         (Function_t*,DataFile_t*) ;
//extern double       (Function_ComputeValue) (Function_t*,double) ;


#define Function_MaxLengthOfKeyWord        (30)
#define Function_MaxLengthOfFileName       (200)
#define Function_MaxLengthOfTextLine       (500)


#define Function_GetType(FCT)            ((FCT)->GetType())
#define Function_GetFunctionFormat(FCT)  ((FCT)->GetFunctionFormat())

#define Function_SetType(FCT,A)          ((FCT)->SetType(A))
#define Function_SetFunctionFormat(FCT,A)   ((FCT)->SetFunctionFormat(A))

#define Function_Set(FCT,...)            ((FCT)->Set(__VA_ARGS__))
#define Function_Print(FCT)              ((FCT)->Print())
#define Function_ComputeValue(FCT,...)   ((FCT)->ComputeValue(__VA_ARGS__))



/* PiecewiseAffine */
extern FunctionPiecewiseAffine_t*  (FunctionPiecewiseAffine_New)(const size_t);
//extern void (FunctionPiecewiseAffine_Delete)(void*);

#define FunctionPiecewiseAffine_GetNbOfPoints(FCT)      ((FCT)->GetNbOfPoints())
#define FunctionPiecewiseAffine_GetXValue(FCT)          ((FCT)->GetXValue())
#define FunctionPiecewiseAffine_GetFValue(FCT)          ((FCT)->GetFValue())

#define FunctionPiecewiseAffine_SetNbOfPoints(FCT,A)    ((FCT)->SetNbOfPoints(A))
#define FunctionPiecewiseAffine_SetXValue(FCT,A)        ((FCT)->SetXValue(A))
#define FunctionPiecewiseAffine_SetFValue(FCT,A)        ((FCT)->SetFValue(A))

#define FunctionPiecewiseAffine_Delete(FCT)             (((FunctionPiecewiseAffine_t*)FCT)->Delete())


#include <algorithm>
#include <vector>
#include <string>

struct FunctionPiecewiseAffine_t {
  private:
  size_t  _n ;
  double* _t ;
  double* _f ;

  public:
  size_t  GetNbOfPoints() {return _n;}
  double* GetXValue() {return _t;}
  double* GetFValue() {return _f;}

  void SetNbOfPoints(size_t n) {_n = n;}
  void SetXValue(double* t) {_t = t;}
  void SetFValue(double* f) {_f = f;}

  public:
  void Delete(void);

  void Set(const std::vector<double>& t,const std::vector<double>& f){
    size_t n = std::min(t.size(),f.size());
    
    Set(n,t.data(),f.data());
  }
  void Set(size_t const&,double const*,double const*);
  //void Set(std::string const& filestr){Set(filestr.c_str());}
  void Set(char const*);

  size_t ReadInFile(char*);

  double ComputeValue(double const& t) {
    size_t  nb_points = _n ;
    double* tm = _t ;
    double* ft = _f ;
    
    /*
      Cas t < tm[0] 
    */
    if(t <= tm[0]) {
      return(ft[0]) ;
    
    /*
      Cas t > tm[n-1] 
    */
    } else if(t >= tm[nb_points - 1]) {
      return(ft[nb_points - 1]) ;
    
    /*
      Cas t[i1] <= t <= t[i2]
      Calcul des deux points i1=Min(i) et i2=Max(i) du tableau fn
      correspondant au plus petit intervalle de temps [t1;t2]
      contenant t. Deux cas de figure se presentent:
      1) t2 > t1 et i2 = i1+1;
      2) t2 = t1 et i2 >= i1 avec i2-i1 maximum.
    */
    } else {
      size_t i1 = 0, i2 = nb_points - 1 ;
      double t1, t2, f1, f2 ;
    
      for(size_t i = 0 ; i < nb_points ; i++) {
        if(t >= tm[i]) i1 = i ;
        if(t <= tm[i]) break  ;
      }
    
      for(size_t i = i1 ; i < nb_points ; i++) {
        if(t < tm[i]) {
          for(size_t j = i ; j > 0 ; j--) {
            if(t <= tm[j]) i2 = j ;
            if(t >= tm[j]) break  ;
          }
          break ;
        }
      }
    
      t1 = tm[i1] ;
      f1 = ft[i1] ;
      t2 = tm[i2] ;
      f2 = ft[i2] ;
    
      /* 1) Intervalle non nul */
      if(t2 > t1) {
        return (f1 + (f2 - f1)*(t - t1)/(t2 - t1)) ;
      
    /* 2) Intervalle nul */
      } else {
        /* un seul point */
        if(i1 == i2) {
          return (f1) ;
        /* plusieurs points */
        } else {
          throw std::runtime_error("FunctionPiecewiseAffine_t::ComputeValue: more than one nul time steps") ;
          return(0.) ;
        }
      }
    }
  
    return(0.) ;
  }
  void Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("\t Nb of points = %lu\n",_n) ;
    for(size_t j = 0 ; j < _n ; j++) {
    PRINT("\t F(%e) = %e\n",_t[j],_f[j]) ;
    }
    #undef PRINT
  }
} ;


#include <string>

struct Function_t {
  private:
  char* _type;
  void* _store;

  public:
  char* GetType(){return _type;}
  void* GetFunctionFormat(){return _store;}

  void SetType(char* type){_type = type;}
  void SetFunctionFormat(void* store){_store = store;}

  private:
  void DeleteStorage();

  public:
  template<typename... Args>
  void Set(const std::string&,Args&&...);

  double ComputeValue(double const& t) {
    std::string type(_type);
    double v;

    if(type.compare("piecewiseaffine") == 0) {
      v = ((FunctionPiecewiseAffine_t*)_store)->ComputeValue(t) ;
    } else {
      throw std::runtime_error("Function_t::ComputeValue: unknown type") ;
    }
    return(v);
  }
  void Print() {
    std::string type(_type);

    if(type.compare("piecewiseaffine") == 0) {
      ((FunctionPiecewiseAffine_t*)_store)->Print() ;
    } else {
      throw std::runtime_error("Function_t::Print: unknown type") ;
    }
  }
} ;


#include <utility>
#include "Curves.h"
#include "Curve.h"
#include "Mry.h"

  inline void Function_t::DeleteStorage(){
    if(_store) {
      std::string type(_type);

      if(type.compare("piecewiseaffine") == 0) {
        ((FunctionPiecewiseAffine_t*)_store)->Delete();
      }
      
      Mry_Free(_store);
      _store = nullptr;
    }
  }

  template<typename... Args>
  inline void Function_t::Set(const std::string& type,Args&&... args){
    DeleteStorage();
    
    strcpy(_type,type.c_str());

    if(type.compare("piecewiseaffine") == 0) {
      _store = (FunctionPiecewiseAffine_t*) Mry_New(FunctionPiecewiseAffine_t) ;
      ((FunctionPiecewiseAffine_t*)_store)->Set(std::forward<Args>(args)...) ;

    } else {
      throw std::runtime_error("Function_t::Set: unknown type") ;
    }
  }

  inline void FunctionPiecewiseAffine_t::Delete() {
    if(_t) {
      Mry_Free(_t);
      _t = NULL;
    }
  
    if(_f) {
      Mry_Free(_f);
      _f = NULL;
    }
  }

  inline void FunctionPiecewiseAffine_t::Set(size_t const& n,double const* t,double const* f){
    FunctionPiecewiseAffine_t* fct = FunctionPiecewiseAffine_New(n) ;

    this[0] = fct[0];
    Mry_Free(fct);

    for(size_t i = 0 ; i < n ; i++) {
      _t[i] = t[i];
      _f[i] = f[i];
    }
  }

  inline void FunctionPiecewiseAffine_t::Set(char const* line1)
  /* Lecture dans le fichier "nom" */
  {
    char   line[Function_MaxLengthOfTextLine];
    char*  c;
    FILE*  fict ;
    int long pos ;
    char nom[Function_MaxLengthOfFileName] ;
  
    String_Scan(line1," %s",nom) ;
      
    if(strlen(nom) > Function_MaxLengthOfFileName) {
      throw std::runtime_error("FunctionPiecewiseAffine_t::ReadInFile: too long file name") ;
    }

    fict = fopen(nom,"r") ;
  
    if(fict) {
      size_t n_points = 1 ;
      size_t n_fonctions = 0 ;
    
      do {
        fgets(line,sizeof(line),fict) ;
        c = (char*) strtok(line," \n") ;
        /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
      } while((c == NULL || *c == '#') && !feof(fict)) ;
    
      while((char*) strtok(NULL," \n") != NULL) n_fonctions++ ;

      if(n_fonctions > 1) {
        throw std::runtime_error("FunctionPiecewseAffine_t::Set: nb of functions > 1") ;
      }

      /* nb de points */
      while(!feof(fict) && fgets(line,sizeof(line),fict)) {
        c = (char*) strtok(line," \n") ;
        if(c != NULL && *c != '#') n_points++ ;
      }
  
      {
        /* positionnement dans le fichier */
        rewind(fict) ;
    
        do {
          pos = ftell(fict) ;
          fgets(line,sizeof(line),fict) ;
          c = (char*) strtok(line," \n") ;
          /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
        } while((c == NULL || *c == '#') && !feof(fict)) ;
    
        fseek(fict,pos,SEEK_SET) ;
      }
  
      /* Read time and function values */
      {
        std::vector<double> t_vec(n_points);
        std::vector<double> f_vec(n_points);
        double* t = t_vec.data();
        double* f = f_vec.data();

        for(size_t i = 0 ; i < n_points ; i++) {
          fscanf(fict,"%le",t + i) ;
          fscanf(fict,"%le",f + i) ;
        }

        Set(t_vec,f_vec);
      }
    
    } else {
      int nbofcurves = 10 ;
      Curves_t* curves = Curves_Create(nbofcurves) ;
      Curve_t*  curve  = Curves_GetCurve(curves) ;
    
      Curves_ReadCurves(curves,line1) ;
    
      {
        size_t n_fonctions = Curves_GetNbOfCurves(curves) ;

        if(n_fonctions > 1) {
          throw std::runtime_error("FunctionPiecewseAffine_t::Set: nb of functions > 1") ;
        }
      }
    
  
      /* Read time and function values */
      {
        size_t n_points = Curve_GetNbOfPoints(curve) ;
        std::vector<double> t_vec(n_points);
        std::vector<double> f_vec(n_points);
        double* t = t_vec.data();
        double* f = f_vec.data();
        double* x = Curve_CreateSamplingOfX(curve) ;  
        double* y = Curve_GetYValue(curve) ;
      
        for(size_t i = 0 ; i < n_points ; i++) {  
          t[i] = x[i] ;
          f[i] = y[i] ;
        }

        Set(t_vec,f_vec);
      
        Mry_Free(x) ;
      }
    
      Curves_Delete(curves) ;
      Mry_Free(curves) ;
    }

    fclose(fict) ;
  }

  inline size_t FunctionPiecewiseAffine_t::ReadInFile(char* line1)
  /* Lecture des fonctions du temps dans le fichier "nom"
     retourne le nb de fonctions lues */
  {
    size_t n_points ;
    size_t n_fonctions ;
    char   line[Function_MaxLengthOfTextLine],*c ;
    FILE   *fict ;
    int long pos ;
    char nom[Function_MaxLengthOfFileName] ;
  
    String_Scan(line1," %s",nom) ;
      
    if(strlen(nom) > Function_MaxLengthOfFileName) {
      throw std::runtime_error("FunctionPiecewiseAffine_t::ReadInFile: too long file name") ;
    }

    fict = fopen(nom,"r") ;
  
    if(fict) {
      /* nb de fonctions */
      n_fonctions = 0 ;
    
      do {
        fgets(line,sizeof(line),fict) ;
        c = (char*) strtok(line," \n") ;
        /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
      } while((c == NULL || *c == '#') && !feof(fict)) ;
    
      while((char*) strtok(NULL," \n") != NULL) n_fonctions++ ;

      /* nb de points */
      n_points = 1 ;
    
      while(!feof(fict) && fgets(line,sizeof(line),fict)) {
        c = (char*) strtok(line," \n") ;
        if(c != NULL && *c != '#') n_points++ ;
      }

      /* reservation de la memoire */
      {    
        for(size_t i = 0 ; i < n_fonctions ;i++) {
          FunctionPiecewiseAffine_t* fct = FunctionPiecewiseAffine_New(n_points) ;
        
          this[i] = fct[0] ;
          Mry_Free(fct) ;
        }
      }
  
      {
        /* positionnement dans le fichier */
        rewind(fict) ;
    
        do {
          pos = ftell(fict) ;
          fgets(line,sizeof(line),fict) ;
          c = (char*) strtok(line," \n") ;
          /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
        } while((c == NULL || *c == '#') && !feof(fict)) ;
    
        fseek(fict,pos,SEEK_SET) ;
  
  
        /* Read time and function values */
        {        
          for(size_t i = 0 ; i < n_points ; i++) {
            double x ;
    
            fscanf(fict,"%le",&x) ;
    
            for(size_t j = 0 ; j < n_fonctions ; j++) {
              double* t = this[j].GetXValue() ;
              double* f = this[j].GetFValue() ;
            
              t[i] = x ;
              fscanf(fict,"%le",f + i) ;
            }
          }
        }
      }
    
    } else {
      int nbofcurves = 10 ;
      Curves_t* curves = Curves_Create(nbofcurves) ;
      Curve_t*  curve  = Curves_GetCurve(curves) ;
    
      Curves_ReadCurves(curves,line1) ;
    
      n_fonctions = Curves_GetNbOfCurves(curves) ;
    
      n_points = Curve_GetNbOfPoints(curve) ;
    
      /* reservation de la memoire */
      {    
        for(size_t i = 0 ; i < n_fonctions ;i++) {
          FunctionPiecewiseAffine_t* fct = FunctionPiecewiseAffine_New(n_points) ;
        
          this[i] = fct[0] ;
          Mry_Free(fct) ;
        }
      }
  
      {
        double* t = Curve_CreateSamplingOfX(curve) ;
      
        /* Read time and function values */
        for(size_t i = 0 ; i < n_points ; i++) {    
          for(size_t j = 0 ; j < n_fonctions ; j++) {
            Curve_t* curve_j = curve + j ;
            double* y = Curve_GetYValue(curve_j) ;
            double* x = this[j].GetXValue() ;
            double* f = this[j].GetFValue() ;
          
            x[i] = t[i] ;
            f[i] = y[i] ;
          }
        }
      
        Mry_Free(t) ;
      }
    
      Curves_Delete(curves) ;
      Mry_Free(curves) ;
    }

    fclose(fict) ;

    return(n_fonctions) ;
  }


/* Old notations which I try to eliminate little by little */
#define fonc_t           Function_t
#define fonction(a,b)    Function_ComputeValue(&(b),(a))

#endif
