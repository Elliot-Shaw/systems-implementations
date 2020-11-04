#include <iostream>
#include <stdio.h>
#include <cstdlib>

int main(int argc, char **argv){
  int lineCount, wordCount, charCount;
  lineCount = wordCount = charCount = 0;

  for (int i = 1; i < argc; i++){

    char *fileName = argv[i];
    FILE *fptr = fopen(fileName, "r");

    if (fptr == NULL){
      fprintf(stderr, "file %s does not exist\n\n", argv[i]);
      exit(1);
    } else {

      int tempW, tempL, tempC;
      tempW = tempL = tempC = 0;
      bool newWord = true;
      char line[100];

      while(!feof(fptr)){
        fgets(line, 100, fptr);
        int j = 0;
        for (char c = line[j]; c != '\0'; c = line[j]){
            if (c != 6){tempC++;}
            if (c >= '!' && c <= '~'){
              if(newWord){
                tempW++;
                newWord = false;
              }
            }else if (c == '\n'){
              tempL++;
              newWord = true;
            }else{
              newWord = true;
            }
            j++;
          }
        }
       wordCount += tempW;
       lineCount += tempL;
       charCount += tempC;

       printf("%d\t%d\t%d\t\t%s\n", tempL, tempW, tempC, argv[i]);

      }
      fclose(fptr);
    }
    if(argc > 2){
      printf("%d\t%d\t%d\t\t%s\n", lineCount, wordCount, charCount, "totals");
    }
  }
