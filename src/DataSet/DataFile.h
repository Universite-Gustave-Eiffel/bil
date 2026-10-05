#ifndef DATAFILE_H
#define DATAFILE_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct DataFile_t; //typedef struct DataFile_t     DataFile_t ;
struct TextFile_t;



#include <stdio.h>
extern DataFile_t*  (DataFile_New)(char const* = nullptr) ;
extern void         (DataFile_Delete)(void*) ;
//extern char*        (DataFile_ReadLineFromCurrentFilePosition)(DataFile_t*) ;
extern char*        (DataFile_ReadLineFromCurrentFilePositionInString)(DataFile_t*) ;
//extern void*        (DataFile_ReadArray)(DataFile_t*,const char*,void*,int,size_t) ;
extern size_t*         (DataFile_ReadInversePermutationOfNodes)(DataFile_t*,size_t) ;



/*
 *  Function-like macros
 */
#define DataFile_OpenFile(DF,MODE) \
        TextFile_OpenFile(DataFile_GetTextFile(DF),MODE)

#define DataFile_CloseFile(DF) \
        TextFile_CloseFile(DataFile_GetTextFile(DF))

#define DataFile_Exists(DF) \
        TextFile_Exists(DataFile_GetTextFile(DF))

#define DataFile_DoesNotExist(DF) \
        (!DataFile_Exists(DF))

#define DataFile_StoreFilePosition(DF) \
        TextFile_StoreFilePosition(DataFile_GetTextFile(DF))

#define DataFile_MoveToStoredFilePosition(DF) \
        TextFile_MoveToStoredFilePosition(DataFile_GetTextFile(DF))

#define DataFile_Rewind(DF) \
        TextFile_Rewind(DataFile_GetTextFile(DF))
        
#define DataFile_ReadArrayFromCurrentFilePosition(DF, ...) \
        TextFile_ReadArrayFromCurrentFilePosition(DataFile_GetTextFile(DF),__VA_ARGS__)
        
#define DataFile_ReadDoublesFromCurrentFilePosition(DF, ...) \
        DataFile_ReadArrayFromCurrentFilePosition(DF,"%le",__VA_ARGS__)

#define DataFile_RemoveComments(DF) \
        TextFile_RemoveComments(DataFile_GetTextFile(DF))




/* Tokens in file content */
#define DataFile_FindToken(DF, ...) \
        String_FindToken(DataFile_GetFileContent(DF),__VA_ARGS__)
        
#define DataFile_FindNthToken(DF, ...) \
        String_FindNthToken(DataFile_GetFileContent(DF),__VA_ARGS__)
        
#define DataFile_CountTokens(DF, ...) \
        String_CountTokensAloneInOneLine(DataFile_GetFileContent(DF),__VA_ARGS__)
        //String_CountTokens(DataFile_GetFileContent(DF),__VA_ARGS__)
        
#define DataFile_CountMainTokens(DF, ...) \
        String_CountTokensAloneInOneLine(DataFile_GetFileContent(DF),__VA_ARGS__)
        
#define DataFile_CountNbOfKeyWords(DF, ...) \
        String_CountTokens(DataFile_GetFileContent(DF),__VA_ARGS__)
        
        

#define DataFile_MaxLengthOfFileName    (TextFile_MaxLengthOfFileName)
#define DataFile_MaxLengthOfTextLine    (TextFile_MaxLengthOfTextLine)
#define DataFile_MaxLengthOfKeyWord     (30)

#define DataFile_MaxNbOfKeyWords        (10)
#define DataFile_MaxLengthOfKeyWords    (DataFile_MaxNbOfKeyWords*DataFile_MaxLengthOfKeyWord)



#define DataFile_GetTextFile(DF)              ((DF)->GetTextFile())
#define DataFile_GetTextLine(DF)              ((DF)->GetTextLine())
#define DataFile_GetInitialization(DF)        ((DF)->GetInitialization())
#define DataFile_GetMaxLengthOfTextLine(DF)   ((DF)->GetMaxLengthOfTextLine())
#define DataFile_GetParent(DF)                ((DF)->GetParent())

#define DataFile_SetTextFile(DF,A)              ((DF)->SetTextFile(A))
#define DataFile_SetTextLine(DF,A)              ((DF)->SetTextLine(A))
#define DataFile_SetInitialization(DF,A)        ((DF)->SetInitialization(A))
#define DataFile_SetMaxLengthOfTextLine(DF,A)   ((DF)->SetMaxLengthOfTextLine(A))
#define DataFile_SetParent(DF,A)                ((DF)->SetParent(A))

#define DataFile_Set(DF,...)                  ((DF)->Set(__VA_ARGS__))



#define DataFile_GetFileName(DF) \
        TextFile_GetFileName(DataFile_GetTextFile(DF))
        
#define DataFile_GetFileContent(DF) \
        TextFile_GetFileContent(DataFile_GetTextFile(DF))

#define DataFile_GetFileStream(DF) \
        TextFile_GetFileStream(DataFile_GetTextFile(DF))

#define DataFile_GetFilePosition(DF) \
        TextFile_GetFilePosition(DataFile_GetTextFile(DF))

#define DataFile_GetCurrentPositionInFileContent(DF) \
        TextFile_GetCurrentPositionInFileContent(DataFile_GetTextFile(DF))

#define DataFile_SetCurrentPositionInFileContent(DF,C) \
        TextFile_SetCurrentPositionInFileContent(DataFile_GetTextFile(DF),C) 
        

/* Test initialization */
#define DataFile_ContextIsDefaultInitialization(DF) \
        (DataFile_GetInitialization(DF) == 0)

#define DataFile_ContextIsFullInitialization \
        DataFile_ContextIsDefaultInitialization
        
#define DataFile_ContextIsPartialInitialization(DF) \
        (DataFile_GetInitialization(DF) == 1)

#define DataFile_ContextIsNoInitialization(DF) \
        (DataFile_GetInitialization(DF) == 2)

#define DataFile_ContextIsInitialization(DF) \
        (DataFile_GetInitialization(DF) < 2)
        
        
/* Set initialization */
#define DataFile_ContextSetToFullInitialization(DF)    DataFile_SetInitialization(DF,0)
        
#define DataFile_ContextSetToPartialInitialization(DF) DataFile_SetInitialization(DF,1)
        
#define DataFile_ContextSetToNoInitialization(DF)      DataFile_SetInitialization(DF,2)
        



/* The dataset */
#define DataFile_GetDataSet(DF) \
        ((DataSet_t*) DataFile_GetParent(DF))


/* The sequential index */
#define DataFile_GetSequentialIndex(DF) \
        DataSet_GetSequentialIndex(DataFile_GetDataSet(DF))


#define DataFile_GetNbOfSequences(DF) \
        DataSet_GetNbOfSequences(DataFile_GetDataSet(DF))

        
#include <string>

struct DataFile_t {
  TextFile_t* _textfile ;      /* Text file */
  char* _line ;                /* memory space for a line */
  int   _initialization ;
  size_t   _linelength ;          /* Length of the longest line */
  void* _parent ;

  TextFile_t* GetTextFile(){return _textfile ;}
  char* GetTextLine(){return _line ;}
  int   GetInitialization(){return _initialization ;}
  size_t  GetMaxLengthOfTextLine(){return _linelength ;}
  void* GetParent(){return _parent ;}

  void SetTextFile( TextFile_t* a){_textfile = a;}
  void SetTextLine(char* a){_line = a;}
  void SetInitialization(int const& a){_initialization = a;}
  void SetMaxLengthOfTextLine(size_t const& a){_linelength = a;}
  void SetParent(void* a){_parent = a;}

  void Set(std::string const&);
} ;


#include "TextFile.h"

  inline void DataFile_t::Set(std::string const& filestr){
    TextFile_t* tf = GetTextFile();

    TextFile_Set(tf,filestr.c_str());
  }


#ifdef __CPLUSPLUS
}
#endif

/* For the macros */
//#include <stdio.h>
#include "DataSet.h"
#include "TextFile.h"
#include "String_.h"
#endif
