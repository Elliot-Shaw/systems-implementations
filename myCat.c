#include <iostream>
#include <stdio.h>

int main(int argc, char **argv){ //mess with this
  if(argc == 1){
    char entered[100];
    while(true){
      scanf(" %[^\n]s", entered);
      printf("%s\n", entered);
    }
  }else{
    for(int i = 1; i < argc; i++){
      char *theFile = argv[i];
      FILE *fptr = fopen(theFile, "r");

      if(fptr == NULL){
        printf("file %s does not exist\n\n", argv[i]);
      } else {
        char content[100];
        while(fscanf(fptr, "%s", content) != EOF){
          printf("%s ", content);
        }
        printf("\n\n");
      }
    }
  }
}
