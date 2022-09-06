#ifndef io__H__
#define io__H__
#define MAX_YEARS 100

typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS+1];
} Investment;

void sendOutput(double *arr, int years);

#endif