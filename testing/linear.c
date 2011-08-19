#include <stdio.h>

int
main ()
{
  int i;
  double lat,lon;

  lat = -0.002;
  lon = 15;

  for (i=0;i<1000;i++) {
    lat += 0.0001;
    lon = 15;
    printf ("%lf, %lf, 200\n",lat,lon);
  }
}
