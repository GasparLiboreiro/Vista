#pragma once
#include <omp.h>
#include <thread>
#include <chrono>
#include <functional>



class GameLoop{
    private:
        std::function<void()> paint;
        std::function<void(double)> update;
        double ups_ideal = 60;
        bool looping = false;

        std::thread loopingthread;

        void loop(){
            int fes = 0; // frames en el segundo
            int ues = 0; // updates en el segundo
            
            double ud_ideal = 1/ups_ideal; //update delay ideal

            double inicio_segundo = omp_get_wtime();
            long double ult_update = omp_get_wtime();
            double espera = 0;
            double elapsedTime = 0;

            while(looping){
                //update
                update(omp_get_wtime() - ult_update);
                ult_update = omp_get_wtime();// guardo el momento de la update para usarse en la proxima update
                ues++;
                
                //paint
                paint();
                //std::cout<<"paint :)"<<std::endl;
                fes++;

                elapsedTime = omp_get_wtime() - inicio_segundo;
                espera = (ues * ud_ideal) - elapsedTime;//              el tiempo a esperar se consigue basandonos en el tiempo ideal que deberia haber transcurrido, las updates que tubimos por el delay ideal de cada una nos deveria dar el tiempo del siguiente frame, si restamos el tiempo transcurrido nos deberia dar el tiempo desde este frame hasta el siguiente, si es positivo, esperar eso, si es negativo, seguir viendo
                if(espera>0){
                    std::this_thread::sleep_for(std::chrono::duration<double>(espera));
                    //std::cout<<"delay de "<<espera<<"segundos"<<std::endl;
                }

                while(espera < 0 && ues < ups_ideal){// si el sleep time es negativo compensamos creando updates dentro del mismo frame, hasta que sleep time sea positivo o que se abastescan las updates del segundo
                    //update
                    //std::cout<<"compensando por frame lento!!"<<std::endl;
                    update(omp_get_wtime() - ult_update);
                    ult_update = omp_get_wtime();
                    ues++;
                    elapsedTime = omp_get_wtime() - inicio_segundo;
                    espera = (ues * ud_ideal) - elapsedTime;
                }
                
                if(elapsedTime >= 1){
                    fps = fes/elapsedTime;
                    ups = ues/elapsedTime;
                    fes = 0;
                    ues = 0;
                    inicio_segundo = omp_get_wtime()+0.0;
                    //std::cout<<"\nfps:"<<fps<<"\nups"<<ups<<std::endl<<std::endl;
                }
            }
        }
    public:
        double fps;
        double ups;

        GameLoop(std::function<void()> _paint, std::function<void(double)> _update):paint(_paint), update(_update){}

        void start(){
            looping = true;
            /*loopingthread=std::thread([this](){this->loop();});*/
            std::cout<<"start loop c:"<<std::endl;
            loop();
        }
        void stop(){
            looping = false;
            /*loopingthread.detach();*/
            std::cout<<"stop loop :c"<<std::endl;
        }

};