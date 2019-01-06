
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include<math.h>
#include <sys/time.h>
#include<string.h>
#include<omp.h>
#include <time.h>

void gtmpi_init(int *, int *, int *);
void gtmpi_barrier(int, int, int);
void gtmpi_finalize();
int source(int,int,int);
int destination(int,int,int);
int logbase2(int);
void centralized_barrier(int*, int*,int*,int);


int main(int argc, char *argv[])
{
	int ret_val;

	int pid, rounds, no_processors;
	int i,j,k;
	ret_val = MPI_Init(&argc,&argv);


if(argc < 4){
	printf("This ain't a test program!:) Please enter three arguments with first one as No. of Threads, second as No of OpenMpBarriers, third as No of MPI Barriers.  Bye for now!\n");
	exit(1);
}
printf("This is a test");
int no_of_threads = atoi(argv[1]);
int no_of_barriers_openmp = atoi(argv[2]);
int no_of_barriers_mpi = atoi(argv[3]);

	//omp_set_num_threads(no_of_threads);

  gtmpi_init(&pid, &rounds, &no_processors);
  omp_set_num_threads(no_of_threads);	
  int count = no_of_threads;
  int sense = 1;
  double start_time, end_time, total_time=0.0;
  double start_barrier_time, end_barrier_time;
  double diff;
  start_time = omp_get_wtime();
  int count2 = 0;
  start_time = MPI_Wtime();
  while(count2 < no_of_barriers_mpi)
    {
	#pragma omp parallel

  	{
  		int local_sense = 1;
  		int i;
	  	centralized_barrier(&local_sense,&sense,&count,no_of_threads);
	  	printf("Thread id is %d\n", omp_get_thread_num());
	  	
  	}
              gtmpi_barrier(pid, rounds, no_processors);
              count2++;
            end_barrier_time = MPI_Wtime();
            total_time += end_barrier_time - start_barrier_time;
            printf("Time taken by barrier %d is %.2lf\n", count2, (end_barrier_time - start_barrier_time));
    
  }
    end_time = MPI_Wtime();
    printf("\n");
    printf("Time taken for all the barriers to complete is %f\n",(end_time-start_time));
    printf("The average time taken is %f\n",(end_time-start_time)/no_of_barriers_mpi);
	//MCS_TreeBarrier_Finalize(threads, no_of_threads);
	gtmpi_finalize();

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





int logbase2(int x){
	return (x>1)? 1+logbase2(x/2):0;
}

void gtmpi_init(int *pid, int *rounds, int *P){

	MPI_Comm_size(MPI_COMM_WORLD, P);
	MPI_Comm_rank(MPI_COMM_WORLD, pid);
	double num_rounds = logbase2(*P);
	*rounds = (int)ceil(num_rounds);


}

int source(int id,int i, int P){

	int mid,result;
	mid = id+pow(2,i);
	result = mid%P;
	return result;
}
int destination(int id,int i, int P){
	int mid,result;
	mid = id-pow(2,i);
	result = mid%P;
	if(result < 0){
		result += P;
	}
	return result;
}
void gtmpi_barrier(int pid, int rounds, int P){

	int ans;
	int receiver;
	int sender;
	MPI_Status stat;


	for(int i=0;i<rounds;i++){
		ans = source(pid,i, P);
		MPI_Send(&sender, 1, MPI_INT, ans, 0, MPI_COMM_WORLD);
		printf("Processor %d SENT TO Processor %d\n",pid, ans);
		ans = destination(pid,i, P);
		MPI_Recv(&receiver, 1, MPI_INT, ans, 0, MPI_COMM_WORLD, &stat);
		printf("Processor %d HAS RECEIVED FROM %d\n",pid, ans);

	}
}

void gtmpi_finalize()
{
	 MPI_Finalize();
}

