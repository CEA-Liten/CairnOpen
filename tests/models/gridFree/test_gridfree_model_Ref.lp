\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 175200 vGFxElec_grid(0) + 175200 vGFxElec_grid(1)
       + 175200 vGFxElec_grid(2) + 175200 vGFxElec_grid(3)
       + 175200 vGFxElec_grid(4) + 175200 vGFxElec_grid(5)
       + 175200 vGFxElec_grid(6) + 175200 vGFxElec_grid(7)
       + 175200 vGFxElec_grid(8) + 175200 vGFxElec_grid(9)
       + 1752000 vGFxElec_grid_backup(0) + 1752000 vGFxElec_grid_backup(1)
       + 1752000 vGFxElec_grid_backup(2) + 1752000 vGFxElec_grid_backup(3)
       + 1752000 vGFxElec_grid_backup(4) + 1752000 vGFxElec_grid_backup(5)
       + 1752000 vGFxElec_grid_backup(6) + 1752000 vGFxElec_grid_backup(7)
       + 1752000 vGFxElec_grid_backup(8) + 1752000 vGFxElec_grid_backup(9)
       + 2628000 vGFxElec_grid_backup_2(0) + 2628000 vGFxElec_grid_backup_2(1)
       + 2628000 vGFxElec_grid_backup_2(2) + 2628000 vGFxElec_grid_backup_2(3)
       + 2628000 vGFxElec_grid_backup_2(4) + 2628000 vGFxElec_grid_backup_2(5)
       + 2628000 vGFxElec_grid_backup_2(6) + 876000 vGFxElec_grid_backup_2(7)
       + 876000 vGFxElec_grid_backup_2(8) + 876000 vGFxElec_grid_backup_2(9)
Subject To
 csInstalledElec_grid0:          vInstalledElec_grid  = 1
 csMaxValElec_grid0:             vMaxFlowElec_grid  = 500
 csBigMInstalledElec_grid0:      - 500 vInstalledElec_grid + vMaxFlowElec_grid
                                 <= 0
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
 cStateGridElec_grid0:           - 500 vStateElec_grid(0) + vGFxElec_grid(0)
                                 <= 0
 cStateGridElec_grid1:           - 500 vStateElec_grid(1) + vGFxElec_grid(1)
                                 <= 0
 cStateGridElec_grid2:           - 500 vStateElec_grid(2) + vGFxElec_grid(2)
                                 <= 0
 cStateGridElec_grid3:           - 500 vStateElec_grid(3) + vGFxElec_grid(3)
                                 <= 0
 cStateGridElec_grid4:           - 500 vStateElec_grid(4) + vGFxElec_grid(4)
                                 <= 0
 cStateGridElec_grid5:           - 500 vStateElec_grid(5) + vGFxElec_grid(5)
                                 <= 0
 cStateGridElec_grid6:           - 500 vStateElec_grid(6) + vGFxElec_grid(6)
                                 <= 0
 cStateGridElec_grid7:           - 500 vStateElec_grid(7) + vGFxElec_grid(7)
                                 <= 0
 cStateGridElec_grid8:           - 500 vStateElec_grid(8) + vGFxElec_grid(8)
                                 <= 0
 cStateGridElec_grid9:           - 500 vStateElec_grid(9) + vGFxElec_grid(9)
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
 cMPowElec_grid0:                vGFxElec_grid(0) <= 500
 cMPowElec_grid1:                vGFxElec_grid(1) <= 500
 cMPowElec_grid2:                vGFxElec_grid(2) <= 500
 cMPowElec_grid3:                vGFxElec_grid(3) <= 500
 cMPowElec_grid4:                vGFxElec_grid(4) <= 100
 cMPowElec_grid5:                vGFxElec_grid(5) <= 100
 cMPowElec_grid6:                vGFxElec_grid(6) <= 100
 cMPowElec_grid7:                vGFxElec_grid(7) <= 100
 cMPowElec_grid8:                vGFxElec_grid(8) <= 100
 cMPowElec_grid9:                vGFxElec_grid(9) <= 100
 csInstalledElec_grid_backup0:   vInstalledElec_grid_backup  = 1
 csMaxValElec_grid_backup0:      vMaxFlowElec_grid_backup  = 500
 csBigMInstalledElec_grid_backup0: - 500 vInstalledElec_grid_backup
                                 + vMaxFlowElec_grid_backup <= 0
 csMinSizeInstalledElec_grid_backup0: vMaxFlowElec_grid_backup >= 0
 cNotRunningIfNotInstalledElec_grid_backup0: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(0) <= 0
 cNotRunningIfNotInstalledElec_grid_backup1: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(1) <= 0
 cNotRunningIfNotInstalledElec_grid_backup2: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(2) <= 0
 cNotRunningIfNotInstalledElec_grid_backup3: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(3) <= 0
 cNotRunningIfNotInstalledElec_grid_backup4: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(4) <= 0
 cNotRunningIfNotInstalledElec_grid_backup5: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(5) <= 0
 cNotRunningIfNotInstalledElec_grid_backup6: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(6) <= 0
 cNotRunningIfNotInstalledElec_grid_backup7: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(7) <= 0
 cNotRunningIfNotInstalledElec_grid_backup8: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(8) <= 0
 cNotRunningIfNotInstalledElec_grid_backup9: - vInstalledElec_grid_backup
                                 + vStateElec_grid_backup(9) <= 0
 cStateGridElec_grid_backup0:    - 500 vStateElec_grid_backup(0)
                                 + vGFxElec_grid_backup(0) <= 0
 cStateGridElec_grid_backup1:    - 500 vStateElec_grid_backup(1)
                                 + vGFxElec_grid_backup(1) <= 0
 cStateGridElec_grid_backup2:    - 500 vStateElec_grid_backup(2)
                                 + vGFxElec_grid_backup(2) <= 0
 cStateGridElec_grid_backup3:    - 500 vStateElec_grid_backup(3)
                                 + vGFxElec_grid_backup(3) <= 0
 cStateGridElec_grid_backup4:    - 500 vStateElec_grid_backup(4)
                                 + vGFxElec_grid_backup(4) <= 0
 cStateGridElec_grid_backup5:    - 500 vStateElec_grid_backup(5)
                                 + vGFxElec_grid_backup(5) <= 0
 cStateGridElec_grid_backup6:    - 500 vStateElec_grid_backup(6)
                                 + vGFxElec_grid_backup(6) <= 0
 cStateGridElec_grid_backup7:    - 500 vStateElec_grid_backup(7)
                                 + vGFxElec_grid_backup(7) <= 0
 cStateGridElec_grid_backup8:    - 500 vStateElec_grid_backup(8)
                                 + vGFxElec_grid_backup(8) <= 0
 cStateGridElec_grid_backup9:    - 500 vStateElec_grid_backup(9)
                                 + vGFxElec_grid_backup(9) <= 0
 cMaxGFxElec_grid_backup0:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(0) <= 0
 cMaxGFxElec_grid_backup1:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(1) <= 0
 cMaxGFxElec_grid_backup2:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(2) <= 0
 cMaxGFxElec_grid_backup3:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(3) <= 0
 cMaxGFxElec_grid_backup4:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(4) <= 0
 cMaxGFxElec_grid_backup5:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(5) <= 0
 cMaxGFxElec_grid_backup6:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(6) <= 0
 cMaxGFxElec_grid_backup7:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(7) <= 0
 cMaxGFxElec_grid_backup8:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(8) <= 0
 cMaxGFxElec_grid_backup9:       - vMaxFlowElec_grid_backup
                                 + vGFxElec_grid_backup(9) <= 0
 csInstalledElec_grid_backup_20: vInstalledElec_grid_backup_2  = 1
 csMaxValElec_grid_backup_20:    vMaxFlowElec_grid_backup_2  = 500
 csBigMInstalledElec_grid_backup_20: - 500 vInstalledElec_grid_backup_2
                                 + vMaxFlowElec_grid_backup_2 <= 0
 csMinSizeInstalledElec_grid_backup_20: vMaxFlowElec_grid_backup_2 >= 0
 cNotRunningIfNotInstalledElec_grid_backup_20: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(0) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_21: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(1) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_22: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(2) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_23: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(3) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_24: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(4) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_25: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(5) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_26: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(6) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_27: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(7) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_28: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(8) <= 0
 cNotRunningIfNotInstalledElec_grid_backup_29: - vInstalledElec_grid_backup_2
                                 + vStateElec_grid_backup_2(9) <= 0
 cStateGridElec_grid_backup_20:  - 500 vStateElec_grid_backup_2(0)
                                 + vGFxElec_grid_backup_2(0) <= 0
 cStateGridElec_grid_backup_21:  - 500 vStateElec_grid_backup_2(1)
                                 + vGFxElec_grid_backup_2(1) <= 0
 cStateGridElec_grid_backup_22:  - 500 vStateElec_grid_backup_2(2)
                                 + vGFxElec_grid_backup_2(2) <= 0
 cStateGridElec_grid_backup_23:  - 500 vStateElec_grid_backup_2(3)
                                 + vGFxElec_grid_backup_2(3) <= 0
 cStateGridElec_grid_backup_24:  - 500 vStateElec_grid_backup_2(4)
                                 + vGFxElec_grid_backup_2(4) <= 0
 cStateGridElec_grid_backup_25:  - 500 vStateElec_grid_backup_2(5)
                                 + vGFxElec_grid_backup_2(5) <= 0
 cStateGridElec_grid_backup_26:  - 500 vStateElec_grid_backup_2(6)
                                 + vGFxElec_grid_backup_2(6) <= 0
 cStateGridElec_grid_backup_27:  - 500 vStateElec_grid_backup_2(7)
                                 + vGFxElec_grid_backup_2(7) <= 0
 cStateGridElec_grid_backup_28:  - 500 vStateElec_grid_backup_2(8)
                                 + vGFxElec_grid_backup_2(8) <= 0
 cStateGridElec_grid_backup_29:  - 500 vStateElec_grid_backup_2(9)
                                 + vGFxElec_grid_backup_2(9) <= 0
 cMaxGFxElec_grid_backup_20:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(0) <= 0
 cMaxGFxElec_grid_backup_21:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(1) <= 0
 cMaxGFxElec_grid_backup_22:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(2) <= 0
 cMaxGFxElec_grid_backup_23:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(3) <= 0
 cMaxGFxElec_grid_backup_24:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(4) <= 0
 cMaxGFxElec_grid_backup_25:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(5) <= 0
 cMaxGFxElec_grid_backup_26:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(6) <= 0
 cMaxGFxElec_grid_backup_27:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(7) <= 0
 cMaxGFxElec_grid_backup_28:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(8) <= 0
 cMaxGFxElec_grid_backup_29:     - vMaxFlowElec_grid_backup_2
                                 + vGFxElec_grid_backup_2(9) <= 0
 csInstalledElec_load0:          vInstalledElec_load  = 1
 csWeightElec_load0:             vWeightElec_load  = 1
 csBigMInstalledElec_load0:      - vInstalledElec_load + vWeightElec_load <= 0
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
 cSNodeLaw#10:                   vGFxElec_grid(0) + vGFxElec_grid_backup(0)
                                 + vGFxElec_grid_backup_2(0)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#11:                   vGFxElec_grid(1) + vGFxElec_grid_backup(1)
                                 + vGFxElec_grid_backup_2(1)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#12:                   vGFxElec_grid(2) + vGFxElec_grid_backup(2)
                                 + vGFxElec_grid_backup_2(2)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#13:                   vGFxElec_grid(3) + vGFxElec_grid_backup(3)
                                 + vGFxElec_grid_backup_2(3)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#14:                   vGFxElec_grid(4) + vGFxElec_grid_backup(4)
                                 + vGFxElec_grid_backup_2(4)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#15:                   vGFxElec_grid(5) + vGFxElec_grid_backup(5)
                                 + vGFxElec_grid_backup_2(5)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#16:                   vGFxElec_grid(6) + vGFxElec_grid_backup(6)
                                 + vGFxElec_grid_backup_2(6)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#17:                   vGFxElec_grid(7) + vGFxElec_grid_backup(7)
                                 + vGFxElec_grid_backup_2(7)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#18:                   vGFxElec_grid(8) + vGFxElec_grid_backup(8)
                                 + vGFxElec_grid_backup_2(8)
                                 - 400 vWeightElec_load  = 0
 cSNodeLaw#19:                   vGFxElec_grid(9) + vGFxElec_grid_backup(9)
                                 + vGFxElec_grid_backup_2(9)
                                 - 400 vWeightElec_load  = 0
Bounds
 0 <= vInstalledElec_grid <= 1
 0 <= vMaxFlowElec_grid <= 500
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
 0 <= vGFxElec_grid(0) <= 500
 0 <= vGFxElec_grid(1) <= 500
 0 <= vGFxElec_grid(2) <= 500
 0 <= vGFxElec_grid(3) <= 500
 0 <= vGFxElec_grid(4) <= 500
 0 <= vGFxElec_grid(5) <= 500
 0 <= vGFxElec_grid(6) <= 500
 0 <= vGFxElec_grid(7) <= 500
 0 <= vGFxElec_grid(8) <= 500
 0 <= vGFxElec_grid(9) <= 500
 0 <= vInstalledElec_grid_backup <= 1
 0 <= vMaxFlowElec_grid_backup <= 500
 0 <= vStateElec_grid_backup(0) <= 1
 0 <= vStateElec_grid_backup(1) <= 1
 0 <= vStateElec_grid_backup(2) <= 1
 0 <= vStateElec_grid_backup(3) <= 1
 0 <= vStateElec_grid_backup(4) <= 1
 0 <= vStateElec_grid_backup(5) <= 1
 0 <= vStateElec_grid_backup(6) <= 1
 0 <= vStateElec_grid_backup(7) <= 1
 0 <= vStateElec_grid_backup(8) <= 1
 0 <= vStateElec_grid_backup(9) <= 1
 0 <= vGFxElec_grid_backup(0) <= 500
 0 <= vGFxElec_grid_backup(1) <= 500
 0 <= vGFxElec_grid_backup(2) <= 500
 0 <= vGFxElec_grid_backup(3) <= 500
 0 <= vGFxElec_grid_backup(4) <= 500
 0 <= vGFxElec_grid_backup(5) <= 500
 0 <= vGFxElec_grid_backup(6) <= 500
 0 <= vGFxElec_grid_backup(7) <= 500
 0 <= vGFxElec_grid_backup(8) <= 500
 0 <= vGFxElec_grid_backup(9) <= 500
 0 <= vInstalledElec_grid_backup_2 <= 1
 0 <= vMaxFlowElec_grid_backup_2 <= 500
 0 <= vStateElec_grid_backup_2(0) <= 1
 0 <= vStateElec_grid_backup_2(1) <= 1
 0 <= vStateElec_grid_backup_2(2) <= 1
 0 <= vStateElec_grid_backup_2(3) <= 1
 0 <= vStateElec_grid_backup_2(4) <= 1
 0 <= vStateElec_grid_backup_2(5) <= 1
 0 <= vStateElec_grid_backup_2(6) <= 1
 0 <= vStateElec_grid_backup_2(7) <= 1
 0 <= vStateElec_grid_backup_2(8) <= 1
 0 <= vStateElec_grid_backup_2(9) <= 1
 50 <= vGFxElec_grid_backup_2(0) <= 500
 50 <= vGFxElec_grid_backup_2(1) <= 500
 50 <= vGFxElec_grid_backup_2(2) <= 500
 50 <= vGFxElec_grid_backup_2(3) <= 500
 50 <= vGFxElec_grid_backup_2(4) <= 500
 50 <= vGFxElec_grid_backup_2(5) <= 500
 50 <= vGFxElec_grid_backup_2(6) <= 500
 50 <= vGFxElec_grid_backup_2(7) <= 500
 50 <= vGFxElec_grid_backup_2(8) <= 500
 50 <= vGFxElec_grid_backup_2(9) <= 500
 0 <= vInstalledElec_load <= 1
 0 <= vWeightElec_load <= 1
Generals
 vInstalledElec_grid  vStateElec_grid(0)  vStateElec_grid(1) 
 vStateElec_grid(2)  vStateElec_grid(3)  vStateElec_grid(4) 
 vStateElec_grid(5)  vStateElec_grid(6)  vStateElec_grid(7) 
 vStateElec_grid(8)  vStateElec_grid(9)  vInstalledElec_grid_backup 
 vStateElec_grid_backup(0)  vStateElec_grid_backup(1) 
 vStateElec_grid_backup(2)  vStateElec_grid_backup(3) 
 vStateElec_grid_backup(4)  vStateElec_grid_backup(5) 
 vStateElec_grid_backup(6)  vStateElec_grid_backup(7) 
 vStateElec_grid_backup(8)  vStateElec_grid_backup(9) 
 vInstalledElec_grid_backup_2  vStateElec_grid_backup_2(0) 
 vStateElec_grid_backup_2(1)  vStateElec_grid_backup_2(2) 
 vStateElec_grid_backup_2(3)  vStateElec_grid_backup_2(4) 
 vStateElec_grid_backup_2(5)  vStateElec_grid_backup_2(6) 
 vStateElec_grid_backup_2(7)  vStateElec_grid_backup_2(8) 
 vStateElec_grid_backup_2(9)  vInstalledElec_load 
End
