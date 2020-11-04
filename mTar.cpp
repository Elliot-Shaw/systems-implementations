#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utime.h>

int main(int argc, char *argv[]){
  if (argc <= 2){
    printf("usage: ./mTar -a file.mtar file1 [ file2 ... ]\n");
    printf("usage: ./mTar -x file.mtar\n");
  }else{
    if(argc == 3){
      if(strcmp(argv[1], "-x") == 0){
        //code for -x
        if(strcmp(strrchr(argv[2], '.'),".mtar") == 0){
          FILE * tarFile = fopen(argv[2],"r");
          if(tarFile == NULL){
            printf("mtar did not open\n");
            exit(1);
          }

          struct stat fStat;
          struct utimbuf timebuff;
          while(fread(&fStat,sizeof(struct stat),1,tarFile) == 1){

            int * nameSize = (int*)malloc(sizeof(int));
            fread(nameSize,sizeof(int),1,tarFile);

            char * fileName = (char*)malloc(*nameSize);
            fread(fileName,*nameSize,1,tarFile);
            memset(fileName + *nameSize, '\0', 1);
            printf("extracting %s (size %ld)\n",fileName,fStat.st_size);

            FILE * createFile = fopen(fileName,"w");
            if(createFile == NULL){
              printf("error creating file\n");
              exit(1);
            }

            void * contents = malloc(fStat.st_size);
            fread(contents,fStat.st_size,1,tarFile);
            fwrite(contents,fStat.st_size,1,createFile);
            free(contents);

            free(nameSize);
            fclose(createFile);

            //change times and permisions
            chmod(fileName, fStat.st_mode & 07777);
            timebuff.modtime = fStat.st_atime;
            timebuff.actime = fStat.st_atime;
            utime(fileName, &timebuff);


            free(fileName);
          }
          fclose(tarFile);
        }else{
          printf("must be 'mtar' file\n");
        }
      }else if(strcmp(argv[1], "-a") == 0){
        printf("usage: ./mTar -a file.mtar file1 [ file2 ... ]\n");
      }
      else{
        printf("error in cmd line arg\n");
        exit(1);
      }
    }else{
      if(strcmp(argv[1], "-a") != 0){
        printf("error cmd arg\n");
        exit(1);
      }
      struct stat fStuff;

      FILE * tarFile = fopen(argv[2],"w");

      if (tarFile!=NULL){
        for(int i = 3; i < argc; i++){
          printf("archiving %s", argv[i]);
          FILE * readFile = fopen(argv[i],"r");
          if (readFile==NULL){
            printf("  failed - doesnt exist\n");
            continue;
          }
          if(stat(argv[i],&fStuff) < 0){
            printf("error in stat call\n");
            exit(1);
          }
          if(!S_ISREG(fStuff.st_mode)){
            printf("  failed - not reg file\n");
            continue;
          }
          printf(" (size %ld)\n", fStuff.st_size);

          fwrite(&fStuff,sizeof(struct stat),1,tarFile);

          int * nameSize = (int*)malloc(sizeof(int));
          *nameSize = strlen(argv[i]);
          fwrite(nameSize,sizeof(int),1,tarFile);
          free(nameSize);

          fwrite(argv[i],strlen(argv[i]),1,tarFile);

          void * contents = malloc(fStuff.st_size);
          fread(contents,fStuff.st_size,1,readFile);
          fwrite(contents,fStuff.st_size,1,tarFile);
          free(contents);


          fclose(readFile);
        }
      } else {
        printf("error opening mtar file\n");
        exit(1);
      }
      fclose(tarFile);
    }
  }
  return 0;
}
