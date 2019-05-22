// author: Riccardo Di Sipio <riccardo.di.sipio@cern.ch>
// Based on Pythia8 main21.cc example
// https://github.com/mortenpi/pythia8/blob/master/examples/main21.cc

#include "Pythia8/Pythia.h"
//#include "Pythia8Plugins/FastJet3.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/Nsubjettiness.hh"

#include "TFile.h"
#include "TH1F.h"

using namespace Pythia8; 
using namespace fastjet::contrib;

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

  int status = 1; // 23=outgoing parton, see: http://home.thep.lu.se/~torbjorn/pythia81php/ParticleProperties.php?filepath=files/
  int col = 0;
  int acol = 0;
  
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
  //pythia.readString("PartonLevel:all = on");
  //pythia.readString("HadronLevel:all = on");
  pythia.readString("ProcessLevel:resonanceDecays = on");
  
  
  // Optionally switch off decays.
  //pythia.readString("HadronLevel:Decay = off");

  // Switch off automatic event listing in favour of manual.
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");

  
  // top/Z/H hadronic decays only
  pythia.readString("6:onMode = off");
  pythia.readString("6:onIfMatch = 5 24 ");
  
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
  jetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm, Rparam,
                                      recombScheme, strategy);

  // Fastjet input
  //std::vector <fastjet::PseudoJet> fjInputs;

  // Book histograms
  TFile * ofile = TFile::Open( "histograms.root", "RECREATE" );
  
  TH1F * h_ljet_pt = new TH1F( "ljet_pt", ";Large-R jet p_{T} [GeV]", 20, 0., 1000. );
  TH1F * h_ljet_m = new TH1F( "ljet_m", ";Large-R jet m [GeV]", 30, 0., 300. );
  TH1F * h_ljet_eta = new TH1F( "ljet_eta", ";Large-R jet #eta", 25, -2.5, 2.5 );
  TH1F * h_ljet_tau21 = new TH1F( "ljet_tau21", ";Large-R jet #tau_{21}", 20, 0., 1.0 );
  TH1F * h_ljet_tau32 = new TH1F( "ljet_tau32", ";Large-R jet #tau_{32}", 20, 0., 1.0 );
  TH1F * h_ljet_nconst = new TH1F( "ljet_nconst", ";Large-R jet constituents", 100, 0.5, 100.5 );

  // Begin of event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    fillParticle( id, pT, eta, phi, event, pdt );

    //if(!pythia.next()) continue;
    pythia.next();
    
    // List first few events.
    if (iEvent < nList) { 
     event.list();
     // Also list junctions.
     event.listJunctions();
    }

    // reset fastjet
    std::vector <fastjet::PseudoJet> fjInputs;
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

    int jets_n = all_jets.size();

    if( jets_n == 0 ) continue;
    
    if( iEvent % 100 == 0 )
      cout << "INFO: event: " << iEvent << " :: no. of jets: " << jets_n << endl;

    fastjet::PseudoJet * ljet = &(all_jets.at(0));

    if( iEvent % 100 == 0 ) {
      cout << "INFO: (pT,eta,phi,E;m) = (" <<
	ljet->perp() << "," <<
	ljet->eta() << "," <<
	ljet->phi() << "," <<
	ljet->E() << "," <<
	ljet->m() << ")" << endl;
	
    }

    double beta=1.0;
    NsubjettinessRatio nSub21( 2, 1,OnePass_WTA_KT_Axes(), UnnormalizedMeasure(beta) );
    NsubjettinessRatio nSub32( 3, 2,OnePass_WTA_KT_Axes(), UnnormalizedMeasure(beta) );

    double tau21 = nSub21.result(*ljet);
    double tau32 = nSub32.result(*ljet);

    // Fill histograms
    h_ljet_pt->Fill( ljet->perp() );
    h_ljet_eta->Fill( ljet->eta() );
    h_ljet_m->Fill( ljet->m() );
    h_ljet_tau21->Fill( tau21 );
    h_ljet_tau32->Fill( tau32 );
    h_ljet_nconst->Fill( ljet->constituents().size() );
  }
  
  pythia.stat();

  ofile->Write();
  ofile->Close();
  
  delete jetDef;

  /*
  delete h_ljet_pt;
  delete h_ljet_eta;
  delete h_ljet_m;
  delete h_ljet_tau21;
  delete h_ljet_tau32;
  */
  
  return 0;
}
