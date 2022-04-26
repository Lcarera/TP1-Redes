#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <atomic>

using namespace std;

float generarNumeroRandom();

int main()
{
    int dias_max;
    int dia;
    int m; //Intentos de reproduccion, cantidad de hijos que van a intentar tener
    int n; //Intentos de reproduccion, cantidad de hijos que van a intentar tener
    float p1; //Probabilidad de nacimiento angeles
    float p2; //Probabilidad de nacimiento evitas
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

    int mainPid = getpid();
    int childPid = 0;
    cout << "Main pid es: " << mainPid << " Terminal pid:" << getppid()  << endl;
    
    int adanPid = fork();
    cout << "Adan pid: " <<  adanPid << endl;
    if (adanPid == 0)
    {
        for (int i = 1; i < dias_max; i++)
        {    
            for (int j = 0; j < m; j++)
            {
                childPid = fork();
                if (childPid == 0){
                    if(generarNumeroRandom() <= p1){
                        cout << "Mi pid es: " << getpid()  << ", mi ppid es: " << getppid() << ", dia: " << i << ", hijo numero: " << j << " soy angel" << endl;
                        break;
                    }
                    
                }
            }
            if (childPid > 0)
            {
                for(int x = 0; x < n; x++)
                {
                    wait(NULL);
                }
                exit(0);
            }
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
                for (int j = 0; j < n; j++)
                {
                    childPid = fork();
                    if (childPid == 0){
                        if(generarNumeroRandom() <= p2){
                            cout << "Mi pid es: " << getpid()  << ", mi ppid es: " << getppid() << ", dia: " << i << ", hijo numero: " << j << " soy evita" << endl;
                            break;
                        }
                    }
                }
                if (childPid > 0)
                {
                    for(int x = 0; x < n; x++)
                    {
                        wait(NULL);
                    }
                    exit(0);
                }
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

tuple<string,int> crearHijos(float probabilidadNacimiento, float probabilidadBerserker, int intentosProcreacion)
{
    //preguntar si proBerserker es 0 entonces es Angel crear variable tipo(string) y crear tupla para devolver tipo, pid
    string tipo;
    tuple<string,int> result;
    
    if(probabilidadBerserker != NULL)
    {
        tipo = "evita"; 
    }
    else
    {
        tipo = "angel";
    }
    for (int j = 0; j < intentosProcreacion; j++)
    {
        childPid = fork();
        if (childPid == 0){
            if(generarNumeroRandom() <= probabilidadNacimiento){
                cout << "Mi pid es: " << getpid()  << ", mi ppid es: " << getppid() << ", dia: " << i << ", hijo numero: " << j << ", soy " << tipo << endl;
                break;
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
}