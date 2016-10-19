TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CFLAGS=-O3 -g
CFLAGS+= -I$(TOPDIR)include -I$(TOPDIR)../../cybozulib/include/ -I$(TOPDIR)../../mcl/include -I$(TOPDIR)../../xbyak/ -I$(TOPDIR)../../PBWT-sec/include -I$(TOPDIR)../include/
LDFLAGS+= -lgmp -lgmpxx -lcrypto -lmcl -L../../mcl/lib/

ROT_OBJ=../../PBWT-sec/src/rot.o
COMM_OBJ=../../PBWT-sec/src/comm.o
SERVER_OBJS=$(ROT_OBJ) wav.o $(COMM_OBJ) server.o
CLIENT_OBJS=$(ROT_OBJ) wav.o $(COMM_OBJ) client.o
WAV_OBJS= $(ROT_OBJ) wav.o wav_main.o
MCL_LIB=../../mcl/lib/libmcl.a

all: pbwt_sec wav_main server client tool

pbwt_sec:
	make -C ../../PBWT-sec/src/

clean:
	rm -rf ../bin/server ../bin/client ../bin/wav_main $(SERVER_OBJS) $(CLIENT_OBJS) $(WAV_OBJS) *~
	make clean -C ../tool/src/

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -c $< -o $@ $(CFLAGS) -fopenmp

server: $(SERVER_OBJS) $(MCL_LIB) 
	$(CXX) $(SERVER_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mkdir -p $(TOPDIR)../bin/
	mv server $(TOPDIR)../bin/
	mkdir -p $(TOPDIR)../comm/
	mkdir -p $(TOPDIR)../comm/server
	mkdir -p $(TOPDIR)../comm/client

client: $(CLIENT_OBJS) $(MCL_LIB) 
	$(CXX) $(CLIENT_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mkdir -p $(TOPDIR)../bin/
	mv client $(TOPDIR)../bin/
	mkdir -p $(TOPDIR)../comm/
	mkdir -p $(TOPDIR)../comm/server
	mkdir -p $(TOPDIR)../comm/client

wav_main: $(WAV_OBJS) $(MCL_LIB) 
	$(CXX) $(WAV_OBJS) -o $@ $(LDFLAGS) -fopenmp
	mkdir -p $(TOPDIR)../bin/
	mv wav_main $(TOPDIR)../bin/

tool:
	make -C ../tool/src/
