// -*- C++ -*-
//
// Package:    PdfInfoFixer
// Class:      PdfInfoFixer
// 
/**\class PdfInfoFixer PdfInfoFixer.cc GeneratorInterface/PdfInfoFixer/src/PdfInfoFixer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Fabian Stoeckli,40 1-B02,+41227671581,
//         Created:  Mon Mar  1 11:11:19 CET 2010
// $Id: PdfInfoFixer.cc,v 1.2 2010/03/01 13:15:54 fabstoec Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include <HepMC/WeightContainer.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>


//
// class declaration
//

class PdfInfoFixer : public edm::EDProducer {
   public:
      explicit PdfInfoFixer(const edm::ParameterSet&);
      ~PdfInfoFixer();

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
PdfInfoFixer::PdfInfoFixer(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

  produces<GenEventInfoProduct>();

}


PdfInfoFixer::~PdfInfoFixer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
void
PdfInfoFixer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  edm::Handle<GenEventInfoProduct> geninfo;
  iEvent.getByType(geninfo);
  
  // new product
  std::auto_ptr<GenEventInfoProduct> genEventInfo( new GenEventInfoProduct(*geninfo) );
  //std::auto_ptr<GenEventInfoProduct> genEventInfo( new GenEventInfoProduct() );
  
  // MepMC event to compute PDF info from
  edm::Handle<edm::HepMCProduct> evt_h;
  iEvent.getByLabel("generator", evt_h);
  const HepMC::GenEvent *evt = evt_h->GetEvent();
  
  // new PdfInfo
  HepMC::PdfInfo pdfInfo;
  
  HepMC::GenParticle* incomingParton = NULL;
  HepMC::GenParticle* targetParton = NULL;
  
  HepMC::GenParticle* incomingProton = NULL;
  HepMC::GenParticle* targetProton = NULL;
  
  // find target partons (first entry with IST=122)
  for(HepMC::GenEvent::particle_const_iterator it = evt->particles_begin(); 
      (it != evt->particles_end() && targetParton==NULL); it++) {

    if((*it)->status()==3 && ( ( abs((*it)->pdg_id()) < 7 && (*it)->pdg_id() != 0) || 
			       (*it)->pdg_id() == 21 )){
      if(!incomingParton)
	incomingParton = (*it);
      else
	targetParton = (*it);
    }
  }
  
  // find incoming Protons (first entry ID=2212, IST=101 or 2 (from HW->PY conversion))
  for(HepMC::GenEvent::particle_const_iterator it = evt->particles_begin(); 
      (it != evt->particles_end() && targetProton==NULL); it++) {        

    if((*it)->status()==2 && (*it)->pdg_id()==2212) {
      if(!incomingProton)
	incomingProton = (*it);
      else
	targetProton = (*it);
    }
  }
  
  // find hard scale Q (computed from colliding partons)
  if( incomingParton && targetParton ) {
    math::XYZTLorentzVector totMomentum(0,0,0,0);
    totMomentum+=incomingParton->momentum();
    totMomentum+=targetParton->momentum();
    double evScale = totMomentum.mass(); 
    
    // get the PDF information
    const int id1=( incomingParton->pdg_id()==21 ? 0 : incomingParton->pdg_id() );
    const int id2=( targetParton->pdg_id()==21 ? 0 : targetParton->pdg_id() );
    pdfInfo.set_id1( id1 );
    pdfInfo.set_id2( id2 );

    if( incomingProton && targetProton ) {
    
      double x1 = incomingParton->momentum().pz()/incomingProton->momentum().pz();
      double x2 = targetParton->momentum().pz()/targetProton->momentum().pz();	
      pdfInfo.set_x1(x1);
      pdfInfo.set_x2(x2);	  
      
    }
    
    // we do not fill pdf1 & pdf2, since they are not easily available (what are they needed for anyways???)
    pdfInfo.set_scalePDF(evScale); // the same as Q above... does this make sense?
  } 
  

  gen::PdfInfo* info = new gen::PdfInfo();
  info->id.first   = pdfInfo.id1();
  info->id.second  = pdfInfo.id2();
  info->x.first    = pdfInfo.x1();
  info->x.second   = pdfInfo.x2();
  info->scalePDF   = pdfInfo.scalePDF();

  genEventInfo->setPDF(info);

  iEvent.put( genEventInfo );

  return;
  
}

// ------------ method called once each job just before starting event loop  ------------
void 
PdfInfoFixer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PdfInfoFixer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(PdfInfoFixer);
