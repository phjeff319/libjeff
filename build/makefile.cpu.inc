BZ2_INC= -I/@BZIP2_INCLUDE_DIR@
CIVETWEB_INC= -I/@civetweb_INCLUDE_DIR@
ECCODES= -I/home/ubuntu/local/ecCodes/include -L/home/ubuntu/local/ecCodes/lib/ -leccodes
GSL_INC= -I/@GSL_INC@
LIBSSH_INC= -I/@libssh_INCLUDE_DIR@
SERIAL_INC= -I/@serial_DIR@
ZLIB_INC= -I/@ZLIB_INCLUDE_DIRS@

CFLAGS= -O3 -w -I../include/ $(BZ2_INC) $(CIVETWEB_INC) $(ECCODES) $(GSL_INC) $(LIBSSH_INC) $(SERIAL_INC) $(ZLIB_INC)
