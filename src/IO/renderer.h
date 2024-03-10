#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include "../modelo/mesh.h"
#include "../modelo/entity.h"
#include "../modelo/matriz.h"
#include "../modelo/camara.h"



unsigned const int ANCHO = 1000;
unsigned const int ALTO = 1000;
class Renderer{
    private:
        SDL_Window* window;
        SDL_Renderer* canvas;
        double** z_buffer;
    public:

        Renderer()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_Init(SDL_INIT_EVENTS);
            SDL_CreateWindowAndRenderer(ANCHO, ALTO, 0, &window, &canvas);
            clear();
            SDL_RenderPresent(canvas);
            
            z_buffer = (double**) malloc(sizeof(double*) * ANCHO);
            for(int i=0; i<ANCHO; i++){
                z_buffer[i]=(double*) malloc(sizeof(double) * ALTO);
            }
        }

        ~Renderer(){
            for(int i=0; i<ANCHO; i++)
                delete z_buffer[i];
            delete z_buffer;
        }


        void clear()
        {
            SDL_SetRenderDrawColor(canvas, 0, 0, 0, 255);
            SDL_RenderClear(canvas);
        }


        void render(std::vector<Entity> *escena, Camara cam)
        {
            
            for(int i=0; i<ANCHO; i++)
                for(int j=0; j<ALTO; j++)
                    z_buffer[i][j]=INT_MIN;
            
            clear();
            SDL_SetRenderDrawColor(canvas, 255, 255, 255, 255);
            //Matriz CT = cam.getCameraTransform();// matriz para pasar de espacio normal a image space desde la perspectiva de la camara
            
            std::vector<Vec3> vertices_image_space = std::vector<Vec3>();
            std::vector<Tri> tris_image_space = getTrisEscena(escena);

            Matriz cam_pov_transform = cam.getTransformacion().inversa(); // es la transformacion que mueve la camara a 0,0,0 apuntando hacia -z y de la misma forma a cualquier otro punto

            for(int i=0; i<escena->size(); i++)
            {
                Entity* e_actal = &escena->at(i);
                //Matriz t_final_entidad = CT * escena->at(i).getTransformation(); inutilizada porq uso la tranformacion y la CT por separado

                for(int j=0; j<e_actal->mesh.vertices.size(); j++)
                {
                    Vec3 v_actual = e_actal->mesh.vertices.at(j);                                           // vector del modelo a transformar
                    Matriz v_rs_actual = e_actal->getTransformation() * Matriz::vec3_a_columna( v_actual ); // vector en real space
                    Matriz v_cs_actual = cam_pov_transform * v_rs_actual;                                   // vector en camera space (lo consigo de una y por separado para hacer un if si se tiene que clipear)

                    if(v_cs_actual[0][2]<=-cam.np && v_cs_actual[0][2]>=-cam.fp)
                    {
                        Matriz image_space_col = cam.getViewTransform() * v_cs_actual; // vector en image space
                        image_space_col[0][0]/=image_space_col[0][3];
                        image_space_col[0][1]/=image_space_col[0][3];
                        image_space_col[0][2]/=image_space_col[0][3];
                        Vec3 image_space = Matriz::columna_a_vec3(image_space_col);
                        
                        vertices_image_space.push_back(image_space);
                    } 
                    else 
                    {
                    //std::cout<<"z: "<<actual[0][2]<<std::endl;
                        Vec3 out;
                        out.nulo=true;
                        vertices_image_space.push_back(out);
                    }

                }
            }
            
            for(int i = 0; i<tris_image_space.size(); i++)
            {
                Vec3 a_i = vertices_image_space[tris_image_space[i].a];
                Vec3 b_i = vertices_image_space[tris_image_space[i].b];
                Vec3 c_i = vertices_image_space[tris_image_space[i].c];
                if(!a_i.nulo && !b_i.nulo && !c_i.nulo){
                    Vec3 a = unitsToPixs(a_i);
                    Vec3 b = unitsToPixs(b_i);
                    Vec3 c = unitsToPixs(c_i);

                    SDL_RenderDrawLineF(canvas, a.x, a.y, b.x, b.y);
                    SDL_RenderDrawLineF(canvas, a.x, a.y, c.x, c.y);
                    SDL_RenderDrawLineF(canvas, b.x, b.y, c.x, c.y);
                }
            }
            SDL_RenderPresent(canvas);
        }

        void renderTri(Vec3 a, Vec3 b, Vec3 c, double** z_buffer)
        {
            //quiero tener a b c ordenados de menor a mayor en relacion a Y
            Vec3 bubb;
            if(a.y>b.y){
                bubb=a;
                a=b;
                b=bubb;
            }
            if(a.y>c.y){
                bubb=a;
                a=c;
                c=bubb;
            }
            if(b.y>c.y){
                bubb=b;
                b=c;
                c=bubb;
            }
            //tengo a b c ordenados de menor a mayor Y
            /*
              ahora, sucederia que el lado a-c es siempre el limite de las lineas horizontales
              y los lados a-b y b-c son los que cambian a mitad de camino
            */
            double ac_h = c.y-a.y;
            double ab_h = b.y-a.y;
            double bc_h = c.y-b.y;

            for(int j=0; j<ab_h; j++){// primera mirad del triangulo, a-c / a-b

            }

        }

        // esta funcion renderiza una sola linea de un poligono, se asume que v1.y y v2.y son iguales
        void rasterizarLinea(Vec3 v1, Vec3 v2, double** z_buffer)
        {
            if(v1.x>v2.x){
                Vec3 bubb=v1;
                v1=v2;
                v2=bubb;
            }
            
            double dx = v2.x-v1.x; // distancia en x a atravesar, delta x

            Vec3 p; // pixel en el que estoy parado
            for(int i=0; i<=dx; i++)
            {
                p = v1.lerp(v2, i/dx);

                if(p.z > z_buffer[(int)p.x][(int)p.y])
                {
                    z_buffer[(int)p.x][(int)p.y] = p.z;
                    SDL_RenderDrawPoint(canvas, (int)p.x, (int)p.y);
                }
            }
        }




        static Vec3 unitsToPixs(Vec3 units)
        {
            return Vec3((units.x+1) * ANCHO/2, (-units.y+1) * ANCHO/2, (units.z+1) * ANCHO/2);
        }

        static Vec3 pixsToUnits(Vec3 pixs)
        {
            return Vec3(2.0/ANCHO*pixs.x-1, -(2.0/ANCHO*pixs.y-1), 2.0/ANCHO*pixs.z-1);
        }


        std::vector<Tri> getTrisEscena(std::vector<Entity> *escena)
        {
            std::vector<Tri> tris;
            int p_inicio = 0;
            for(int i=0; i<escena->size(); i++){
                
                for(int j=0; j<escena->at(i).mesh.tris.size(); j++){
                    tris.push_back(escena->at(i).mesh.tris.at(j).add(p_inicio));
                }
                p_inicio+=escena->at(i).mesh.vertices.size();// los siguientes tris tienen que tener el index shifteado por la cantidad de vertices que ya hay en el vector
            }
            return tris;
        }


        bool clip(Camara c, Vec3 p){
            Vec3 res = Matriz::columna_a_vec3(c.getTransformacion().inversa() * Matriz::vec3_a_columna(p)); // paso el punto a camera space (la camara en el origen apuntando hacia z)
            if(res.z<=-c.np && res.z>=-c.fp){ // si el punto esta entre el near plane y el far plane, se devuelve true
                return true;
            } else {
                return false;
            }
        }

};