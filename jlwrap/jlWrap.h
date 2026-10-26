#ifndef JLWRAP_H
#define JLWRAP_H

#define MIRROR(T) \
        template<> struct IsMirroredType<T> : std::false_type {}

#define ADD_TYPE(T)      mod.add_type<T>(#T)

/* Definitions of overloaded functions in julia's world */
#define NBRS        (1,2,3,4,5,6,7,8,9)
#define ARG(A,B)    Utils_CAT(arg,B)
#define PARAM(A,B)  A ARG(A,B)
#define PARAMS(...) Tuple_SEQ(Algos_MAP2(Tuple_TUPLE(__VA_ARGS__),NBRS,PARAM))
#define ARGS(...)   Tuple_SEQ(Algos_MAP2(Tuple_TUPLE(__VA_ARGS__),NBRS,ARG))


/*
   Wrapper functions in julias's world are named O_F(...), after Bil's syntax:
   "O" is the object's name, and "F" is the function's name of "O", namely:
      a)  O_F(...)       for non-member function,
      b) (O_t& o).F(...) for member function
*/

#define JLNAME(O,F)  #O"_"#F

/* Non-member functions */
#define FREE_METHOD(...) \
        Logic_IF(Logic_GE(Arg_NARG(__VA_ARGS__),3))\
        (FREE_METHODN,FREE_METHOD2)(__VA_ARGS__)
#define FREE_METHODN(O,F,...) \
        mod.method(JLNAME(O,F),[](PARAMS(__VA_ARGS__)){return O##_##F(ARGS(__VA_ARGS__));})
#define FREE_METHOD2(O,F) \
        mod.method(JLNAME(O,F),[](){return O##_##F();})
        
/* Non-static member functions */
#define MEM_METHOD(...) \
        Logic_IF(Logic_GE(Arg_NARG(__VA_ARGS__),3))\
        (MEM_METHODN,MEM_METHOD2)(__VA_ARGS__)
#define MEM_METHODN(O,F,...) \
        mod.method(JLNAME(O,F),[](O##_t* o,PARAMS(__VA_ARGS__)) {return o->F(ARGS(__VA_ARGS__));})
#define MEM_METHOD2(O,F) \
        mod.method(JLNAME(O,F),[](O##_t* o) {return o->F();})
        
/* Static member functions */
#define MEM_STATICMETHOD(...) \
        Logic_IF(Logic_GE(Arg_NARG(__VA_ARGS__),3))\
        (MEM_STATICMETHODN,MEM_STATICMETHOD2)(__VA_ARGS__)
#define MEM_STATICMETHODN(O,F,...) \
        mod.method(JLNAME(O,F),[](PARAMS(__VA_ARGS__)) {return O##_t::F(ARGS(__VA_ARGS__));})
#define MEM_STATICMETHOD2(O,F) \
        mod.method(JLNAME(O,F),[]() {return O##_t::F();})

#endif

