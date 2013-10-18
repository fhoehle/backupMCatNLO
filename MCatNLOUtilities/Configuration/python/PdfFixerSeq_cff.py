import FWCore.ParameterSet.Config as cms

from MCatNLOUtilities.PdfInfoFixer.PdfInfoFixer_cfi import *

PdfFixer = cms.Sequence(pdfInfoFixing)
