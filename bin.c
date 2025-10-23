#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char ** argv)
{
  if(argc != 4 && argc != 5){
    fprintf(stderr, "syntax: [root]bin nbin low high [log|name]\n");
    exit(1);
  }

  const int root = !strcmp(argv[0], "rootbin");
  const int dolog = argc > 4 && !strcmp(argv[4], "log");
  const char * name = argc > 4? argv[4]: "h";

  const unsigned int nbin = atoi(argv[1]);
  const float low = atof(argv[2]);
  const float hig = atof(argv[3]);

  const float range = hig - low;

  double * bincontents = malloc(nbin*sizeof(double));

  memset(bincontents, 0, nbin*sizeof(double));

  float in;
  int scanret;
  while(1){
    scanret = scanf("%f", &in);
    if(scanret == EOF) break;
    if(scanret != 1){
      fprintf(stderr, "Junk in input\n");
      break;
    }

    if(in >= hig || in < low) continue;
    const unsigned int bin = nbin*(in - low)/range;

    // Protects against nan input
    if(bin < nbin) bincontents[bin]++;
  }

  if(root)
    printf("{\nTH1D * %s = new TH1D(\"%s\", \"\", %d, %f, %f);\n",
           name, name, nbin, low, hig);
  
  double * binerrors = malloc(nbin*sizeof(double));
  memset(binerrors, 0, nbin*sizeof(double));

  unsigned int i;
  for(i = 0; i < nbin; i++){
    if(dolog){
      if(bincontents[i] > 0){
        binerrors[i] = sqrt(bincontents[i])/bincontents[i]/log(10);
        bincontents[i] = log10(bincontents[i]);
      }
    }
    else{
      binerrors[i] = sqrt(bincontents[i]);
    }
  }

  for(i = 0; i < nbin; i++)
    if(root)
      printf("%s.SetBinContent(%d, %lf);\n", name, i+1, bincontents[i]);
    else 
      printf("%f %lf %f\n", low + (i+0.5)*range/nbin, bincontents[i],
                           binerrors[i]);
  if(root) puts("}");
  return 0;
}
