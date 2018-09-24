/*
Práctica 3. Versión Paralela con Hilos POSIX:
  Ricardo Delsordo  IE702570
  Federico Ibarra   IS702981

Referencia:
 - https://stackoverflow.com/questions/4586405/how-to-get-the-number-of-cpus-in-linux-using-c
 - http://cursos.iteso.mx/mod/folder/view.php?id=974386
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/sysinfo.h>

/**Number of iterations (to perform calculation of PI)*/
#define NUMITER 2000000000
/**Number of threads*/
int NTHREADS = 0;
/**Value of PI*/
long double pi = 0;

/**Thread mutex*/
pthread_mutex_t m;

/**Thread function*/
void *tfunc(void *args)
{
  /**thread number*/
  int nthread=*((int *) args);
  /**Variable to perform  loop calculations*/
  int n;
  /**Defines start of calculation according to the thread*/
  int inicio = nthread*(NUMITER/NTHREADS);
  /**Defines end of calculation according to the thread*/
  int fin = (nthread+1)*(NUMITER/NTHREADS);
  /**Variable to set alternation of sign*/
	float signo = -1;

  /**Thread lock (exclusion concurrency)*/
  long double x = 0;

  /**Loop to calculate PI*/
  for (n = inicio; n < fin; n++)
  {
    signo *= -1;
		x += signo / (2*n + 1);
	}

  /**Multiply to get value of PI (according to Leibniz)*/
  pthread_mutex_lock(&m);
  /**Thread lock (exclusion concurrency)*/
  pi += x;
  pthread_mutex_unlock(&m);
}

int main(int argc, char const *argv[])
{

  /**Variable sto get time of execution*/
	struct timeval ts;
	long long start_ts;      /**Start of execution time*/
	long long stop_ts;       /**End of execution time*/
	long long elapsed_time;  /**Total execution time*/

  /**Iterations variable*/
  int i;
  /**Gets total number of available cores*/
  NTHREADS =  get_nprocs();

  /**Declaration of threads*/
  pthread_t tid[NTHREADS];
  int parArr[NTHREADS];

  /**Sets initial time*/
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec;

  /**Creation of threads to execute*/
  for(i=0;i<NTHREADS;i++)
	{
		parArr[i]=i;
		pthread_create(&tid[i],NULL,tfunc,(void *) &parArr[i]);
	}

  /**Waits for thread termination*/
	for(i=0;i<NTHREADS-1;i++)
  {
		pthread_join(tid[i],NULL);
  }

  /**Gets final time*/
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec;

  /**Obtains time of execution*/
	elapsed_time = stop_ts - start_ts;

  pi *= 4;
  /**Prints value (calculated) of PI*/
  printf("π = %0.30Lf\n", pi);
  /**Prints time of execution*/
	printf("T = %llds\n", elapsed_time);

	return 0;
}
