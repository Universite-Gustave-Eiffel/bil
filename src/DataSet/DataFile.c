#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "DataFile.h"
#include "Message.h"
#include "String_.h"
#include "Mry.h"




DataFile_t*  (DataFile_New)(char const* filename)
{
  DataFile_t* datafile = (DataFile_t*) Mry_New(DataFile_t) ;
  
  /* Memory space for textfile */
  {
    TextFile_t* textfile = TextFile_New(filename) ;
    
    DataFile_SetTextFile(datafile,textfile) ;
  }
  
  
  /* Memory space for line */
  {
    size_t n = DataFile_MaxLengthOfTextLine ;
    
    if(filename) {
      TextFile_t* textfile = DataFile_GetTextFile(datafile) ;
      size_t n1 = TextFile_CountTheMaxNbOfCharactersPerLine(textfile) ;

      if(n1 > n) n = n1;
    }
    
    DataFile_SetMaxLengthOfTextLine(datafile,n) ;
    
    {
      char* line = (char*) Mry_New(char,n+1) ;
    
      DataFile_SetTextLine(datafile,line) ;
    }
  }
  
  
  /* The datafile content */
  {
    TextFile_t* textfile = DataFile_GetTextFile(datafile) ;
    
    TextFile_StoreFileContent(textfile) ;
  }
  
  return(datafile) ;
}



void (DataFile_Delete)(void* self)
{
  DataFile_t* datafile = (DataFile_t*) self ;
  
  {
    TextFile_t* textfile = DataFile_GetTextFile(datafile) ;
    
    if(textfile) {
      TextFile_Delete(textfile) ;
      Mry_Free(textfile) ;
      DataFile_SetTextFile(datafile,NULL) ;
    }
  }
  
  {
    char* line = DataFile_GetTextLine(datafile) ;
    
    if(line) {
      Mry_Free(line) ;
      DataFile_SetTextLine(datafile,NULL) ;
    }
  }
}



char* (DataFile_ReadLineFromCurrentFilePositionInString)(DataFile_t* datafile)
/** Reads the first non-commented line from the datafile at the current position of its string.
 *  Return a pointer to the string line if succeeded or stop if failed. */
{
  char* line = DataFile_GetTextLine(datafile) ;
  TextFile_t* textfile = DataFile_GetTextFile(datafile) ;
  size_t n = DataFile_GetMaxLengthOfTextLine(datafile) ;
  char* c ;
  
  do {
    
    c = TextFile_ReadLineFromCurrentFilePositionInString(textfile,line,n) ;
      
  } while((c) && (c[0] == '#')) ;

  return(c) ;
}






size_t*  DataFile_ReadInversePermutationOfNodes(DataFile_t* datafile,size_t n_no)
/** Read the inverse permutation vector of nodes in a file if it exists 
 *  or initialize it with identity function. Return a pointer to n_no int. 
 **/
{
  size_t* perm = (size_t*) Mry_New(size_t,n_no) ;

  {
    char   nom_iperm[DataFile_MaxLengthOfFileName] ;
    
    {
      char*  filename = DataFile_GetFileName(datafile) ;
    
      if(strlen(filename) + 12 > DataFile_MaxLengthOfFileName) {
        arret("DataFile_ReadInversePermutationOfNodes") ;
      }
    
      sprintf(nom_iperm,"%s.graph.iperm",filename) ;
    }
  
    {
      FILE* fic_iperm = fopen(nom_iperm,"r") ;
  
      if(!fic_iperm) {
    
        for(size_t i = 0 ; i < n_no ; i++) perm[i] = i ;
    
      } else {
    
        for(size_t i = 0 ; i < n_no ; i++) {
          int   j ;
      
          fscanf(fic_iperm,"%d",&j) ;
          perm[j] = i ;
        }
      }
    
      if(fic_iperm) fclose(fic_iperm) ;
    }
  }
  
  return(perm) ;
}
