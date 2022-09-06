#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100       // max length of string input

void getString(char *str);  // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
  int len;                  // length of the entered string
  char str[MAXLENGTH];      // input should be no longer than MAXLENGTH
  // here, any other variables you need
  int i,j;

  getString(str);
  len = strlen(str);        // get length of the string, from string.h
  // put nested loops here to put the string in sorted order

  for(j=0; j<len-1; j++){
      for(i=0; i<len-1; i++){
          if(greaterThan(str[i],str[i+1])){
              swap(str, i, i+1);
          }
      }
  }
  printResult(str);
  return(0);
}

// helper functions go here
void getString (char *str){
    printf ("Enter the string you would like to sort (up to %d): ", MAXLENGTH);
    scanf("%s",str);
}

void printResult (char *str){
    printf("Here is the sorted string: %s\n", str);
}

int greaterThan(char ch1, char ch2){
    return(ch1>ch2);
}

void swap(char *str, int index1, int index2){
    char s;
    s=str[index1];
    str[index1]=str[index2];
    str[index2]=s;
}