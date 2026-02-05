\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 11.1335595242702 vMaxPowerCompressor + 0 vPOutCompCompressor(0)
       + 0 vPOutCompCompressor(1) + 0 vPOutCompCompressor(2)
       + 0 vPOutCompCompressor(3) + 0 vPOutCompCompressor(4)
       + 0 vPOutCompCompressor(5) + 0 vPOutCompCompressor(6)
       + 0 vPOutCompCompressor(7) + 0 vPOutCompCompressor(8)
       + 0 vPOutCompCompressor(9)
Subject To
 csBigMInstalledCompressor0:     - 2.67780822512165 vInstalledCompressor
                                 + vMaxPowerCompressor <= 0
 csMinSizeInstalledCompressor0:  vMaxPowerCompressor >= 0
 cNotRunningIfNotInstalledCompressor0: - vInstalledCompressor
                                 + vStateCompressor(0) <= 0
 cNotRunningIfNotInstalledCompressor1: - vInstalledCompressor
                                 + vStateCompressor(1) <= 0
 cNotRunningIfNotInstalledCompressor2: - vInstalledCompressor
                                 + vStateCompressor(2) <= 0
 cNotRunningIfNotInstalledCompressor3: - vInstalledCompressor
                                 + vStateCompressor(3) <= 0
 cNotRunningIfNotInstalledCompressor4: - vInstalledCompressor
                                 + vStateCompressor(4) <= 0
 cNotRunningIfNotInstalledCompressor5: - vInstalledCompressor
                                 + vStateCompressor(5) <= 0
 cNotRunningIfNotInstalledCompressor6: - vInstalledCompressor
                                 + vStateCompressor(6) <= 0
 cNotRunningIfNotInstalledCompressor7: - vInstalledCompressor
                                 + vStateCompressor(7) <= 0
 cNotRunningIfNotInstalledCompressor8: - vInstalledCompressor
                                 + vStateCompressor(8) <= 0
 cNotRunningIfNotInstalledCompressor9: - vInstalledCompressor
                                 + vStateCompressor(9) <= 0
 cPowCompCompressor0:            vUsedPowerCompressor(0)
                                 - 0.00107112329004866 vMassFlowCompressor(0)
                                  = 0
 cPowCompCompressor1:            vUsedPowerCompressor(1)
                                 - 0.00107112329004866 vMassFlowCompressor(1)
                                  = 0
 cPowCompCompressor2:            vUsedPowerCompressor(2)
                                 - 0.00107112329004866 vMassFlowCompressor(2)
                                  = 0
 cPowCompCompressor3:            vUsedPowerCompressor(3)
                                 - 0.00107112329004866 vMassFlowCompressor(3)
                                  = 0
 cPowCompCompressor4:            vUsedPowerCompressor(4)
                                 - 0.00107112329004866 vMassFlowCompressor(4)
                                  = 0
 cPowCompCompressor5:            vUsedPowerCompressor(5)
                                 - 0.00107112329004866 vMassFlowCompressor(5)
                                  = 0
 cPowCompCompressor6:            vUsedPowerCompressor(6)
                                 - 0.00107112329004866 vMassFlowCompressor(6)
                                  = 0
 cPowCompCompressor7:            vUsedPowerCompressor(7)
                                 - 0.00107112329004866 vMassFlowCompressor(7)
                                  = 0
 cPowCompCompressor8:            vUsedPowerCompressor(8)
                                 - 0.00107112329004866 vMassFlowCompressor(8)
                                  = 0
 cPowCompCompressor9:            vUsedPowerCompressor(9)
                                 - 0.00107112329004866 vMassFlowCompressor(9)
                                  = 0
 cUseCompCompressor0:            - 2.67780822512165 vStateCompressor(0)
                                 + vUsedPowerCompressor(0) <= 0
 cPowMinCompressor0:             vUsedPowerCompressor(0) >= 0
 cMaxCompressor0:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(0) <= 0
 cUseCompCompressor1:            - 2.67780822512165 vStateCompressor(1)
                                 + vUsedPowerCompressor(1) <= 0
 cPowMinCompressor1:             vUsedPowerCompressor(1) >= 0
 cMaxCompressor1:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(1) <= 0
 cUseCompCompressor2:            - 2.67780822512165 vStateCompressor(2)
                                 + vUsedPowerCompressor(2) <= 0
 cPowMinCompressor2:             vUsedPowerCompressor(2) >= 0
 cMaxCompressor2:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(2) <= 0
 cUseCompCompressor3:            - 2.67780822512165 vStateCompressor(3)
                                 + vUsedPowerCompressor(3) <= 0
 cPowMinCompressor3:             vUsedPowerCompressor(3) >= 0
 cMaxCompressor3:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(3) <= 0
 cUseCompCompressor4:            - 2.67780822512165 vStateCompressor(4)
                                 + vUsedPowerCompressor(4) <= 0
 cPowMinCompressor4:             vUsedPowerCompressor(4) >= 0
 cMaxCompressor4:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(4) <= 0
 cUseCompCompressor5:            - 2.67780822512165 vStateCompressor(5)
                                 + vUsedPowerCompressor(5) <= 0
 cPowMinCompressor5:             vUsedPowerCompressor(5) >= 0
 cMaxCompressor5:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(5) <= 0
 cUseCompCompressor6:            - 2.67780822512165 vStateCompressor(6)
                                 + vUsedPowerCompressor(6) <= 0
 cPowMinCompressor6:             vUsedPowerCompressor(6) >= 0
 cMaxCompressor6:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(6) <= 0
 cUseCompCompressor7:            - 2.67780822512165 vStateCompressor(7)
                                 + vUsedPowerCompressor(7) <= 0
 cPowMinCompressor7:             vUsedPowerCompressor(7) >= 0
 cMaxCompressor7:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(7) <= 0
 cUseCompCompressor8:            - 2.67780822512165 vStateCompressor(8)
                                 + vUsedPowerCompressor(8) <= 0
 cPowMinCompressor8:             vUsedPowerCompressor(8) >= 0
 cMaxCompressor8:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(8) <= 0
 cUseCompCompressor9:            - 2.67780822512165 vStateCompressor(9)
                                 + vUsedPowerCompressor(9) <= 0
 cPowMinCompressor9:             vUsedPowerCompressor(9) >= 0
 cMaxCompressor9:                - vMaxPowerCompressor
                                 + vUsedPowerCompressor(9) <= 0
 cTOutCompressor0:               vToutletCompressor  = 26.3522769417062
 csInstalledElecGrid0:           vInstalledElecGrid  = 1
 csMaxValElecGrid0:              vMaxFlowElecGrid  = 10000
 csBigMInstalledElecGrid0:       - 10000 vInstalledElecGrid + vMaxFlowElecGrid
                                 <= 0
 csMinSizeInstalledElecGrid0:    vMaxFlowElecGrid >= 0
 cNotRunningIfNotInstalledElecGrid0: - vInstalledElecGrid + vStateElecGrid(0)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid1: - vInstalledElecGrid + vStateElecGrid(1)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid2: - vInstalledElecGrid + vStateElecGrid(2)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid3: - vInstalledElecGrid + vStateElecGrid(3)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid4: - vInstalledElecGrid + vStateElecGrid(4)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid5: - vInstalledElecGrid + vStateElecGrid(5)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid6: - vInstalledElecGrid + vStateElecGrid(6)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid7: - vInstalledElecGrid + vStateElecGrid(7)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid8: - vInstalledElecGrid + vStateElecGrid(8)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid9: - vInstalledElecGrid + vStateElecGrid(9)
                                 <= 0
 cStateGridElecGrid0:            - 10000 vStateElecGrid(0) + vGFxElecGrid(0)
                                 <= 0
 cStateGridElecGrid1:            - 10000 vStateElecGrid(1) + vGFxElecGrid(1)
                                 <= 0
 cStateGridElecGrid2:            - 10000 vStateElecGrid(2) + vGFxElecGrid(2)
                                 <= 0
 cStateGridElecGrid3:            - 10000 vStateElecGrid(3) + vGFxElecGrid(3)
                                 <= 0
 cStateGridElecGrid4:            - 10000 vStateElecGrid(4) + vGFxElecGrid(4)
                                 <= 0
 cStateGridElecGrid5:            - 10000 vStateElecGrid(5) + vGFxElecGrid(5)
                                 <= 0
 cStateGridElecGrid6:            - 10000 vStateElecGrid(6) + vGFxElecGrid(6)
                                 <= 0
 cStateGridElecGrid7:            - 10000 vStateElecGrid(7) + vGFxElecGrid(7)
                                 <= 0
 cStateGridElecGrid8:            - 10000 vStateElecGrid(8) + vGFxElecGrid(8)
                                 <= 0
 cStateGridElecGrid9:            - 10000 vStateElecGrid(9) + vGFxElecGrid(9)
                                 <= 0
 cMaxGFxElecGrid0:               - vMaxFlowElecGrid + vGFxElecGrid(0) <= 0
 cMaxGFxElecGrid1:               - vMaxFlowElecGrid + vGFxElecGrid(1) <= 0
 cMaxGFxElecGrid2:               - vMaxFlowElecGrid + vGFxElecGrid(2) <= 0
 cMaxGFxElecGrid3:               - vMaxFlowElecGrid + vGFxElecGrid(3) <= 0
 cMaxGFxElecGrid4:               - vMaxFlowElecGrid + vGFxElecGrid(4) <= 0
 cMaxGFxElecGrid5:               - vMaxFlowElecGrid + vGFxElecGrid(5) <= 0
 cMaxGFxElecGrid6:               - vMaxFlowElecGrid + vGFxElecGrid(6) <= 0
 cMaxGFxElecGrid7:               - vMaxFlowElecGrid + vGFxElecGrid(7) <= 0
 cMaxGFxElecGrid8:               - vMaxFlowElecGrid + vGFxElecGrid(8) <= 0
 cMaxGFxElecGrid9:               - vMaxFlowElecGrid + vGFxElecGrid(9) <= 0
 csInstalledH2Grid0:             vInstalledH2Grid  = 1
 csMaxValH2Grid0:                vMaxFlowH2Grid  = 10000
 csBigMInstalledH2Grid0:         - 10000 vInstalledH2Grid + vMaxFlowH2Grid <= 0
 csMinSizeInstalledH2Grid0:      vMaxFlowH2Grid >= 0
 cNotRunningIfNotInstalledH2Grid0: - vInstalledH2Grid + vStateH2Grid(0) <= 0
 cNotRunningIfNotInstalledH2Grid1: - vInstalledH2Grid + vStateH2Grid(1) <= 0
 cNotRunningIfNotInstalledH2Grid2: - vInstalledH2Grid + vStateH2Grid(2) <= 0
 cNotRunningIfNotInstalledH2Grid3: - vInstalledH2Grid + vStateH2Grid(3) <= 0
 cNotRunningIfNotInstalledH2Grid4: - vInstalledH2Grid + vStateH2Grid(4) <= 0
 cNotRunningIfNotInstalledH2Grid5: - vInstalledH2Grid + vStateH2Grid(5) <= 0
 cNotRunningIfNotInstalledH2Grid6: - vInstalledH2Grid + vStateH2Grid(6) <= 0
 cNotRunningIfNotInstalledH2Grid7: - vInstalledH2Grid + vStateH2Grid(7) <= 0
 cNotRunningIfNotInstalledH2Grid8: - vInstalledH2Grid + vStateH2Grid(8) <= 0
 cNotRunningIfNotInstalledH2Grid9: - vInstalledH2Grid + vStateH2Grid(9) <= 0
 cStateGridH2Grid0:              - 10000 vStateH2Grid(0) + vGFxH2Grid(0) <= 0
 cStateGridH2Grid1:              - 10000 vStateH2Grid(1) + vGFxH2Grid(1) <= 0
 cStateGridH2Grid2:              - 10000 vStateH2Grid(2) + vGFxH2Grid(2) <= 0
 cStateGridH2Grid3:              - 10000 vStateH2Grid(3) + vGFxH2Grid(3) <= 0
 cStateGridH2Grid4:              - 10000 vStateH2Grid(4) + vGFxH2Grid(4) <= 0
 cStateGridH2Grid5:              - 10000 vStateH2Grid(5) + vGFxH2Grid(5) <= 0
 cStateGridH2Grid6:              - 10000 vStateH2Grid(6) + vGFxH2Grid(6) <= 0
 cStateGridH2Grid7:              - 10000 vStateH2Grid(7) + vGFxH2Grid(7) <= 0
 cStateGridH2Grid8:              - 10000 vStateH2Grid(8) + vGFxH2Grid(8) <= 0
 cStateGridH2Grid9:              - 10000 vStateH2Grid(9) + vGFxH2Grid(9) <= 0
 cMaxGFxH2Grid0:                 - vMaxFlowH2Grid + vGFxH2Grid(0) <= 0
 cMaxGFxH2Grid1:                 - vMaxFlowH2Grid + vGFxH2Grid(1) <= 0
 cMaxGFxH2Grid2:                 - vMaxFlowH2Grid + vGFxH2Grid(2) <= 0
 cMaxGFxH2Grid3:                 - vMaxFlowH2Grid + vGFxH2Grid(3) <= 0
 cMaxGFxH2Grid4:                 - vMaxFlowH2Grid + vGFxH2Grid(4) <= 0
 cMaxGFxH2Grid5:                 - vMaxFlowH2Grid + vGFxH2Grid(5) <= 0
 cMaxGFxH2Grid6:                 - vMaxFlowH2Grid + vGFxH2Grid(6) <= 0
 cMaxGFxH2Grid7:                 - vMaxFlowH2Grid + vGFxH2Grid(7) <= 0
 cMaxGFxH2Grid8:                 - vMaxFlowH2Grid + vGFxH2Grid(8) <= 0
 cMaxGFxH2Grid9:                 - vMaxFlowH2Grid + vGFxH2Grid(9) <= 0
 csInstalledH2Load0:             vInstalledH2Load  = 1
 csWeightH2Load0:                vWeightH2Load  = 1
 csBigMInstalledH2Load0:         - vInstalledH2Load + vWeightH2Load <= 0
 csMinSizeInstalledH2Load0:      vWeightH2Load >= 0
 cMSFxH2Load0:                   <= 10000
 cMSFxH2Load1:                   <= 10000
 cMSFxH2Load2:                   <= 10000
 cMSFxH2Load3:                   <= 10000
 cMSFxH2Load4:                   <= 10000
 cMSFxH2Load5:                   <= 10000
 cMSFxH2Load6:                   <= 10000
 cMSFxH2Load7:                   <= 10000
 cMSFxH2Load8:                   <= 10000
 cMSFxH2Load9:                   <= 10000
 cSNodeLaw#10:                   - vUsedPowerCompressor(0) + vGFxElecGrid(0)
                                  = 0
 cSNodeLaw#11:                   - vUsedPowerCompressor(1) + vGFxElecGrid(1)
                                  = 0
 cSNodeLaw#12:                   - vUsedPowerCompressor(2) + vGFxElecGrid(2)
                                  = 0
 cSNodeLaw#13:                   - vUsedPowerCompressor(3) + vGFxElecGrid(3)
                                  = 0
 cSNodeLaw#14:                   - vUsedPowerCompressor(4) + vGFxElecGrid(4)
                                  = 0
 cSNodeLaw#15:                   - vUsedPowerCompressor(5) + vGFxElecGrid(5)
                                  = 0
 cSNodeLaw#16:                   - vUsedPowerCompressor(6) + vGFxElecGrid(6)
                                  = 0
 cSNodeLaw#17:                   - vUsedPowerCompressor(7) + vGFxElecGrid(7)
                                  = 0
 cSNodeLaw#18:                   - vUsedPowerCompressor(8) + vGFxElecGrid(8)
                                  = 0
 cSNodeLaw#19:                   - vUsedPowerCompressor(9) + vGFxElecGrid(9)
                                  = 0
 cSNodeLaw#20:                   - vMassFlowCompressor(0) + vGFxH2Grid(0)  = 0
 cSNodeLaw#21:                   - vMassFlowCompressor(1) + vGFxH2Grid(1)  = 0
 cSNodeLaw#22:                   - vMassFlowCompressor(2) + vGFxH2Grid(2)  = 0
 cSNodeLaw#23:                   - vMassFlowCompressor(3) + vGFxH2Grid(3)  = 0
 cSNodeLaw#24:                   - vMassFlowCompressor(4) + vGFxH2Grid(4)  = 0
 cSNodeLaw#25:                   - vMassFlowCompressor(5) + vGFxH2Grid(5)  = 0
 cSNodeLaw#26:                   - vMassFlowCompressor(6) + vGFxH2Grid(6)  = 0
 cSNodeLaw#27:                   - vMassFlowCompressor(7) + vGFxH2Grid(7)  = 0
 cSNodeLaw#28:                   - vMassFlowCompressor(8) + vGFxH2Grid(8)  = 0
 cSNodeLaw#29:                   - vMassFlowCompressor(9) + vGFxH2Grid(9)  = 0
 cSNodeLaw#30:                   vMassFlowCompressor(0) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#31:                   vMassFlowCompressor(1) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#32:                   vMassFlowCompressor(2) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#33:                   vMassFlowCompressor(3) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#34:                   vMassFlowCompressor(4) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#35:                   vMassFlowCompressor(5) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#36:                   vMassFlowCompressor(6) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#37:                   vMassFlowCompressor(7) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#38:                   vMassFlowCompressor(8) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#39:                   vMassFlowCompressor(9) - 2000 vWeightH2Load
                                  = 0
Bounds
 0 <= vInstalledCompressor <= 1
 0 <= vMaxPowerCompressor <= 2.67780822512165
 0 <= vStateCompressor(0) <= 1
 0 <= vStateCompressor(1) <= 1
 0 <= vStateCompressor(2) <= 1
 0 <= vStateCompressor(3) <= 1
 0 <= vStateCompressor(4) <= 1
 0 <= vStateCompressor(5) <= 1
 0 <= vStateCompressor(6) <= 1
 0 <= vStateCompressor(7) <= 1
 0 <= vStateCompressor(8) <= 1
 0 <= vStateCompressor(9) <= 1
 0 <= vUsedPowerCompressor(0) <= 2.67780822512165
 0 <= vUsedPowerCompressor(1) <= 2.67780822512165
 0 <= vUsedPowerCompressor(2) <= 2.67780822512165
 0 <= vUsedPowerCompressor(3) <= 2.67780822512165
 0 <= vUsedPowerCompressor(4) <= 2.67780822512165
 0 <= vUsedPowerCompressor(5) <= 2.67780822512165
 0 <= vUsedPowerCompressor(6) <= 2.67780822512165
 0 <= vUsedPowerCompressor(7) <= 2.67780822512165
 0 <= vUsedPowerCompressor(8) <= 2.67780822512165
 0 <= vUsedPowerCompressor(9) <= 2.67780822512165
 0 <= vMassFlowCompressor(0) <= 2500
 0 <= vMassFlowCompressor(1) <= 2500
 0 <= vMassFlowCompressor(2) <= 2500
 0 <= vMassFlowCompressor(3) <= 2500
 0 <= vMassFlowCompressor(4) <= 2500
 0 <= vMassFlowCompressor(5) <= 2500
 0 <= vMassFlowCompressor(6) <= 2500
 0 <= vMassFlowCompressor(7) <= 2500
 0 <= vMassFlowCompressor(8) <= 2500
 0 <= vMassFlowCompressor(9) <= 2500
 0 <= vPOutCompCompressor(0) <= 10000
 0 <= vPOutCompCompressor(1) <= 10000
 0 <= vPOutCompCompressor(2) <= 10000
 0 <= vPOutCompCompressor(3) <= 10000
 0 <= vPOutCompCompressor(4) <= 10000
 0 <= vPOutCompCompressor(5) <= 10000
 0 <= vPOutCompCompressor(6) <= 10000
 0 <= vPOutCompCompressor(7) <= 10000
 0 <= vPOutCompCompressor(8) <= 10000
 0 <= vPOutCompCompressor(9) <= 10000
 0 <= vToutletCompressor <= 3000
 0 <= vInstalledElecGrid <= 1
 0 <= vMaxFlowElecGrid <= 10000
 0 <= vStateElecGrid(0) <= 1
 0 <= vStateElecGrid(1) <= 1
 0 <= vStateElecGrid(2) <= 1
 0 <= vStateElecGrid(3) <= 1
 0 <= vStateElecGrid(4) <= 1
 0 <= vStateElecGrid(5) <= 1
 0 <= vStateElecGrid(6) <= 1
 0 <= vStateElecGrid(7) <= 1
 0 <= vStateElecGrid(8) <= 1
 0 <= vStateElecGrid(9) <= 1
 0 <= vGFxElecGrid(0) <= 10000
 0 <= vGFxElecGrid(1) <= 10000
 0 <= vGFxElecGrid(2) <= 10000
 0 <= vGFxElecGrid(3) <= 10000
 0 <= vGFxElecGrid(4) <= 10000
 0 <= vGFxElecGrid(5) <= 10000
 0 <= vGFxElecGrid(6) <= 10000
 0 <= vGFxElecGrid(7) <= 10000
 0 <= vGFxElecGrid(8) <= 10000
 0 <= vGFxElecGrid(9) <= 10000
 0 <= vInstalledH2Grid <= 1
 0 <= vMaxFlowH2Grid <= 10000
 0 <= vStateH2Grid(0) <= 1
 0 <= vStateH2Grid(1) <= 1
 0 <= vStateH2Grid(2) <= 1
 0 <= vStateH2Grid(3) <= 1
 0 <= vStateH2Grid(4) <= 1
 0 <= vStateH2Grid(5) <= 1
 0 <= vStateH2Grid(6) <= 1
 0 <= vStateH2Grid(7) <= 1
 0 <= vStateH2Grid(8) <= 1
 0 <= vStateH2Grid(9) <= 1
 0 <= vGFxH2Grid(0) <= 10000
 0 <= vGFxH2Grid(1) <= 10000
 0 <= vGFxH2Grid(2) <= 10000
 0 <= vGFxH2Grid(3) <= 10000
 0 <= vGFxH2Grid(4) <= 10000
 0 <= vGFxH2Grid(5) <= 10000
 0 <= vGFxH2Grid(6) <= 10000
 0 <= vGFxH2Grid(7) <= 10000
 0 <= vGFxH2Grid(8) <= 10000
 0 <= vGFxH2Grid(9) <= 10000
 0 <= vInstalledH2Load <= 1
 0 <= vWeightH2Load <= 1
Generals
 vInstalledCompressor  vStateCompressor(0)  vStateCompressor(1) 
 vStateCompressor(2)  vStateCompressor(3)  vStateCompressor(4) 
 vStateCompressor(5)  vStateCompressor(6)  vStateCompressor(7) 
 vStateCompressor(8)  vStateCompressor(9)  vInstalledElecGrid 
 vStateElecGrid(0)  vStateElecGrid(1)  vStateElecGrid(2)  vStateElecGrid(3) 
 vStateElecGrid(4)  vStateElecGrid(5)  vStateElecGrid(6)  vStateElecGrid(7) 
 vStateElecGrid(8)  vStateElecGrid(9)  vInstalledH2Grid  vStateH2Grid(0) 
 vStateH2Grid(1)  vStateH2Grid(2)  vStateH2Grid(3)  vStateH2Grid(4) 
 vStateH2Grid(5)  vStateH2Grid(6)  vStateH2Grid(7)  vStateH2Grid(8) 
 vStateH2Grid(9)  vInstalledH2Load 
End
