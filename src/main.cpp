#include "mpi.h"
#include <iostream>

int main(int argc, char**argv) {
	
	MPI_Init(&argc, &argv);

	std::cout << "Hello" << std::endl;

	MPI_Finalize();

	//std::cin.get();
}