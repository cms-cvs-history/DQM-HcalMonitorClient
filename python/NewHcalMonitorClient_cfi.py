import FWCore.ParameterSet.Config as cms

newHcalClient = cms.EDAnalyzer("NewHcalMonitorClient",
                               debug=cms.untracked.int32(0),
                               inputFile=cms.untracked.string(""),
                               mergeRuns=cms.untracked.bool(False),
                               cloneME=cms.untracked.bool(False),
                               prescaleFactor=cms.untracked.int32(-1),
                               prefixME=cms.untracked.string("Hcal/"),
                               enableCleanup=cms.untracked.bool(False),

                               baseHtmlDir = cms.untracked.string(""),
                               htmlUpdateTime = cms.untracked.int32(0),
                               databasedir = cms.untracked.string(""),
                               databaseUpdateTime = cms.untracked.int32(0),

                               DeadCell_minerrorrate = cms.untracked.double(0.25),
                               DeadCell_minevents    = cms.untracked.int32(10),
                               
                               enabledClients = cms.untracked.vstring(["DeadCellMonitor"]),
                               )
