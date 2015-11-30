#ifndef FUNCIONESSENIALES_H_
#define FUNCIONESSENIALES_H_

#include <signal.h>
#include<commons/collections/list.h>

t_list* listaSeniales;
int idHijo;
int estado;

pthread_mutex_t mutexTurnoSenial;
pthread_mutex_t mutexLimpiarTLB;
pthread_mutex_t mutexLimpiarRam;
pthread_mutex_t mutexDump;

void tratarSenial(int signal);

void crearSeniales();

void prepararSenialVolcarRamALog();
void prepararSenialLimpiarTLB();
void prepararSenialLimpiarRAM();

void agregarSenialEnLaLista(int signal);

#endif /* FUNCIONESSENIALES_H_ */

