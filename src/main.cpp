#include "mpi.h"
#include <iostream>

const int N = 1000000;

int Consistent(char* c) {
	int res = 0;
	int i = 1;
	for (; i < N && c[i]; i++) {
		if (c[i-1]!=' ' && c[i] == ' ') res++;
	}
	if (!c[i] && c[i - 1] == ' ') res--;
	return res+1;
}

int Parallel(char* c, int* argc, char*** argv) {

	MPI_Init(argc, argv);

	std::cout << "Hello" << std::endl;

	MPI_Finalize();

	return 0;
}



int main(int argc, char**argv) {
	
	char c[N];

	std::cin.getline(c, N);
	std::cout << Consistent(c) << std::endl;

	std::cin.get();

	return 0;
}