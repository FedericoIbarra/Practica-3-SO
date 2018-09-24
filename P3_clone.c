#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <malloc.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <linux/sched.h>


#define NUMITER 2000000000
#define FIBER_STACK 1024*1024

/** Global Pi numer. */
long double globalPi = 0;
/** Number of avaliable Cores on the system. */
int processors;

/** Calculate Pi number with Leibniz series. */
int tfunc(void * args)
{
  /** Index for looping. */
	int n;

  /** Sign for the series to properly calculate. */
  float sign = -1;

  /** Partial Pi value. */
	long double pi = 0;

	for (n = 0; n < NUMITER/processors; n++){
		sign *= -1;
		pi += sign / (2*n + 1);
	}

  /** Add GlobalPi a partial value. */
  globalPi += pi;

  return 0;
}



int main(int argc, char const *argv[]) {

  /** Variable sto get time of execution*/
	struct timeval ts;
  /** Start of execution time. */
	long long stop_ts;
  /** End of execution time.*/
	long long start_ts;
  /** Total execution time.*/
	long long elapsed_time;

  /** Portion of the memory used by the clone() function.*/
	void * stack;

  /** Return status of n clone() functions. */
	int * pid;

  /** Index for recursion. */
	int i;

  /** Get the Number of processors on the system. */
  processors = get_nprocs();

  /** Memory allocation.*/
  stack = malloc( FIBER_STACK * processors);
	pid = malloc(sizeof(int) *  processors);

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec;


  /** Done i number of clones. */
	for(i = 0; i < 4; i++){
    pid[i] = clone(tfunc, (char*) stack + (FIBER_STACK*(i+1)), SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_VM , 0 );
  }

	for ( i= 0; i < 4; i++) {
		waitpid(pid[i], NULL, 0);
	}

	free(stack);

	gettimeofday(&ts, NULL);

	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;



  globalPi *= 4/processors;

	printf("π = %0.30Lf\n", globalPi);
  printf("T = %lld\n", elapsed_time);

	return 0;
}
