#include<stdio.h>
#include <string.h>
#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <math.h>

using namespace std;

int comm_sz = -1;
int my_rank = -1;


class PRTTstr
{
   public:
      int n;
      int d;
      int s;
      double time;
      PRTTstr(int n_tmp,int d_tmp,int s_tmp, double time_tmp):n(n_tmp), d(d_tmp), s(s_tmp),time(time_tmp){}
};


void server(int n, int s)
{
   char* recving = new char[s];
   MPI_Barrier(MPI_COMM_WORLD);
   for(int i = 0; i < n; ++i)
   {
      MPI_Recv(recving, s, MPI_CHAR, 1, i, MPI_COMM_WORLD,MPI_STATUS_IGNORE );
   }
   MPI_Send(recving, s, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
   delete[] recving;
}

double client(int n, int d, int s)
{
   char* sending = new char[s];
   char* recving = new char[s];
   MPI_Barrier(MPI_COMM_WORLD);
   double result_time = -MPI_Wtime();
   MPI_Send(sending, s, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
   for(int i = 1;i < n; ++i)
   {
      usleep(d);
      MPI_Send(sending, s, MPI_CHAR, 0, i, MPI_COMM_WORLD);
   }
   MPI_Recv(sending, s, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   result_time += MPI_Wtime();

   delete[] sending;
   delete[] recving;
   return result_time;

}

PRTTstr* PRTT(int n, int d, int s)
{
   if(my_rank == 0)
   {
      server(n, s);
      return NULL;
   }

   if(my_rank == 1)
   {
      double result_time = client(n, d, s);
      return new PRTTstr(n, d, s, result_time);
   }

   return NULL;
}

void get_Os_print(int n, int start, int end, int stride)
{
   int d = -1;
   for(int s = start;s < end; s += stride)
   {
      PRTTstr* prttn;
      PRTTstr* prtt1;
      prtt1 = PRTT(1, 0, s);
      if(my_rank == 1)
         d = (int)(2*prtt1->time+0.0001)*pow(10,6);
      prttn = PRTT(n, d, s); 
      if(my_rank == 1)
      {
         double oplusd = (double)(prttn->time - prtt1->time)/(n-1);
         cout << s << "\t"  << oplusd - (double)d/pow(10,6) <<endl;
         delete prttn;
         delete prtt1;
      }
   }

}
void get_Gg_print(int n, int start, int end, int stride)
{
   for(int s = start;s < end; s += stride)
   {
      PRTTstr* prttn;
      PRTTstr* prtt1;
      prtt1 = PRTT(1, 0, s);
      prttn = PRTT(n, 0, s); 
      if(my_rank == 1)
      {
         double oplusd = (double)(prttn->time - prtt1->time)/(n-1);
         cout << s << "\t"  << oplusd <<endl;
         delete prttn;
         delete prtt1;
      }
   }

}

void get_Os()
{
   int n = 16;
   if(my_rank == 1)
      cout << "Os" << endl;
   get_Os_print(n, 1, 2, 1);
   get_Os_print(n, 1, 2, 1);
   get_Os_print(n, 10, 200, 10);
   get_Os_print(n, 200, 10000, 100);
   get_Os_print(n,10000,100000,1000);
   get_Os_print(n,100000,1000000,10000);
   get_Os_print(n,1000000,10000000,100000);
}

void get_Gg()
{
   int n = 16;
   if(my_rank == 1)
      cout << "Gg" << endl;
   get_Gg_print(n, 1, 2, 1);
   get_Gg_print(n, 10, 200, 10);
   get_Gg_print(n, 200, 10000, 100);
   get_Gg_print(n,10000,100000,1000);
   get_Gg_print(n,100000,1000000,10000);
   get_Gg_print(n,1000000,10000000,100000);
}

int main(int argc, char* argv[])
{
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   get_Os();
   get_Gg();

   MPI_Finalize();
   return 0;
}
