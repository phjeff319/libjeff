GSL_INC= -I/home/ubuntu/local/include
ECCODES= -I/home/ubuntu/local/ecCodes/include -L/home/ubuntu/local/ecCodes/lib/ -leccodes
CFLAGS= -O3 -w -I../include/ $(GSL_INC) 
