#pragma once
#include "../vista.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <omp.h>

const int NORMAL = 0;
const int DRAGGING = 1;

const double t_click = 0.2;
double press_time = 0; 

int btn_izq_state = NORMAL;
int btn_der_state = NORMAL;




/** te devuelve un vector unidad apuntando en la direccion clickeada desde una camara
 * @param px la posicion en pixeles clickeada en la pantalla
 * @param c camara desde la que se esta viendo 
*/
Vec3 getDireccionClickeada(Vec3 px, Camara c){
    Vec3 pu = Renderer::pixsToUnits(px);
    pu.setZ(1);                             // lo paso a image space haciendo que este contra el near plane (z = 1)

    pu.y=-pu.y;

    Matriz p_col = Matriz::vec3_a_columna(pu);
    p_col = p_col * (c.np);                 // lo paso a vector columna y lo multiplico por el w que resultaria de hacer la view transofrm
    
    Matriz res = c.getViewTransform().inversa()*p_col;  // hago la view transform a la inversa
    res = c.getTransformacion()*res;        // paso de camera space a real space
    return Matriz::columna_a_vec3(res).sub(c.getPov()).norm();     // paso el punto en real space de columna a vector y hago que sea desde la camara en vez del origen
}


/**Debuelve el punto clickeado (o el punto mas cercano) en una trackball virtual posicionada en el origen
 * @param c      Camara desde donde se esta clickeando
 * @param pixs   Posicion en la ventana que se clickeo
 * @param radio  Radio de la Trackball
*/
Vec3 getPuntoEnTrackball(Camara c, Vec3 pixs/*, Vec3 posTB*/, double radio){
    //mucha matematica, chequear la hoja del dia 24/02/24 de mi carpeta que habla de interseccion entre punto y esfera
    Vec3 pos_cam = c.getPov();
    Vec3 dir = getDireccionClickeada(pixs, c);

    if( pow(2*(pos_cam*dir),2)  >=  4*(dir*dir*(pos_cam*pos_cam - radio*radio)) ) // clickeo la esfera
    {
        double a = ( -2*(  pos_cam*dir  ) - sqrt(  pow( 2*(pos_cam*dir), 2 ) -4*( dir*dir*(  pos_cam*pos_cam - radio*radio  ) )  ) )/( 2*(  dir*dir  ) );
        Vec3 r = pos_cam + dir*a; // primer cruce con la esfera
        return r;
    } 
    else // clickeo fuera de la esfera
    {
        double a = (pos_cam*dir*(-2))/(dir*dir*2);
        Vec3 r = (pos_cam + dir*a).norm()*radio; // punto mas cercano a la esfera
        return r;
    }
}




void manage_events(Vista* vista){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){

            case SDL_QUIT:
                vista->pause();
                break;

            case SDL_MOUSEBUTTONDOWN:
                press_time=omp_get_wtime();
                if(e.button.button == SDL_BUTTON_LEFT)  btn_izq_state = DRAGGING;
                if(e.button.button == SDL_BUTTON_RIGHT) btn_der_state = DRAGGING;

                break;

            case SDL_MOUSEBUTTONUP:
                if(e.button.button == SDL_BUTTON_LEFT)  btn_izq_state = NORMAL;
                if(e.button.button == SDL_BUTTON_RIGHT) btn_der_state = NORMAL;

                if(omp_get_wtime() - press_time <= t_click)
                {
                    if(e.button.button == SDL_BUTTON_LEFT) // click izquierdo
                    {
                        vista->cam.vangular=Quat();
                        vista->cam._girar(Vec3(0,0,1), 0.2);
                    }
                    if(e.button.button == SDL_BUTTON_RIGHT) // click derecho
                    {

                    }
                }
                break;

            case SDL_MOUSEMOTION: // e.motion.xrel/yrel son lo que se movio el mouse desde la update anterior, sino e.motion.x/y son en pixeles 
                if(omp_get_wtime() - press_time > t_click)
                {
                    if(btn_izq_state == DRAGGING) // dragging btn izquierdo 
                    {
                        double radio = vista->cam.dist*3/4;

                        Vec3 pixs1 = Vec3(e.motion.x-e.motion.xrel, e.motion.y-e.motion.yrel);
                        Vec3 pixs2 = Vec3(e.motion.x              , e.motion.y              );

                        Vec3 r1 = getPuntoEnTrackball(vista->cam, pixs1, radio).norm(); // punto que toca la esfera en la direccion 1
                        Vec3 r2 = getPuntoEnTrackball(vista->cam, pixs2, radio).norm(); // punto que toca la esfera en la direccion 2
                        Vec3 eje = r1.mult_vectorial(r2);                               // eje de rotacion perpendicular a los dos puntos
                        double angulo = acos(r1*r2);
                        //vista->cam.vangular = Quat(cos(-angulo*2), eje.norm()*sin(-angulo*2));
                        vista->cam._girar(eje, -angulo*2);
                    }

                    if(btn_der_state == DRAGGING) // dragging btn derecho 
                    { 
                        
                    }
                }
                break;

            case SDL_MOUSEWHEEL:
                vista->cam.dist-=e.wheel.preciseY/10;
                break;
            
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_w:
                    vista->cam.pos.z-=1;
                    break;
                case SDLK_a:
                    vista->cam.pos.x-=1;
                    break;
                case SDLK_s:
                    vista->cam.pos.z+=1;
                    break;
                case SDLK_d:
                    vista->cam.pos.x+=1;
                    break;
                case SDLK_LCTRL:
                    vista->cam.pos.y-=1;
                    break;
                case SDLK_SPACE:
                    vista->cam.pos.y+=1;
                    break;
                
                default:
                    break;
                }
                break;
            
        }
    }
}