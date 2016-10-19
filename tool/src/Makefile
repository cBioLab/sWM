TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CFLAGS=-O3 -g
VFLAGS=-std=c++11

all: bwt chtoi makeWM

bwt: bwt.cpp
	$(CXX) bwt.cpp -o $@ $(LDFLAGS)
	mv $@ $(TOPDIR)../bin/

chtoi: chtoi.cpp
	$(CXX) chtoi.cpp -o $@ $(LDFLAGS)
	mv $@ $(TOPDIR)../bin/

makeWM: wavelet.cpp
	$(CXX) $(VFLAGS) wavelet.cpp -o $@ $(LDFLAGS)
	mv $@ $(TOPDIR)../bin/

clean:
	rm -rf ../bin/bwt ../bin/chtoi ../bin/makeWM ./*.o ./*~

