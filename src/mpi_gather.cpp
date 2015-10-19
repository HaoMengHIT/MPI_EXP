#include <iostream>
#include <mpi.h>
#include <iomanip>
#include <unistd.h>
#include <time.h>
#include <cstdlib>
#include "calfunc.h"

using namespace std;

char *create_rand_chars(int num_elements)
{
   char * rand_chars = new char[num_elements];
   for( int i = 0;i <  num_elements; ++i)
   {
      rand_chars[i] = rand()%128;
   }
   return rand_chars;
}

int main(int argc, char* argv[])
{
   if(argc != 3)
   {
      fprintf(stderr, "Usage: mpi_gather ite_num num_elements");
      exit(1);
   }
   int comm_sz;
   int my_rank;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   int parNum = atoi(argv[1]);
   int size = atoi(argv[2]);
   
   double values[parNum];

   for(int i = 0; i < parNum; ++i)
   {
      double total_time = 0.0;
      char* rand_chars = NULL;

      srand(time(NULL));
      if(my_rank == 0)
      {
         rand_chars = create_rand_chars(size*comm_sz);
      }

      char* sub_rand_chars = create_rand_chars(size);

      MPI_Barrier(MPI_COMM_WORLD);
      total_time -= MPI_Wtime();
      MPI_Gather(sub_rand_chars, size, MPI_CHAR, rand_chars, size, MPI_CHAR, 0, MPI_COMM_WORLD);

      MPI_Barrier(MPI_COMM_WORLD);
      total_time += MPI_Wtime();

      values[i] = total_time;

      if(my_rank == 0)
         delete[] rand_chars;
      delete[] sub_rand_chars;
   }
   if(my_rank == 0)
   {
      cout << size * sizeof(char) << "\t" << fixed << setprecision(15) << getMed(values, parNum) <<"\n";
   }
   MPI_Finalize();
   return 0;
}
