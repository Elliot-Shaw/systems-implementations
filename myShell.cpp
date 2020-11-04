#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* getInput(){
  char* input = NULL;
  char buf[100];
  size_t inlength = 0, templen = 0;
  do {
    if (fgets(buf, 100, stdin)==NULL){
      return NULL;
    }
    templen = strlen(buf);
    input = (char*)realloc(input, inlength+templen+1);
    if(input == NULL){
      printf("something went wrong\n");
      exit(1);
    }
    strcpy(input+inlength, buf);
    inlength += templen;
  } while (templen==100-1 && buf[100-2]!='\n');
  return input;
}

void restoreStreams(int in, int out, int err){
  if(dup2(in,0) < 0){
    exit(1);
  }
  if(dup2(out,1) < 0){
    exit(1);
  }
  if(dup2(err,2) < 0){
    exit(1);
  }
}

int main(int argc, char **argv){

  int standardIn = dup(0), standardOut = dup(1), standardErr = dup(2);

  char* input = NULL;
  if(argc == 1){
    do {
      input = getInput();
      if (input != NULL){
        fprintf(stdout, "stdout: %s", input);
        fprintf(stderr, "stderr: %s", input);
      }
    } while (input != NULL);

    free(input);
  } else {

    bool append = false;
    for (int i = 1; i < argc; i++){
      if(strcmp(argv[i],"-append") == 0){
        append = true;
      }
    }

    char * check;
    FILE * opened[100];
    int openFiles = 0;
    int no;

    for (int i = 1; i < argc; i++){
      check = strstr(argv[i],"in=");
      if(check != NULL && check == argv[i]){
        opened[openFiles] = fopen(argv[i]+3, "r");
        if(opened[openFiles] == NULL){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("failed to open %s\n", argv[i]+3);
          exit(1);
        }
        no = fileno(opened[openFiles]);
        if(dup2(no,0) < 0){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("dup2() error\n");
          exit(1);
        }
        openFiles++;
        continue;
      }
      check = strstr(argv[i],"out=");
      if(check != NULL && check == argv[i]){
        if(append){
          opened[openFiles] = fopen(argv[i]+4, "a");
        }else{
          opened[openFiles] = fopen(argv[i]+4, "w");
        }
        if(opened[openFiles] == NULL){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("failed to open %s\n", argv[i]+4);
          exit(1);
        }
        no = fileno(opened[openFiles]);
        if(dup2(no,1) < 0){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("dup2() error\n");
          exit(1);
        }
        openFiles++;
        continue;
      }
      check = strstr(argv[i],"err=");
      if(check != NULL && check == argv[i]){
        if(append){
          opened[openFiles] = fopen(argv[i]+4, "a");
        }else{
          opened[openFiles] = fopen(argv[i]+4, "w");
        }
        if(opened[openFiles] == NULL){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("failed to open %s\n", argv[i]+4);
          exit(1);
        }
        no = fileno(opened[openFiles]);
        if(dup2(no,2) < 0){
          restoreStreams(standardIn,standardOut,standardErr);
          printf("dup2() error\n");
          exit(1);
        }
        openFiles++;
        continue;
      }
      if(strcmp(argv[i],"-append") == 0){
        continue;
      }else{
        restoreStreams(standardIn,standardOut,standardErr);
        printf("%s is not a recognized command\n", argv[i]);
        exit(1);
      }
    }

    //try something here
    char buf[10000];

    while(fgets(buf, 10000, stdin)){
      fprintf(stdout, "stdout: %s", buf);
      fprintf(stderr, "stderr: %s", buf);
    }

    //printf("open files: %d\n", openFiles);

    //for(int i = 0; i < openFiles; i++){
    //  printf("closing file %d\n", i);
  //    fclose(opened[i]);
  //  }
  }

  return 0;
}
