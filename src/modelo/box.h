#pragma once

#include "vec3.h"

class Box{
    public:
        Vec3 tam, offset;

        Box(Vec3 _tam=Vec3(), Vec3 _offset=Vec3()):tam(_tam), offset(_offset) {}
        Box(double w=0, double h=0, double d=0, double x=0, double y=0, double z=0):tam(Vec3(w,h,d)), offset(Vec3(x,y,z)) {}
        
        bool inBox(Vec3 pos){
            return  ((pos.x>=offset.x && pos.x<=offset.x+tam.x) || tam.x==0) && 
                    ((pos.y>=offset.y && pos.y<=offset.y+tam.y) || tam.y==0) && 
                    ((pos.z>=offset.z && pos.z<=offset.z+tam.z) || tam.z==0);
        }

};