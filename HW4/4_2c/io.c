#include "io.h"

int getUserInput(Investment *invp);

int getUserInput(Investment *invp) {

  int valid;
  char parameter[100], input[100], vparameter[100], invparameter[100];

  sprintf(parameter,"Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32_WriteUART3(parameter);

  NU32_ReadUART3(input, 100);
  sscanf(input,"%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  sprintf(vparameter,"Valid input?  %d\r\n",valid);
  NU32_WriteUART3(vparameter);

  if (!valid) {

    sprintf(invparameter,"Invalid input; exiting.\r\n");
    NU32_WriteUART3(invparameter);
  }
  return(valid);
}

void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100], resultStatement[100];

  sprintf(resultStatement,"RESULTS:\r\n");
  NU32_WriteUART3(resultStatement);
  for (i=0; i<=yrs; i++) {
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]);
    NU32_WriteUART3(outstring);
  }
  
}