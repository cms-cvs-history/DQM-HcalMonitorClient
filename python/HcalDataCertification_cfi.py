import FWCore.ParameterSet.Config as cms

hcalDataCertification = cms.EDAnalyzer('HcalDataCertification',
                                       # analyzer variables go here
                                       debug = cms.untracked.int32(0)
)
