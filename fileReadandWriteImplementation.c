#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>

struct mFILE{
  int fd;
  int size;
  int offset;
  bool read;
  bool write;
};

mFILE *mfopen(const char *fileName, const char *mode) {
  mFILE *fp = (mFILE*)malloc(sizeof(mFILE));
  int fd;
  fp->read = false;
  fp->write = false;

  if (strcmp(mode, "r") == 0){
    fd = open(fileName, O_RDONLY);
    fp->read = true;
  } else if (strcmp(mode, "w") == 0){
    fd = open(fileName, O_CREAT | O_TRUNC | O_WRONLY, S_IWUSR);
    fp->write = true;
  } else if (strcmp(mode, "a") == 0){
    fd = open(fileName, O_CREAT | O_APPEND, S_IWUSR);
    fp->write = true;
  } else if (strcmp(mode, "r+") == 0){
    fd = open(fileName, O_RDWR);
    fp->write = true;
    fp->read = true;
  } else if (strcmp(mode, "w+") == 0){
    fd = open(fileName, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    fp->read = true;
    fp->write = true;
  } else if (strcmp(mode, "a+") == 0){
    fd = open(fileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
    fp->read = true;
    fp->write = true;
  } else {
    return NULL;
  }

  if (fd < 0){
    return NULL;
  }

  fp->fd = fd;
  fp->offset = lseek(fd,0,SEEK_CUR);
  fp->size = lseek(fd,0,SEEK_END);
  lseek(fd,fp->offset,SEEK_SET);

  return fp;
}

int mfread(void *ptr, size_t size, size_t count, mFILE *fptr){
  if (!fptr->read){
    return 0;
  }
  int counter = 0;
  while ((counter < count) && (fptr->offset+size < fptr->size)){
    int err = read(fptr->fd,ptr,size);
    if (err < 0){
      break;
    }
    counter++;
    ptr+=size;
    fptr->offset+=size;
  }
  return counter;

}

int mfwrite(const void *ptr, size_t size, size_t count, mFILE *fptr){
  if (!fptr->write){
    return 0;
  }
  int counter = 0;
  while (counter<count) {
    int err = write(fptr->fd,ptr,size);
    if (err < 0){
      break;
    }
    fptr->offset+=size;
    counter++;
  }
  return counter;
}

int mfclose(mFILE *toClose){
  int closed = close(toClose->fd);
  free(toClose);
  if (closed == -1){
    return EOF;
  } else {
    return 0;
  }
}

int main(){
  mFILE *ptr = mfopen("testycles.txt", "r");

  if (ptr == NULL){
    printf("you suck kido");
  }

  char *boom[100];
  mfread(boom, 1, 100, ptr);
  printf("%s\n",boom);

  mfwrite(boom,1,100,ptr);


  mfclose(ptr);



  return 0;
}
