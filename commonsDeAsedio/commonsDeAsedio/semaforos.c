
#include "semaforos.h"



void inicializarMutex(pthread_mutex_t* mutex){
	 pthread_mutex_init(mutex, NULL);
}


void destruirMutex(pthread_mutex_t* mutex){
	pthread_mutex_destroy(mutex);
}

void bloquearRecurso(pthread_mutex_t* mutex){
	pthread_mutex_lock(mutex);

}

void liberarRecurso(pthread_mutex_t* mutex){
	pthread_mutex_unlock( mutex );
}


void inicializarSemaforo(sem_t* semaforo, int valorInicial){
	 sem_init(semaforo,0,valorInicial);
}

void destruirSemaforo(sem_t* semaforo){
	sem_destroy(semaforo);
}

void decrementarSemaforo(sem_t* semaforo){
	sem_wait(semaforo);
}

void incrementarSemaforo(sem_t* semaforo){
	sem_post(semaforo);
}



