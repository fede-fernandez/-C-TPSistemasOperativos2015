#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int crearThread(pthread_t* thread,void* funcionAEjecutar,void* parametros){
	int retornoDeThread = pthread_create(thread,NULL,funcionAEjecutar,parametros);
	return retornoDeThread;
}

void destruirThread(pthread_t thread){
	pthread_join(thread,NULL);
}
