#ifndef POINT_H
#define POINT_H


/* Forward declarations */
struct Point_t; //typedef struct Point_t        Point_t ;
struct Mesh_t;
struct Element_t;


extern Point_t*   (Point_New)                  (void) ;
extern void       (Point_Delete)               (void*) ;
extern void       (Point_EnclosingElement)     (Point_t*,Mesh_t*) ;
extern void       (Point_Scan)                 (Point_t*,char*) ;



#define Point_MaxLengthOfRegionName      Region_MaxLengthOfRegionName


#define Point_GetCoordinate(PT)                    ((PT)->GetCoordinate())
#define Point_GetEnclosingElement(PT)              ((PT)->GetEnclosingElement())
#define Point_GetRegionName(PT)                    ((PT)->GetRegionName())

#define Point_SetEnclosingElement(PT,A)            ((PT)->SetEnclosingElement(A))
#define Point_SetRegionName(PT,A)                  ((PT)->SetRegionName(A))

#define Point_Set(PT,...)                          ((PT)->Set(__VA_ARGS__))


#include <vector>
#include <string>
#include <optional>

struct Point_t {
  private:
  double _Coordinate[3] ;
  char* _RegionName ;
  Element_t* _EnclosingElement ;  /* Element inside which the point lies */

  public:
  double* GetCoordinate(){return _Coordinate ;}
  char* GetRegionName(){return _RegionName ;}
  Element_t* GetEnclosingElement(){return _EnclosingElement ;}

  void SetRegionName(char* a){_RegionName = a;}
  void SetEnclosingElement(Element_t* a){_EnclosingElement = a;}

  void Set(const std::vector<double>& coor,std::optional<std::string> const& region = std::nullopt){
    if(region) {
      Set(coor.data(),region->c_str());
    } else {
      Set(coor.data());
    }
  }
  void Set(double const* coor,char const* region = nullptr){
    /* Coordinates */
    for(int i = 0 ; i < 3 ; i++) {
      _Coordinate[i] = coor[i];
    }
  
    /* Region */
    if(region) {
      if(strlen(region) < Point_MaxLengthOfRegionName) {
        strncpy(_RegionName,region,Point_MaxLengthOfRegionName);
      } else {
        arret("Point_t::Set: too long name") ;
      }
    }
  }
} ;


#endif
