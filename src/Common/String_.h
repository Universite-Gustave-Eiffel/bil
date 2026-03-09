#ifndef STRING_H
#define STRING_H



#define String_FindToken  String_t::FindToken
#define String_FindAndSkipToken  String_t::FindAndSkipToken
#define String_FindNthToken  String_t::FindNthToken
#define String_CountTokens  String_t::CountTokens
#define String_CountTokensAloneInOneLine  String_t::CountTokensAloneInOneLine
#define String_CopyLine  String_t::CopyLine
#define String_FindPositionIndex  String_t::FindPositionIndex
#define String_RemoveComments  String_t::RemoveComments

#define String_bytes String_t::_bytes
#define String_pchar String_t::_pchar



#define String_MaxLengthOfKeyWord     (30)
#define String_MaxNbOfKeyWords        (10)
#define String_MaxLengthOfKeyWords    (String_MaxNbOfKeyWords*String_MaxLengthOfKeyWord)
#define String_MaxLengthOfLine        (500)



/* Scan string
 * ----------- */
/** Scan a string with a given format and return the nb of characters read. */
#define String_Scan(STR,...) \
        Logic_IF(Logic_GE(Arg_NARG(__VA_ARGS__),2))\
        (String_ScanN,String_Scan2)(STR,__VA_ARGS__)


#define String_ScanStringUntil(STR,KEY,END) \
        String_Scan(STR,"%*[ ]%[^" END "]",KEY)


#define String_ScanAffectedKeyphrase(STR,KEY) \
        String_Scan(STR,"%*[ ]%[^=]",KEY)


#define String_FindAndScanExp(STR,TOK,DEL, ...) \
        ((String_pchar  = String_FindAndSkipToken(STR,TOK,DEL)) ? \
        ((String_pchar  = String_FindToken(String_pchar," "))   ? \
        ((String_Scan(String_pchar,__VA_ARGS__))) : 0) : 0)


/** Reads N data from the string STR with the format "FMT" and store into V . */
#define String_ScanArray(STR,N,FMT,V) \
        do { \
          char* String_c = STR ; \
          for(size_t String_i = 0 ; String_i < (N) ; String_i++) { \
            String_c += String_Scan(String_c,FMT,(V) + String_i) ; \
          } \
        } while(0)
        
/** Reads N data for each entries from the string STR with the format "FMT". */
#define String_ScanArrays(STR,N,FMT,...) \
        do { \
          char* String_c = STR ; \
          for(size_t String_i = 0 ; String_i < (N) ; String_i++) { \
            String_c += String_Scan(String_c,FMT,String_IncrementAll(__VA_ARGS__)) ; \
          } \
        } while(0)
        
#define String_ScanArrays0(STR,N,FMT,...) \
        do { \
          char* String_c = STR ; \
          for(size_t String_i = 0 ; String_i < (N) ; String_i++) { \
            String_c += String_Scan(String_c,FMT,__VA_ARGS__) ; \
            Algos_SEPWITH(String_IncrementAll0(__VA_ARGS__),;) ; \
          } \
        } while(0)


/** Gets the advanced position in the string. */
#define String_GetAdvancedPosition \
        String_t::GetAdvancedPosition()


/* Implementation */
#define String_Scan2(STR,FMT) \
        (sscanf(STR,String_Fmt(FMT),&String_bytes) , \
         String_pchar = STR + String_bytes , \
         String_bytes)
        
#define String_ScanN(STR,FMT, ...) \
        (sscanf(STR,String_Fmt(FMT),__VA_ARGS__,&String_bytes) , \
         String_pchar = STR + String_bytes , \
         String_bytes)

#define String_Fmt(FMT)  FMT"%n"

#define String_Increment(a) \
        ((a) + String_i)
        
#define String_IncrementAll(...) \
        Tuple_SEQ(Algos_MAP(Tuple_TUPLE(__VA_ARGS__),String_Increment))

#define String_Increment0(a) \
        (a++)
        
#define String_IncrementAll0(...) \
        Algos_MAP(Tuple_TUPLE(__VA_ARGS__),String_Increment0)

//        std::strcat(strcpy(String_Save,FMT),"%n")




/* Find characters
 * --------------- */
#define String_FindChar(STR,C) \
        ((STR) ? std::strchr(STR,C) : NULL)
        

#define String_FindAnyChar(STR,Cs) \
        ((STR) ? std::strpbrk(STR,Cs) : NULL)
        

#define String_FindEndOfLine(STR) \
        String_FindChar(STR,'\n')
        

#define String_FindEndOfString(STR) \
        String_FindChar(STR,'\0')



/* Skip tokens/characters
 * ---------------------- */
#define String_SpaceChars \
        " \f\n\r\t\v"

#define String_BlankChars \
        " \t\r"


#define String_SkipAnyChars(STR,Cs) \
        ((STR) ? (STR) + std::strspn(STR,Cs) : NULL)
        

#define String_SkipAnyOtherChars(STR,Cs) \
        ((STR) ? (STR) + std::strcspn(STR,Cs) : NULL)
        

#define String_SkipBlankChars(STR) \
        String_SkipAnyChars(STR,String_BlankChars)
        

#define String_SkipNonBlankChars(STR) \
        String_SkipAnyOtherChars(STR,String_BlankChars)
        

#define String_SkipSpaceChars(STR) \
        String_SkipAnyChars(STR,String_SpaceChars)
        

#define String_SkipNonSpaceChars(STR) \
        String_SkipAnyOtherChars(STR,String_SpaceChars)
        

#define String_SkipLine(STR) \
        ((String_pchar = String_FindEndOfLine(STR)),String_SkipSpaceChars(String_pchar))


#define String_SkipNextToken(STR) \
        String_SkipNonBlankChars(String_SkipBlankChars(STR))
        



/* Compare with characters
 * ----------------------- */
#define String_Is(...) \
        Utils_CAT_NARG(String_Is,__VA_ARGS__)(__VA_ARGS__)
        
#define String_IsNot(...) \
        !String_Is(__VA_ARGS__)

#define String_CaseIgnoredIs(...) \
        Utils_CAT_NARG(String_CaseIgnoredIs,__VA_ARGS__)(__VA_ARGS__)
        
#define String_BeginsWithAnyChar(STR,Cs) \
        ((STR) ? std::strspn(STR,Cs) : 0)

#define String_BeginsWithSingleLineComment(STR) \
        (String_Is(STR,"#",1) || String_Is(STR,"//",2))

#define String_BeginsWithMultiLineComment(STR) \
        String_Is(STR,"/*",2)

#define String_SkipMultiLineComment(STR) \
        String_FindAndSkipToken(STR,"*/")


/* Implementation */
#define String_Is2(STR,...) \
        ((STR) ? (!std::strcmp(STR,__VA_ARGS__)) : 0)
        
#define String_Is3(STR,...) \
        ((STR) ? (!std::strncmp(STR,__VA_ARGS__)) : 0)
        
#define String_CaseIgnoredIs2(STR,...) \
        ((STR) ? (!strcasecmp(STR,__VA_ARGS__)) : 0)
        
#define String_CaseIgnoredIs3(STR,...) \
        ((STR) ? (!strncasecmp(STR,__VA_ARGS__)) : 0)




#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <cstring>
#include <stdarg.h>

#include "Arg.h"
#include "Tuple.h"
#include "Algos.h"
#include "Logic.h"
#include "Utils.h"


struct String_t {
  /* inline specifier is used to define the variables 
   * otherside they would have been declared only. */
  private:
  inline static char* _tokens[String_MaxNbOfKeyWords];
  inline static char  _save[String_MaxLengthOfKeyWords];
  inline static char  _line[String_MaxLengthOfLine];
  
  public:
  inline static int   _bytes;
  inline static char* _pchar;

  private:
  static char** BreakIntoTokens(const char* str,const char* del){
    assert(strlen(str) < String_MaxLengthOfKeyWords) ;
    
    strcpy(_save,str) ;
    
    if(!(_tokens[0] = strtok(_save,del))) return(NULL) ;
    
    {
      int ntok = 1 ;
    
      while((_tokens[ntok] = strtok(NULL,del))) {
        ntok++ ;

        assert(ntok < String_MaxNbOfKeyWords) ;
      }
    }
    
    return(_tokens) ;
  }
  
  static int NbOfTokens(char const* const* tok){    
    int ntok = 0 ;
    
    if(tok) {
      while(tok[ntok]) ntok++ ;
    }
    
    return(ntok) ;
  }

  public:
  static char* GetAdvancedPosition(void){return _pchar;}
        
  static char* FindToken(char* str,const char* cle){
    return((str) ? std::strstr(str,cle) : NULL);
  }

  static char const* FindToken(char const* str,const char* cle){
    return((str) ? std::strstr(str,cle) : NULL);
  }

  static char* FindToken(char* str,const char* cle,const char* del){
    char** tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    char* c = FindToken(str,tok[0]) ;
  
    if(ntok > 1) {
      for(int itok = 1 ; itok < ntok ; itok++) {
        char* c0 = FindToken(str,tok[itok]) ;
  
        if(!c) c = c0 ;
  
        if(c0 && (c0 < c)) c = c0 ;
      }
    }
    
    return(c) ;
  }
  
  static char const* FindToken(char const* str,const char* cle,const char* del){
    char** tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    char const* c = FindToken(str,tok[0]) ;
  
    if(ntok > 1) {
      for(int itok = 1 ; itok < ntok ; itok++) {
        char const* c0 = FindToken(str,tok[itok]) ;
  
        if(!c) c = c0 ;
  
        if(c0 && (c0 < c)) c = c0 ;
      }
    }
    
    return(c) ;
  }

  static char* FindAndSkipToken(char* str,const char* cle){
    char* c = FindToken(str,cle);
    
    if(c) {
      c += std::strlen(cle);
    }
          
    return(c);
  }
  
  static char const* FindAndSkipToken(char const* str,const char* cle){
    char const* c = FindToken(str,cle);
    
    if(c) {
      c += std::strlen(cle);
    }
          
    return(c);
  }

  static char* FindAndSkipToken(char* str,const char* cle,const char* del){
    char**    tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    char* c  = FindToken(str,tok[0]) ;
    char* c1 = FindAndSkipToken(c,tok[0]) ;
  
    if(ntok > 1) {
      for(int itok = 1 ; itok < ntok ; itok++) {
        char* c0 = FindToken(str,tok[itok]) ;
  
        if(!c) {
          c = c0 ;
          c1 = FindAndSkipToken(c,tok[itok]) ;
        }
  
        if(c0 && (c0 < c)) {
          c = c0 ;
          c1 = FindAndSkipToken(c,tok[itok]) ;
        }
  
        /* In case of same location select the longest token */
        if(c0 && (c0 == c)) {
          char* c2 = FindAndSkipToken(c,tok[itok]) ;
          
          if(c2 > c1) c1 = c2 ;
        }
      }
    }
    
    return(c1) ;
  }
  
  static char const* FindAndSkipToken(char const* str,const char* cle,const char* del){
    char**    tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    char const* c  = FindToken(str,tok[0]) ;
    char const* c1 = FindAndSkipToken(c,tok[0]) ;
  
    if(ntok > 1) {
      for(int itok = 1 ; itok < ntok ; itok++) {
        char const* c0 = FindToken(str,tok[itok]) ;
  
        if(!c) {
          c = c0 ;
          c1 = FindAndSkipToken(c,tok[itok]) ;
        }
  
        if(c0 && (c0 < c)) {
          c = c0 ;
          c1 = FindAndSkipToken(c,tok[itok]) ;
        }
  
        /* In case of same location select the longest token */
        if(c0 && (c0 == c)) {
          char const* c2 = FindAndSkipToken(c,tok[itok]) ;
          
          if(c2 > c1) c1 = c2 ;
        }
      }
    }
    
    return(c1) ;
  }

  static char* FindNthToken(char* str,const char* tok,const int n){
    char* c = FindToken(str,tok) ;
    int i = 1 ;
      
    while(c && i++ < n) {
      c = FindAndSkipToken(c,tok) ;
      if(c) c = FindToken(c,tok) ;
    }
    
    return(c) ;
  }
  
  static char* FindNthToken(char* str,const char* cle,const char* del,const int n){
    char**    tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    char*  line = NULL ;
    
    {
      int itok = 0 ;
      
      while((itok < ntok) && !(line = FindNthToken(str,tok[itok],n))) itok++ ;
    }
    
    return(line) ;
  }
  
  static int CountTokens(char* str,const char* tok){
    char* c = str ;
    int i = 0 ;
  
    while((c = FindAndSkipToken(c,tok))) i++ ;
  
    return(i) ;
  }
  
  static int CountTokens(char* str,const char* cle,const char* del){
    char** tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    int     n = 0 ;
    
    /* Compute the nb of times we find the tokens in string */
    {
      int itok ;
      
      for(itok = 0 ; itok < ntok ; itok++) {
        n += CountTokens(str,tok[itok]) ;
      }
    }
    
    return(n) ;
  }
  
  static int CountTokensAloneInOneLine(char* str,const char* tok){
    char* c = str ;
    int i = 0 ;
  
    while((c = FindAndSkipToken(c,tok)) && (String_SkipBlankChars(c)[0] == '\n')) i++ ;
  
    return(i) ;
  }

  static int CountTokensAloneInOneLine(char* str,const char* cle,const char* del){
    char**    tok  = BreakIntoTokens(cle,del) ;
    int ntok = NbOfTokens(tok) ;
    int     n = 0 ;
    
    /* Compute the nb of times we find the tokens in string */
    {
      for(int itok = 0 ; itok < ntok ; itok++) {
        n += CountTokensAloneInOneLine(str,tok[itok]) ;
      }
    }
    
    return(n) ;
  }
  
  static char* CopyLine(const char* str){
    size_t   len = (str) ? strlen(str) : 0 ;
    char const* eol = String_FindEndOfLine(str) ;
    size_t   n   = (eol) ? (size_t) (eol - str) : len ;
  
    assert(n < String_MaxLengthOfLine) ;
    
    strncpy(_line,str,n) ;
    _line[n] = '\0' ;
    
    return(_line) ;
  }

  static int FindPositionIndex(const char* str,const char* const* ss,const int n){
  /** Return the position index in ss whose name is pointed to by str 
   *  or -1 if it fails. */
    int i ;
  
    if(isdigit(str[0])) {
      
      i  = atoi(str) - 1 ;
      
    } else {
      
      for(i = 0 ; i < n ; i++) {
        if(String_Is(str,ss[i])) break ;
      }
      
      if(i == n) i = -1 ;
    }
  
    return(i) ;
  }
  
  static char*   RemoveComments(char const* src,char* dest){
  /**  Remove the comments from src and copy it to dest.
   *   The pointer dest should point to an allocated space 
   *   large enough to accomodate the uncommented src. 
   *   We can give the same pointer, i.e. dest = src,
   *   in that case the string src will be modified.
   *   Return the pointer dest. */
    char const* cin = src ;
    char* cou = dest ;
  
    if(src) {
      while(cin[0]) {
      
        if(String_BeginsWithSingleLineComment(cin)) {
        
          cin = String_FindEndOfLine(cin) ;
        
        } else if(String_BeginsWithMultiLineComment(cin)) {
          char const* c = String_SkipMultiLineComment(cin) ;
        
          assert(c) ;
          
          cin = c ;
        }
      
        *cou++ = *cin++ ;
      }
    
      cou[0] = 0 ;
    }
    
    return dest ;
  }
} ;


  
#if 0
  #include "TextFile.h"
  
  static int String_Test(int, char**) ;
  
  int String_Test(int argc, char** argv)
  {
    char* filename = argv[1] ;
    TextFile_t* textfile = TextFile_Create(filename) ;
    //char* str = String_Create(filename) ;
    char* str = TextFile_StoreFileContent(textfile) ;
    
    if(argc > 2) {
      char* tok = argv[2] ;
      int n ;
      char* c ;
      
      /* Test CountTokens */
      {
        n = CountTokens(str,tok) ;
      
        printf("nb of tokens = %d\n",n) ;
      }
    
      /* Test FindToken */
      {
        c = FindToken(str,tok) ;
      
        printf("token = %s\n%s\n",tok,c) ;
      }
    
      /* Test FindNthToken */
      {
        int j ;
        
        for(j = 0 ; j < n ; j++) {
          c = FindNthToken(str,tok,j+1) ;
      
          printf("token %d = %s\n%s\n",j,tok,c) ;
        }
      }
    }
      
    
    if(argc > 1) {
      /* Test RemoveComments */
      {
        printf("File content before removing the comments\n") ;
        printf("%s",str) ;
        
        RemoveComments(str,str) ;
        
        printf("File content after removing the comments\n") ;
        printf("%s",str) ;
      }
    }
  
    
    return(0) ;
  }
  
  
  
  int main(int argc, char** argv)
  {
    return(String_Test(argc,argv)) ;
  }
#endif
#endif
