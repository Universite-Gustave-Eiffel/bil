#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Message.h"
#include "OutputFile.h"
#include "TextFile.h"
#include "Mry.h"


char   OutputFile_TypeOfCurrentFile ;



OutputFile_t*   (OutputFile_Create)(char* filename)
{
  OutputFile_t* outputfile = (OutputFile_t*) Mry_New(OutputFile_t) ;
  
  {
    TextFile_t* textfile = TextFile_New(filename) ;

    OutputFile_GetTextFile(outputfile) = textfile ;
  }
    
  return(outputfile) ;
}



void   (OutputFile_Delete)(void* self)
{
  OutputFile_t* outputfile = (OutputFile_t*) self ;
  
  {
    TextFile_t* textfile = OutputFile_GetTextFile(outputfile) ;
    
    TextFile_Delete(textfile) ;
    Mry_Free(textfile) ;
  }
}
