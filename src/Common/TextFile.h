#ifndef TEXTFILE_H
#define TEXTFILE_H

#ifdef __CPLUSPLUS
extern "C" {
#endif



/* Forward declarations */
struct TextFile_t; //typedef struct TextFile_t       TextFile_t ;


#include <stdio.h>

extern TextFile_t*     (TextFile_New)(const char* = nullptr) ;
extern void            (TextFile_Delete)(void*) ;
extern FILE*           (TextFile_OpenFile)(TextFile_t*,const char*) ;
extern void            (TextFile_CloseFile)(TextFile_t*) ;
extern void            (TextFile_StoreFilePosition)(TextFile_t*) ;
extern void            (TextFile_MoveToStoredFilePosition)(TextFile_t*) ;
extern char*           (TextFile_ReadLineFromCurrentFilePosition)(TextFile_t*,char*,int) ;
extern char*           (TextFile_ReadLineFromCurrentFilePositionInString)(TextFile_t*,char*,size_t) ;
extern size_t          (TextFile_CountNbOfCharacters)(TextFile_t*) ;
extern size_t          (TextFile_CountNbOfEatenCharacters)(TextFile_t*) ;
extern size_t          (TextFile_CountTheMaxNbOfCharactersPerLine)(TextFile_t*) ;
extern int             (TextFile_Exists)(TextFile_t*) ;
extern void            (TextFile_CleanTheStream)(TextFile_t*) ;
extern FILE*           (TextFile_FileStreamCopy)(TextFile_t*) ;
//extern char*           (TextFile_FileCopy)(TextFile_t*) ;
extern char*           (TextFile_StoreFileContent)(TextFile_t*) ;


/*
 *  Function-like macros
 */

#define TextFile_Rewind(TF) \
        do { \
          rewind(TextFile_GetFileStream(TF)) ; \
          TextFile_SetPreviousPositionInFileContent(TF,TextFile_GetFileContent(TF));\
          TextFile_SetCurrentPositionInFileContent(TF,TextFile_GetFileContent(TF));\
        } while(0)


#define TextFile_DoesNotExist(TF) \
        (!TextFile_Exists(TF))



#define TextFile_Scan(TF, ...) \
        String_Scan(TextFile_GetCurrentPositionInFileContent(TF),__VA_ARGS__)
        
#define TextFile_SkipLine(TF) \
        String_SkipLine(TextFile_GetCurrentPositionInFileContent(TF))
        

/** Reads N data of size sizeof(V) with the format "FMT" from 
 *  the current position in the string and advance accordingly. */
#define TextFile_ReadArrayFromCurrentFilePosition(TF,N,FMT,V) \
        do { \
          char* TextFile_c = TextFile_GetCurrentPositionInFileContent(TF) ; \
          String_ScanArray(TextFile_c,N,FMT,V) ; \
        } while(0)


/* Remove comments */
#define TextFile_RemoveComments(TF) \
        String_RemoveComments(TextFile_GetFileContent(TF),TextFile_GetFileContent(TF))



#define TextFile_MaxLengthOfTextLine      (500)
#define TextFile_SizeOfBuffer             (500*sizeof(char))
#define TextFile_MaxLengthOfFileName      (200)



#define TextFile_GetFileName(TF)          ((TF)->GetFileName())
#define TextFile_GetFileStream(TF)        ((TF)->GetFileStream())
#define TextFile_GetFilePosition(TF)      ((TF)->GetFilePosition())
#define TextFile_GetFileContent(TF)       ((TF)->GetFileContent())
#define TextFile_GetPreviousPositionInFileContent(TF)   ((TF)->GetPreviousPositionInFileContent())
#define TextFile_GetCurrentPositionInFileContent(TF)    ((TF)->GetCurrentPositionInFileContent())


#define TextFile_SetFileName(TF,A)          ((TF)->SetFileName(A))
#define TextFile_SetFileStream(TF,A)        ((TF)->SetFileStream(A))
#define TextFile_SetFilePosition(TF,A)      ((TF)->SetFilePosition(A))
#define TextFile_SetFileContent(TF,A)       ((TF)->SetFileContent(A))
#define TextFile_SetPreviousPositionInFileContent(TF,A)   ((TF)->SetPreviousPositionInFileContent(A))
#define TextFile_SetCurrentPositionInFileContent(TF,A)    ((TF)->SetCurrentPositionInFileContent(A))

#define TextFile_Set(TF,...)    ((TF)->Set(__VA_ARGS__))


#include <stdexcept>

struct TextFile_t {
  private:
  char*     _filename ;
  char*     _filecontent ;
  FILE*     _stream ;          /* Current file stream if any */
  fpos_t*   _pos ;             /* Previous stored file position of the stream */
  //size_t    prestrpos ;       /* Previous position in the string file content */
  //size_t    curstrpos ;       /* Current position in the string file content */
  /* char*     line ;            *//* memory space for a line */
  /* Buffer_t* buffer ;          *//* Buffer */
  /* long int ccount ;           *//* Nb of characters in file */
  /* long int wcount ;           *//* Nb of words in file */
  /* long int lcount ;           *//* Nb of lines in file */
  /* int linelength ;            *//* Length of the longest line */
  char*    _prepos ;       /* Previous position in the file content */
  char*    _curpos ;       /* Current position in the file content */

  public:
  char*    GetFileName(){return _filename ;}
  char*    GetFileContent(){return _filecontent ;}
  FILE*    GetFileStream(){return _stream ;}
  fpos_t*  GetFilePosition(){return _pos ;}
  char*    GetPreviousPositionInFileContent(){return _prepos ;}
  char*    GetCurrentPositionInFileContent(){return _curpos ;}

  void SetFileName(char* a){_filename = a;}
  void SetFileContent(char* a){
    _filecontent = a;
    _prepos = a;
    _curpos = a;
  }
  void SetFileStream(FILE* a){_stream = a;}
  void SetFilePosition(fpos_t* a){_pos = a;}
  void SetPreviousPositionInFileContent(char* a){_prepos = a;}
  void SetCurrentPositionInFileContent(char* a){_curpos = a;}

  void Set(char const* filename){
    if(strlen(filename) > TextFile_MaxLengthOfFileName) {
      throw std::length_error("TextFile_t::Set") ;
    }
    strcpy(GetFileName(),filename) ;
  }
} ;


#ifdef __CPLUSPLUS
}
#endif

#include "String_.h"
#include <cstddef>
#endif
