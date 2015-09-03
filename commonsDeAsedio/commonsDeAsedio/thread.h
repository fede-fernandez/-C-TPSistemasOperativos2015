
#ifndef THREAD_H_
#define THREAD_H_

	#include <pthread.h>
	int crearThread(pthread_t* thread,void* funcionAEjecutar,void* parametros);//Retorna 0 si el thread se creo correctamente.
	void destruirThread(pthread_t thread);


#endif /* THREAD_H_ */
