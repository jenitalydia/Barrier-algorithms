#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include<math.h>
#include <sys/time.h>

struct flags {
	int myflags[2][1000];
	int *partnerflags[2][1000];
};


int source(int,int,int);

int logbase2(int);

void dissemination_barrier(struct flags *local_flags,int *sense, int *parity, int rounds){
	int i;
	for(i=0;i<rounds;i++){
		*local_flags->partnerflags[*parity][i] = (*sense);
		while(local_flags->myflags[*parity][i]!=(*sense));
	}
	if(*parity==1){
		*sense = !(*sense);

	}
	*parity = 1-(*parity);
}

int main(int argc, char *argv[])
{
	
	double start_time, end_time, total_time=0;
	int no_of_threads = atoi(argv[1]);
	int no_of_barriers_openmp = atoi(argv[2]);

	int rounds = ceil(log2(no_of_threads));
	struct flags allnodes[no_of_threads]; 
	int i=0;
	omp_set_num_threads(no_of_threads);
	for(i=0;i<no_of_threads;i++){
		int j;
		for(j=0;j<rounds;j++){
			allnodes[i].myflags[0][j] = 0;
			allnodes[i].myflags[1][j] = 0;
		}
	}

	#pragma omp parallel

  	{	
  		int parity = 0;
  		int sense = 1;
  		struct flags *local_flags;
  		int id = omp_get_thread_num();
  		int i=0;
  		int k=0;
  		#pragma omp critical
  		{
  			for(k=0;k<no_of_threads;k++){
  				int j;
  				
  				for(j=0;j<rounds;j++){
  					int ans = source(id,j,no_of_threads);
  					if(k==ans){
  						allnodes[id].partnerflags[0][j] = &allnodes[k].myflags[0][j];
  						allnodes[id].partnerflags[1][j] = &allnodes[k].myflags[1][j];
  					}
  	
  				}
  	
  			}
  		}
  		local_flags=&allnodes[id];
		for(i=0;i<no_of_barriers_openmp;i++){

			start_time = omp_get_wtime();
			dissemination_barrier(local_flags,&sense,&parity,rounds);
			printf("Thread id is %d\n", omp_get_thread_num());
			end_time = omp_get_wtime();
			total_time+=(end_time-start_time);
		}
	  	}
	
	printf("The average time taken is %f\n",total_time/no_of_barriers_openmp);
	 
	
}



int logbase2(int x){	
	return (x>1)? 1+logbase2(x/2):0;
}

int source(int pid,int i, int P){
	
	int mid,result;
	mid = pid+pow(2,i);
	result = mid%P;
	return result;
}
