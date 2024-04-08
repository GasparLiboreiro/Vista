#pragma once
#include "vec3.h"
#include "math.h"
#include "box.h"
#include <string>
#include <vector>
#include <iostream>

class Color{
    public:

        unsigned int r, g, b, a;

        Color(unsigned int _r=255, unsigned int _g=255, unsigned int _b=255, unsigned int _a=255):r(_r),g(_g),b(_b),a(_a){
        }

        // color aleatorio
        static Color rColor(){
            return Color(rand()%255, rand()%255, rand()%255);
        }
};

/** Tri almacena la informacion necesaria para un triangulo, un detalle es que las vertices del triangulo tienen que estar puestas en sentido antihorario, para identificar hacia adonde apunta la normal del triangulo
 * @param a a la id de la primera vertice del triangulo en el array vertices
 * @param b la id de la segunda vertice del triangulo en el array vertices
 * @param c la id de la tercera vertice del triangulo en el array vertices
 * @param o origen del triangulo en la textura, tambien seria su primer vertice
 * @param t1 pos de la segunda vertice del triangulo en la textura medido desde el origen
 * @param t2 pos de la tercera vertice del triangulo en la textura medido desde el origen
*/
class Tri{
    public:

        int a=0, b=0, c=0;
        Vec3 o;
        Vec3 t1, t2;
        Color textura[64][64];
        Box texturaBox = Box(64,64);


        Tri(int _a, int _b, int _c, Vec3 _t1=Vec3(6,0), Vec3 _t2=Vec3(0,6), Vec3 _o=Vec3()):
            a(_a),
            b(_b),
            c(_c),
            t1(_t1),
            t2(_t2),
            o(_o)
        {
            
            for(int i=0; i<64; i++){
                for(int j=0; j<64; j++){
                    textura[i][j] = Color(255, 0, 220);
                }
            }
            for(int i=0; i<64; i++){
                for(int j=0; j<64; j+=2){
                    if(j==0 && ((double)i)/2 != i/2)
                        j++;
                    textura[j][i] = Color(255*0.0, 255*0.0, 255*0.0);
                }
            }
        }

        Tri add(int amm){
            Tri out = *this; // copio
            out.a+=amm;
            out.b+=amm;
            out.c+=amm;
            return out;
        }

        std::string to_string(){
            return "[ "+std::to_string(a)+" , "+std::to_string(b)+" , "+std::to_string(c)+" ]";
        }

        Color getColorFromCoords(Vec3 coords){
            Vec3 p = (t1*coords.x + t2*coords.y) + o;
            if(texturaBox.inBox( p._toInt() )){
                return textura[(int)p.x][(int)p.y];
            } else {
                return Color();
            }
        }
        
        Color getColorFromBarycentricCoords(Vec3 coords){
            Vec3 p = o*coords.x + (o+t1)*coords.y + (o+t2)*coords.z;
            if(texturaBox.inBox( p._toInt() )){
                return textura[(int)p.x][(int)p.y];
            } else {
                return Color();
            }
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

            tris.push_back(Tri(0, 2, 1));
            tris.push_back(Tri(3, 1, 2));

            tris.push_back(Tri(1, 3, 5));
            tris.push_back(Tri(7, 5, 3));

            tris.push_back(Tri(7, 3, 6));
            tris.push_back(Tri(2, 6, 3));

            tris.push_back(Tri(6, 2, 4));
            tris.push_back(Tri(0, 4, 2));
            
            tris.push_back(Tri(0, 1, 4));
            tris.push_back(Tri(5, 4, 1));
            
            tris.push_back(Tri(4, 5, 6));
            tris.push_back(Tri(7, 6, 5));

            return Mesh(vertices, tris);
        }
};
