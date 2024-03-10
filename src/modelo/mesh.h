#pragma once
#include "vec3.h"
#include "math.h"
#include <string>
#include <vector>
#include <iostream>


class Tri{
    public:
        int a=0, b=0, c=0;

        bool clipped = false;

        Tri(int _a, int _b, int _c):a(_a),b(_b),c(_c){}

        Tri add(int amm){
            return Tri(a+amm, b+amm, c+amm);
        }

        std::string to_string(){
            return "[ "+std::to_string(a)+" , "+std::to_string(b)+" , "+std::to_string(c)+" ]";
        }

};



/**
 * Meshes almacenan la informacion necesaria para recrear la forma del objeto deseado;
 * Esta informacion esta planteada como que el objeto este centrado en (0,0,0). 
 * Que se mueva y haga cosas se encargan las entidades
*/
class Mesh{
    public:
        std::vector<Vec3> vertices;
        std::vector<Tri> tris;

        Mesh(std::vector<Vec3> _vertices, std::vector<Tri> _tris):vertices(_vertices), tris(_tris) {} //inicializador basico

        //debuelve una mesh de un solo triangulo
        static Mesh getTriangulo(Vec3 v1, Vec3 v2, Vec3 v3){
            std::vector<Vec3> vertices;
            std::vector<Tri> tris;
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            tris.push_back(Tri(0,1,2));
            return Mesh(vertices, tris);
        }

        static Mesh getCubo(double tam){
            std::vector<Vec3> vertices = std::vector<Vec3>();
            std::vector<Tri> tris = std::vector<Tri>();
            for(int i=0; i<2; i++)
                for(int j=0; j<2; j++)
                    for(int k=0; k<2; k++)
                        vertices.push_back(Vec3(tam/2*pow(-1,i), tam/2*pow(-1,j), tam/2*pow(-1,k)));  

            tris.push_back(Tri(0, 1, 2));
            tris.push_back(Tri(3, 1, 2));

            tris.push_back(Tri(1, 3, 5));
            tris.push_back(Tri(7, 3, 5));

            tris.push_back(Tri(7, 3, 6));
            tris.push_back(Tri(2, 3, 6));

            tris.push_back(Tri(2, 4, 6));
            tris.push_back(Tri(2, 4, 0));
            
            tris.push_back(Tri(1, 4, 0));
            tris.push_back(Tri(1, 4, 5));
            
            tris.push_back(Tri(6, 4, 5));
            tris.push_back(Tri(6, 7, 5));

            return Mesh(vertices, tris);
        }
};
