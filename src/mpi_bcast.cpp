#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <iomanip>
using namespace std;

double getMed(double values[], int size)
{
   int x,y;
   double handle;

   for(x = 0; x < size; ++x)
      for(y = 0; y < size; ++y)
         if(values[x] > values[y])
         {
            handle = values[x];
            values[x] = values[y];
            values[y] = handle;
         }

   return values[(size+1)/2];
}
int main(int argc ,char* argv[])
{
   if(argc != 3)
   {
      fprintf(stderr, "Usage: mpi_bcast ite_num num_elements");
      exit(1);
   }
   int comm_sz;
   int my_rank;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
   int parNum = atoi(argv[1]);
   int size = atoi(argv[2]);
   //MPI_Barrier(MPI_COMM_WORLD);


   double values[parNum];

   for(int i = 0;i < parNum; ++i)
   {
      double total_mpi_bcast_time = 0.0;
      char* data = new char[size];

      MPI_Barrier(MPI_COMM_WORLD);
      total_mpi_bcast_time -= MPI_Wtime();
      MPI_Bcast(data, size,MPI_CHAR,0,MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      total_mpi_bcast_time += MPI_Wtime();

      values[i] = total_mpi_bcast_time;


      delete[] data;
   }
   if(my_rank == 0)
   {
      cout << size * sizeof(char) << "\t" << fixed << setprecision(15) << getMed(values, parNum) << "\n";  
   }
   MPI_Finalize();
   return 0;
}
