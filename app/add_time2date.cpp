#include<iostream>
#include"date_time.h"

int main(int argc,char *argv[]){
  if(argc < 3){
    std::cout << "Usage: " << argv[0] << " [Date in format yyyymmddhhmmss to be added to] [Time to be added]" << std::endl;
    return 1;
  }

  date_time time1, time2;
  time1.set_time(argv[1]);
  time2.set_time(argv[2]);

  if(time1 != NOTIME && time2 != NOTIME){
    std::cout << time1 + time2 << std::endl;
    return 0;
  }
  else{
    return 1;
  }
}
