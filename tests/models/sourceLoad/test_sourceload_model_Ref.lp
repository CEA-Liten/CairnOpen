\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 122.671190485405 vWeightPV_Source
Subject To
 csInstalledElec_grid0:          vInstalledElec_grid  = 1
 csMaxValElec_grid0:             vMaxFlowElec_grid  = 10000
 csBigMInstalledElec_grid0:      - 10000 vInstalledElec_grid
                                 + vMaxFlowElec_grid <= 0
 csMinSizeInstalledElec_grid0:   vMaxFlowElec_grid >= 0
 cNotRunningIfNotInstalledElec_grid0: - vInstalledElec_grid
                                 + vStateElec_grid(0) <= 0
 cNotRunningIfNotInstalledElec_grid1: - vInstalledElec_grid
                                 + vStateElec_grid(1) <= 0
 cNotRunningIfNotInstalledElec_grid2: - vInstalledElec_grid
                                 + vStateElec_grid(2) <= 0
 cNotRunningIfNotInstalledElec_grid3: - vInstalledElec_grid
                                 + vStateElec_grid(3) <= 0
 cNotRunningIfNotInstalledElec_grid4: - vInstalledElec_grid
                                 + vStateElec_grid(4) <= 0
 cNotRunningIfNotInstalledElec_grid5: - vInstalledElec_grid
                                 + vStateElec_grid(5) <= 0
 cNotRunningIfNotInstalledElec_grid6: - vInstalledElec_grid
                                 + vStateElec_grid(6) <= 0
 cNotRunningIfNotInstalledElec_grid7: - vInstalledElec_grid
                                 + vStateElec_grid(7) <= 0
 cNotRunningIfNotInstalledElec_grid8: - vInstalledElec_grid
                                 + vStateElec_grid(8) <= 0
 cNotRunningIfNotInstalledElec_grid9: - vInstalledElec_grid
                                 + vStateElec_grid(9) <= 0
 cStateGridElec_grid0:           - 10000 vStateElec_grid(0) + vGFxElec_grid(0)
                                 <= 0
 cStateGridElec_grid1:           - 10000 vStateElec_grid(1) + vGFxElec_grid(1)
                                 <= 0
 cStateGridElec_grid2:           - 10000 vStateElec_grid(2) + vGFxElec_grid(2)
                                 <= 0
 cStateGridElec_grid3:           - 10000 vStateElec_grid(3) + vGFxElec_grid(3)
                                 <= 0
 cStateGridElec_grid4:           - 10000 vStateElec_grid(4) + vGFxElec_grid(4)
                                 <= 0
 cStateGridElec_grid5:           - 10000 vStateElec_grid(5) + vGFxElec_grid(5)
                                 <= 0
 cStateGridElec_grid6:           - 10000 vStateElec_grid(6) + vGFxElec_grid(6)
                                 <= 0
 cStateGridElec_grid7:           - 10000 vStateElec_grid(7) + vGFxElec_grid(7)
                                 <= 0
 cStateGridElec_grid8:           - 10000 vStateElec_grid(8) + vGFxElec_grid(8)
                                 <= 0
 cStateGridElec_grid9:           - 10000 vStateElec_grid(9) + vGFxElec_grid(9)
                                 <= 0
 cMaxGFxElec_grid0:              - vMaxFlowElec_grid + vGFxElec_grid(0) <= 0
 cMaxGFxElec_grid1:              - vMaxFlowElec_grid + vGFxElec_grid(1) <= 0
 cMaxGFxElec_grid2:              - vMaxFlowElec_grid + vGFxElec_grid(2) <= 0
 cMaxGFxElec_grid3:              - vMaxFlowElec_grid + vGFxElec_grid(3) <= 0
 cMaxGFxElec_grid4:              - vMaxFlowElec_grid + vGFxElec_grid(4) <= 0
 cMaxGFxElec_grid5:              - vMaxFlowElec_grid + vGFxElec_grid(5) <= 0
 cMaxGFxElec_grid6:              - vMaxFlowElec_grid + vGFxElec_grid(6) <= 0
 cMaxGFxElec_grid7:              - vMaxFlowElec_grid + vGFxElec_grid(7) <= 0
 cMaxGFxElec_grid8:              - vMaxFlowElec_grid + vGFxElec_grid(8) <= 0
 cMaxGFxElec_grid9:              - vMaxFlowElec_grid + vGFxElec_grid(9) <= 0
 csInstalledElec_load0:          vInstalledElec_load  = 1
 csWeightElec_load0:             vWeightElec_load  = 20
 csBigMInstalledElec_load0:      - 20 vInstalledElec_load + vWeightElec_load
                                 <= 0
 csMinSizeInstalledElec_load0:   vWeightElec_load >= 0
 cMSFxElec_load0:                <= 10000
 cMSFxElec_load1:                <= 10000
 cMSFxElec_load2:                <= 10000
 cMSFxElec_load3:                <= 10000
 cMSFxElec_load4:                <= 10000
 cMSFxElec_load5:                <= 10000
 cMSFxElec_load6:                <= 10000
 cMSFxElec_load7:                <= 10000
 cMSFxElec_load8:                <= 10000
 cMSFxElec_load9:                <= 10000
 csBigMInstalledPV_Source0:      - 25 vInstalledPV_Source + vWeightPV_Source
                                 <= 0
 csMinSizeInstalledPV_Source0:   vWeightPV_Source >= 0
 cMSFxPV_Source0:                <= 10
 cMSFxPV_Source1:                <= 10
 cMSFxPV_Source2:                <= 10
 cMSFxPV_Source3:                <= 10
 cMSFxPV_Source4:                <= 10
 cMSFxPV_Source5:                <= 10
 cMSFxPV_Source6:                <= 10
 cMSFxPV_Source7:                <= 10
 cMSFxPV_Source8:                <= 10
 cMSFxPV_Source9:                <= 10
 cSNodeLaw#10:                   - vGFxElec_grid(0) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#11:                   - vGFxElec_grid(1) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#12:                   - vGFxElec_grid(2) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#13:                   - vGFxElec_grid(3) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#14:                   - vGFxElec_grid(4) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#15:                   - vGFxElec_grid(5) - 0.9 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#16:                   - vGFxElec_grid(6) - 1.2 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#17:                   - vGFxElec_grid(7) - 1.2 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#18:                   - vGFxElec_grid(8) - 1.5 vWeightElec_load
                                 + 10 vWeightPV_Source  = 0
 cSNodeLaw#19:                   - vGFxElec_grid(9) - 1.5 vWeightElec_load
                                 + 5 vWeightPV_Source  = 0
Bounds
 0 <= vInstalledElec_grid <= 1
 0 <= vMaxFlowElec_grid <= 10000
 0 <= vStateElec_grid(0) <= 1
 0 <= vStateElec_grid(1) <= 1
 0 <= vStateElec_grid(2) <= 1
 0 <= vStateElec_grid(3) <= 1
 0 <= vStateElec_grid(4) <= 1
 0 <= vStateElec_grid(5) <= 1
 0 <= vStateElec_grid(6) <= 1
 0 <= vStateElec_grid(7) <= 1
 0 <= vStateElec_grid(8) <= 1
 0 <= vStateElec_grid(9) <= 1
 0 <= vGFxElec_grid(0) <= 10000
 0 <= vGFxElec_grid(1) <= 10000
 0 <= vGFxElec_grid(2) <= 10000
 0 <= vGFxElec_grid(3) <= 10000
 0 <= vGFxElec_grid(4) <= 10000
 0 <= vGFxElec_grid(5) <= 10000
 0 <= vGFxElec_grid(6) <= 10000
 0 <= vGFxElec_grid(7) <= 10000
 0 <= vGFxElec_grid(8) <= 10000
 0 <= vGFxElec_grid(9) <= 10000
 0 <= vInstalledElec_load <= 1
 0 <= vWeightElec_load <= 20
 0 <= vInstalledPV_Source <= 1
 0 <= vWeightPV_Source <= 25
Generals
 vInstalledElec_grid  vStateElec_grid(0)  vStateElec_grid(1) 
 vStateElec_grid(2)  vStateElec_grid(3)  vStateElec_grid(4) 
 vStateElec_grid(5)  vStateElec_grid(6)  vStateElec_grid(7) 
 vStateElec_grid(8)  vStateElec_grid(9)  vInstalledElec_load 
 vInstalledPV_Source 
End
