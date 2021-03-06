PYTHIA_CFLAGS = $(shell pythia8-config --cflags)
PYTHIA_LIBS = $(shell pythia8-config --libs)

FASTJET_CFLAGS = $(shell fastjet-config --cxxflags)
FASTJET_LIBS = $(shell fastjet-config --libs) -lNsubjettiness

ROOT_CFLAGS = $(shell root-config --cflags)
ROOT_LIBS = $(shell root-config --libs)

CXX_COMMON=-O2  -pedantic -W -Wall -Wshadow -fPIC

reshower : reshower.cc $(shell pythia8-config --libdir)/libpythia8.a
	$(CXX) $< -o $@ $(CXX_COMMON) $(PYTHIA_CFLAGS) $(PYTHIA_LIBS) $(ROOT_CFLAGS) $(ROOT_LIBS) $(FASTJET_LIBS) $(FASTJET_CFLAGS)

clean:
	@rm -f reshower *.o


