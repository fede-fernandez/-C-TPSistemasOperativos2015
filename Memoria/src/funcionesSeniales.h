#ifndef FUNCIONESSENIALES_H_
#define FUNCIONESSENIALES_H_

#include <signal.h>

void tratarSenial();

void crearSeniales();

void prepararSenialVolcarRamALog(int signal);
void prepararSenialLimpiarTLB(int signal);
void prepararSenialLimpiarRAM(int signal);

void agregarSenialEnLaLista(int signal);

#endif /* FUNCIONESSENIALES_H_ */

