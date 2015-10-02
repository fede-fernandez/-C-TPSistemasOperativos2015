/*
 * semaforos.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef COMMONSDEASEDIO_SEMAFOROS_H_
#define COMMONSDEASEDIO_SEMAFOROS_H_

#include <semaphore.h>
#include <pthread.h>

/**************************Mutex**************************************/

void inicializarMutex(pthread_mutex_t* mutex);

void destruirMutex(pthread_mutex_t* mutex);

void bloquearRecurso(pthread_mutex_t* mutex);//Es equivalente a wait()

void liberarRecurso(pthread_mutex_t* mutex);//Es equivalente a signal()


/***************************Semaforos***********************************/

void inicializarSemaforo(sem_t* semaforo, int valorInicial);

void destruirSemaforo(sem_t* semaforo);

void decrementarSemaforo(sem_t* semaforo);//Es equivalente a wait()

void incrementarSemaforo(sem_t* semaforo);//Es equivalente a signal()

#endif /* COMMONSDEASEDIO_SEMAFOROS_H_ */
