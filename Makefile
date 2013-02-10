CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

speech : capture_blocking.o wav2feat.o
	g++ -lportaudio $(CFlAGS) -o speech capture_blocking.o wav2feat.o $(LIBS)
wav2feat.o : wav2feat.cpp wav2feat.hpp
	g++ $(CFLAGS)  -c wav2feat.cpp 
capture_blocking.o : capture_blocking.c capture_blocking.h
	g++ $(CFLAGS) -c  capture_blocking.c 
clean :
	rm -r *.o speech
