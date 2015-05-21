CXXFLAGS = -I${CAENDIGITIZER_INC} -std=c++0x
LDLIBS = -L${CAENDIGITIZER_LIB} -L${CAENCOMM_LIB} -L${CAENVME_LIB} -lCAENDigitizer -lCAENComm -lCAENVME
CC = g++

claudio: claudio.o main.o attila.o #giotto.o

clean:
	rm -f claudio.o main.o attila.o
