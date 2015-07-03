CXXFLAGS = -I${CAENDIGITIZER_INC} -I${BOOST_INC} $(shell root-config --cflags |sed s/c..11/c++0x/) -g
LDLIBS = -L${CAENDIGITIZER_LIB} -L${CAENCOMM_LIB} -L${CAENVME_LIB} -lCAENDigitizer -lCAENComm -lCAENVME -L${BOOST_LIB} -lboost_program_options $(shell root-config --glibs) -lsndfile -g
CC = g++

flamel: flamel.o main.o attila.o sibilla.o evaristo.o giotto.o omero.o

clean:
	rm -f flamel flamel.o main.o attila.o sibilla.o evaristo.o giotto.o omero.o
