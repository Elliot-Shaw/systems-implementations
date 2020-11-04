#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>

int main(int argc, char **argv){
  bool proceed = true;
  int month, day, year, hour, minute, second;

  if(argc < 8){
    printf("usage: ./timechange file month day year hour minute second\n");
    return 0;
  }

  month = atoi(argv[2]);
  day = atoi(argv[3]);
  year = atoi(argv[4]);
  hour = atoi(argv[5]);
  minute = atoi(argv[6]);
  second = atoi(argv[7]);

  if (month>12 || month<1){
    proceed = false;
    printf("month must be from 1 to 12\n");
  }

  if (month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12){
    if (day<1 || day>31){
      proceed = false;
      printf("day must be from 1 to 31 for month %d\n", month);
    }
  } else if (month==4 || month==6 || month==9 || month==11){
    if (day<1 || day>30){
      proceed = false;
      printf("day must be from 1 to 30 for month %d\n", month);
    }
  } else if (month==2){
    if (year%4!=0){
      if (day<1 || day>28){
        proceed = false;
        printf("day must be from 1 to 28 for month %d year %d\n", month, year);
      }
    }else if (year%100!=0){
      if(day<1 || day>29){
        proceed = false;
        printf("day must be from 1 to 29 for month %d year %d\n", month, year);
      }
    }else if (year%400==0){
      if(day<1 || day>29){
        proceed = false;
        printf("day must be from 1 to 29 for month %d year %d\n", month, year);
      }
    }else{
      if (day<1 || day>28){
        proceed = false;
        printf("day must be from 1 to 28 for month %d year %d\n", month, year);
      }
    }
  }

  if (year<1){
    proceed = false;
    printf("year must be greater than zero\n");
  }

  if (hour<0 || hour>23){
    proceed = false;
    printf("hour must be from 0 to 23\n");
  }

  if (minute<0 || minute>59) {
    proceed = false;
    printf("muntes must be from 0 to 59\n");
  }

  if (second<0 || second>59){
    proceed = false;
    printf("seconds must be from 0 to 59\n");
  }

  if (proceed==false){
    return 0;
  }

  /*
  FILE *fptr = fopen(argv[1]);
  if (fptr == NULL){
    printf("file %s did not open");
    exit(1);
  }
  */

  struct utimbuf timbuff;
  struct tm times;

  times.tm_hour = hour + 1;
  times.tm_sec = second;
  times.tm_mon = month - 1;
  times.tm_min = minute;
  times.tm_year = year - 1900; //Since its year from 1900 duh
  times.tm_mday = day;

  time_t change = mktime(&times);

  if (change == -1){
    printf("Error in time chosen... probably too far back\n");
    exit(1);
  }

  timbuff.actime = change;
  timbuff.modtime = change;

  if(utime(argv[1], &timbuff) < 0){
    printf("file doesnt exist or error changing file time\n");
    exit(1);
  }

  printf("%s access and modify time changed to: %s", argv[1], asctime(&times));


  return 0;
}
