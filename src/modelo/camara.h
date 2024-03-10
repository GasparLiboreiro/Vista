#pragma once
#include "vec3.h"
#include "quat.h"
#include "matriz.h"




#include <iostream>

class Camara{
    public:
        Vec3 pos;
        Quat direccion;
        Quat vangular;
        double dist = 2;            //distancia de la camara a su origen
        double fov = 3.1415926/2;   //angulo en radianes representando la aplitud del campo de vision
        double np = 0.05;            //distancia de la camara al punto mas cercano renderizable (near plane)
        double fp = 10000;          //distancia de la camara al punto mas lejano renderizable  (far plane)

        
        Camara _girar(Vec3 eje, double a){
            Quat rot(cos(a), eje.norm()*sin(a));
            direccion=rot*direccion;
            return *this;
        }

        Vec3 getPov(){
            return Matriz::columna_a_vec3( Matriz::get_rotacion_quaternion(direccion)*Matriz::get_traslacion(Vec3(0,0,dist))*Matriz::vec3_a_columna(pos) );
        }

        Matriz getTransformacion(){
            return Matriz::get_traslacion(pos)*Matriz::get_rotacion_quaternion(direccion)*Matriz::get_traslacion(Vec3(0,0,dist));
        }
        // incluye la transofrmacion de un punto a un punto en camera space antes de la transformacion de camera space a image space
        Matriz getCameraTransform(){
            return Matriz::get_view_transform(fov, np, fp) * getTransformacion().inversa();
        }


        Matriz getViewTransform(){
            return Matriz::get_view_transform(fov, np, fp);
        }

        Camara(Vec3 _pos, Quat _direccion = Quat()):pos(_pos), direccion(_direccion){}
};