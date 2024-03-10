#pragma once
#include <vector>
#include "modelo/entity.h"
#include "modelo/camara.h"
#include "modelo/gameloop.h"
#include "IO/renderer.h"

class Vista; //clase que usa funcion que usa clase e_e
void manage_events(Vista* vista);

class Vista{
    private:
        Renderer renderer;
        GameLoop gl;

        void load_face(){
            escena.emplace_back(Entity::getCubo(1, Vec3(1,1,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(-1,1,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(2,-1,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(-2,-1,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(-1,-2,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(0,-2,0)));
            escena.emplace_back(Entity::getCubo(1, Vec3(1,-2,0)));
        }

    public:
        Camara cam;
        std::vector<Entity> escena = std::vector<Entity>();

        Vista():cam(Vec3(0,0,0)), gl([this](){this->paint();},  [this](double dt){this->update(dt);})
        {
            escena.emplace_back(Entity::getCubo(0.1, Vec3(0,0,0)));
            load_face();
        }


        void run()
        {
            gl.start();
        }

        void pause()
        {
            gl.stop();
        }


        void paint()
        {
            renderer.render(&escena,cam);
        }

        void update(double dt)
        {
            manage_events(this);
            for(int i=0; i<escena.size(); i++)
            {
                escena.at(i).direccion = escena.at(i).direccion*escena.at(i).vangular;
            }
            cam.direccion = cam.direccion*cam.vangular;
        }

        
};

#include "IO/inputmanager.h"