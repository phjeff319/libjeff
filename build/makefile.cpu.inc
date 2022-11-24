BZ2_INC= -I@BZIP2_INCLUDE_DIR@
ECCODES= -I/home/ubuntu/local/ecCodes/include -L/home/ubuntu/local/ecCodes/lib/ -leccodes
GSL_INC= -I@GSL_INC@
ZLIB_INC= -I@ZLIB_INCLUDE_DIRS@

CFLAGS= -O3 -w -I../include/ $(BZ2_INC) $(ECCODES) $(GSL_INC) $(ZLIB_INC)
