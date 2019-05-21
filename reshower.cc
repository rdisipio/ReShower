// author: Riccardo Di Sipio <riccardo.di.sipio@cern.ch>
// Based on Pythia8 main21.cc example
// https://github.com/mortenpi/pythia8/blob/master/examples/main21.cc

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/FastJet3.h"

using namespace Pythia8; 

//==========================================================================

void fillParticle(int id, double pT, double eta, double phi, 
  Event& event, ParticleData& pdt, Rndm& rndm) {

  event.reset();

  // E^2 - P^2 = m^2
  
  double m = pdt.mass(id);
  
  double px = pT * cos(phi);
  double py = pT * sin(phi);
  double pz = pT * sinh(eta);
  double P = sqrtpos( px*px + py*py + pz*pz );
  double E = sqrtpos( P*P + m*m );

  int status = 23; // http://home.thep.lu.se/~torbjorn/pythia81php/ParticleProperties.php?filepath=files/
  int col = 0;
  int acol = 0;
  
  // int Event::append(int id, int status, int col, int acol, double px, double py, double pz, double e, double m = 0., double scale = 0., double pol = 9.)   
  event.append( id, status, col, acol, px, py, pz, E, m );
  
}


//==========================================================================

int main() {
  
  int id = 6; // top quark
  double pT = 350.;
  double eta = 0.5; 
  double phi =  0.0;

  int nEvent = 10000;
  int nList = 3;

  // Generator; shorthand for event and particleData.
  Pythia pythia;  
  Event& event      = pythia.event;
  ParticleData& pdt = pythia.particleData;

  // Key requirement: switch off ProcessLevel, and thereby also PartonLevel.
  pythia.readString("ProcessLevel:all = off");

  // Also allow resonance decays, with showers in them
  pythia.readString("Standalone:allowResDec = on");

  // Optionally switch off decays.
  //pythia.readString("HadronLevel:Decay = off");

  // Switch off automatic event listing in favour of manual.
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  // top/H hadronic decays only
  pythia.particleData.readString("6:2::products = 5 -24"); // t
  pythia.particleData.readString("-6:2::products = -5 24"); // tbar
  pythia.particleData.readString("23:2::products = 5 -5"); // Z0
  pythia.particleData.readString("25:2::products = 5 -5"); // H0
  
  // Initialize.
  pythia.init();

  // Book histograms

  // Begin of event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    fillParticle( id, pT, eta, phi, event, pdt, pythia.rndm );

    // Generate events. Quit if failure.
    if (!pythia.next()) {
      cout << " Event generation aborted prematurely, owing to error!\n"; 
      break;
    }
  }

  // List first few events.
  if (iEvent < nList) { 
    event.list();
    // Also list junctions.
    event.listJunctions();
  }

  
  
}
