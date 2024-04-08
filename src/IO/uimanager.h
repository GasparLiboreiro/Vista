#pragma once
#include "../modelo/vec3.h"
#include "../modelo/box.h"
#include "renderer.h"





class UIManager{
    public:

        //(pixeles de alto) todos los popups van a ser como ventanas y van a tener la barrita de arriba para moverlos o cerrarlos
        static const int grab_bar_size = 16;  


        static const unsigned int c_popups = 2;
        // id de las ventanas para editar texturas
        static const int TEXTURE_EDITOR = 0;
        Box default_te_box = Box(256+8, 256+8+grab_bar_size ,0/*depth*/, ANCHO - (256+8), 0);


        class Popup{
            public:
                int const id;
                int capa = 0;                                       // capas de los popups, si esta mas adelante el numero es menor y si esta mas atras es mayor
                Box popupBox;


                Popup(int _id, Box _popupBox=Box(64,64), int _capa=0): 
                    id(_id),
                    capa(_capa),
                    popupBox(_popupBox)
                {} 

        };

        
        class TextureEditor: public Popup{
            public:
                Tri* tri_data;
                
                int zoom = 2;
    


            
                TextureEditor(Tri* _tri_data=nullptr, int _zoom = 2, Box _popupBox = UIManager::getInstance()->default_te_box):
                    Popup(TEXTURE_EDITOR, _popupBox),
                    tri_data(_tri_data),
                    zoom(_zoom)
                {

                }

              

        };

        //para test
        class ModelEditor: public Popup{
            public:
                Mesh* mesh;


                ModelEditor(Mesh* _mesh=nullptr, Box _popupBox = UIManager::getInstance()->default_te_box):
                    Popup(1, _popupBox),
                    mesh(_mesh)
                {

                }
            

        };



    protected:
    
        static UIManager* instance;

    public:

        static UIManager* getInstance(){    //singleton
            if(instance==nullptr){
                instance = new UIManager();
            }
            return instance;
        }

        UIManager(UIManager &other) = delete; // no se puede clonar
        void operator=(const UIManager &) = delete; // no se puede asignar



        UIManager(){

        }



        UIManager::TextureEditor* te = nullptr;
        UIManager::ModelEditor* me = nullptr;


        
        Popup* getPopupById (int i){
            switch(i){
                case TEXTURE_EDITOR:
                    return te;
                    break;
                case 1:
                    return me;
                    break;
                default:
                    return nullptr;
            }
        }
        
        void deletePopupById (int i){
            switch(i){
                case TEXTURE_EDITOR:
                    delete te;
                    te = nullptr;
                    break;
                case 1:
                    delete me;
                    te = nullptr;
                    break;
                default:
                    std::cout<<"deletePopupById con Id inexistente"<<std::endl;
            }
        }




        
        bool posTocaUI(Vec3 pos){
            bool ret = false;
            
            for(int i=0; i<c_popups; i++){
                Popup* popup = getPopupById(i);
                if(popup!=nullptr && popup->popupBox.inBox(pos))
                    ret = true;    
            }

            return ret;
        }



        Popup* getPopupAt(int x, int y)
        {
            int id_popup_click = -1;
            int capa_popup=INT16_MAX;

            for(int i=0; i<c_popups; i++)
            {
                Popup* popup = getPopupById(i);

                if(popup!=nullptr && popup->popupBox.inBox(x,y) && popup->capa<=capa_popup)
                {
                    id_popup_click = popup->id;
                    capa_popup = popup->capa;
                }
            }
            

            return getPopupById(id_popup_click);
        }




        //<!0> para poder customizar mejor que es un arrastre y que es un click, deberia hacer funciones click y drag, y llamarlas en el contexto q me parezca

        void manageClickInteraction(SDL_MouseButtonEvent e)// SDL_MouseButtonEvent e
        { 
            Popup* popup = getPopupAt(e.x, e.y);

            // (textureEditor*) porque no puedo pedir atributos
            if(popup!=nullptr)
            {

                //operaciones estandar de ventanas

                Box x_box = Box(10,10,0, popup->popupBox.offset.x+4, popup->popupBox.offset.y+4);
                Box full_box = Box(10,10,0, popup->popupBox.offset.x+4+10, popup->popupBox.offset.y+4);
                Box corner_box = Box(10,10,0, popup->popupBox.offset.x+4+10+10, popup->popupBox.offset.y+4);
                
                if(x_box.inBox(e.x, e.y))
                {
                    deletePopupById(popup->id);
                }
                else if(full_box.inBox(e.x, e.y))
                {
                    popup->popupBox.offset = Vec3();
                    popup->popupBox.tam = Vec3(ANCHO, ALTO);
                }
                else if(corner_box.inBox(e.x, e.y))
                {
                    popup->popupBox = default_te_box;
                }
                else
                {

                    //operaciones especificas por id
                    switch(popup->id)
                    {
                        case TEXTURE_EDITOR:

                            /*      click a el popup TextureEditor
                                casteando el puntero popup a TextureEditor* conseguis la instancia clickeada

                                    tambien ahora mismo UIManager::te es la unica existencia de este popup
                                asi que usar eso en este contexto seria lo mismo
                            */




                            break;
                        case 1:
                            /*                          //--POPUP DE TESTING--//

                                    click a el popup ModelEditor
                                casteando el puntero popup a ModelEditor* conseguis la instancia clickeada

                                    tambien ahora mismo UIManager::me es la unica existencia de este popup
                                asi que usar eso en este contexto seria lo mismo
                            */

                            break;
                        default:
                            std::cout<<"click a popup con id desconocida"<<std::endl;
                    }
                }


            }
            else
                std::cout<<"no toco ningun popup: ("<<e.x<<","<<e.y<<")"<<std::endl;
        }

        

        void manageDragInteraction(SDL_MouseMotionEvent e){
            Popup* popup = getPopupAt(e.x-e.xrel, e.y-e.yrel); // la anterior pos del drag, para mejor agarre

            // (textureEditor*) porque no puedo pedir atributos
            if(popup!=nullptr)
            {

                //operaciones estandar de ventanas
                Box esquina = Box(4,4,0, popup->popupBox.offset.x+popup->popupBox.tam.x-1-4, popup->popupBox.offset.y+popup->popupBox.tam.y-1-4);
                Box grab_bar = Box(Vec3(popup->popupBox.tam.x, grab_bar_size), popup->popupBox.offset);
                if(esquina.inBox(e.x-e.xrel, e.y-e.yrel)) // si esta tocando la esquinita para resize
                {
                    popup->popupBox.tam._add(Vec3(e.xrel,e.yrel));
                    
                    if(popup->popupBox.tam.x<50)
                        popup->popupBox.tam.x = 50;
                    if(popup->popupBox.tam.y<50+grab_bar_size)
                        popup->popupBox.tam.y = 50+grab_bar_size;
                }
                else if(grab_bar.inBox(e.x-e.xrel, e.y-e.yrel))
                {
                    popup->popupBox.offset._add(Vec3(e.xrel, e.yrel));
                }
                else
                {

                    //operaciones especificas por id
                    switch(popup->id)
                    {
                        case TEXTURE_EDITOR:

                            /*      click a el popup TextureEditor
                                casteando el puntero popup a TextureEditor* conseguis la instancia clickeada

                                    tambien ahora mismo UIManager::te es la unica existencia de este popup
                                asi que usar eso en este contexto seria lo mismo
                            */




                            break;
                        case 1:
                            /*                          //--POPUP DE TESTING--//

                                    click a el popup ModelEditor
                                casteando el puntero popup a ModelEditor* conseguis la instancia clickeada

                                    tambien ahora mismo UIManager::me es la unica existencia de este popup
                                asi que usar eso en este contexto seria lo mismo
                            */

                            break;
                        default:
                            std::cout<<"click a popup con id desconocida"<<std::endl;
                    }
                }


            }
            else
                std::cout<<"no toco ningun popup: ("<<e.x<<","<<e.y<<")"<<std::endl;
        }

        void manageScrollWheelInteraction(SDL_MouseWheelEvent e){
            
        }














































        // paint behabiour

        const Color COL_BORDE_POPUP = Color(164,164,164);
        const Color COL_BORDE_INTERNO_POPUP = Color(75,75,75);
        const Color COL_INTERNO_POPUP = Color(35,35,35);
        const Color COL_X_POPUP = Color(255,255,255);


        // paint texture editor
        const Color COL_UV_TRIANGLE = Color(230, 252, 255);

        void paint_popup_box(Box caja, SDL_Renderer* canvas)
        {
            
            setRenderColor(canvas, COL_INTERNO_POPUP);
            Box caja_maleable = caja;
            caja_maleable.offset._add(Vec3(2,2));
            caja_maleable.tam._sub(Vec3(4,4));

            renderCaja(canvas, caja_maleable);
            

            
            setRenderColor(canvas, COL_BORDE_POPUP);
            SDL_RenderDrawLineF(canvas, caja.offset.x+4, caja.offset.y, caja.offset.x+caja.tam.x-1-4, caja.offset.y); // arriba
            SDL_RenderDrawLineF(canvas, caja.offset.x+4, caja.offset.y+1, caja.offset.x+caja.tam.x-1-4, caja.offset.y+1); // arriba x2
            SDL_RenderDrawLineF(canvas, caja.offset.x+4, caja.offset.y+caja.tam.y-2, caja.offset.x+caja.tam.x-1, caja.offset.y+caja.tam.y-2); // abajo
            SDL_RenderDrawLineF(canvas, caja.offset.x+4, caja.offset.y+caja.tam.y-1, caja.offset.x+caja.tam.x-1, caja.offset.y+caja.tam.y-1); // abajo x2
            SDL_RenderDrawLineF(canvas, caja.offset.x, caja.offset.y+4, caja.offset.x, caja.offset.y+caja.tam.y-1-4); // izquierda
            SDL_RenderDrawLineF(canvas, caja.offset.x+1, caja.offset.y+4, caja.offset.x+1, caja.offset.y+caja.tam.y-1-4); // izquierda x2
            SDL_RenderDrawLineF(canvas, caja.offset.x+caja.tam.x-1, caja.offset.y+4, caja.offset.x+caja.tam.x-1, caja.offset.y+caja.tam.y-1); // derecha
            SDL_RenderDrawLineF(canvas, caja.offset.x+caja.tam.x-2, caja.offset.y+4, caja.offset.x+caja.tam.x-2, caja.offset.y+caja.tam.y-1); // derecha

            SDL_RenderDrawLineF(canvas, caja.offset.x+2, caja.offset.y+2, caja.offset.x+3, caja.offset.y+2);
            SDL_RenderDrawLineF(canvas, caja.offset.x+2, caja.offset.y+3, caja.offset.x+3, caja.offset.y+3); // punto arriba izquierda
            SDL_RenderDrawLineF(canvas, caja.offset.x+2, caja.offset.y+caja.tam.y-1-2, caja.offset.x+3, caja.offset.y+caja.tam.y-1-2);
            SDL_RenderDrawLineF(canvas, caja.offset.x+2, caja.offset.y+caja.tam.y-1-3, caja.offset.x+3, caja.offset.y+caja.tam.y-1-3); // punto abajo izquierda
            SDL_RenderDrawLineF(canvas, caja.offset.x+caja.tam.x-1-2, caja.offset.y+2, caja.offset.x+caja.tam.x-1-3, caja.offset.y+2);
            SDL_RenderDrawLineF(canvas, caja.offset.x+caja.tam.x-1-2, caja.offset.y+3, caja.offset.x+caja.tam.x-1-3, caja.offset.y+3); // punto arriba derecha

            
            setRenderColor(canvas, COL_BORDE_INTERNO_POPUP);
            SDL_RenderDrawLineF(canvas, caja.offset.x+6, caja.offset.y+16, caja.offset.x+caja.tam.x-6-1, caja.offset.y+16);
            SDL_RenderDrawLineF(canvas, caja.offset.x+6, caja.offset.y+17, caja.offset.x+caja.tam.x-6-1, caja.offset.y+17);


            setRenderColor(canvas, COL_X_POPUP); // X
            SDL_RenderDrawLine(canvas, caja.offset.x+6, caja.offset.y+6, caja.offset.x+11, caja.offset.y+11);
            SDL_RenderDrawLine(canvas, caja.offset.x+11, caja.offset.y+6, caja.offset.x+6, caja.offset.y+11);

            SDL_RenderDrawLine(canvas, caja.offset.x+6, caja.offset.y+7, caja.offset.x+7, caja.offset.y+6);
            SDL_RenderDrawLine(canvas, caja.offset.x+10, caja.offset.y+11, caja.offset.x+11, caja.offset.y+10);
            SDL_RenderDrawLine(canvas, caja.offset.x+6, caja.offset.y+10, caja.offset.x+7, caja.offset.y+11);
            SDL_RenderDrawLine(canvas, caja.offset.x+11, caja.offset.y+7, caja.offset.x+10, caja.offset.y+6);

            // boton full size
            SDL_RenderDrawLine(canvas, caja.offset.x+16, caja.offset.y+6, caja.offset.x+19, caja.offset.y+6);
            SDL_RenderDrawLine(canvas, caja.offset.x+16, caja.offset.y+7, caja.offset.x+19, caja.offset.y+7);
            SDL_RenderDrawLine(canvas, caja.offset.x+16, caja.offset.y+8, caja.offset.x+17, caja.offset.y+8);
            SDL_RenderDrawLine(canvas, caja.offset.x+16, caja.offset.y+9, caja.offset.x+17, caja.offset.y+9);

            SDL_RenderDrawLine(canvas, caja.offset.x+20, caja.offset.y+8, caja.offset.x+21, caja.offset.y+8);
            SDL_RenderDrawLine(canvas, caja.offset.x+20, caja.offset.y+9, caja.offset.x+21, caja.offset.y+9);
            SDL_RenderDrawLine(canvas, caja.offset.x+18, caja.offset.y+10, caja.offset.x+21, caja.offset.y+10);
            SDL_RenderDrawLine(canvas, caja.offset.x+18, caja.offset.y+11, caja.offset.x+21, caja.offset.y+11);

            // boton corner
            SDL_RenderDrawLine(canvas, caja.offset.x+26, caja.offset.y+6, caja.offset.x+31, caja.offset.y+6);
            SDL_RenderDrawLine(canvas, caja.offset.x+26, caja.offset.y+7, caja.offset.x+31, caja.offset.y+7);
            SDL_RenderDrawLine(canvas, caja.offset.x+30, caja.offset.y+8, caja.offset.x+30, caja.offset.y+11);
            SDL_RenderDrawLine(canvas, caja.offset.x+31, caja.offset.y+8, caja.offset.x+31, caja.offset.y+11);
        }







        void paint(SDL_Renderer* canvas)
        {
            if(te != nullptr)
            {
                paint_popup_box(te->popupBox, canvas);
                //por hacer: dibujar la textura que se esta editando y el triangulo correspondiente
            }
        }


        void renderCaja(SDL_Renderer* canvas, Box caja){
            for(int y=caja.offset.y; y<caja.offset.y+caja.tam.y; y++)
            {
                SDL_RenderDrawLineF(canvas,
                    caja.offset.x,
                    y,
                    caja.offset.x+caja.tam.x-1,
                    y
                );
            }
        }

        void setRenderColor(SDL_Renderer* canvas, Color c){
            SDL_SetRenderDrawColor(canvas, c.r, c.g, c.b, c.a);
        }

};

UIManager* UIManager::instance = nullptr;