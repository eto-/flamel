CXXFLAGS = -I${CAENDIGITIZER_INC} -std=c++0x
LDLIBS = -L${CAENDIGITIZER_LIB} -L${CAENCOMM_LIB} -L${CAENVME_LIB} -lCAENDigitizer -lCAENComm -lCAENVME -lboost_program_options
CC = g++

claudio: claudio.o main.o attila.o sibilla.o #giotto.o

clean:
	rm -f claudio claudio.o main.o attila.o sibilla.o
