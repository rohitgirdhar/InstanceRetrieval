CC=g++
CFLAGS=`pkg-config --cflags opencv`
BOOST_LIBS=-L/usr/local/lib
INC_BOOST=-I${BOOST_ROOT}
LIBS=`pkg-config --libs opencv` $(BOOST_LIBS)
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system

all: getDescriptors

%: %.cpp
	$(CC) $(CFLAGS) $(INC_BOOST) -o $@ $< $(LIBS) $(LDFLAGS) 

clean: 
	rm -rf getDescriptors
