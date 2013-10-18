import FWCore.ParameterSet.Config as cms

process = cms.Process("ANA")

process.load('MCatNLOUtilities/Configuration/PdfFixerSeq_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('file:MCatNLO_LHE_Zgamma_7TeV_cff_py_GEN.root')
)

process.myanalysis = cms.EDAnalyzer("ZeePDFAnalyzer")

process.TFileService = cms.Service("TFileService",
	fileName = cms.string("ZeePDF_histo.root")
)


process.p = cms.Path(process.pdfInfoFixing*process.myanalysis)
