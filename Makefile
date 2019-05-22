PYTHIA_CFLAGS = $(shell pythia8-config --cflags)
PYTHIA_LIBS = $(shell pythia8-config --libs)

FASTJET_LIBS = $(shell fastjet-config --libs)

ROOT_CFLAGS = $(shell root-config --cflags)
ROOT_LIBS = $(shell root-config --libs)

CXX_COMMON=-O2  -pedantic -W -Wall -Wshadow -fPIC

reshower : reshower.cc $(shell pythia8-config --libdir)/libpythia8.a
	$(CXX) $< -o $@ $(CXX_COMMON) $(PYTHIA_CFLAGS) $(PYTHIA_LIBS) $(ROOT_LIBS) $(FASTJET_LIBS)

clean:
	@rm -f reshower *.o


