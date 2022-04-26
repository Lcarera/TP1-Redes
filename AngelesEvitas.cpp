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
#include <fstream>

using namespace std;

float generarNumeroRandom();

tuple<string,double, int, int, string> crearHijos(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion);

void* share_mem(int size);

void verificarResultado(float eps, int maximoDias, atomic<float>* evitas, atomic<float>* angeles);

void agregarHijo(atomic<float>* lista, int dia, float poder, int maximoDias);

void experimento(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion, atomic<float>* lista, int maximoDias);

int main(int argc, char* argv[])
{
    // Declaracion de valores inciales
    int maximoDias = atoi(argv[1]); 
	int M = atoi(argv[2]); 
	int N = atoi(argv[3]); 
	float ps[3];
	ps[0] = stof(argv[4]);
	ps[1] = stof(argv[5]);
	ps[2] = stof(argv[6]);
	float eps = stof(argv[7]);

    int size = maximoDias;
	atomic<float> *evitas = (atomic<float> *) share_mem(size * sizeof(atomic<int>));
	atomic<float> *angeles = (atomic<float> *) share_mem(size * sizeof(atomic<int>));
	
	for(int i = 0;i < 2*size; i++)evitas[i] = 0;
	for(int i = 0;i < 2*size; i++)angeles[i] = 0;

	evitas[0] = 1;
	angeles[0] = 1;	

    //Comienzo de experimento
    int mainPid = getpid();
    int childPid = 0;
    cout << "Main pid es: " << mainPid << " Terminal pid:" << getppid()  << endl;
    
    int adanPid = fork();
    cout << "Adan pid: " <<  adanPid << endl;
    float probabilidadNacimiento;
    float probabilidadBerserker;
    int intentosProcreacion;
    atomic<float> *lista;
    if (adanPid == 0)
    {
        probabilidadNacimiento = ps[0];
        probabilidadBerserker = 1.1;
        intentosProcreacion = M; 
        lista = angeles;
    }
    else
    {
        int lilithPid = fork();
        cout << "Lilith pid: " <<  lilithPid << endl;
        if (lilithPid == 0)
        {
            probabilidadNacimiento = ps[1];
            probabilidadBerserker = ps[2];
            intentosProcreacion = N;
            lista = evitas;
        }
        else
        {
            wait(NULL);
            wait(NULL);
            std::ofstream myfile;
            myfile.open ("experimento.csv");
            myfile << "Parametros:\n";
            myfile << "Dias maximos: "<< maximoDias << ", cantidad de ensayos de reproducción de los Evitas: " << M << ", cantidad de ensayos de reproducción de los Ángeles: " << N << ", probabilidad de nacimiento para un Evita: " << ps[0] << ", probabilidad de nacimiento para un Ángel: " << ps[1] << ", probabilidad de que una evita se vuelva berserk en un día: " << ps[2] << ", diferencia de poder para dar positivo" << eps << "\n";
            myfile << "\n";
            for(int i = 0; i < maximoDias; i++)
            {
                myfile << "Dia: " << i << ", Cantidad Evitas: " << evitas[i] << ", Cantidad Angeles: " << angeles[i] << "\n";
            }
            myfile.close();
            verificarResultado(eps, maximoDias, evitas, angeles);
            exit(0);
        }
    }
   
    experimento(probabilidadNacimiento, probabilidadBerserker, intentosProcreacion, lista, maximoDias);
    exit(0);
}

tuple<string,double, int, int, string> crearHijos(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion)
{
    string tipo;
    tuple<string,double, int, int, string> result;
    string naturaleza = "normal";
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
        default_random_engine generator (seed);
        uniform_real_distribution<double> distribution(0.0,1.0);
        normal_distribution<double> ap_distribution(u,std);
        if(generarNumeroRandom() <= probabilidadNacimiento)
        {
            if ( tipo == "evita" && generarNumeroRandom() <= probabilidadBerserker)
            {
                childPid = fork();
                if (childPid == 0)
                {
                    naturaleza = "berserker";
                    result = make_tuple(tipo,ap_distribution(generator)*2, getpid(), getppid(), naturaleza);
                    return result;
                }
                
            }
            else
            {
                childPid = fork();
                if (childPid == 0)
                {
                    result = make_tuple(tipo,fabsf(ap_distribution(generator)), getpid(), getppid(), naturaleza);
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

void* share_mem(int size)
{
	// Vamos a pedir size * sizeof(atomic<int>) para reservar suficiendte memoria 
	// para un vector sizeof nos dice el tamaño del tipo atomic int.
    void * mem;
    if( MAP_FAILED == (mem = (atomic<float>*)mmap(NULL, sizeof(atomic<float>)*size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0) ) )
    {
        perror( "mmap failed" );
        exit( EXIT_FAILURE );
    }
    return mem;
}

float generarNumeroRandom()
{
    srand( time( NULL ) );
    float num = ( "%1.3f\n", ( rand( ) % 1001 ) / 1000.0f );

    return num;
}


void verificarResultado(float eps, int maximoDias, atomic<float>* evitas, atomic<float>* angeles)
{
    cout << "------------------RESULTADO FINAL ----------------" << endl;
    for(int i = 1; i < maximoDias; i++)
    {   
        evitas[i] = evitas[i] + evitas[i - 1];
        evitas[i + maximoDias] = evitas[i + maximoDias] + evitas[i - 1 + maximoDias];
        angeles[i] = angeles[i] + angeles[i - 1];
        angeles[i + maximoDias] = angeles[i + maximoDias] + angeles[i - 1 + maximoDias];
        cout << "DIA: "<< i << endl;
        cout << "CANTIDAD ANGELES NACIDOS: "<<angeles[i] - angeles[i - 1]<< endl;
        cout << "PODER TOTAL ANGELES: "<<angeles[i + maximoDias] << endl;
        cout << "CANTIDAD EVITAS NACIDOS: "<<evitas[i] - evitas[i - 1] << endl;
        cout << "PODER TOTAL EVITAS: "<<evitas[i + maximoDias]  << endl;
        if(fabsf(angeles[i + maximoDias] - evitas[i + maximoDias] ) < eps)
        {
            cout << "------------------SIMULACION EXITOSA ----------------" << endl;
            cout << "En el dia: " << i << endl;
            cout << "Hay: "<< angeles[i] << " angeles" << " , con un poder total de : "<< angeles[i + maximoDias] << endl;
            cout << "Hay: "<< evitas[i] << " evitas" << " , con un poder total de : "<< evitas[i + maximoDias] << endl;
            return;        
        }
    }                       
    cout << "------------------SIMULACION FALLIDA ----------------" << endl;
    cout << "No se encontro un dia de mismo poder" << endl;
    return;
}                                                                                       

void agregarHijo(atomic<float>* lista, int dia, float poder, int maximoDias)
{
    lista[dia] =  lista[dia] + 1;
    lista[dia + maximoDias] = lista[dia + maximoDias] + poder;
}

void experimento(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion, atomic<float>* lista, int maximoDias)
{
    for (int i = 1; i < maximoDias; i++)
    {    
        auto datosHijo = crearHijos(probabilidadNacimiento,probabilidadBerserker,intentosProcreacion);
        auto [tipo, poder, pid, ppid, naturaleza] = datosHijo;
        agregarHijo(lista, i , poder, maximoDias);
        cout << "Tipo: " << tipo << " Poder: " << poder << " Pid: "<< pid <<" PPID:"<< ppid<< " DIA: "<< i <<endl;      
        if(naturaleza == "berserker")
        {
            exit(0);
        }
        
    }
    return;
}
