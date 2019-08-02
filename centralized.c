#include <stdlib.h>
#include <stdio.h>
#include<math.h>
#include <sys/time.h>
#include<string.h>
#include<omp.h>


void centralized_barrier(int*, int*,int*,int);
int main(int argc, char *argv[])
{
	

int no_of_threads = atoi(argv[1]);
int no_of_barriers_openmp = atoi(argv[2]);
	double start_time, end_time, total_time=0;

	int count = no_of_threads;
	int sense = 1;
	omp_set_num_threads(count);
	
	#pragma omp parallel

  	{
  		int local_sense = 1;
  		int i;
  		for(i=0;i<no_of_barriers_openmp;i++){
	  		start_time = omp_get_wtime();
	  		centralized_barrier(&local_sense,&sense,&count,no_of_threads);
	  		printf("Thread id is %d\n", omp_get_thread_num());
	  		end_time = omp_get_wtime();
	  		total_time+=(end_time-start_time);
	  	}

  	}
  	
  	printf("The average time take is %f\n",total_time/no_of_barriers_openmp);
	 

	return 0;
}

void centralized_barrier(int* local_sense,int* sense, int *count, int no_of_threads){
	
	*local_sense=!(*local_sense);
	if(__sync_fetch_and_sub(count,1)==1){
		*count = no_of_threads;
		*sense = *local_sense;
	}
	else{

		while(*sense!=*local_sense);
	}
}
