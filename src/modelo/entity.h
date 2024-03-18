#pragma once
#include "vec3.h"
#include "mesh.h"
#include <vector>
#include "matriz.h"

class Entity{
    public:
        Vec3 pos;
        Quat direccion;
        Quat vangular;
        Mesh mesh;
        
        Entity(Mesh _mesh, Vec3 _pos=0):mesh(_mesh), pos(_pos){} //inicializador basico
        static Entity getCubo(double size, Vec3 pos=Vec3()){
            return Entity(Mesh::getCubo(size), pos);
        }
        
        Entity _girar(Vec3 eje, double a){
            Quat rot = Quat::get_rotation_quaternion(eje, a);
            direccion=rot * direccion;
            return *this;
        }

        Matriz getTransformation(){
            Matriz out = Matriz::get_traslacion(pos) * Matriz::get_rotacion_quaternion(direccion);
            return out;
        }

};