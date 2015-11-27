#ifndef FUNCIONESSENIALES_H_
#define FUNCIONESSENIALES_H_

#include <signal.h>

void imprimirYElegirOpcionesSeniales();
void elegirOpcion();

void tratarSenial();

void crearSeniales();

void prepararSenialVolcarRamALog(int signal);
void prepararSenialLimpiarTLB(int signal);
void prepararSenialLimpiarRAM(int signal);

#endif /* FUNCIONESSENIALES_H_ */

