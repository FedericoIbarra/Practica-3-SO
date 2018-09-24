#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define iteraciones 2000000000

void tfunc()
{
	int n;
	long double pi = 0;
	float signo = -1;

	for (n = 0; n < iteraciones; n++){
		signo *= -1;
		pi += signo / (2*n + 1);
	}

	pi *= 4;
	printf("π = %0.30Lf\n", pi);
}

int main(int argc, char const *argv[]) {

	struct timeval ts;
	long long stop_ts;
	long long start_ts;
	long long elapsed_time;

	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	tfunc();

	gettimeofday(&ts, NULL);

	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;


	printf("T = %llds\n", elapsed_time);


	return 0;
}
