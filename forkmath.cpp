#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char **argv){
  if(argc < 3){
    printf("Uasage: ./forkmath file N\n");
    return 0;
  }

  int N = atoi(argv[2]);

  pid_t retval = fork();
  if(retval == -1){
    printf("forking error\n");
    exit(1);
  }

  if(retval == 0){
    FILE * fptr = fopen(argv[1], "w");
    if(fptr == NULL){
      printf("failed to open file\n");
      exit(1);
    }

    srand(time(NULL));

    for(int i = 0; i < N; i++){
      char write[6];
      int fir, sec, opInt;
      char opChar;

      fir = rand()%10;
      sec = rand()%10;
      opInt = rand()%5;
      switch (opInt)
      {
        case 0: opChar = '+';
        break;
        case 1: opChar = '-';
        break;
        case 2: opChar = '*';
        break;
        case 3: opChar = '/';
        break;
        case 4: opChar = '%';
        break;
      }
      sprintf(write,"%d %c %d\n", fir, opChar, sec);
      fwrite(write, sizeof(char), 6, fptr);
    }
    fclose(fptr);
  } else if (retval > 0){
    int status;
    if(wait(&status) == -1){
      printf("error wait call on child proccess status: %d\n", status);
      exit(1);
    }

    FILE *fptr = fopen(argv[1],"r");

    int fir, sec, res;
    char op;
    char buffer[6];

    for (int i = 0; i < N; i++ ){
      fread(buffer,6*sizeof(char),1,fptr);
      int fir = buffer[0] - '0';
      char op = buffer[2];
      int sec = buffer[4] - '0';
      if((op=='/' || op=='%') && sec==0){
        res = -1;
      }else{
        switch(op){
          case '+':
          res = fir + sec;
          break;
          case '-':
          res = fir - sec;
          break;
          case '/':
          res = fir/sec;
          break;
          case '*':
          res = fir*sec;
          break;
          case '%':
          res = fir%sec;
          break;
        }
      }
      printf("%d %c %d = %d\n",fir,op,sec,res);
    }
    fclose(fptr);
  }
  return 0;
}
