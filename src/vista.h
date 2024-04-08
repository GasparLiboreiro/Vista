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
            load_face();
            escena.emplace_back(Entity::getCubo(0.1, Vec3(0,0,0)));
            //escena.emplace_back(Entity(Mesh({Vec3(0,0,0), Vec3(1,2), Vec3(1,-2)}, {Tri(0,1,2)})));
            /*Entity c1 = Entity::getCubo(0.99, Vec3());
            c1.direccion = Quat::get_rotation_quaternion(Vec3(0,0,1), 3.14159/4);
            escena.emplace_back(c1);
            Entity c2 = Entity::getCubo(0.99, Vec3());
            c2.direccion = Quat::get_rotation_quaternion(Vec3(0,1,0), 3.14159/4);
            escena.emplace_back(c2);
            Entity c3 = Entity::getCubo(0.99, Vec3());
            c3.direccion = Quat::get_rotation_quaternion(Vec3(1,0,0), 3.14159/4);
            escena.emplace_back(c3);
            Entity c4 = Entity::getCubo(1, Vec3());
            escena.emplace_back(c4);*/
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