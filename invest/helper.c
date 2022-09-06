#include "helper.h"

void calculateGrowth(Investment *invp) {

  int i;

  for (i = 1; i <= invp->years; i= i + 1) {

    invp->invarray[i] = invp->growth * invp->invarray[i-1];
  }
}

int getUserInput(Investment *invp) {

  int valid;
  char parameter[100], input[100], vparameter[100], invparameter[100];

  sprintf(parameter,"Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32_WriteUART3(parameter);
  NU32_WriteUART3("\r\n");

  NU32_ReadUART3(input, 100);
  sscanf(input,"%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  sprintf(vparameter,"Valid input?  %d\n",valid);
  NU32_WriteUART3(vparameter);
  NU32_WriteUART3("\r\n");

  if (!valid) {

    sprintf(invparameter,"Invalid input; exiting.\n");
    NU32_WriteUART3(invparameter);
    NU32_WriteUART3("\r\n");
  }
  return(valid);
}

void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100], rparameter[100];

  sprintf(rparameter,"\nRESULTS:\n\n");
  NU32_WriteUART3(rparameter);
  NU32_WriteUART3("\r\n");

  for (i=0; i<=yrs; i++) {
    sprintf(outstring,"Year %3d:  %10.2f\n",i,arr[i]);
    NU32_WriteUART3(outstring);
    NU32_WriteUART3("\r\n");
  }

  NU32_WriteUART3("\r\n");
}