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
      double d;
      int s;
      double time;
      PRTTstr(int n_tmp,double d_tmp,int s_tmp, double time_tmp):n(n_tmp), d(d_tmp), s(s_tmp),time(time_tmp){}
};

static void my_wait(double d)
{
   double ts = MPI_Wtime();
   double time;
   double targettime = d+ts;
   do
   {
      time = MPI_Wtime();
   } while(time < targettime);

}

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

double client(int n, double d, int s)
{
   char* sending = new char[s];
   char* recving = new char[s];
   MPI_Barrier(MPI_COMM_WORLD);
   double result_time = -MPI_Wtime();
   MPI_Send(sending, s, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
   for(int i = 1;i < n; ++i)
   {
      my_wait(d);
      MPI_Send(sending, s, MPI_CHAR, 0, i, MPI_COMM_WORLD);
   }
   MPI_Recv(sending, s, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   result_time += MPI_Wtime();

   delete[] sending;
   delete[] recving;
   return result_time;

}

PRTTstr* PRTT(int n, double d, int s)
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

/* Get median of all mesurements */
double getMed(double values[], int size)
{
   /* Bubble-sort data */
   int x,y;
   double holder;

   for(x = 0; x < size; x++)
      for(y = x; y < size; y++)
         if(values[x] > values[y])
         {
            holder = values[x];
            values[x] = values[y];
            values[y] = holder;
         }
   return values[(size+1)/2];
}


void get_Os_print(int ite_num, int n, int start, int end)
{
   double d;
   for(int s = start;s < end; s+=(s < 1e4?128:(s < 1e5?1024:(s < 1e6?1024*10:s < 1e7?1024*100:1024*100))))
   {
      double values[ite_num];
      for(int i = 0;i < ite_num; ++i)
      {
         PRTTstr* prttn;
         PRTTstr* prtt1;
         double times[100];
         for(int k = 0; k < 100; ++k)
         {
            prtt1 = PRTT(1, 0, s);
            if(my_rank == 1)
            {
               times[k] = prtt1->time;
               delete prtt1;
            }
         }
         double prtt1_time;
         if(my_rank == 1)
         {
            prtt1_time = getMed(times, 100);
            d = 2*prtt1_time+0.0001;
         }
         prttn = PRTT(n, d, s); 
         if(my_rank == 1)
         {
            double oplusd = (double)(prttn->time - prtt1_time)/(n-1);
            values[i] = oplusd - d;
            delete prttn;
         }
      }
      if(my_rank == 1)
         cout << s << "\t"  << fixed << setprecision(15) <<  getMed(values, ite_num) <<endl;
   }

}
void get_Gg_print(int ite_num, int n, int start, int end)
{
   for(int s = start;s < end; s+=(s < 1e4?128:(s < 1e5?1024:(s < 1e6?1024*10:s < 1e7?1024*100:1024*100))))
   {
      double values[ite_num];
      for(int i = 0;i < ite_num; ++i)
      {
         PRTTstr* prttn;
         PRTTstr* prtt1;
         double times[100];
         for(int k = 0; k < 100; ++k)
         {
            prtt1 = PRTT(1, 0, s);
            if(my_rank == 1)
            {
               times[k] = prtt1->time;
               delete prtt1;
            }
         }
         prttn = PRTT(n, 0, s); 
         if(my_rank == 1)
         {
            double prtt1_time = getMed(times, 100);
            double oplusd = (double)(prttn->time - prtt1_time)/(n-1);
            values[i] = oplusd;
            delete prttn;
         }
      }
      if(my_rank == 1)
         cout << s << "\t"  << fixed << setprecision(15) << getMed(values, ite_num) <<endl;
   }

}

const int pac_num = 10; //The count of packets sent from  client to server.
const int ite_num = 20; //The count of the test about every data size.

void get_Os()
{
   if(my_rank == 1)
      cout << "Os" << endl;
   get_Os_print(ite_num, pac_num, 1, 1e7);
}

void get_Gg()
{
   if(my_rank == 1)
      cout << "Gg" << endl;
   get_Gg_print(ite_num, pac_num, 1, 1e7);
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
