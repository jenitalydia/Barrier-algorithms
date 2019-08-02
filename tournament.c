#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include<math.h>
#include <sys/time.h>



void gtmpi_init();
void gtmpi_barrier();
void gtmpi_finalize();

int logbase2(int);


struct rounds {
	int role;
	int opponent;
	int sendbuf;
	int recbuf;
	MPI_Status stats;
	
};

 

int P;
int num_in_rounds;
int ans,i;
int size;
int WINNER = 0;
int LOSER = 1;
int BYE = 2;
int CHAMPION = 3;
int DROPOUT = 4;

int id;

struct rounds *in_round;


int main(int argc, char *argv[]) {
	double time_before, time_after, total_time=0;
	int ret_val;
	int i;
	ret_val = MPI_Init(&argc,&argv);
	
	gtmpi_init();
	
	if (ret_val != MPI_SUCCESS) {
		printf("MPI failed\n");
		MPI_Abort(MPI_COMM_WORLD, ret_val);
	}
	for(i = 0; i < 1000; i++) {
		time_before = MPI_Wtime();
		gtmpi_barrier();
		time_after = MPI_Wtime();
		total_time+=(time_after-time_before);
		printf("Processor %d is at barrier %d and the time taken is %f\n",id,i,time_after-time_before);	

	}
	printf("Average Time =  %f\n",total_time/1000);
	gtmpi_finalize();
	return 0;
}
int logbase2(int x) {	
	return (x>1)? 1+logbase2(x/2):0;
}

void gtmpi_init(){
	
	int k;
	MPI_Comm_size(MPI_COMM_WORLD,&P);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
   
	num_in_rounds = (int) ceil(logbase2(P));

	in_round = calloc(num_in_rounds+1, sizeof(struct rounds));
	
	for(k = 0; k <= num_in_rounds; k++) {

		in_round[k].sendbuf = 1;
		if(k == 0){
			in_round[k].role = DROPOUT;
		}
		
			if(k>0){
				if(id % (int)pow(2,k)==0){
					if(id+ pow(2,k-1) < P && pow(2,k)<P){
						in_round[k].role = WINNER;
						in_round[k].opponent = id + pow(2,k-1);
						
					}
				   if(id+pow(2,k-1)>=P){
						in_round[k].role = BYE;
					}
				}
				if(id % (int)pow(2,k) == pow(2,k-1)){
					in_round[k].role = LOSER;
					in_round[k].opponent = id - pow(2,k-1);
				}
				if(id == 0 && (int) pow(2,k)>=P)
				{
					in_round[k].role = CHAMPION;
						
					in_round[k].opponent = id + pow(2,k-1);
						
				}
		}

	}
}

void gtmpi_barrier() {
	int i = 1;
	int flag = 0;

	while(i <= num_in_rounds){

		if(in_round[i].role == LOSER){
			
			MPI_Send(&in_round[i].sendbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD);
			
			MPI_Recv(&in_round[i].recbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD, &in_round[i].stats);
			
			break;
		}
		if(in_round[i].role == WINNER){
			
			MPI_Recv(&in_round[i].recbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD, &in_round[i].stats);
			
		}
		if(in_round[i].role == CHAMPION){
			
			MPI_Recv(&in_round[i].recbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD, &in_round[i].stats);
			
			MPI_Send(&in_round[i].sendbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD);
			
			break;
		}
		if(in_round[i].role == BYE)
			continue;
		if(in_round[i].role == DROPOUT){

			continue;
		}

		 i++;
	}

	
	while(i > 0) {
		i--;
		if(in_round[i].role == WINNER) {
			
			MPI_Send(&in_round[i].sendbuf, 1, MPI_INT, in_round[i].opponent, i, MPI_COMM_WORLD);
			
			continue;
		}
		
		if(in_round[i].role == DROPOUT)
		{
			break;
		}
		if(in_round[i].role == CHAMPION || in_round[i].role == LOSER ){
			continue;
		}
		
		if(in_round[i].role ==  BYE){
		continue;
		}
	}
}

void gtmpi_finalize()
{
	free(in_round);
	MPI_Finalize();
}
