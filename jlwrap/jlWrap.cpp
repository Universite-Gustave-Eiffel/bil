#include "jlcxx/jlcxx.hpp"
#include "jlcxx/stl.hpp"
#include <bil/bil.h>
#include "jlWrap.h"


namespace jlcxx {
  MIRROR(BConds_t);
  MIRROR(Context_t);
  MIRROR(DataFile_t);
  MIRROR(DataSet_t);
  MIRROR(Dates_t);
  MIRROR(Date_t);
  MIRROR(Entry_t);
  MIRROR(Fields_t);
  MIRROR(Functions_t);
  MIRROR(Geometry_t);
  MIRROR(IConds_t);
  MIRROR(IterProcess_t);
  MIRROR(Loads_t);
  MIRROR(Mesh_t);
  MIRROR(Models_t);
  MIRROR(Model_t);
  MIRROR(Materials_t);
  MIRROR(Material_t);
  MIRROR(Module_t);
  MIRROR(ObVals_t);
  MIRROR(Options_t);
  MIRROR(Points_t);
  MIRROR(PosFilesForGMSH_t);
  MIRROR(Session_t);
  MIRROR(TextFile_t);
  MIRROR(TimeStep_t);
  MIRROR(Units_t);
  //MIRROR(InternationalSystemOfUnits_t);
  
  //template<> struct DefaultConstructible<Module_t> : std::false_type {};
}
  
JLCXX_MODULE define_julia_module(jlcxx::Module& mod) {
  ADD_TYPE(BConds_t);
  ADD_TYPE(Context_t);
  ADD_TYPE(DataFile_t);
  ADD_TYPE(DataSet_t);
  ADD_TYPE(Dates_t);
  ADD_TYPE(Date_t);
  ADD_TYPE(Entry_t);
  ADD_TYPE(Fields_t);
  ADD_TYPE(Functions_t);
  ADD_TYPE(Geometry_t);
  ADD_TYPE(IConds_t);
  ADD_TYPE(IterProcess_t);
  ADD_TYPE(Loads_t);
  ADD_TYPE(Mesh_t);
  ADD_TYPE(Materials_t);
  ADD_TYPE(Material_t);
  ADD_TYPE(Models_t);
  ADD_TYPE(Model_t);
  ADD_TYPE(Module_t);
  ADD_TYPE(ObVals_t);
  ADD_TYPE(Options_t);
  ADD_TYPE(Points_t);
  ADD_TYPE(PosFilesForGMSH_t);
  ADD_TYPE(Session_t);
  ADD_TYPE(TextFile_t);
  ADD_TYPE(TimeStep_t);
  ADD_TYPE(Units_t);
  //ADD_TYPE(InternationalSystemOfUnits_t);
  
  

  /* BConds */
  MEM_METHOD(BConds,EmplaceBack,std::string const&,std::string const&,std::string const&,size_t const&,size_t const&);
  MEM_METHOD(BConds,EmplaceBack,std::string const&,std::string const&,size_t const&,size_t const&);
  
  /* DataSet */
  FREE_METHOD(DataSet,New,std::string const&,Context_t*);
  FREE_METHOD(DataSet,New,std::string const&);
  FREE_METHOD(DataSet,Delete,void*);
  FREE_METHOD(DataSet,Delete,DataSet_t*);
  FREE_METHOD(DataSet,PrintData,DataSet_t*,std::string const&);
  MEM_METHOD(DataSet,GetUnits);
  MEM_METHOD(DataSet,GetDataFile);
  MEM_METHOD(DataSet,GetGeometry);
  MEM_METHOD(DataSet,GetModule);
  MEM_METHOD(DataSet,GetMesh);
  MEM_METHOD(DataSet,GetMaterials);
  MEM_METHOD(DataSet,GetDates);
  MEM_METHOD(DataSet,GetPoints);
  MEM_METHOD(DataSet,GetIConds);
  MEM_METHOD(DataSet,GetBConds);
  MEM_METHOD(DataSet,GetLoads);
  MEM_METHOD(DataSet,GetFunctions);
  MEM_METHOD(DataSet,GetFields);
  MEM_METHOD(DataSet,GetObVals);
  MEM_METHOD(DataSet,GetModels);
  MEM_METHOD(DataSet,GetTimeStep);
  MEM_METHOD(DataSet,GetIterProcess);
  MEM_METHOD(DataSet,GetOptions);
  MEM_METHOD(DataSet,Finalize);
  
  /* Dates */
  MEM_METHOD(Dates,Set,size_t const&,double const*);
  mod.method(JLNAME(Dates,Set),[](Dates_t* o,jlcxx::ArrayRef<double> a1) {std::vector<double> b1(a1.begin(),a1.end());return o->Set(b1);});
  
  /* Fields */
  MEM_METHOD(Fields,EmplaceBack,std::string const&,double const&,double const*,double const*);
  MEM_METHOD(Fields,EmplaceBack,std::string const&,std::string const&);
  MEM_METHOD(Fields,EmplaceBack,std::string const&,double const&,double const&);
  MEM_METHOD(Fields,EmplaceBack,std::string const&,double const&);
  
  /* Functions */
  MEM_METHOD(Functions,EmplaceBack,std::string const&,size_t const&,double const*,double const*);
  MEM_METHOD(Functions,EmplaceBack,std::string const&,std::string const&);

  /* Geometry */
  MEM_METHOD(Geometry,Set,unsigned short int,std::string&);

  /* IConds */
  MEM_METHOD(IConds,EmplaceBack,std::string const&,std::string const&,size_t const&,size_t const&);
  MEM_METHOD(IConds,EmplaceBack,std::string const&,std::string const&,std::string const&,size_t const&);
  
  /* IterProcess */
  MEM_METHOD(IterProcess,Set,int const&,double const&,int const&);
  
  /* Loads */
  MEM_METHOD(Loads,EmplaceBack,std::string const&,std::string const&,std::string const&,size_t const&,size_t const&);
  
  /* Materials */
  MEM_METHOD(Materials,EmplaceBack,std::string const&);
  
  /* Material */
  MEM_METHOD(Material,Set,std::string const&,double const&);
  MEM_METHOD(Material,Set,std::string const&,std::string const&);
  MEM_METHOD(Material,Finalize);
  
  /* Mesh */
  FREE_METHOD(Mesh,Set,Mesh_t*,std::string const&);
  FREE_METHOD(Mesh,WriteInversePermutation,Mesh_t*,char const*,char const*);
  
  /* Models */
  MEM_METHOD(Models,EmplaceBack,std::string const&);
  MEM_METHOD(Models,EmplaceBack,std::string const&,std::string const&,std::string const&);
  
  /* Module */
  MEM_METHOD(Module,Set,std::string const&);
  MEM_METHOD(Module,ComputeProblem,DataSet_t*);
  
  /* ObVals */
  MEM_METHOD(ObVals,EmplaceBack,std::string const&,double const&,std::string const&,double const&);
  MEM_METHOD(ObVals,EmplaceBack,std::string const&,double const&);
  
  /* Options */
  MEM_METHOD(Options,Set,char const*);
  
  /* Points */
  MEM_METHOD(Points,EmplaceBack,double const*);
  MEM_METHOD(Points,EmplaceBack,double const*,char const*);
  
  /* PostFilesForGMSH */
  FREE_METHOD(PosFilesForGMSH,Create,DataSet_t*);
  FREE_METHOD(PosFilesForGMSH,ParsedFileFormat,PosFilesForGMSH_t*);
  FREE_METHOD(PosFilesForGMSH,ASCIIFileFormat,PosFilesForGMSH_t*);
  FREE_METHOD(PosFilesForGMSH,Delete,void*);
  FREE_METHOD(PosFilesForGMSH,Delete,PosFilesForGMSH_t*);
  
  /* Session */
  MEM_STATICMETHOD(Session,Open);
  
  /* TimeStep */
  MEM_METHOD(TimeStep,Set,double const&,double const&);
  
  /* Units */
  MEM_METHOD(Units,EmplaceBack,std::string const&,std::string const&);
}
