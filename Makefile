CC=/opt/nfs/mpich-3.2/bin/mpicc
CFLAGS=

OBJ = parallel_matrix_multiplication.o

all: parallel

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

parallel: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean run nodes runall

nodes:
	/opt/nfs/config/station206_name_list.sh 1 16 > nodes

run:
	/opt/nfs/mpich-3.2/bin/mpiexec -f nodes -n 5 ./parallel

runall: parallel nodes run

clean:
	rm -f *.o parallel nodes
