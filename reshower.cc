// author: Riccardo Di Sipio <riccardo.di.sipio@cern.ch>
// Based on Pythia8 main21.cc example
// https://github.com/mortenpi/pythia8/blob/master/examples/main21.cc

#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */

#include "Pythia8/Pythia.h"
//#include "Pythia8Plugins/FastJet3.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/Nsubjettiness.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace Pythia8; 
using namespace fastjet::contrib;

//==========================================================================

void AddBoson(int id, double pT, double eta, double phi, 
  Event& event, ParticleData& pdt) {

  event.reset();

  // E^2 - P^2 = m^2
  
  double m = pdt.m0(id);
  
  double px = pT * cos(phi);
  double py = pT * sin(phi);
  double pz = pT * sinh(eta);
  double P = sqrtpos( px*px + py*py + pz*pz );
  double E = sqrtpos( P*P + m*m );

  int status = 1;
   
  event.append( id, status, 0, 0, px, py, pz, E, m );
}

void AddQuark(int id, double pT, double eta, double phi, 
  Event& event, ParticleData& pdt) {

  event.reset();

  // E^2 - P^2 = m^2
  
  double m_t = pdt.m0(id);
  
  double px = pT * cos(phi);
  double py = pT * sin(phi);
  double pz = pT * sinh(eta);
  double P = sqrtpos( px*px + py*py + pz*pz );
  double E = sqrtpos( P*P + m_t*m_t );

  int status = 1; 
    
  event.append( id, status, 101, 0, px, py, pz, E, m_t );

  double m_q = pdt.m0(1);
  double E_q = sqrtpos( P*P + m_q*m_q );
  event.append( -1, status, 0, 101, -px, -py, -pz, E_q, m_q );
}

/// return a vector of jets sorted into decreasing energy
vector<fastjet::PseudoJet> sorted_by_mass(const vector<fastjet::PseudoJet> & jets) {
  vector<double> masses(jets.size());
  for (size_t i = 0; i < jets.size(); i++) {masses[i] = -jets[i].m();}
  return objects_sorted_by_values(jets, masses);
}

void Normalize( TH1F* h )
{
   const double area = h->Integral();
   h->Scale( 1./area );
}

//==========================================================================

int main(int argc, char *argv[]) {

  const int id = (argc > 1) ? atoi(argv[1]) : 6; // 6=top quark, 23=Z, 25=higgs
  
  const double pT = (argc > 2) ? atof(argv[2]) : 350.;
  
  const double eta = 1.0; 
  const double phi =  0.0;

  const int nEvent = (argc > 3) ? atoi(argv[3]) : 10000;
  const int nList = 3;

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
  double R_large = 1.0;
  double R_small = 0.4;
  fastjet::Strategy               strategy = fastjet::Best;
  fastjet::RecombinationScheme    recombScheme = fastjet::E_scheme;
  
  fastjet::JetDefinition * jetDef_large = new fastjet::JetDefinition(fastjet::antikt_algorithm, R_large,
                                      recombScheme, strategy);
  
  fastjet::JetDefinition * jetDef_small = new fastjet::JetDefinition(fastjet::antikt_algorithm, R_small,
                                      recombScheme, strategy);

  // Fastjet input
  std::vector <fastjet::PseudoJet> fjInputs_large;
  std::vector <fastjet::PseudoJet> fjInputs_small;

  // Book histograms
  char buf[256];
  sprintf( buf, "histograms/histograms.pid_%i.pt_%.0f.root", id, pT );
  TFile * ofile = TFile::Open( buf, "RECREATE" );

  TH1F * h_jets_n = new TH1F("jets_n", ";Small-R jets multiplicity", 6, -0.5, 5.5 );
  TH1F * h_ljets_n = new TH1F("ljets_n", ";Large-R jets multiplicity", 5, -0.5, 4.5 );
  TH1F * h_ljet_pt = new TH1F( "ljet_pt", ";Large-R jet p_{T} [GeV]", 50, 0., 1000. );
  TH1F * h_ljet_m = new TH1F( "ljet_m", ";Large-R jet m [GeV]", 50, 0., 250. );
  TH1F * h_ljet_eta = new TH1F( "ljet_eta", ";Large-R jet #eta", 50, 0, 2.0 );
  TH1F * h_ljet_tau21 = new TH1F( "ljet_tau21", ";Large-R jet #tau_{21}", 50, 0., 1.0 );
  TH1F * h_ljet_tau32 = new TH1F( "ljet_tau32", ";Large-R jet #tau_{32}", 50, 0., 1.0 );
  TH1F * h_ljet_nconst = new TH1F( "ljet_nconst", ";Large-R jet constituents", 100, 0.5, 100.5 );
  TH1F * h_ljet_topTag = new TH1F( "ljet_topTag", ";Large-R jet top tagging", 2, -0.5, 1.5 );
  TH1F * h_ljet_higgsTag = new TH1F( "ljet_higgsTag", ";Large-R jet Higgs tagging", 2, -0.5, 1.5 );
  TH2F * h_ljet_m_vs_tau21 = new TH2F( "ljet_m_vs_tau21", ";Large-R jet #tau_{21};Large-R jet m [GeV]", 50, 0., 1., 50, 0., 250. );
  TH2F * h_ljet_m_vs_tau32 = new TH2F( "ljet_m_vs_tau32", ";Large-R jet #tau_{32};Large-R jet m [GeV]", 50, 0., 1., 50, 0., 250. );

  // Begin of event loop.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if( ( id == 6 ) or ( id == -6 ) )
      AddQuark( id, pT, eta, phi, event, pdt );
    else
      AddBoson( id, pT, eta, phi, event, pdt );
    

    if(!pythia.next()) continue;
    
    // List first few events.
    if (iEvent < nList) { 
     event.list();
     // Also list junctions.
     event.listJunctions();
    }

    // reset fastjet
    
    fjInputs_large.resize(0);
    fjInputs_small.resize(0);
    
    for (int i = 0; i < pythia.event.size(); ++i) {
      if (!pythia.event[i].isFinal())        continue;

      // No neutrinos
      if (pythia.event[i].idAbs() == 12 || pythia.event[i].idAbs() == 14 ||
          pythia.event[i].idAbs() == 16)     continue;

      // Only |eta| < 2.5
      if (fabs(pythia.event[i].eta()) > 2.5) continue;

      fastjet::PseudoJet pjet( pythia.event[i].px(),
			       pythia.event[i].py(),
			       pythia.event[i].pz(),
			       pythia.event[i].e() );
      fjInputs_large.push_back( pjet );
      fjInputs_small.push_back( pjet );

    } // end loop over particles

    if (fjInputs_large.size() == 0) {
      cout << "Error: event with no final state particles" << endl;
      continue;
    }

    // Run Fastjet algorithm
    vector <fastjet::PseudoJet> ljets;
    fastjet::ClusterSequence ljets_cs(fjInputs_large, *jetDef_large);

    vector <fastjet::PseudoJet> jets;
    fastjet::ClusterSequence jets_cs(fjInputs_small, *jetDef_small);

    // Extract inclusive jets sorted by pT
    ljets = sorted_by_mass( ljets_cs.inclusive_jets(250.) );
    jets = sorted_by_mass( jets_cs.inclusive_jets(20.) );

    int ljets_n = ljets.size();
    int jets_n  = jets.size();

    if( ljets_n == 0 ) continue;

    fastjet::PseudoJet * ljet = NULL;
    
    if( iEvent % 10000 == 0 ) {
      cout << "INFO: event: " << iEvent << " :: no. of jets: " << jets_n << endl;
      cout << "INFO: event: " << iEvent << " :: no. of ljets: " << ljets_n << endl;
      for( int j = 0 ; j < ljets_n ; ++j ) {
	ljet = &(ljets.at(j));
	cout << "INFO: (pT,eta,phi,E;m) = (" <<
		 ljet->perp() << "," <<
		 ljet->eta() << "," <<
		 ljet->phi() << "," <<
		 ljet->E() << "," <<
		 ljet->m() << ")" << endl;
      }
    }

    ljet = &(ljets.at(0));

    double beta=1.0;
    NsubjettinessRatio nSub21( 2, 1,OnePass_WTA_KT_Axes(), UnnormalizedMeasure(beta) );
    NsubjettinessRatio nSub32( 3, 2,OnePass_WTA_KT_Axes(), UnnormalizedMeasure(beta) );

    double tau21 = nSub21.result(*ljet);
    double tau32 = nSub32.result(*ljet);

    int topTag = 0;
    if( (fabs(ljet->m()-171.0) < 30) && (tau32<0.6) ) topTag = 1;

    int higgsTag = 0;
    if( (fabs(ljet->m()-125.0) < 30) && (tau21<0.5) ) higgsTag = 1;

    // Fill histograms
    h_jets_n->Fill( jets_n );
    h_ljets_n->Fill( ljets_n );
    h_ljet_pt->Fill( ljet->perp() );
    h_ljet_eta->Fill( ljet->eta() );
    h_ljet_m->Fill( ljet->m() );
    h_ljet_tau21->Fill( tau21 );
    h_ljet_tau32->Fill( tau32 );
    h_ljet_nconst->Fill( ljet->constituents().size() );
    h_ljet_topTag->Fill( topTag );
    h_ljet_higgsTag->Fill( higgsTag );
    h_ljet_m_vs_tau21->Fill( tau21, ljet->m() );
    h_ljet_m_vs_tau32->Fill( tau32, ljet->m() );
  }

  Normalize( h_ljet_topTag );
  Normalize( h_ljet_higgsTag );
  
  pythia.stat();

  ofile->Write();
  ofile->Close();

  delete jetDef_large;
  delete jetDef_small;
  
  return 0;
}
