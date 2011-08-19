#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main ()
{
  union {
    long int   liseed;
    char       cseed[15];
  } seed;
  
  int i;
  double lat,lon;

  strcpy (seed.cseed, "meego");
  seed.liseed += 2;
  srand48 (seed.liseed);
  fprintf (stderr, "seed size is %d\n",sizeof(seed));
  fprintf (stderr, "seed is %ld or %s\n",seed.liseed, seed.cseed);
  lat = 34.0;
  lon = -90.0;

  for (i=0;i<1000;i++) {
    lat += drand48() * 0.001 - 0.0005;
    lon += drand48() * 0.001 - 0.0005;
    printf ("%lf, %lf, 200\n",lat,lon);
  }
}
