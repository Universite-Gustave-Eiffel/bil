#ifndef POINTS_H
#define POINTS_H


/* Forward declarations */
struct Points_t; //typedef struct Points_t       Points_t ;
struct DataFile_t;
struct Mesh_t;
struct Point_t;


extern Points_t*  (Points_New)     (void) ;
extern Points_t*  (Points_Create)  (DataFile_t*,Mesh_t*) ;
extern void       (Points_Scan)    (Points_t*,DataFile_t*,Mesh_t*);
extern void       (Points_Delete)  (void*) ;


#define Points_MaxNbOfPoints             (1000)


#define Points_GetNbOfPoints(PTS)    ((PTS)->GetNbOfPoints())
#define Points_GetPoint(PTS)         ((PTS)->GetPoint())

#define Points_SetNbOfPoints(PTS,A)  ((PTS)->SetNbOfPoints(A))
#define Points_SetPoint(PTS,A)       ((PTS)->SetPoint(A))

#define Points_EmplaceBack(PTS,...)  ((PTS)->EmplaceBack(__VA_ARGS__))



#include <vector>
#include <string>
#include <optional>

struct Points_t {
  private:
  size_t _n_points ;
  Point_t*  _point ;

  public:
  size_t GetCapacity(){return Points_MaxNbOfPoints;}
  void EmplaceBack(const std::vector<double>& coor,std::optional<std::string> const& region = std::nullopt){
    if(region) {
      EmplaceBack(coor.data(),region->c_str());
    } else {
      EmplaceBack(coor.data());
    }
  }
  void EmplaceBack(double const*,char const* = nullptr);
  
  size_t GetNbOfPoints(){return _n_points ;}
  Point_t*  GetPoint(){return _point ;}

  void SetNbOfPoints(size_t a){_n_points = a ;}
  void SetPoint(Point_t* a){_point = a ;}
} ;


#include "Point.h"
  
  inline void Points_t::EmplaceBack(double const* coor,char const* region){
    Point_t* point = _point + _n_points;
    
    if(_n_points >= GetCapacity()) {
      throw std::length_error("Maximum number of points reached");
    }

    Point_Set(point,coor,region);
    _n_points++;
  }


#endif
