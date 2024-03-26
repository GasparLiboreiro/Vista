#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include "../modelo/mesh.h"
#include "../modelo/entity.h"
#include "../modelo/matriz.h"
#include "../modelo/camara.h"
#include "../modelo/box.h"


class edgeTracker {
    public:

        Vec3 a, b;

        // coords del punto actual del borde
        double x, y, z;
        // cantidad que cambia x por cada 1 de y
        double x_vinc;
        // cantidad que cambia z por cada 1 de y
        double z_vinc;

        //para comodidad, cualquier edgeTracker que sea inicializado solo asi no anda
        edgeTracker(){}

        // se deben brindar en orden de y creciente
        edgeTracker(Vec3 a, Vec3 b){
            //Vec3 normal = (b-a).mult_vectorial(c-a);
            //z_hinc = -normal.x/normal.z;                  <!0>

            if(a.y<b.y){
                Vec3 bub = a;
                a = b;
                b = bub;

                std::cout<<"WARN IO/renderer/edgeTracker(): deberia ingresar vectores a y b en orden de Y decreciente"<<std::endl;
            }

            this->a = a;
            this->b = b;

            x_vinc = (a.x - b.x) / (a.y - b.y);
            z_vinc = (a.z - b.z) / (a.y - b.y);

            x = a.x;
            y = a.y;
            z = a.z;

            if(y!=floor(y)) // si y no esta en una posicion entera
            {
                x-=x_vinc*(y-floor(y));
                z-=z_vinc*(y-floor(y)); // x/z_vinc representa cuanto cambia cada 1 de y, como aca nos movemos menos que 1, multiplico por lo que nos movemos
                y=floor(y);
            }
        }

        //hace avanzar las coords actuales al siguiente scanline/renglon del edge/borde
        void next(){
            y--;
            x-=x_vinc;
            z-=z_vinc;
        }

        /**me debuelve la posicion desde la que empezar a iterar horizontalmente 
         * lo que pasa aca es que muevo x a un valor entero, en ese movimiento, pensando en el supuesto triangulo como un plano 3d, cambia z, por eso el parametro
         * @param z_hinc es la cantidad que cambia z por cada valor de x
        */
        Vec3 getPosIterHorizontal(double z_hinc){
            Vec3 pos(
                floor(x),
                y,
                z-z_hinc*(x-(floor(x)))
            );

            return pos;
        }
};


class triTracker {
    public:
        // vertices del triangulo (en orden de y decreciente)
        Vec3 t1, t2, t3;
        Vec3 normal;

        // coords del punto actual
        Vec3 p;

        // edge tracker izquierdo y derecho
        edgeTracker ei, ed;
        
        // cantidad que cambia z por cada 1 de x
        double z_hinc;

        triTracker(Vec3 a, Vec3 b, Vec3 c){
            
            // consigo cuanto cambia z por cada valor de x
            normal = (b-a).mult_vectorial(c-a);
            z_hinc = -normal.x/normal.z; // tengo que contemplar que si normal.z==0 no se haga nada con el triangulo
            //std::cout<<"n.z: "<<normal.z<<std::endl;

            // ordeno los vectores con Y creciente
            Vec3 bubb;
            if(a.y<b.y){
                bubb=a;
                a=b;
                b=bubb;
            }
            if(a.y<c.y){
                bubb=a;
                a=c;
                c=bubb;
            }
            if(b.y<c.y){
                bubb=b;
                b=c;
                c=bubb;
            }

            t1 = a;
            t2 = b;
            t3 = c;
            
            Vec3 a_c = c-a;
            Vec3 a_b = b-a;

            // comparo que esta mas a la izquierda, el punto de la linea a->c a la altura del final de a_b.y o el punto final de la recta a->b, esto lo pregunto para tener un edge a la izquierda y otro a la derecha
            if( ( ( a_c/a_c.y ) * a_b.y ).x < a_b.x ){
                ei = edgeTracker(a,c);
                ed = edgeTracker(a,b);
            } else {
                ei = edgeTracker(a,b);
                ed = edgeTracker(a,c);
            }

            p = ei.getPosIterHorizontal(z_hinc);

        }



        void next(){
            //siguiente pos horizontal
            p.x++;
            p.z+=z_hinc;
            
            
            // sig renglon?
            if(p.x>ed.x){
                ei.next();
                ed.next();
                
                p = ei.getPosIterHorizontal(z_hinc);
            }
            // cambio de borde?
            if( p.y<=t2.y   && ei.a!=t2 && ed.a!=t2){
                
                if(ei.b == t2){
                    ei = edgeTracker(t2, t3);
                } else {
                    ed = edgeTracker(t2, t3);
                }

                p = ei.getPosIterHorizontal(z_hinc);
            }
        }

        bool fin(){
            if(p.y > floor(t3.y) && normal.z>0.005){
                return false;
            } else {
                return true;
            }  
        }

        
};




unsigned const int ANCHO = 1000;
unsigned const int ALTO = 1000;
class Renderer{
    private:
        SDL_Window* window;
        SDL_Renderer* canvas;
        Box screen = Box(ANCHO, ALTO);
        double** z_buffer;
    public:

        Renderer()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_Init(SDL_INIT_EVENTS);
            SDL_CreateWindowAndRenderer(ANCHO, ALTO, 0, &window, &canvas);
            SDL_SetRenderDrawColor(canvas, 0, 0, 0, 255);
            SDL_RenderClear(canvas);
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
            SDL_SetRenderDrawColor(canvas, 50, 50, 50, 255);
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

            Matriz cam_pov_transform = cam.getTransformacionInversa(); // es la transformacion que mueve la camara a 0,0,0 apuntando hacia -z y de la misma forma a cualquier otro punto

            for(int i=0; i<escena->size(); i++)
            {
                Entity* e_actal = &escena->at(i);
                //Matriz t_final_entidad = CT * escena->at(i).getTransformation(); inutilizada porq uso la tranformacion y la CT por separado

                for(int j=0; j<e_actal->mesh.vertices.size(); j++)
                {
                    Vec3 v_actual = e_actal->mesh.vertices.at(j);                                           // vector del modelo a transformar
                    Matriz v_rs_actual = e_actal->getTransformation() * Matriz::vec3_a_columna( v_actual ); // vector en real space
                    Matriz v_cs_actual = cam_pov_transform * v_rs_actual;                                   // vector en camera space (lo consigo de una y por separado para hacer un if si se tiene que clipear)

                    Matriz image_space_col = cam.getViewTransform() * v_cs_actual; // vector en image space
                    image_space_col[0][0]/=image_space_col[0][3];
                    image_space_col[0][1]/=image_space_col[0][3];
                    image_space_col[0][2]/=image_space_col[0][3];
                    Vec3 image_space = Matriz::columna_a_vec3(image_space_col);

                    
                    

                    if(image_space.z>1 || image_space.z<-1)
                    {
                        image_space.nulo=true;
                        vertices_image_space.push_back(image_space);
                    } 
                    else
                    {
                        image_space.z = v_cs_actual[0][3]; // las z estan distorcionadas de formas re iregulares, con esto consigo la distancia a la camara enm el espacio real, imporntante para despues (texturas)
                        vertices_image_space.push_back(image_space);
                    }

                }
            }
            
            for(int i = 0; i<tris_image_space.size(); i++)
            {
                Vec3 a_i = vertices_image_space[tris_image_space[i].a];
                Vec3 b_i = vertices_image_space[tris_image_space[i].b];
                Vec3 c_i = vertices_image_space[tris_image_space[i].c];
                
                if(!a_i.nulo && !b_i.nulo && !c_i.nulo){
                    Vec3 a = unitsToPixs(a_i).setZ(a_i.z);
                    Vec3 b = unitsToPixs(b_i).setZ(b_i.z);
                    Vec3 c = unitsToPixs(c_i).setZ(c_i.z);
                    //SDL_SetRenderDrawColor(canvas, rand()%255, rand()%255, rand()%255, 255);

                    renderTri(tris_image_space[i],a,b,c,z_buffer);
                    /*
                    SDL_SetRenderDrawColor(canvas, 255,255,255,255);
                    SDL_RenderDrawLineF(canvas, a.x, a.y, b.x, b.y);
                    SDL_RenderDrawLineF(canvas, a.x, a.y, c.x, c.y);
                    SDL_RenderDrawLineF(canvas, b.x, b.y, c.x, c.y);*/


                    
                }
            }
            SDL_RenderPresent(canvas);
        }

        void renderTri(Tri tri, Vec3 a, Vec3 b, Vec3 c, double** z_buffer)
        {

            // chequear mi hoja del dia 18/3/24 respecto a Berrycentric Coordinates

            Vec3 o = a;
            Vec3 e1 = b-o; 
            Vec3 e2 = c-o;

            double divisor = 1 / ((e2*e2)*(e1*e1) - (e1*e2)*(e1*e2));
            Vec3 mb = (e1*(e2*e2) - e2*(e1*e2)) * divisor; // mu beta
            Vec3 mg = (e2*(e1*e1) - e1*(e1*e2)) * divisor; // mu gamma

            // parte de la correccion por la perspectiva para las texturas (https://www.youtube.com/watch?v=VdLZCyHNdHc)
            
            Vec3 tri_a_div = tri.o/a.z; 
            tri_a_div.z=1./a.z;
            Vec3 tri_b_div = (tri.t1+tri.o)/b.z; 
            tri_b_div.z=1./b.z;
            Vec3 tri_c_div = (tri.t2+tri.o)/c.z; 
            tri_c_div.z=1./c.z;

            double alfa, beta, gamma;


            triTracker t_tracker (a,b,c);
            while (!t_tracker.fin()){
                //std::cout<<"t.p: "<<t_tracker.p.to_string()<<std::endl;
                if(screen.inBox(t_tracker.p) && t_tracker.p.z > z_buffer[(int) t_tracker.p.x][(int) t_tracker.p.y]){
                    //std::cout<<"entra??"<<std::endl;
                    z_buffer[(int) t_tracker.p.x][(int) t_tracker.p.y] = t_tracker.p.z;

                    beta = mb * (t_tracker.p - o);
                    gamma = mg * (t_tracker.p - o);
                    alfa = 1 - beta - gamma;
                    Vec3 tri_p_div = tri_a_div * alfa + tri_b_div * beta + tri_c_div * gamma;
                    Vec3 p = tri_p_div * (1./tri_p_div.z);

                    /*
                    Vec3 coords = Vec3::coordsEnBase2D(e1,e2, t_tracker.p-o);
                    Vec3 tri_p_div = (tri_t1_div * coords.x + tri_t2_div * coords.y) + tri_o_div;
                    Vec3 p = tri_p_div*(1/tri_p_div.z);*/

                    Vec3 pt = tri.texturaBox.punto2DMasCercano(p); // si p se sale de la textura, usamos el color mas cercano
                    if(tri.texturaBox.inBox(pt)){
                        Color c = tri.textura[(int)pt.x][(int)pt.y];
                        SDL_SetRenderDrawColor(canvas, c.r, c.g, c.b, 255);
                    } else {
                        SDL_SetRenderDrawColor(canvas, 255, 0, 0, 255);
                    }

                    SDL_RenderDrawPoint(canvas, t_tracker.p.x, ALTO-1- t_tracker.p.y);
                }

                t_tracker.next();
            }
            

        }


        static Vec3 unitsToPixs(Vec3 units)
        {
            return Vec3((units.x+1) * ANCHO/2, (units.y+1) * ANCHO/2, (units.z+1) * ANCHO/2);
        }

        static Vec3 pixsToUnits(Vec3 pixs)
        {
            return Vec3(2.0/ANCHO*pixs.x-1, 2.0/ANCHO*pixs.y-1, 2.0/ANCHO*pixs.z-1);
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