#include "mpi.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <ctime>

const int N = 1000000000;
const int K = 10000;

int Consistent(char* c, int n) {
	int res = 0;
	int i = 1;
	for (; i < n; i++) {
		if (c[i-1]!=' ' && c[i] == ' ') res++;
	}
	if (c[n-1] == ' ') res--;
	return res+1;
}

void Parallel(char* c, int n, int size, int rank) {

	double t1;
	if (rank == 0) t1 = MPI_Wtime();

	int total = 0;

	int res = 0, count = 0;

	//printf("size=%d\n", size);
	if (rank == 0) {
		if (size != 1) count = n / (size - 1);//... ... ... .  n=10, size=4, count=3 =10/(4-1)
		else count = n - 1;
	}
	//printf("rank=%d count=%d\n", rank, count);

	if (rank == 0) {

		for (int i = 1; i < size - 1; i++) {
			MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(c + i*count - 1, count + 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
			//printf("send %d\n", i);
		}
		if (size != 1) {
			MPI_Send(&count, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
			MPI_Send(&n, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
			MPI_Send(c + (size - 1)*count - 1, n - (size - 1)*count + 1, MPI_CHAR, size - 1, 1, MPI_COMM_WORLD);
			//printf("send %d\n", size - 1);
		}
	}

	int bufsize = count+1;
	if (size!=1 && rank == size - 1) {
		MPI_Status status;
		MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		bufsize = n - (size - 1)*count + 1;
		//printf("rank=%d bufsize=%d\n", rank, bufsize);
	}
	else if (rank!=0){
		MPI_Status status;
		MPI_Recv(&bufsize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		bufsize++;
		//printf("rank=%d bufsize=%d\n", rank, bufsize);
	}

	char* buf = new char[bufsize];

	if (rank != 0) {
		MPI_Status status;
		MPI_Recv(buf, bufsize, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
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

	if (rank == 0) {
		std::cout << "Parallel version" << std::endl;
		std::cout <<  total << std::endl;
		double t2 = MPI_Wtime();
		std::cout << "parallel time " << t2 - t1 << std::endl;
	}
}



int main(int argc, char**argv) {

	MPI_Init(&argc, &argv);

	int size, rank, n=0;
	char* c=0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		std::cout << argc << ' ' << argv << std::endl;
		if (argc > 1) {
			c = argv[argc - 1];
			n = strlen(c);
		}
		else if (argc==1){
			c = new char[N];
			srand(time(0));
			for (int i = 0; i < N; i++)
				c[i] = 'a';
			for (int i = 0; i < K; i++) 
				c[rand() % N] = ' ';
			n = N;
		}
		std::cout << "Consistent version" << std::endl;
		double t1 = MPI_Wtime();
		std::cout<<Consistent(c, n)<<std::endl;
		double t2 = MPI_Wtime();
		std::cout << "consistent time=" <<t2-t1<< std::endl << std::endl;
	}


	Parallel(c, n, size, rank);


	if (rank==0 && argc == 1) delete[] c;

	MPI_Finalize();

	return 0;
}