#ifndef FEM_H
#define FEM_H


/* Forward declarations */
struct FEM_t;  //typedef struct FEM_t     FEM_t ;
struct IntFct_t;
struct Element_t;
struct Buffers_t;


//#define FEM_Create()    new FEM_t
//#define FEM_Delete(fem) delete ((FEM_t*) fem)



#define FEM_ComputeElasticMatrix(...) \
        FEM_ComputeStiffnessMatrix(__VA_ARGS__)

#define FEM_ComputePoroelasticMatrix \
        FEM_ComputePoroelasticMatrixOriginal

#define FEM_ComputePoroelasticMatrixOriginal(...) \
        Utils_CAT_NARG(FEM_ComputePoroelasticMatrixOriginal,__VA_ARGS__)(__VA_ARGS__)

#define FEM_ComputePoroelasticMatrixOriginal5(...) \
        FEM_ComputePoroelasticMatrixOriginal6(__VA_ARGS__,0)

#define FEM_ComputePoroelasticMatrixBis(...) \
        Utils_CAT_NARG(FEM_ComputePoroelasticMatrixBis,__VA_ARGS__)(__VA_ARGS__)

#define FEM_ComputePoroelasticMatrixBis5(...) \
        FEM_ComputePoroelasticMatrixBis6(__VA_ARGS__,0)



#define FEM_MaxShift                             (81 + 18*Model_MaxNbOfEquations + 9*Model_MaxNbOfEquations*Model_MaxNbOfEquations)
#define FEM_MaxNbOfMatrices                      (4)
#define FEM_MaxSizeOfMatrix                      (Element_MaxNbOfDOF*Element_MaxNbOfDOF*sizeof(double))
#define FEM_SizeOfBuffer                         (FEM_MaxNbOfMatrices*FEM_MaxSizeOfMatrix)


#define FEM_GetElement(fem)                      ((fem)->GetElement())
#define FEM_GetPointerToIntFct(fem)              ((fem)->GetPointerToIntFct())
#define FEM_GetInput(fem)                        ((fem)->GetInput())
#define FEM_GetOutput(fem)                       ((fem)->GetOutput())
#define FEM_GetShiftOfInput(fem)                 ((fem)->GetShiftOfInput())
#define FEM_GetBuffers(fem)                      ((fem)->GetBuffers())

#define FEM_SetElement(fem,A)                    ((fem)->SetElement(A))
#define FEM_SetPointerToIntFct(fem,A)            ((fem)->SetPointerToIntFct(A))
#define FEM_SetInput(fem,A)                      ((fem)->SetInput(A))
#define FEM_SetOutput(fem,A)                     ((fem)->SetOutput(A))
#define FEM_SetShiftOfInput(fem,A)               ((fem)->SetShiftOfInput(A))
#define FEM_SetBuffers(fem,A)                    ((fem)->SetBuffers(A))


/* Buffer */
#define FEM_GetBuffer(fem) \
        Buffers_GetBufferOfCurrentThread(FEM_GetBuffers(fem))
        

#define FEM_AllocateInBuffer(fem,sz) \
        (Buffer_Allocate(FEM_GetBuffer(fem),(sz)))
        
#define FEM_FreeBuffer(fem) \
        (Buffer_Free(FEM_GetBuffer(fem)))
        
#define FEM_FreeBufferFrom(fem,p) \
        (Buffer_FreeFrom(FEM_GetBuffer(fem),(char*) (p)))



struct FEM_t {
  public:
  
  #if 0
  /* Constructor */
  FEM_t() {
    /* Space allocation for output */
    {
      int nn = Element_MaxNbOfNodes ;
      int neq = Model_MaxNbOfEquations ;
      int ndof = nn*neq ;
      
      _output = (double*) Mry_New(double,ndof*ndof) ;
    }
  
    /* Space allocation for input */
    {
      int np = IntFct_MaxNbOfIntPoints ;
      
      _input = (double*) Mry_New(double,np*FEM_MaxShift) ;
    }
  
    /* Space allocation for pintfct */
    {
      _pintfct = (IntFct_t**) Mry_New(IntFct_t*,IntFcts_MaxNbOfIntFcts) ;
    }
  
    /* Space allocation for buffer */
    {
      _buffers = Buffers_Create(FEM_SizeOfBuffer) ;
    }
  }

  /* Destructor */
  ~FEM_t() {
    if(_output) {
      free(_output);
      _output = NULL;
    }
    
    if(_input) {
      free(_input);
      _input = NULL;
    }
    
    if(_pintfct) {
      free(_pintfct);
      _pintfct = NULL;
    }

    if(_buffers) {
      Buffers_Delete(_buffers)  ;
      free(_buffers) ;
      _buffers = NULL ;
    }
  }
  #endif
  
  void SetElement(Element_t* el) {_el = el;}
  void SetPointerToIntFct(IntFct_t** pintfct) {_pintfct = pintfct;}
  void SetInput(void* input) {_input = input;}
  void SetOutput(void* output) {_output = output;}
  void SetShiftOfInput(int shift) {_shift = shift;}
  void SetBuffers(Buffers_t* buffers) {_buffers = buffers;}
  
  /* Accessors */
  Element_t* GetElement() {return(_el);}
  IntFct_t** GetPointerToIntFct() {return(_pintfct);}
  void*      GetInput() {return(_input);}
  void*      GetOutput() {return(_output);}
  int        GetShiftOfInput() {return(_shift);}
  Buffers_t* GetBuffers() {return(_buffers);}
  
  //GenericObject_Delete_t* Delete ;
  
  private:
  Element_t* _el ;             /* Element */
  IntFct_t** _pintfct ;        /* Pointer to interpolation functions */
  void*      _input ;
  void*      _output ;
  int        _shift ;
  Buffers_t* _buffers ;        /* Buffers */
} ;



struct Mesh_t;
struct Load_t;


/* Non-member functions */
extern void     (FEM_Delete)(void*);
extern FEM_t*   (FEM_GetInstance)(Element_t*) ;
/* Matrices */
extern double*  (FEM_ComputeStiffnessMatrix)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputeMassMatrix)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputeMassMatrix)(FEM_t*,IntFct_t*,const double*,const int,int const) ;
extern double*  (FEM_ComputeConductionMatrix)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputeConductionMatrix)(FEM_t*,IntFct_t*,double const*,int const,int const);
extern double*  (FEM_ComputeBiotMatrix)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputePoroelasticMatrixOriginal6)(FEM_t*,IntFct_t*,const double*,const int,const int,const int) ;
extern double*  (FEM_ComputePoroelasticMatrixBis6)(FEM_t*,IntFct_t*,const double*,const int,const int,const int) ;
extern double*  (FEM_AssemblePoroelasticAndConductionMatrices)(FEM_t*,int const,int const,double const*,double*) ;
extern void     (FEM_TransformMatrixFromDegree2IntoDegree1)(FEM_t*,const int,const int,double*) ;
/* Residus */
extern double*  (FEM_ComputeSurfaceLoadResidu)(FEM_t*,IntFct_t*,Load_t*,const double,const double) ;
extern double*  (FEM_ComputeBodyForceResidu)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputeStrainWorkResidu)(FEM_t*,IntFct_t*,const double*,const int) ;
extern double*  (FEM_ComputeFluxResidu)(FEM_t*,IntFct_t*,const double*,const int) ;
extern void     (FEM_TransformResiduFromDegree2IntoDegree1)(FEM_t*,const int,const int,double*) ;
extern double*  (FEM_ComputeMassResidu)(FEM_t*,IntFct_t*,const double*,const double*,const int) ;
extern double*  (FEM_ComputeMassBalanceEquationResidu)(FEM_t*,IntFct_t*,const double*,const double*,const double*,const double,const int) ;
/* Compute unknowns and unknown gradients: method 1 */
extern double   (FEM_ComputeCurrentUnknown)(FEM_t*,double*,int,int) ;
extern double   (FEM_ComputePreviousUnknown)(FEM_t*,double*,int,int) ;
extern double*  (FEM_ComputeCurrentUnknownGradient)(FEM_t*,double*,int,int) ;
extern double*  (FEM_ComputePreviousUnknownGradient)(FEM_t*,double*,int,int) ;
extern double*  (FEM_ComputeCurrentLinearStrainTensor)(FEM_t*,double*,double*,int,int) ;
extern double*  (FEM_ComputePreviousLinearStrainTensor)(FEM_t*,double*,double*,int,int) ;
extern double*  (FEM_ComputeIncrementalLinearStrainTensor)(FEM_t*,double*,double*,int,int) ;
/* Compute unknowns and unknown gradients: method 2 */
extern double   (FEM_ComputeUnknown)(FEM_t*,double const* const*,IntFct_t*,int,int) ;
extern double*  (FEM_ComputeUnknownGradient)(FEM_t*,double const* const*,IntFct_t*,int,int) ;
extern double*  (FEM_ComputeLinearStrainTensor)(FEM_t*,double const* const*,IntFct_t*,int,int) ;
extern double*  (FEM_ComputeDisplacementVector)(FEM_t*,double const* const*,IntFct_t*,int,int) ;
/* Averaging */
extern void     (FEM_AverageStresses)(Mesh_t*,double*) ;
extern double   (FEM_AverageCurrentImplicitTerm)(Mesh_t*,const char*,const int,const int) ;
extern double   (FEM_AveragePreviousImplicitTerm)(Mesh_t*,const char*,const int,const int) ;
extern void     (FEM_AddAverageTerms)(FEM_t*,const int,const int,const int) ;
extern double   (FEM_ComputeVolume)(Mesh_t*) ;


/* For the macros */
#include "Utils.h"
#include "Model.h"
#include "Element.h"

//#include "FEM.h.in"

#endif
