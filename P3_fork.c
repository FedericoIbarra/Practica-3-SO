/*
Práctica 3. Versión Paralela con Procesos:
  Ricardo Delsordo  IE702570
  Federico Ibarra   IS702981

Referencias:
 - http://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

/**Number of iterations (to perform calculation of PI)*/
#define NUMITER 2000000000
/**Number of processes*/
int NPROCCS = 0;

/**Variable to store values into the shared memory*/
long double *shm, *s;

/**Key to access shared memory*/
key_t key = 1984;
/**Size of shared memory*/
int size = 256;
/**Shared memory flag*/
int shmflg = (IPC_CREAT | 0666);
/**Shared memory ID*/
int shmid;

/**Function to calculate PI through fork()*/
void tfunc(int *n_pid)
{
  /**Variable to perform  loop calculations*/
  int n;
  /**Value of PI*/
	long double pi = 0;
  /**Variable to set alternation of sign*/
	float signo = -1;
  /**Defines start of calculation according to the process*/
  int inicio = (*n_pid)*(NUMITER/NPROCCS);
  /**Defines end of calculation according to the process*/
  int fin = (*n_pid+1)*(NUMITER/NPROCCS);

  /**Recovery of shared memory ID*/
  if((shmid = shmget(key, size*NPROCCS, shmflg)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  /**Recovey of value inside location of shared memory*/
  if((shm = shmat(shmid, NULL, 0)) == (long double *) -1)
  {
    perror("shmat");
    exit(1);
  }
  /**Recovey of value inside location of shared memory*/
  s = shm;

  /**Loop to calculate PI*/
  for (n = inicio; n < fin; n++){
		signo *= -1;
		pi += signo / (2*n + 1);
	}
  /**Multiply to get value of PI (according to Leibniz)*/
	pi *= 4;

  /**Moves from segemts of shared memory (one for each process)*/
  s += *n_pid;
  /**Saves segment of PI calculation into shared memory*/
  *s = pi;

}

int main(int argc, char const *argv[])
{
  /**Variables to indicate n child, and parent, process (respectively)*/
  int i, parent_id;

  /**Variable sto get time of execution*/
	struct timeval ts;
	long long start_ts;      /**Start of execution time*/
	long long stop_ts;       /**End of execution time*/
	long long elapsed_time;  /**Total execution time*/

  /**Value of PI*/
  long double pi = 0.00;

  /**Gets total number of available cores*/
  NPROCCS =  get_nprocs();
  /**Declaration of processes*/
  pid_t pid[NPROCCS];

  /**Sets initial time*/
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec;

  /**Recovery of shared memory ID*/
  if((shmid = shmget(key, size*NPROCCS, shmflg)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  /**Recovey of value inside location of shared memory*/
  if((shm = shmat(shmid, NULL, 0)) == (long double *) -1)
  {
    perror("shmat");
    exit(1);
  }

  /**Calculation of PI through fork() if there is more than one core available*/
  if(NPROCCS > 1)
  {
    /** Create N number of processes.*/
    for (i = 1; i < NPROCCS; i++) {
      /** Only fork if the root is the parent.*/
      pid[i] = fork();
      if(pid[i] == 0) {
        /** Executes calculation of PI by child process*/
        tfunc(&i);
        /**Ends child process*/
        exit(0);
      }
      else
      {
        /** Executes calculation of PI by parent process*/
        parent_id = 0;
        tfunc(&parent_id);
      }
    }
  }
  else
  {
    /**Execution of calculaction of PI if there is only one core available*/
    parent_id = 0;
    tfunc(&parent_id);
  }

  /**Recovey of value inside location of shared memory*/
  if((shm = shmat(shmid, NULL, 0)) == (long double *) -1)
  {
    perror("shmat");
    exit(1);
  }
  /**Recovey of value inside location of shared memory*/
  s = shm;

  /**Loop tp sum all the segments of PI calculatios inside the shared memory*/
  for(i = 0; i < NPROCCS; i++)
  {
    pi += *s;
    s++;
  }

  /**Gets final time*/
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec;

  /**Obtains time of execution*/
	elapsed_time = stop_ts - start_ts;

  /**Prints value (calculated) of PI*/
  printf("π = %0.30Lf\n", pi);
  /**Prints time of execution*/
	printf("T = %llds\n", elapsed_time);

	return 0;
}
