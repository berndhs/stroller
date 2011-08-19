#include <stdio.h>
#include <math.h>

int
main ()
{
  int t_i;
  double t,x,y;

  t = 0.0;
  for (t_i=0;t_i<1000;t_i++) {
    t += 0.05;
    x = t * cos(t);
    y = t * sin(t);
    printf ("%lf, %lf, 200\n",x,y);
  }
}
