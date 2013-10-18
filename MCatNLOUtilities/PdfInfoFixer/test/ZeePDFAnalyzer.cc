/* This is en example for an Analyzer of a MCatNLO HeoMCProduct
   The analyzer fills a histogram with the event weights,
   and looks for electron/positron pairs and fills a histogram
   with the invaraint mass of the two. 
*/

//
// Original Author:  Fabian Stoeckli
//         Created:  Tue Nov 14 13:43:02 CET 2006
// $Id: ZeePDFAnalyzer.cc,v 1.1 2010/03/01 13:15:54 fabstoec Exp $
//
//


// system include files
#include <memory>
#include <iostream>


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HepMC/WeightContainer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"

//
// class decleration
//

class ZeePDFAnalyzer : public edm::EDAnalyzer {
   public:
      explicit ZeePDFAnalyzer(const edm::ParameterSet&);
      ~ZeePDFAnalyzer();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      
      TH1D *histo_x1, *histo_x2;	
      double weightSign;

};


ZeePDFAnalyzer::ZeePDFAnalyzer(const edm::ParameterSet& iConfig)
{

  edm::Service<TFileService> fs;
  histo_x1 = fs->make<TH1D>("x1", "x1", 100, 0., 1.);
  histo_x2 = fs->make<TH1D>("x2", "x2", 100, 0., 1.);
  weightSign=1.;

}


ZeePDFAnalyzer::~ZeePDFAnalyzer()
{
 
}


// ------------ method called to for each event  ------------
void ZeePDFAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<GenEventInfoProduct> info;

  iEvent.getByLabel("pdfInfoFixing",info);

  weightSign = ( info->weight() > 0 ) ? 1. : -1.;
  histo_x1->Fill(info->pdf()->x.first,   weightSign);
  histo_x2->Fill(info->pdf()->x.second,  weightSign);

}


// ------------ method called once each job just before starting event loop  ------------
void 
ZeePDFAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ZeePDFAnalyzer::endJob() {

}

//define this as a plug-in
DEFINE_FWK_MODULE(ZeePDFAnalyzer);
