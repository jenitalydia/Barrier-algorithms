MPICC = /opt/openmpi-1.4.3-gcc44/bin/mpicc             # default is CC = cc
CFLAGS = -g -Wall      # default is CFLAGS = [blank]
CC = gcc            # default is CPPFLAGS = [blank]
LDFLAGS =  -lm # default is LDFLAGS = [blank]


combine: combine.c
	$(MPICC) -o ./combine ./combine.c  -lgomp -fopenmp -lm -std=c99

dissemination: dissemination.c
	$(MPICC) -o ./dissemination ./dissemination.c -lgomp -fopenmp -lm -std=c99
# default linking command: $(CC) $(LDFLAGS) <foo>.o -o <foo>

tournament: tournament.c
	$(MPICC) -o ./tournament ./tournament.c -lgomp -fopenmp -lm -std=c99

centralized: centralized.c
	$(CC) -o ./centralized ./centralized.c -lgomp -fopenmp -lm -std=c99

dissemination_openmp: dissemination_openmp.c
	$(CC) -o ./dissemination_openmp ./dissemination_openmp.c -lgomp -fopenmp -lm -std=c99
clean:
	rm -f ./dissemination ./combine ./tournament ./dissemination_openmp ./centralized 
