#pragma once
#include "vec3.h"

//mi implementacion de quaternions
class Quat{
    public:
        double a;
        double b;
        double c;
        double d;

        
        Quat(double _a=1, double _b=0, double _c=0, double _d=0):   a(_a),  b(_b),  c(_c),  d(_d)   {}
        Quat(double _a, Vec3 v):                            a(_a),  b(v.x), c(v.y), d(v.z)  {}


        double getA(){
            return a;
        }

        Vec3 getV(){
            return Vec3(b,c,d);
        }



        Quat operator+ (Quat q){
            return suma(*this, q);
        }

        Quat operator- (Quat q){
            return resta(*this, q);
        }

        Quat operator* (Quat q){
            return multi(*this, q);
        }

        double len(){
            return sqrt(pow(a,2) + pow(b,2) + pow(c,2) + pow(d,2));
        }

        Quat norm(){
            double len = this->len();
            return Quat(a/len, b/len, c/len, d/len);
        }

        //retorna la inversa del cuaterion, es decir, 1/q, que a su vez es cambiarle el signo a las partes imaginarias
        Quat opuesto(){
            return Quat(a, -b, -c, -d);
        }


        Vec3 getDireccion(){
            Quat p = Quat(0,Vec3(0,0,-1)); // estandarizo como que todos los objetos vanilla empiezan mirando hacia z=-1, como la camara
            Quat ret = *this*p*opuesto();
            return ret.getV();
        }


        static Quat get_rotation_quaternion(Vec3 eje, double a){
            return Quat(cos(a/2), eje.norm()*sin(a/2));
        }




        static Quat suma(Quat q1, Quat q2){
            return Quat(q1.a + q2.a, q1.b + q2.b, q1.c + q2.c, q1.d + q2.d);
        }
        static Quat resta(Quat q1, Quat q2){
            return Quat(q1.a - q2.a, q1.b - q2.b, q1.c - q2.c, q1.d - q2.d);
        }
        static Quat multi(Quat q1, Quat q2){
            return Quat(q1.getA()*q2.getA() - q1.getV()*q2.getV(), q2.getV()*q1.getA() + q1.getV()*q2.getA() + q1.getV().mult_vectorial(q2.getV()));
        }


        std::string to_string(){
            return "("+std::to_string(a)+", "+std::to_string(b)+", "+std::to_string(c)+", "+std::to_string(d)+")";
        }
};