#pragma once
#include <SDL2/SDL_rect.h>
#include "vec3.h"

// se incluyen los puntos que cumplan:  offset.x  <=  x  <  offset.x + tam.x   &&   offset.y  <=  y  <  offset.y + tam.y
class Box{
    public:

        Vec3 tam, offset;

        Box(Vec3 _tam=Vec3(), Vec3 _offset=Vec3()):tam(_tam), offset(_offset) {}
        Box(double w=0, double h=0, double d=0, double x=0, double y=0, double z=0):tam(Vec3(w,h,d)), offset(Vec3(x,y,z)) {}
        
        bool inBox(Vec3 pos){
            return  ((pos.x>=offset.x && pos.x<offset.x+tam.x) || tam.x==0) && 
                    ((pos.y>=offset.y && pos.y<offset.y+tam.y) || tam.y==0) && 
                    ((pos.z>=offset.z && pos.z<offset.z+tam.z) || tam.z==0);
        }
        bool inBox(int x=0, int y=0, int z=0){
            return  ((    x>=offset.x &&     x<offset.x+tam.x) || tam.x==0) && 
                    ((    y>=offset.y &&     y<offset.y+tam.y) || tam.y==0) && 
                    ((    z>=offset.z &&     z<offset.z+tam.z) || tam.z==0);
        }

        Vec3 punto2DMasCercano(Vec3 pos){
            if(inBox(pos)){
                return pos;
            } else {
                if(pos.x<offset.x){
                    pos.x=offset.x;
                } else if(pos.x>=offset.x + tam.x){
                    pos.x=offset.x + tam.x - 1;
                }
                if(pos.y<offset.y){
                    pos.y=offset.y;
                } else if(pos.y>=offset.y + tam.y){
                    pos.y=offset.y + tam.y - 1;
                }
                return pos;
            }
        }

        // de Box a SDL_Rect, tener en cuenta que un SDL_Rect es unicamente 2D
        SDL_Rect SDLparse(){
            return SDL_Rect{
                (int)offset.x,
                (int)offset.y,
                (int)tam.x,
                (int)tam.y
            };
        }

        std::string to_string(){
            return "pos:"+offset.to_string()+" tam:"+tam.to_string();
        }

};