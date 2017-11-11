#include "mpi.h"
#include <iostream>
#include <cstdlib>

#define MAIN_RANK 0

const int LEN = 100;
int n[2];
int sourceCoords[2], destCoords[2];
char* message;
int len = 0;

MPI_Comm myComm;
int rank;

void TransformArgs(int& argc, char**&argv) {
	n[0]=atoi(argv[1]);
	n[1] = atoi(argv[2]);
	sourceCoords[0] = atoi(argv[3]);
	sourceCoords[1] = atoi(argv[4]);
	destCoords[0] = atoi(argv[5]);
	destCoords[1] = atoi(argv[6]);
	message = argv[7];
	len = strlen(message);
}

void CreateNetwork() {
	int periods[2] = { true,true };
	MPI_Cart_create(MPI_COMM_WORLD, 2, n, periods, false, &myComm);

	MPI_Comm_rank(myComm, &rank);

	if (rank == MAIN_RANK) std::cout << "main process " << rank << ": network created\n";
}

void DetProcesses(int& source, int& dest) {
	MPI_Cart_rank(myComm, sourceCoords, &source);
	MPI_Cart_rank(myComm, destCoords, &dest);

	if (rank == MAIN_RANK) {
		std::cout << "main process " << rank << ": rank of source process (" << sourceCoords[0] << "," << sourceCoords[1] << ") is " << source << std::endl;
		std::cout << "main process " << rank << ": rank of dest process (" << destCoords[0] << "," << destCoords[1] << ") is " << dest << std::endl;
	}
}

void AllocateBuffer(char*& buffer) {
	buffer = new char[len];
}

void DeleteBuffer(char*& buffer) {
	delete[] buffer;
}

void PrintMessage(int rank, char*& buffer) {
	std::cout <<"process "<<rank<<": message is \""<< buffer <<"\""<< std::endl;
}

void SendMessage() {
	int dest, source;
	DetProcesses(source, dest);

	if (rank == source) MPI_Send(message, len, MPI_CHAR, dest, 1, myComm);
	if (rank == dest) {
		char* buffer;
		MPI_Status status;
		AllocateBuffer(buffer);
		MPI_Recv(buffer, len, MPI_CHAR, source, 1, myComm, &status);
		PrintMessage(rank,buffer);
		DeleteBuffer(buffer);
	}
}

int main(int argc, char**argv) {

	MPI_Init(&argc, &argv);

	TransformArgs(argc, argv);
	CreateNetwork();

	SendMessage();

	MPI_Finalize();

	return 0;
}