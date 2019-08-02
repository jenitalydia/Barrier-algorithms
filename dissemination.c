#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include<math.h>
#include <sys/time.h>


int P;
double num_rounds;
int ans,i;
int size;
int rounds;
int id;


void gtmpi_init();
void gtmpi_barrier();
void gtmpi_finalize();
int source(int,int);
int destination(int,int);
int logbase2(int);



int main(int argc, char *argv[])
{
	int ret_val;
	int i;
	double time_before, time_after, total_time=0;
	ret_val = MPI_Init(&argc,&argv);
	

	gtmpi_init();
	int no_of_barriers = atoi(argv[1]);
	if (ret_val != MPI_SUCCESS)
	{
		printf("MPI failed\n");
		MPI_Abort(MPI_COMM_WORLD, ret_val);
	}
	for(i=0;i<no_of_barriers;i++){
		time_before = MPI_Wtime();
		gtmpi_barrier();
		time_after = MPI_Wtime();
		total_time+=(time_after-time_before);
		printf("Processor %d is on barrier %d and time taken is %f\n",id,i,time_after-time_before);

	}
	printf("Average Time =  %f\n",total_time/1000);
	gtmpi_finalize();
	return 0;
}

int logbase2(int x){	
	return (x>1)? 1+logbase2(x/2):0;
}

void gtmpi_init(){

	MPI_Comm_size(MPI_COMM_WORLD,&P);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
   // printf("size = %d\n", P);
	num_rounds = logbase2(P);
	rounds = (int)ceil(num_rounds);
}
int source(int id,int i){
	
	int mid,result;
	mid = id+pow(2,i);
	result = mid%P;
	return result;
}
int destination(int id,int i){
	int mid,result;
	mid = id-pow(2,i);
	result = mid%P;
	if(result < 0){
		result += P;
	}
	return result;
}
void gtmpi_barrier(){

	int receiver;
	int sender;
	MPI_Status stat;

	for(i=0;i<rounds;i++){
		ans = source(id,i); 
		MPI_Send(&sender, 1, MPI_INT, ans, 0, MPI_COMM_WORLD);
		ans = destination(id,i);
		MPI_Recv(&receiver, 1, MPI_INT, ans, 0, MPI_COMM_WORLD, &stat);
		
	}

}

void gtmpi_finalize()
{
	 MPI_Finalize();
}
