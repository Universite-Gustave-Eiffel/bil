#ifndef FIELDS_H
#define FIELDS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Fields_t; //typedef struct Fields_t       Fields_t ;
struct Field_t;
struct DataFile_t;


extern Fields_t*   (Fields_New)     (void) ;
extern Fields_t*   (Fields_Create)  (DataFile_t*) ;
extern void        (Fields_Scan)    (Fields_t*,DataFile_t*);
extern void        (Fields_Delete)  (void*) ;


#define Fields_MaxNbOfFields             (100)


#define Fields_GetNbOfFields(FLDS)    ((FLDS)->GetNbOfFields())
#define Fields_GetField(FLDS)         ((FLDS)->GetField())
#define Fields_GetCapacity(FLDS)      ((FLDS)->GetCapacity())

#define Fields_SetNbOfFields(FLDS,A)  ((FLDS)->SetNbOfFields(A))
#define Fields_SetField(FLDS,A)       ((FLDS)->SetField(A))

#define Fields_EmplaceBack(FLDS,...)  ((FLDS)->EmplaceBack(__VA_ARGS__))

#define Fields_Print(FLDS)            ((FLDS)->Print())


#include <string>
#include <stdexcept>

struct Fields_t {
  private:
  size_t _n_ch ;
  Field_t* _ch ;

  public:
  size_t GetCapacity() {return Fields_MaxNbOfFields;}
  template<typename... Args>
  void EmplaceBack(const std::string&,const Args&...);

  public:
  size_t GetNbOfFields() const {return _n_ch;}
  Field_t* GetField() {return _ch;}
  void SetNbOfFields(size_t n) {
    if(n >= GetCapacity()) {
      throw std::length_error("Maximum number of fields reached");
    }
    _n_ch = n;
  }
  void SetField(Field_t* ch) {_ch = ch;}
  void Print();
} ;


#include "Field.h"

  template<typename... Args>
  inline void Fields_t::EmplaceBack(const std::string& type,const Args&... args) {
    Field_t* field = _ch + _n_ch;
    
    if(_n_ch >= GetCapacity()) {
      throw std::length_error("Maximum number of fields reached");
    }

    Field_Set(field,type,args...);
    _n_ch++;
  }

  inline void Fields_t::Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("Fields:\n") ;
    PRINT("\t Nb of fields = %lu\n",_n_ch) ;
    for(size_t i = 0 ; i < _n_ch ; i++) {
      PRINT("Field(%lu):\n",i) ;
      (_ch+i)->Print();
    }
    #undef PRINT
  }


#ifdef __CPLUSPLUS
}
#endif

#endif
