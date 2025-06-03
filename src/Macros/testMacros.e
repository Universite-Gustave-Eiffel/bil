#include "Algos.h"
#include "Utils.h"
#include "Codes.h"



#if 0
To test the macros below, copy and execute the line:
"gcc -x c++ -std=c++17 -iprefix ../../ -iwithprefix . -iprefix ../ -iwithprefix . -iwithprefix DataSet -iwithprefix Outputs -iwithprefix Solver -iwithprefix Tools -iwithprefix Models -iwithprefix Modules -iwithprefix Help -iwithprefix Common -iwithprefix Main -iwithprefix Macros -E testMacros.e"
#endif



Test Macros
-----------


#if 1

Test Algos:

#define T         (A,b,C,d,E,f,G,h,I,j)
#define F(a,b)    Utils_CAT3(a,_,b)
"T                  " = T
"F(a,b)               = Utils_CAT3(a,_,b)"
"Algos_FOLDL(T,P_,F)" = Algos_FOLDL(T,P_,F)
#undef T
#undef F



#define T         (A,b,C,d,E,f,G,h,I,j)
#define F(a,b)    Utils_CAT3(a,_,b)
"T                  " = T
"F(a,b)               = Utils_CAT3(a,_,b)"
"Algos_FOLDR(T,_P,F)" = Algos_FOLDR(T,_P,F)
#undef T
#undef F



#define T         (A,b,C,d,E,f,G,h,I,j)
#define F(a,b)    Utils_CAT3(a,_,b)
"T                  " = T
"F(a,b)               = Utils_CAT3(a,_,b)"
"Algos_SCANL(T,U,F)" = Algos_SCANL(T,U,F)
#undef T
#undef F



#define T (1,2,3,4,5,6,7,8,9,10,11,12)
#define F(a,b)    Utils_CAT3(a,_,b)
"T                  " = T
"F(a,b)               = Utils_CAT3(a,_,b)"
"Algos_SCANR(T,U,F) " = Algos_SCANR(T,U,F)
#undef T
#undef F



#define T         (A,b,C,d,E,f,G,h,I,j)
#define F(a)      Codes_ISUPPER(a)
"T                 " = T
"F(a)                = Codes_ISUPPER(a)"
"Algos_FILTER(T,F) " = Algos_FILTER(T,F);
#undef T
#undef F



#define T         (A,b,C,d,E,f,G,h,I,j)
#define F(a)        Utils_CAT(_,a)
"T              " = T
"F(a)             = Utils_CAT(_,a)"
"Algos_MAP(T,F) " = Algos_MAP(T,F)
#undef T
#undef F


#define T         A_t ,b_t ,C_t , d_t, u E , sh f,G,h,I,j
#define F(a)        a*
"T              " = T
"F(a)             = a*"
"Tuple_SEQ(Algos_MAP(Tuple_TUPLE(T),F)) " = Tuple_SEQ(Algos_MAP(Tuple_TUPLE(T),F))
#undef T
#undef F

#endif
#if 0



#define T1         (1,5,9,6,8,0,4)
#define T2         (10,50,90,60,80,0,40)
#define F(a,b)      Arith_ADD(a,b)
"T1             " = T1
"T2             " = T2
"F(a,b)           = Arith_ADD(a,b)"
"Algos_MAP2(T1,T2,F)" = Algos_MAP2(T1,T2,F)
#undef T1
#undef T2
#undef F



#define T1        (A,b,C,d,E,f,G,h,I,j,K,l,M,n,O,p)
#define T2        (1,2,3,4,5,6,7,8,9,10,11,12)
#define F(a,b)    Utils_CAT3(a,_,b)
"T1                 " = T1
"T2                 " = T2
"F(a,b)               = Utils_CAT3(a,_,b)"
"Algos_MAP2(T1,T2,F)" = Algos_MAP2(T1,T2,F)
#undef T1
#undef T2
#undef F



#define T1         (A,b,C,d,E,f,G,h,I,j)
#define T2         (1,2,3,4,5,6,7,8,9,10,11,12)
#define T3         (aA,bB,cC,dD)
#define F(a,b,c)   Utils_CAT3(a,b,c)
"T1                    " = T1
"T2                    " = T2
"T3                    " = T3
"F(a,b,c)                = Utils_CAT3(a,b,c)"
"Algos_MAP3(T1,T2,T3,F)" = Algos_MAP3(T1,T2,T3,F)
#undef T1
#undef T2
#undef T3
#undef F



#define T         (A,b,C,d,E,f,G,h,I,j)
"T                 " = T
"Algos_SEP(T)      " = Algos_SEP(T);
"Algos_SEPWITH(T, )" = Algos_SEPWITH(T, )
#undef T



#define T           (aA,bB,cC,dD)
"T                 " = T
"Algos_SEPWITH(T,@)" = Algos_SEPWITH(T,@)
#undef T




#include "TypeId.h"

Test TypeId:

#define T long,double
#define F(a,b)    Utils_CAT3(a,_,b)
"T          " = T
"F(a,b)       = Utils_CAT3(a,_,b)"
"Utils_CAT(TypeId,Algos_FOLDL(Tuple_TUPLE(T),,F))" = Utils_CAT(TypeId,Algos_FOLDL(Tuple_TUPLE(T),,F))




"TypeId_Type(TypeId_undefined_c) " = TypeId_Type(TypeId_undefined_c)
"TypeId_Type(TypeId_char_c) " = TypeId_Type(TypeId_char_c)
"TypeId_Type(TypeId_long_double_c) " = TypeId_Type(TypeId_long_double_c)
"TypeId_Type(TypeId_Model_t_c) " = TypeId_Type(TypeId_Model_t_c)
"TypeId_Type(TypeId_ObVal_t_c) " = TypeId_Type(TypeId_ObVal_t_c)
"TypeId_Type(TypeId_Options_t_c) " = TypeId_Type(TypeId_Options_t_c)
"TypeId_Type(TypeId_OutputFile_t_c) " = TypeId_Type(TypeId_OutputFile_t_c)
"TypeId_Type(TypeId_OutputFiles_t_c) " = TypeId_Type(TypeId_OutputFiles_t_c)
"TypeId_Type(TypeId_Views_t_c) " = TypeId_Type(TypeId_Views_t_c)
"TypeId_Type(83) " = TypeId_Type(83)
#endif
