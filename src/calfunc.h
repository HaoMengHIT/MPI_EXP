/* Get median of all mesurements */
template <class T> T getMed(T values[], int size)
{
   /* Bubble-sort data */
   int x,y;
   T holder;

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
