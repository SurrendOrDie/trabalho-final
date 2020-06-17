#include "Header.h"
/*********************************************************************************************************/
/**SEMÁFOROS*******************************************************************************************/
/********************************************************************************************************/
void inicializarSemaforo(semaforo_t * sem, int v)
{
	sem_init(sem,0,v);
}
void Esperar(semaforo_t * sem)
{
	sem_wait(sem);
}
void Assinalar(semaforo_t * sem)
{
	sem_post(sem);
}
void destruirSemaforo(semaforo_t * sem)
{
	sem_destroy(sem);
}

/************************************************************************************************************/
/**TRINCO (mutex)*******************************************************************************************/
/************************************************************************************************************/
void inicializarMutex(mutex_t * mutex) 
{ 
	pthread_mutex_init(mutex,NULL); 
}
void Fechar(mutex_t * mutex)
{ 
	pthread_mutex_lock(mutex); 
}
void Abrir(mutex_t * mutex)
{ 
	pthread_mutex_unlock(mutex); 
}
void destruirMutex(mutex_t * mutex)
{
	pthread_mutex_destroy(mutex);
}
