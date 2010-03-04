import FWCore.ParameterSet.Config as cms

hcalClient = cms.EDAnalyzer("HcalMonitorClient",
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

                            # each client has a 'minerrror' (double) rate
                            # (minimum fraction of events that must be bad to be considered a problem),
                            # a 'minevents' integer
                            # (minimum number of events to be processed before evaluation occurs),
                            # and a BadChannelStatusMask value
                            # (channel status types that should be checked when looking for known bad channels)

                            # if the following are uncommented, they override the defaults on any
                            # clients that are not specified explicitly with their own prefixes
                            # minerrorrate         = cms.untracked.double(0.05),
                            # minevents            = cms.untracked.int32(1),
                            # BadChannelStatusMask = cms.untracked.int32(0),
                            
                            DeadCell_minerrorrate = cms.untracked.double(0.25),
                            DeadCell_minevents    = cms.untracked.int32(10),
                            
                            # Specify all clients to be run (name = prefix+"Monitor")

                            enabledClients = cms.untracked.vstring(["DeadCellMonitor",
                                                                    "HotCellMonitor",
                                                                    "RecHitMonitor",
                                                                    "DigiMonitor",
                                                                    "TrigPrimMonitor",
                                                                    "Summary"
                                                                    ]
                                                                   ),
                            )
