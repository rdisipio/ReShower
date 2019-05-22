// author: Riccardo Di Sipio <riccardo.di.sipio@cern.ch>
// Based on Pythia8 main21.cc example
// https://github.com/mortenpi/pythia8/blob/master/examples/main21.cc

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/FastJet3.h"

using namespace Pythia8; 

//==========================================================================

void fillParticle(int id, double pT, double eta, double phi, 
  Event& event, ParticleData& pdt) {

  event.reset();

  // E^2 - P^2 = m^2
  
  double m = pdt.m0(id);
  
  double px = pT * cos(phi);
  double py = pT * sin(phi);
  double pz = pT * sinh(eta);
  double P = sqrtpos( px*px + py*py + pz*pz );
  double E = sqrtpos( P*P + m*m );

  int status = 23; // http://home.thep.lu.se/~torbjorn/pythia81php/ParticleProperties.php?filepath=files/
  int col = 101;
  int acol = 102;
  
  // int Event::append(int id, int status, int col, int acol, double px, double py, double pz, double e, double m = 0., double scale = 0., double pol = 9.)   
  event.append( id, status, col, acol, px, py, pz, E, m );
  
}


//==========================================================================

int main() {
  
  //int id = 6; // top quark
  int id = 25; // higgs
  double pT = 350.;
  double eta = 0.5; 
  double phi =  0.0;

  int nEvent = 10000;
  int nList = 3;

  double pT_min = 0.;
  if((id==6)||(id==-6)) pT_min = 350.;
  if(id==23) pT_min = 200.;
  if(id==25) pT_min = 250.;

  // Generator; shorthand for event and particleData.
  Pythia pythia;  
  Event& event      = pythia.event;
  ParticleData& pdt = pythia.particleData;

  // Key requirement: switch off ProcessLevel, and thereby also PartonLevel.
  pythia.readString("ProcessLevel:all = off");

  // Also allow resonance decays, with showers in them
  //pythia.readString("Standalone:allowResDec = on");

  // Optionally switch off decays.
  //pythia.readString("HadronLevel:Decay = off");

  // Switch off automatic event listing in favour of manual.
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  // top/Z/H hadronic decays only
  pythia.readString("24:onMode = off");
  pythia.readString("24:onIfAny = 1 2 3 4 -1 -2 -3 -4");

  pythia.readString("23:onMode = off");
  pythia.readString("23:onIfAny = 5 -5");
  
  pythia.readString("25:onMode = off");
  pythia.readString("25:onIfAny = 5 -5");
  
  // Initialize.
  pythia.init();

  // Setup fastjet
  // Fastjet analysis - select algorithm and parameters
  double Rparam = 1.0;
  fastjet::Strategy               strategy = fastjet::Best;
  fastjet::RecombinationScheme    recombScheme = fastjet::E_scheme;
  fastjet::JetDefinition         *jetDef = NULL;
  jetDef = new fastjet::JetDefinition(fastjet::kt_algorithm, Rparam,
                                      recombScheme, strategy);

  // Fastjet input
  std::vector <fastjet::PseudoJet> fjInputs;

  // Book histograms

  // Begin of event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    fillParticle( id, pT, eta, phi, event, pdt );

    // Generate events. Quit if failure.
    if (!pythia.next()) {
      cout << " Event generation aborted prematurely, owing to error!\n"; 
      break;
    }

    // List first few events.
    if (iEvent < nList) { 
     event.list();
     // Also list junctions.
     event.listJunctions();
    }

    // reset fastjet
    fjInputs.resize(0);
    
    for (int i = 0; i < pythia.event.size(); ++i) {
      if (!pythia.event[i].isFinal())        continue;

      // No neutrinos
      if (pythia.event[i].idAbs() == 12 || pythia.event[i].idAbs() == 14 ||
          pythia.event[i].idAbs() == 16)     continue;

      // Only |eta| < 2.5
      if (fabs(pythia.event[i].eta()) > 2.5) continue;

      fjInputs.push_back( fastjet::PseudoJet( pythia.event[i].px(),
        pythia.event[i].py(), pythia.event[i].pz(), pythia.event[i].e() ) );
    } // end loop over particles

    if (fjInputs.size() == 0) {
      cout << "Error: event with no final state particles" << endl;
      continue;
    }

    // Run Fastjet algorithm
    vector <fastjet::PseudoJet> all_jets;
    fastjet::ClusterSequence clustSeq(fjInputs, *jetDef);

    // Extract inclusive jets sorted by pT
    all_jets = sorted_by_pt( clustSeq.inclusive_jets(pT_min) );
   
  }
  
  pythia.stat();

  return 0;
}
