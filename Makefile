PYTHIA_CFLAGS=$(pythia8-config --cflags)
PYTHIA_LIBS=$(pythia8-config --libs)

ROOT_CFLAGS=$(root-config --cflags)
ROOT_LIBS=$(root-config --libs)

CXX_COMMON=-O2  -pedantic -W -Wall -Wshadow -fPIC

reshower : reshower.cc
	$(CXX) $(ROOT_CFLAGS) $(ROOT_LIBS) $(PYTHIA_CFLAGS) $(PYTHIA_LIBS)  $< -o $@ $(CXX_COMMON)



