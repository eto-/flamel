CXXFLAGS = -I${CAENDIGITIZER_INC} $(shell root-config --cflags |sed s/c..11/c++0x/)
LDLIBS = -L${CAENDIGITIZER_LIB} -L${CAENCOMM_LIB} -L${CAENVME_LIB} -lCAENDigitizer -lCAENComm -lCAENVME -lboost_program_options $(shell root-config --glibs)
CC = g++

claudio: claudio.o main.o attila.o sibilla.o evaristo.o giotto.o

clean:
	rm -f claudio claudio.o main.o attila.o sibilla.o evaristo.o giotto.o
