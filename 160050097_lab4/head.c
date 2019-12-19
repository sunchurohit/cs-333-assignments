#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];
char temp[512];
void
cat(int fd,int count)
{
  int n; int temp_counter = 0; int sig = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0) {

    for(int i=0;i<n;i++)
    {
      if(buf[i] == '\n')
      {
        temp[temp_counter] = '\n';
        temp_counter++;
        if(count > 0)
        {
          count--;
          if(write(1,temp,temp_counter) != temp_counter)
          {
            printf(1, "cat: write error\n");
            exit();
          };
        }
        else
        {
          sig = 1;
          temp_counter = 0;
          break;
        }
        temp_counter = 0;
      }
      else
      {
        temp[temp_counter] = buf[i];
        temp_counter++;
      }
    }
    if(sig)
    {
      break;  
    }
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 2){
    cat(0,1);
    exit();
  }

  int count = atoi(argv[1]);

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    printf(1, "-- %s --\n", argv[i]);
    cat(fd,count);
    close(fd);
  }
  exit();
}
