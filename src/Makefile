CXX=g++
CXXFLAGS=`pkg-config --cflags opencv` -O2
BOOST_LIBS=-L/usr/local/lib
INC_BOOST=-I${BOOST_ROOT}
LIBS=`pkg-config --libs opencv` $(BOOST_LIBS)
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system

all: getDescriptors buildSearchIndex ImageSearch

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_BOOST) -o $@ $< $(LIBS) $(LDFLAGS) 

clean: 
	rm -rf getDescriptors buildSearchIndex ImageSearch
