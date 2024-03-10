#pragma once
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <string>

class Vec3{
    public:
        double x;
        double y;
        double z;
        
        //Campo opcional para saber si el vec3 tiene algun problema o no deberia existir
        //Es de uso externo al Vec3
        bool nulo=false;
        
        Vec3(double x=0, double y=0, double z=0){
            this->x = x;
            this->y = y;
            this->z = z;
        }

        double getX(){
            return x;
        }
        double getY(){
            return y;
        }
        double getZ(){
            return z;
        }

        Vec3 setX(double x){
            this->x=x;
            return *this;
        }
        Vec3 setY(double y){
            this->y=y;
            return *this;
        }
        Vec3 setZ(double z){
            this->z=z;
            return *this;
        }

        Vec3 operator+ (int a){
            return Vec3(this->x+a, this->y+a, this->z+a);
        }

        Vec3 operator+ (double a){
            return Vec3(this->x+a, this->y+a, this->z+a);
        }

        Vec3 operator+ (Vec3 v){
            return add(v);
        }

        Vec3 operator- (Vec3 v){
            return sub(v);
        }

        Vec3 operator* (double k){
            return mult_const(k);
        }

        //accion default para multiplicar vectores es dot product
        double operator* (Vec3 v){
            return mult_escalar(v);
        }

        double len(){
            return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
        }

        double distancia(Vec3 v){
            return sqrt(pow(x-v.x,2) + pow(y-v.y,2) + pow(z-v.z,2));
        }
        
        Vec3 norm(){
            Vec3 ret(x,y,z);
            double len = ret.len();
            if(len!=0){
                ret.x = ret.x/len;
                ret.y = ret.y/len;
                ret.z = ret.z/len;
            }
            return ret;
        }
        Vec3 _norm(){
            double len = this->len();
            x = x/len;
            y = y/len;
            z = z/len;
            return *this;
        }

        Vec3 opuesto(){
            return Vec3(-x, -y, -z);
        }
        Vec3 _opuesto(){
            x=-x;
            y=-y;
            z=-z;
            return *this;
        }

        Vec3 add(Vec3 v){
            return Vec3(x+v.x, y+v.y, z+v.z);
        }
        Vec3 _add(Vec3 v){
            x+=v.x;
            y+=v.y;
            z+=v.z;
            return *this;
        }
        
        Vec3 sub(Vec3 v){
            return Vec3(x-v.x, y-v.y, z-v.z);
        }
        Vec3 _sub(Vec3 v){
            x-=v.x;
            y-=v.y;
            z-=v.z;
            return *this;
        }
        
        Vec3 mult_const(double k){
            return Vec3(x*k, y*k, z*k);
        }
        Vec3 _mult_const(double k){
            x*=k;
            y*=k;
            z*=k;
            return *this;
        }

        //"dot product"
        double mult_escalar(Vec3 v){
            return x*v.x + y*v.y + z*v.z;
        }

        Vec3 mult_vectorial(Vec3 v){
            return Vec3(this->y*v.z - this->z*v.y, this->z*v.x - this->x*v.z, this->x*v.y - this->y*v.x);
        }

        /**
         * interpolacion lineal entre this y el primer parametro, el segundo parametro(0<=t<=1) determina el punto de la terpolacion
         * 
         * @param v Extremo de la interpolacion
         * @param t Punto de la interpolacion, si t=0 se retorna this, si t=1 se retorna v (primer parametro)
        */
        Vec3 lerp(Vec3 v, double t){
            return Vec3(
                this->x*(1-t) + v.x *t, 
                this->y*(1-t) + v.y *t, 
                this->z*(1-t) + v.z *t
            );
        }

        Vec3 rot_x(double angulo){
            return Vec3(
                this->x,
                this->y*cos(angulo)-this->z*sin(angulo),
                this->y*sin(angulo)+this->z*cos(angulo)
            );
        }

        Vec3 rot_y(double angulo){
            return Vec3(
                this->x*cos(angulo)+this->z*sin(angulo),
                this->y,
                -this->x*sin(angulo)+this->z*cos(angulo)
            );
        }

        Vec3 rot_z(double angulo){
            return Vec3(
                this->x*cos(angulo)-this->y*sin(angulo),
                this->x*sin(angulo)+this->y*cos(angulo),
                this->z
            );
        }

        std::string to_string(){
            return "("+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+")";
        }

        void print(){
            std::cout<<this->to_string()<<std::endl;
        }

};