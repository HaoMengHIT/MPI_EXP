#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <time.h>
using namespace std;

int main(int argc ,char* argv[])
{
	int comm_sz;
	int my_rank;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	int parNum = atoi(argv[1]);
	int size = atoi(argv[2]);


	if(my_rank == 0)
	{

		char* sending = new char[size];
		double calcu_time;
		MPI_Barrier(MPI_COMM_WORLD);
		calcu_time = -MPI_Wtime();
		for(int i = 0; i < parNum; ++i)
		{
			MPI_Send(sending,size,MPI_CHAR,1,i,MPI_COMM_WORLD);
			MPI_Recv(sending,size,MPI_CHAR,1,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		calcu_time += MPI_Wtime();

		cout << sizeof(char)*size << "\t" << fixed << setprecision(15) << calcu_time/(double)parNum << endl;
		delete[] sending;
	}
	else
	{
		char* sending = new char[size];
		double calcu_time;
		MPI_Barrier(MPI_COMM_WORLD);
		calcu_time = -MPI_Wtime();
		for(int i = 0;i < parNum; ++i)
		{
			MPI_Recv(sending,size,MPI_CHAR,0,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Send(sending,size,MPI_CHAR,0,i,MPI_COMM_WORLD);
		}
		calcu_time += MPI_Wtime();

		delete[] sending;

	}
	MPI_Finalize();
	return 0;
}
