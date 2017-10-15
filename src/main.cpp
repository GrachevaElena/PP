#include "mpi.h"
#include <iostream>
#include <cstring>
#include <cstdio>

const int N = 1000000;

int Consistent(char* c, int n) {
	int res = 0;
	int i = 1;
	for (; i < n; i++) {
		if (c[i-1]!=' ' && c[i] == ' ') res++;
	}
	if (c[n-1] == ' ') res--;
	return res+1;
}

void Parallel(char* c, int n) {

	int size, total = 0;
	int argc; char** argv;

	MPI_Init(&argc, &argv);

	int res = 0, rank, count = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//printf("size=%d\n", size);
	if (size!=1) count = n / (size - 1);//... ... ... .  n=10, size=4, count=3 =10/(4-1)
	else count = n;
	//printf("count=%d\n", count);

	if (rank == 0) {

		for (int i = 1; i < size - 1; i++) {
			MPI_Send(c + i*count - 1, count + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
			//printf("send %d\n", i);
		}
		if (size!=1) MPI_Send(c + (size - 1)*count - 1, n - (size - 1)*count + 1, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD);
		//printf("send %d\n", size - 1);
	}

	int bufsize = count + 1;
	if (rank == size - 1) bufsize = n - (size - 1)*count + 1;

	char* buf = new char[bufsize];

	if (rank != 0) {
		MPI_Status status;
		MPI_Recv(buf, bufsize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
		//printf("recv %d\n####################  \"", rank);
		//for (int i = 0; i < bufsize; i++)
		//	std::cout << buf[i];
		//std::cout << "\"\n";
	}

	if (rank!=0) res = Consistent(buf, bufsize);
	else res = Consistent(c, count);

	if (rank != 0 && buf[0] != ' ') res--;
	//printf("proc %d calc %d\n", rank, res);

	delete[] buf;

	MPI_Reduce(&res, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	//printf("reduced %d\n", rank);


	if (rank == 0) std::cout /*<< "%%%%%%%%%%%%%%%%%%% parallel " */<< total << std::endl;;

	MPI_Finalize();

}



int main(int argc, char**argv) {

	char *c = argv[argc - 1];

	//std::cout<<Consistent(c, strlen(c))<<std::endl;
	Parallel(c, strlen(c));

	return 0;
}