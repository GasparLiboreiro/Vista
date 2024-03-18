#pragma once
#include <cstdlib>
#include <iostream>
#include <initializer_list>
#include <math.h>
#include "vec3.h"
#include "quat.h"

using namespace std;
// clase para el facil almacenamiento, creacion y manipulacion de matrices matematicas
class Matriz{
    public:
        // pointer que representa el array de arrays de la matriz ( se accede: m[ancho][alto])
        double** m;
        // anchura del array
        int w;
        // altura del array
        int h;

        Matriz(int w, int h){
            m = (double**) malloc(w*sizeof(double*));
            for(int i=0; i<w; i++){
                m[i] = (double*) malloc(h*sizeof(double));
            }


            this->w=w;
            this->h=h;


            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    m[i][j]=0;
                }
            }
        }

        Matriz(double def, int w, int h){
            m = (double**) malloc(w*sizeof(double*));
            for(int i=0; i<w; i++){
                m[i] = (double*) malloc(h*sizeof(double));
            }


            this->w=w;
            this->h=h;


            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    m[i][j]=def;
                }
            }
        }

        Matriz(initializer_list<initializer_list<double>> arr)
        {
            m = (double**) malloc(arr.begin()->size()*sizeof(double*));
            for(int i=0; i<arr.begin()->size(); i++)
            {
                m[i] = (double*) malloc(arr.size()*sizeof(double));
            }

            //cout<<"alto: "<<arr.size()<<endl;
            //cout<<"ancho: "<<arr.begin()->size()<<endl;

            int w=0;
            int h=0;

            for(initializer_list<double> fila : arr)
            {
                w=0;
                for(double val : fila)
                {
                    m[w][h]=val;
                    w++;
                }
                h++;
            }

            this->w = w;
            this->h = h;
        }

        ~Matriz() // destruir
        {
            for(int i=0; i<w; i++)
                delete m[i];
            delete m;
        }


        Matriz(const Matriz& n) // copiar
        {
            w=n.w;
            h=n.h;

            m = (double**) malloc(w * sizeof(double*));
            for(int i=0; i<w; i++)
                m[i] = (double*) malloc(h * sizeof(double));

            for(int i=0; i<w; i++)
                for(int j=0; j<h; j++)
                    m[i][j]=n.m[i][j];
        }


        Matriz& operator=(Matriz n) // asignar
        {
            if(this != &n)
            {
                for(int i=0; i<w; i++) // libero la memoria q habia en la matriz
                    delete m[i];
                delete m;

                w = n.w; // reasigno el tamaño
                h = n.h;

                
                m = (double**) malloc(w * sizeof(double*)); // pido la memoria necesaria
                for(int i=0; i<w; i++)
                    m[i] = (double*) malloc(h * sizeof(double));

                for(int i=0; i<w; i++) // igualo los valores de la matriz n a esta matriz
                    for(int j=0; j<h; j++)
                        m[i][j]=n.m[i][j];
            }
            return *this;
        }





        static Matriz mul_matrices(Matriz m1, Matriz m2){
            if(m1.w!=m2.h){
                cout<<"MULTIPLICACION DE MATRICES INVALIDA"<<endl;
                cout<<"m1.w ("<<m1.w<<") deberia ser igual a m2.h ("<<m2.h<<")"<<endl;
                cout<<"m1:"<<endl;
                m1.print();
                cout<<"m2:"<<endl;
                m2.print();
                throw "ERROR src/matriz.h/mul_matrices tamanios de matrices erroneos";
            }else{
                Matriz mr = Matriz(m2.w, m1.h);

                for(int j=0; j<mr.h; j++){
                    for(int i=0; i<mr.w; i++){
                        for(int k=0; k<m1.w; k++){
                            mr[i][j]+=m1[k][j]*m2[i][k];
                        }
                    }
                }

                return mr;
            }
        }

        Matriz transponer(){
            Matriz res = Matriz(h,w);
            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    res[j][i] = m[i][j];
                }
            }
            return res;
        }

        Matriz _transponer(){
            Matriz res = Matriz(h,w);
            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    res[j][i] = m[i][j];
                }
            }
            *this = res;
            return *this;
        }

        //funcion recursiva, sale del ciclo cuando las determinantes son reducidas a determinantes 2x2
        //falla si la matriz no es cuadrada o tiene tamaño nulo
        double determinante(){
            if(w==0 || h==0) {std::cout<< "determinante de matriz "+std::to_string(w)+"x"+std::to_string(h) <<std::endl; throw ECANCELED; }
            if(w!=h)         {std::cout<< "DETERMINANTE DE MATRIZ NO CUADRADA" <<std::endl; throw ECANCELED; }
            else if(w==1) // metodo trivial para 1x1
            {
                return m[0][0];
            }
            else if(w==2) // metodo base para 2x2
            {
                return m[0][0]*m[1][1] - m[1][0]*m[0][1];
            } 
            else // regla de laplace
            {
                double ret = 0;
                for(int i=0; i<w; i++){
                    ret += m[i][0] * adjunto(i,0);
                }
                return ret;
            }
        }





        double menor_complementario(int h, int k)
        {
            Matriz matriz_del_mc = Matriz(this->w -1, this->h -1); //copia la matriz

            for(int i=0, biasx=0; i<this->w; i++)
            {
                if(i!=h)
                {
                    for(int j=0, biasy=0; j<this->h; j++)
                    {
                        if(j!=k)
                        {
                            matriz_del_mc[i + biasx][j + biasy] = m[i][j];
                        } 
                        else 
                        {
                            biasy = -1;
                        }
                    }
                } 
                else 
                {
                    biasx = -1;
                }
            }
            //matriz_del_mc.print();
            return matriz_del_mc.determinante();
        }

        double adjunto(int h, int k){
            return pow(-1, h+k) * menor_complementario(h,k);
        }

        //no confundir con matriz de los adjuntos, esta sirve para la inversa de la matriz
        Matriz matriz_adjunta(){
            Matriz ret = Matriz(w, h);
            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    ret[i][j]=adjunto(i,j);
                }
            }
            return ret._transponer();
        }

        //no confundir con matriz de los adjuntos, esta sirve para la inversa de la matriz
        Matriz _matriz_adjunta(){
            Matriz ret = Matriz(w, h);
            for(int i=0; i<w; i++){
                for(int j=0; j<h; j++){
                    ret[j][i]=adjunto(i,j); // armo la matriz y traspongo al mismo tiempo
                }
            }
            *this = ret;
            return *this;
        }


        Matriz inversa(){
            return this->matriz_adjunta() / this->determinante();
        }
        
        Matriz _inversa(){
            *this = this->matriz_adjunta() / this->determinante();
            return *this;
        }


        /**
         * OPERACIONES BASICAS Y COMODAS :
        */

        
        // posibilidad de acceder a los objetos de la matriz de la forma mi_matriz[x][y] en vez de usando mi_matriz.m[x][y]
        double* operator[](const unsigned int _Pos)
        {
            return m[_Pos];
        }


        Matriz operator* (Matriz m){ // multiplicacion entre matrices
            Matriz n = mul_matrices(*this, m);
            return n;
        }

        Matriz operator* (double d){ // multiplicacion por constante
            Matriz n = mul_const(*this, d);
            return n;
        }

        Matriz operator/ (Matriz m){ // division entre matrices (multiplicacion entre primera y inversa de la segunda)
            Matriz n = mul_matrices(*this, m.inversa());
            return n;
        }

        Matriz operator/ (double d){ // division por constante
            Matriz n = mul_const(*this, 1/d);
            return n;
        }

        Matriz operator+ (Matriz m){ // suma entre matrices
            Matriz n = sum_matrices(*this, m);
            return n;
        }

        Matriz operator+ (double d){ // suma por constante
            Matriz n = sum_const(*this, d);
            return n;
        }


        void print(){
            cout<<"ancho: "<<w<<"  alto: "<<h<<endl;
            for(int j=0; j<h; j++){
                cout<<"[";
                for(int i=0; i<w; i++){
                    cout<<" "<<m[i][j];
                }
                cout<<" ]"<<endl;
            }
        }

        
        static Matriz mul_const(Matriz m1, double d){
            Matriz mr = Matriz(m1.w, m1.h);

            for(int i=0; i<mr.w; i++){
                for(int j=0; j<mr.h; j++){
                    mr[i][j]=m1[i][j]*d;
                }
            }

            return mr;
        }
        static Matriz sum_matrices(Matriz m1, Matriz m2){
            if(m1.h == m2.h && m1.w == m2.w){
                Matriz mr = Matriz(m1.w, m1.h);

                for(int i=0; i<mr.w; i++){
                    for(int j=0; j<mr.h; j++){
                        mr[i][j]=m1[i][j]+m2[i][j];
                    }
                }

                return mr;
            } else {
                std::cout<<"SUMANDO MATRICES DE DISTINTO TAMAÑO"<<std::endl;
            }
        }
        static Matriz sum_const(Matriz m1, double d){
            Matriz mr = Matriz(m1.w, m1.h);

            for(int i=0; i<mr.w; i++){
                for(int j=0; j<mr.h; j++){
                    mr[i][j]=m1[i][j]+d;
                }
            }

            return mr;
        }


        /**
         * Conseguir una matriz la cual premultiplicada por un vector columna te devuelva el vector sumado a *vec*
         * @param vec cantidades a trasladar en xyz
        */
        static Matriz get_traslacion(Vec3 vec){
            return Matriz({
                {1,0,0,vec.x},
                {0,1,0,vec.y},
                {0,0,1,vec.z},
                {0,0,0,1}
            });
        }
        /**
         * Conseguir una matriz la cual premultiplicada por un vector columna te devuelva el vector rotado en x *ang* radianes
         * @param ang angulo a rotar en radianes
        */
        static Matriz get_rotacion_x(double ang){
            return Matriz({
                {1,0,0,0},
                {0,cos(ang),-sin(ang),0},
                {0,sin(ang),cos(ang),0},
                {0,0,0,1}
            });
        }
        /**
         * Conseguir una matriz la cual premultiplicada por un vector columna te devuelva el vector rotado en y *ang* radianes
         * @param ang angulo a rotar en radianes
        */
        static Matriz get_rotacion_y(double ang){
            return Matriz({
                {cos(ang),0,sin(ang),0},
                {0,1,0,0},
                {-sin(ang),0,cos(ang),0},
                {0,0,0,1}
            });
        }
        /**
         * Conseguir una matriz la cual premultiplicada por un vector columna te devuelva el vector rotado en z *ang* radianes
         * @param ang angulo a rotar en radianes
        */
        static Matriz get_rotacion_z(double ang){
            return Matriz({
                {cos(ang),-sin(ang),0,0},
                {sin(ang),cos(ang),0,0},
                {0,0,1,0},
                {0,0,0,1}
            });
        }
        /**
         * Conseguir una matriz la cual premultiplicada por un vector columna te devuelva el vector listo para estar en screen space (faltaria dividir por w)  
         * para detalles: Discord/archivero/formulas/manipulacion-3d/view transform explicada
         * @param alpha "fov" hipotetico
         * @param n near plane, la distancia al z=0 minima para que un objeto sea renderizado
         * @param f far plane, la distancia al z=0 maxima para que un objeto sea renderizado
        */
        static Matriz get_view_transform(double alpha, double n, double f){
            return Matriz({
                {1.0/tan(alpha/2),       0        ,      0      ,       0     },
                {        0       ,1.0/tan(alpha/2),      0      ,       0     },
                {        0       ,        0       , (f+n)/(f-n) ,(2*f*n)/(f-n)},
                {        0       ,        0       ,     -1      ,       0     }
            });
        }
        
        static Matriz get_rotacion_quaternion(Vec3 eje, double a){
            Quat rot(cos(a/2), eje*sin(a/2));
            return get_rotacion_quaternion(rot);
        }
        
/*      // este nesecita que el quat sea hecho: (cos(a/2) , eje*sin(a/2))   . pero yo prefiero que sea (cos(a) , eje*sin(a)), porq es lo mismo y mas legible, la funcion de abajo lo acepta de esa forma 
        static Matriz get_rotacion_quaternion(Quat q){
            return Matriz({
                {1 - 2*(q.c*q.c + q.d*q.d),      2*(q.b*q.c - q.a*q.d),      2*(q.b*q.d + q.a*q.c),  0  },
                {    2*(q.b*q.c + q.a*q.d),  1 - 2*(q.b*q.b + q.d*q.d),      2*(q.c*q.d - q.a*q.b),  0  },
                {    2*(q.b*q.d - q.a*q.c),      2*(q.c*q.d + q.a*q.b),  1 - 2*(q.b*q.b + q.c*q.c),  0  },
                {             0           ,               0           ,               0           ,  1  }
            });
        }*/

        
        static Matriz get_rotacion_quaternion(Quat q){
            return Matriz({
                {2*(q.a*q.a + q.b*q.b)-1,  2*(q.b*q.c - q.a*q.d),    2*(q.b*q.d + q.a*q.c),    0  },
                {2*(q.b*q.c + q.a*q.d),    2*(q.a*q.a + q.c*q.c)-1,  2*(q.c*q.d - q.a*q.b),    0  },
                {2*(q.b*q.d - q.a*q.c),    2*(q.c*q.d + q.a*q.b),    2*(q.a*q.a + q.d*q.d)-1,  0  },
                {           0         ,               0         ,               0           ,  1  }
            });
        }

        static Matriz vec3_a_columna(Vec3 v){
            return Matriz{
                {v.x},
                {v.y},
                {v.z},
                {1},
            };
        }

        //tal cual como esta, se pierde el valor w
        static Vec3 columna_a_vec3(Matriz m){
            if(m.w!=1){
                std::cout<<"modelo/matriz.h/columna_a_vec3() : MATRIZ NO ES COLUMNA!!!!!!!!!!!!! se retorno Vec3(0,0,0)"<<std::endl;
                return Vec3();
            }else 
                return Vec3(m[0][0], m[0][1], m[0][2]);
        }

        

};