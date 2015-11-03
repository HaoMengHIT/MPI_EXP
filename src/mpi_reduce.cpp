#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iomanip>
#include <unistd.h>
#include "calfunc.h"

using namespace std;
double *create_rand_nums(int num_elements)
{
   double* rand_nums = new double[num_elements];
   for( int i = 0;i < num_elements;++i)
   {
      rand_nums[i] = (double)(rand() / (double)RAND_MAX);
   }
   return rand_nums;
}
int main(int argc, char* argv[])
{
   if(argc != 4)
   {
      fprintf(stderr, "Usage: mpi_reduce ite_num num_elements MPI_op");
      exit(1);
   }
   int comm_sz;
   int my_rank;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   int parNum = atoi(argv[1]);
   int size = atoi(argv[2]);
   string mpiOp = string(argv[3]);

   double init_time = MPI_Wtime();

   double values[parNum];
   for(int i = 0;i < parNum; ++i)
   {
      double total_time = 0.0;
      srand((unsigned)(time(NULL) + my_rank + i));

      double* local_nums = create_rand_nums(size);
      double* total_nums = create_rand_nums(size);

      MPI_Barrier(MPI_COMM_WORLD);
      double tmp = MPI_Wtime();
      double start_time = MPI_Wtime();
      MPI_Reduce(local_nums, total_nums, size, MPI_DOUBLE, MPI_SUM,0,MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      double end_time = MPI_Wtime();
      total_time = end_time - start_time - (start_time - tmp);
      values[i] = total_time;

      delete[] local_nums;
      delete[] total_nums;
   }
   if(my_rank == 0)
   {
      cout << size * sizeof(double) << "\t" << fixed << setprecision(15) << getMed(values, parNum) << "\n";
   }
   MPI_Finalize();
   return 0;
}
