#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <random>
#include <chrono>
#include <math.h>
#include <sys/mman.h>  
#include <string.h>  
#include <atomic>
#include <tuple>
#include <cassert>

using namespace std;

float generarNumeroRandom();
tuple<string,double, int, int> crearHijos(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion);

int main()
{
    int dias_max;
    int dia;
    int m; //Intentos de reproduccion, cantidad de hijos que van a intentar tener
    int n; //Intentos de reproduccion, cantidad de hijos que van a intentar tener
    float p1; //Probabilidad de nacimiento angeles
    float p2; //Probabilidad de nacimiento evitas
    float p3;
    cout << "Ingrese dias: ";
    cin >> dias_max;
    cout << "Ingrese intentos de reproduccion deseados de angeles: ";
    cin >> m;
    cout << "Ingrese intentos de reproduccion deseados de evitas: ";
    cin >> n;
    cout << "Ingrese la probabilidad de nacimiendo de un angel: ";
    cin >> p1;
    cout << "Ingrese la probabilidad de nacimiendo de un evita: ";
    cin >> p2;
    cout << "Ingrese la probabilidad de nacimiendo de un evita berserker: ";
    cin >> p3;

    int mainPid = getpid();
    int childPid = 0;
    cout << "Main pid es: " << mainPid << " Terminal pid:" << getppid()  << endl;
    
    int adanPid = fork();
    cout << "Adan pid: " <<  adanPid << endl;
    if (adanPid == 0)
    {
        for (int i = 1; i < dias_max; i++)
        {    
            auto t = crearHijos(p1,1.1,m);

            auto [a, b, c, d] = t;
            cout << "Tipo: " << a << " Poder: " << b << " Pid: "<< c <<" PPID:"<< d<<endl;
        }
    }
    else
    {
        int evaPid = fork();
        cout << "Eva pid: " <<  evaPid << endl;
        if (evaPid == 0)
        {
            for (int i = 1; i < dias_max; i++)
            {    
                auto t = crearHijos(p2,p3,n);

                auto [a, b, c, d] = t;
                cout << "Tipo: " << a << " Poder: " << b << " Pid: "<< c <<" PPID:"<< d<<endl;
            }
        }
    }
    for(int x = 0; x < n + m; x++)
    {
        wait(NULL);
    }
    exit(0);
}

float generarNumeroRandom(){
    srand( time( NULL ) );
    float num = ( "%1.3f\n", ( rand( ) % 1001 ) / 1000.0f );

    return num;
}



tuple<string,double, int, int> crearHijos(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion)
{
    //preguntar si proBerserker es 0 entonces es Angel crear variable tipo(string) y crear tupla para devolver tipo, pid
    string tipo;
    tuple<string,double, int, int> result;
    
    if(probabilidadBerserker < 1.1)
    {
        tipo = "evita"; 
    }
    else
    {
        tipo = "angel";
    }
    int childPid = -1;
    for (int j = 0; j < intentosProcreacion; j++)
    {
        childPid = -1;
        float u = (tipo == "evita") ? 20 : 25;
	    float std = (tipo == "evita") ? 5 : 10;
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator (3);
        uniform_real_distribution<double> distribution(0.0,1.0);
        normal_distribution<double> ap_distribution(u,std);
        if ( tipo == "evita" && generarNumeroRandom() <= probabilidadBerserker)
        {
            result = make_tuple(tipo,ap_distribution(generator)*2, getpid(), getppid());
            return result;
            exit(0);
        }
        else{
            childPid = fork();
            if (childPid == 0){
                if(generarNumeroRandom() <= probabilidadNacimiento){
                    result = make_tuple(tipo,ap_distribution(generator), getpid(), getppid());
                    return result;
                }
            }
        }
    }
    if (childPid > 0)
    {
        for(int x = 0; x < intentosProcreacion; x++)
        {
            wait(NULL);
        }
        exit(0);
    }
    return result;
}