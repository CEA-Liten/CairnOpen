\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 11.1335595242702 vMaxPowerCompressor + 0 vPOutCompCompressor(0)
       + 0 vPOutCompCompressor(1) + 0 vPOutCompCompressor(2)
       + 0 vPOutCompCompressor(3) + 0 vPOutCompCompressor(4)
       + 0 vPOutCompCompressor(5) + 0 vPOutCompCompressor(6)
       + 0 vPOutCompCompressor(7) + 0 vPOutCompCompressor(8)
       + 0 vPOutCompCompressor(9) + 0 vPOutCompCompressor(10)
       + 0 vPOutCompCompressor(11) + 0 vPOutCompCompressor(12)
       + 0 vPOutCompCompressor(13) + 0 vPOutCompCompressor(14)
       + 0 vPOutCompCompressor(15) + 0 vPOutCompCompressor(16)
       + 0 vPOutCompCompressor(17) + 0 vPOutCompCompressor(18)
       + 0 vPOutCompCompressor(19) + 0 vPOutCompCompressor(20)
       + 0 vPOutCompCompressor(21) + 0 vPOutCompCompressor(22)
       + 0 vPOutCompCompressor(23)
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
 cNotRunningIfNotInstalledCompressor10: - vInstalledCompressor
                                 + vStateCompressor(10) <= 0
 cNotRunningIfNotInstalledCompressor11: - vInstalledCompressor
                                 + vStateCompressor(11) <= 0
 cNotRunningIfNotInstalledCompressor12: - vInstalledCompressor
                                 + vStateCompressor(12) <= 0
 cNotRunningIfNotInstalledCompressor13: - vInstalledCompressor
                                 + vStateCompressor(13) <= 0
 cNotRunningIfNotInstalledCompressor14: - vInstalledCompressor
                                 + vStateCompressor(14) <= 0
 cNotRunningIfNotInstalledCompressor15: - vInstalledCompressor
                                 + vStateCompressor(15) <= 0
 cNotRunningIfNotInstalledCompressor16: - vInstalledCompressor
                                 + vStateCompressor(16) <= 0
 cNotRunningIfNotInstalledCompressor17: - vInstalledCompressor
                                 + vStateCompressor(17) <= 0
 cNotRunningIfNotInstalledCompressor18: - vInstalledCompressor
                                 + vStateCompressor(18) <= 0
 cNotRunningIfNotInstalledCompressor19: - vInstalledCompressor
                                 + vStateCompressor(19) <= 0
 cNotRunningIfNotInstalledCompressor20: - vInstalledCompressor
                                 + vStateCompressor(20) <= 0
 cNotRunningIfNotInstalledCompressor21: - vInstalledCompressor
                                 + vStateCompressor(21) <= 0
 cNotRunningIfNotInstalledCompressor22: - vInstalledCompressor
                                 + vStateCompressor(22) <= 0
 cNotRunningIfNotInstalledCompressor23: - vInstalledCompressor
                                 + vStateCompressor(23) <= 0
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
 cPowCompCompressor10:           vUsedPowerCompressor(10)
                                 - 0.00107112329004866 vMassFlowCompressor(10)
                                  = 0
 cPowCompCompressor11:           vUsedPowerCompressor(11)
                                 - 0.00107112329004866 vMassFlowCompressor(11)
                                  = 0
 cPowCompCompressor12:           vUsedPowerCompressor(12)
                                 - 0.00107112329004866 vMassFlowCompressor(12)
                                  = 0
 cPowCompCompressor13:           vUsedPowerCompressor(13)
                                 - 0.00107112329004866 vMassFlowCompressor(13)
                                  = 0
 cPowCompCompressor14:           vUsedPowerCompressor(14)
                                 - 0.00107112329004866 vMassFlowCompressor(14)
                                  = 0
 cPowCompCompressor15:           vUsedPowerCompressor(15)
                                 - 0.00107112329004866 vMassFlowCompressor(15)
                                  = 0
 cPowCompCompressor16:           vUsedPowerCompressor(16)
                                 - 0.00107112329004866 vMassFlowCompressor(16)
                                  = 0
 cPowCompCompressor17:           vUsedPowerCompressor(17)
                                 - 0.00107112329004866 vMassFlowCompressor(17)
                                  = 0
 cPowCompCompressor18:           vUsedPowerCompressor(18)
                                 - 0.00107112329004866 vMassFlowCompressor(18)
                                  = 0
 cPowCompCompressor19:           vUsedPowerCompressor(19)
                                 - 0.00107112329004866 vMassFlowCompressor(19)
                                  = 0
 cPowCompCompressor20:           vUsedPowerCompressor(20)
                                 - 0.00107112329004866 vMassFlowCompressor(20)
                                  = 0
 cPowCompCompressor21:           vUsedPowerCompressor(21)
                                 - 0.00107112329004866 vMassFlowCompressor(21)
                                  = 0
 cPowCompCompressor22:           vUsedPowerCompressor(22)
                                 - 0.00107112329004866 vMassFlowCompressor(22)
                                  = 0
 cPowCompCompressor23:           vUsedPowerCompressor(23)
                                 - 0.00107112329004866 vMassFlowCompressor(23)
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
 cUseCompCompressor10:           - 2.67780822512165 vStateCompressor(10)
                                 + vUsedPowerCompressor(10) <= 0
 cPowMinCompressor10:            vUsedPowerCompressor(10) >= 0
 cMaxCompressor10:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(10) <= 0
 cUseCompCompressor11:           - 2.67780822512165 vStateCompressor(11)
                                 + vUsedPowerCompressor(11) <= 0
 cPowMinCompressor11:            vUsedPowerCompressor(11) >= 0
 cMaxCompressor11:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(11) <= 0
 cUseCompCompressor12:           - 2.67780822512165 vStateCompressor(12)
                                 + vUsedPowerCompressor(12) <= 0
 cPowMinCompressor12:            vUsedPowerCompressor(12) >= 0
 cMaxCompressor12:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(12) <= 0
 cUseCompCompressor13:           - 2.67780822512165 vStateCompressor(13)
                                 + vUsedPowerCompressor(13) <= 0
 cPowMinCompressor13:            vUsedPowerCompressor(13) >= 0
 cMaxCompressor13:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(13) <= 0
 cUseCompCompressor14:           - 2.67780822512165 vStateCompressor(14)
                                 + vUsedPowerCompressor(14) <= 0
 cPowMinCompressor14:            vUsedPowerCompressor(14) >= 0
 cMaxCompressor14:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(14) <= 0
 cUseCompCompressor15:           - 2.67780822512165 vStateCompressor(15)
                                 + vUsedPowerCompressor(15) <= 0
 cPowMinCompressor15:            vUsedPowerCompressor(15) >= 0
 cMaxCompressor15:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(15) <= 0
 cUseCompCompressor16:           - 2.67780822512165 vStateCompressor(16)
                                 + vUsedPowerCompressor(16) <= 0
 cPowMinCompressor16:            vUsedPowerCompressor(16) >= 0
 cMaxCompressor16:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(16) <= 0
 cUseCompCompressor17:           - 2.67780822512165 vStateCompressor(17)
                                 + vUsedPowerCompressor(17) <= 0
 cPowMinCompressor17:            vUsedPowerCompressor(17) >= 0
 cMaxCompressor17:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(17) <= 0
 cUseCompCompressor18:           - 2.67780822512165 vStateCompressor(18)
                                 + vUsedPowerCompressor(18) <= 0
 cPowMinCompressor18:            vUsedPowerCompressor(18) >= 0
 cMaxCompressor18:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(18) <= 0
 cUseCompCompressor19:           - 2.67780822512165 vStateCompressor(19)
                                 + vUsedPowerCompressor(19) <= 0
 cPowMinCompressor19:            vUsedPowerCompressor(19) >= 0
 cMaxCompressor19:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(19) <= 0
 cUseCompCompressor20:           - 2.67780822512165 vStateCompressor(20)
                                 + vUsedPowerCompressor(20) <= 0
 cPowMinCompressor20:            vUsedPowerCompressor(20) >= 0
 cMaxCompressor20:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(20) <= 0
 cUseCompCompressor21:           - 2.67780822512165 vStateCompressor(21)
                                 + vUsedPowerCompressor(21) <= 0
 cPowMinCompressor21:            vUsedPowerCompressor(21) >= 0
 cMaxCompressor21:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(21) <= 0
 cUseCompCompressor22:           - 2.67780822512165 vStateCompressor(22)
                                 + vUsedPowerCompressor(22) <= 0
 cPowMinCompressor22:            vUsedPowerCompressor(22) >= 0
 cMaxCompressor22:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(22) <= 0
 cUseCompCompressor23:           - 2.67780822512165 vStateCompressor(23)
                                 + vUsedPowerCompressor(23) <= 0
 cPowMinCompressor23:            vUsedPowerCompressor(23) >= 0
 cMaxCompressor23:               - vMaxPowerCompressor
                                 + vUsedPowerCompressor(23) <= 0
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
 cNotRunningIfNotInstalledElecGrid10: - vInstalledElecGrid + vStateElecGrid(10)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid11: - vInstalledElecGrid + vStateElecGrid(11)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid12: - vInstalledElecGrid + vStateElecGrid(12)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid13: - vInstalledElecGrid + vStateElecGrid(13)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid14: - vInstalledElecGrid + vStateElecGrid(14)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid15: - vInstalledElecGrid + vStateElecGrid(15)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid16: - vInstalledElecGrid + vStateElecGrid(16)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid17: - vInstalledElecGrid + vStateElecGrid(17)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid18: - vInstalledElecGrid + vStateElecGrid(18)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid19: - vInstalledElecGrid + vStateElecGrid(19)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid20: - vInstalledElecGrid + vStateElecGrid(20)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid21: - vInstalledElecGrid + vStateElecGrid(21)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid22: - vInstalledElecGrid + vStateElecGrid(22)
                                 <= 0
 cNotRunningIfNotInstalledElecGrid23: - vInstalledElecGrid + vStateElecGrid(23)
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
 cStateGridElecGrid10:           - 10000 vStateElecGrid(10) + vGFxElecGrid(10)
                                 <= 0
 cStateGridElecGrid11:           - 10000 vStateElecGrid(11) + vGFxElecGrid(11)
                                 <= 0
 cStateGridElecGrid12:           - 10000 vStateElecGrid(12) + vGFxElecGrid(12)
                                 <= 0
 cStateGridElecGrid13:           - 10000 vStateElecGrid(13) + vGFxElecGrid(13)
                                 <= 0
 cStateGridElecGrid14:           - 10000 vStateElecGrid(14) + vGFxElecGrid(14)
                                 <= 0
 cStateGridElecGrid15:           - 10000 vStateElecGrid(15) + vGFxElecGrid(15)
                                 <= 0
 cStateGridElecGrid16:           - 10000 vStateElecGrid(16) + vGFxElecGrid(16)
                                 <= 0
 cStateGridElecGrid17:           - 10000 vStateElecGrid(17) + vGFxElecGrid(17)
                                 <= 0
 cStateGridElecGrid18:           - 10000 vStateElecGrid(18) + vGFxElecGrid(18)
                                 <= 0
 cStateGridElecGrid19:           - 10000 vStateElecGrid(19) + vGFxElecGrid(19)
                                 <= 0
 cStateGridElecGrid20:           - 10000 vStateElecGrid(20) + vGFxElecGrid(20)
                                 <= 0
 cStateGridElecGrid21:           - 10000 vStateElecGrid(21) + vGFxElecGrid(21)
                                 <= 0
 cStateGridElecGrid22:           - 10000 vStateElecGrid(22) + vGFxElecGrid(22)
                                 <= 0
 cStateGridElecGrid23:           - 10000 vStateElecGrid(23) + vGFxElecGrid(23)
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
 cMaxGFxElecGrid10:              - vMaxFlowElecGrid + vGFxElecGrid(10) <= 0
 cMaxGFxElecGrid11:              - vMaxFlowElecGrid + vGFxElecGrid(11) <= 0
 cMaxGFxElecGrid12:              - vMaxFlowElecGrid + vGFxElecGrid(12) <= 0
 cMaxGFxElecGrid13:              - vMaxFlowElecGrid + vGFxElecGrid(13) <= 0
 cMaxGFxElecGrid14:              - vMaxFlowElecGrid + vGFxElecGrid(14) <= 0
 cMaxGFxElecGrid15:              - vMaxFlowElecGrid + vGFxElecGrid(15) <= 0
 cMaxGFxElecGrid16:              - vMaxFlowElecGrid + vGFxElecGrid(16) <= 0
 cMaxGFxElecGrid17:              - vMaxFlowElecGrid + vGFxElecGrid(17) <= 0
 cMaxGFxElecGrid18:              - vMaxFlowElecGrid + vGFxElecGrid(18) <= 0
 cMaxGFxElecGrid19:              - vMaxFlowElecGrid + vGFxElecGrid(19) <= 0
 cMaxGFxElecGrid20:              - vMaxFlowElecGrid + vGFxElecGrid(20) <= 0
 cMaxGFxElecGrid21:              - vMaxFlowElecGrid + vGFxElecGrid(21) <= 0
 cMaxGFxElecGrid22:              - vMaxFlowElecGrid + vGFxElecGrid(22) <= 0
 cMaxGFxElecGrid23:              - vMaxFlowElecGrid + vGFxElecGrid(23) <= 0
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
 cNotRunningIfNotInstalledH2Grid10: - vInstalledH2Grid + vStateH2Grid(10) <= 0
 cNotRunningIfNotInstalledH2Grid11: - vInstalledH2Grid + vStateH2Grid(11) <= 0
 cNotRunningIfNotInstalledH2Grid12: - vInstalledH2Grid + vStateH2Grid(12) <= 0
 cNotRunningIfNotInstalledH2Grid13: - vInstalledH2Grid + vStateH2Grid(13) <= 0
 cNotRunningIfNotInstalledH2Grid14: - vInstalledH2Grid + vStateH2Grid(14) <= 0
 cNotRunningIfNotInstalledH2Grid15: - vInstalledH2Grid + vStateH2Grid(15) <= 0
 cNotRunningIfNotInstalledH2Grid16: - vInstalledH2Grid + vStateH2Grid(16) <= 0
 cNotRunningIfNotInstalledH2Grid17: - vInstalledH2Grid + vStateH2Grid(17) <= 0
 cNotRunningIfNotInstalledH2Grid18: - vInstalledH2Grid + vStateH2Grid(18) <= 0
 cNotRunningIfNotInstalledH2Grid19: - vInstalledH2Grid + vStateH2Grid(19) <= 0
 cNotRunningIfNotInstalledH2Grid20: - vInstalledH2Grid + vStateH2Grid(20) <= 0
 cNotRunningIfNotInstalledH2Grid21: - vInstalledH2Grid + vStateH2Grid(21) <= 0
 cNotRunningIfNotInstalledH2Grid22: - vInstalledH2Grid + vStateH2Grid(22) <= 0
 cNotRunningIfNotInstalledH2Grid23: - vInstalledH2Grid + vStateH2Grid(23) <= 0
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
 cStateGridH2Grid10:             - 10000 vStateH2Grid(10) + vGFxH2Grid(10) <= 0
 cStateGridH2Grid11:             - 10000 vStateH2Grid(11) + vGFxH2Grid(11) <= 0
 cStateGridH2Grid12:             - 10000 vStateH2Grid(12) + vGFxH2Grid(12) <= 0
 cStateGridH2Grid13:             - 10000 vStateH2Grid(13) + vGFxH2Grid(13) <= 0
 cStateGridH2Grid14:             - 10000 vStateH2Grid(14) + vGFxH2Grid(14) <= 0
 cStateGridH2Grid15:             - 10000 vStateH2Grid(15) + vGFxH2Grid(15) <= 0
 cStateGridH2Grid16:             - 10000 vStateH2Grid(16) + vGFxH2Grid(16) <= 0
 cStateGridH2Grid17:             - 10000 vStateH2Grid(17) + vGFxH2Grid(17) <= 0
 cStateGridH2Grid18:             - 10000 vStateH2Grid(18) + vGFxH2Grid(18) <= 0
 cStateGridH2Grid19:             - 10000 vStateH2Grid(19) + vGFxH2Grid(19) <= 0
 cStateGridH2Grid20:             - 10000 vStateH2Grid(20) + vGFxH2Grid(20) <= 0
 cStateGridH2Grid21:             - 10000 vStateH2Grid(21) + vGFxH2Grid(21) <= 0
 cStateGridH2Grid22:             - 10000 vStateH2Grid(22) + vGFxH2Grid(22) <= 0
 cStateGridH2Grid23:             - 10000 vStateH2Grid(23) + vGFxH2Grid(23) <= 0
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
 cMaxGFxH2Grid10:                - vMaxFlowH2Grid + vGFxH2Grid(10) <= 0
 cMaxGFxH2Grid11:                - vMaxFlowH2Grid + vGFxH2Grid(11) <= 0
 cMaxGFxH2Grid12:                - vMaxFlowH2Grid + vGFxH2Grid(12) <= 0
 cMaxGFxH2Grid13:                - vMaxFlowH2Grid + vGFxH2Grid(13) <= 0
 cMaxGFxH2Grid14:                - vMaxFlowH2Grid + vGFxH2Grid(14) <= 0
 cMaxGFxH2Grid15:                - vMaxFlowH2Grid + vGFxH2Grid(15) <= 0
 cMaxGFxH2Grid16:                - vMaxFlowH2Grid + vGFxH2Grid(16) <= 0
 cMaxGFxH2Grid17:                - vMaxFlowH2Grid + vGFxH2Grid(17) <= 0
 cMaxGFxH2Grid18:                - vMaxFlowH2Grid + vGFxH2Grid(18) <= 0
 cMaxGFxH2Grid19:                - vMaxFlowH2Grid + vGFxH2Grid(19) <= 0
 cMaxGFxH2Grid20:                - vMaxFlowH2Grid + vGFxH2Grid(20) <= 0
 cMaxGFxH2Grid21:                - vMaxFlowH2Grid + vGFxH2Grid(21) <= 0
 cMaxGFxH2Grid22:                - vMaxFlowH2Grid + vGFxH2Grid(22) <= 0
 cMaxGFxH2Grid23:                - vMaxFlowH2Grid + vGFxH2Grid(23) <= 0
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
 cMSFxH2Load10:                  <= 10000
 cMSFxH2Load11:                  <= 10000
 cMSFxH2Load12:                  <= 10000
 cMSFxH2Load13:                  <= 10000
 cMSFxH2Load14:                  <= 10000
 cMSFxH2Load15:                  <= 10000
 cMSFxH2Load16:                  <= 10000
 cMSFxH2Load17:                  <= 10000
 cMSFxH2Load18:                  <= 10000
 cMSFxH2Load19:                  <= 10000
 cMSFxH2Load20:                  <= 10000
 cMSFxH2Load21:                  <= 10000
 cMSFxH2Load22:                  <= 10000
 cMSFxH2Load23:                  <= 10000
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
 cSNodeLaw#110:                  - vUsedPowerCompressor(10) + vGFxElecGrid(10)
                                  = 0
 cSNodeLaw#111:                  - vUsedPowerCompressor(11) + vGFxElecGrid(11)
                                  = 0
 cSNodeLaw#112:                  - vUsedPowerCompressor(12) + vGFxElecGrid(12)
                                  = 0
 cSNodeLaw#113:                  - vUsedPowerCompressor(13) + vGFxElecGrid(13)
                                  = 0
 cSNodeLaw#114:                  - vUsedPowerCompressor(14) + vGFxElecGrid(14)
                                  = 0
 cSNodeLaw#115:                  - vUsedPowerCompressor(15) + vGFxElecGrid(15)
                                  = 0
 cSNodeLaw#116:                  - vUsedPowerCompressor(16) + vGFxElecGrid(16)
                                  = 0
 cSNodeLaw#117:                  - vUsedPowerCompressor(17) + vGFxElecGrid(17)
                                  = 0
 cSNodeLaw#118:                  - vUsedPowerCompressor(18) + vGFxElecGrid(18)
                                  = 0
 cSNodeLaw#119:                  - vUsedPowerCompressor(19) + vGFxElecGrid(19)
                                  = 0
 cSNodeLaw#120:                  - vUsedPowerCompressor(20) + vGFxElecGrid(20)
                                  = 0
 cSNodeLaw#121:                  - vUsedPowerCompressor(21) + vGFxElecGrid(21)
                                  = 0
 cSNodeLaw#122:                  - vUsedPowerCompressor(22) + vGFxElecGrid(22)
                                  = 0
 cSNodeLaw#123:                  - vUsedPowerCompressor(23) + vGFxElecGrid(23)
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
 cSNodeLaw#210:                  - vMassFlowCompressor(10) + vGFxH2Grid(10)
                                  = 0
 cSNodeLaw#211:                  - vMassFlowCompressor(11) + vGFxH2Grid(11)
                                  = 0
 cSNodeLaw#212:                  - vMassFlowCompressor(12) + vGFxH2Grid(12)
                                  = 0
 cSNodeLaw#213:                  - vMassFlowCompressor(13) + vGFxH2Grid(13)
                                  = 0
 cSNodeLaw#214:                  - vMassFlowCompressor(14) + vGFxH2Grid(14)
                                  = 0
 cSNodeLaw#215:                  - vMassFlowCompressor(15) + vGFxH2Grid(15)
                                  = 0
 cSNodeLaw#216:                  - vMassFlowCompressor(16) + vGFxH2Grid(16)
                                  = 0
 cSNodeLaw#217:                  - vMassFlowCompressor(17) + vGFxH2Grid(17)
                                  = 0
 cSNodeLaw#218:                  - vMassFlowCompressor(18) + vGFxH2Grid(18)
                                  = 0
 cSNodeLaw#219:                  - vMassFlowCompressor(19) + vGFxH2Grid(19)
                                  = 0
 cSNodeLaw#220:                  - vMassFlowCompressor(20) + vGFxH2Grid(20)
                                  = 0
 cSNodeLaw#221:                  - vMassFlowCompressor(21) + vGFxH2Grid(21)
                                  = 0
 cSNodeLaw#222:                  - vMassFlowCompressor(22) + vGFxH2Grid(22)
                                  = 0
 cSNodeLaw#223:                  - vMassFlowCompressor(23) + vGFxH2Grid(23)
                                  = 0
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
 cSNodeLaw#310:                  vMassFlowCompressor(10)  = 0
 cSNodeLaw#311:                  vMassFlowCompressor(11)  = 0
 cSNodeLaw#312:                  vMassFlowCompressor(12)  = 0
 cSNodeLaw#313:                  vMassFlowCompressor(13)  = 0
 cSNodeLaw#314:                  vMassFlowCompressor(14) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#315:                  vMassFlowCompressor(15) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#316:                  vMassFlowCompressor(16) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#317:                  vMassFlowCompressor(17) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#318:                  vMassFlowCompressor(18) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#319:                  vMassFlowCompressor(19) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#320:                  vMassFlowCompressor(20) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#321:                  vMassFlowCompressor(21) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#322:                  vMassFlowCompressor(22) - 2000 vWeightH2Load
                                  = 0
 cSNodeLaw#323:                  vMassFlowCompressor(23) - 2000 vWeightH2Load
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
 0 <= vStateCompressor(10) <= 1
 0 <= vStateCompressor(11) <= 1
 0 <= vStateCompressor(12) <= 1
 0 <= vStateCompressor(13) <= 1
 0 <= vStateCompressor(14) <= 1
 0 <= vStateCompressor(15) <= 1
 0 <= vStateCompressor(16) <= 1
 0 <= vStateCompressor(17) <= 1
 0 <= vStateCompressor(18) <= 1
 0 <= vStateCompressor(19) <= 1
 0 <= vStateCompressor(20) <= 1
 0 <= vStateCompressor(21) <= 1
 0 <= vStateCompressor(22) <= 1
 0 <= vStateCompressor(23) <= 1
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
 0 <= vUsedPowerCompressor(10) <= 2.67780822512165
 0 <= vUsedPowerCompressor(11) <= 2.67780822512165
 0 <= vUsedPowerCompressor(12) <= 2.67780822512165
 0 <= vUsedPowerCompressor(13) <= 2.67780822512165
 0 <= vUsedPowerCompressor(14) <= 2.67780822512165
 0 <= vUsedPowerCompressor(15) <= 2.67780822512165
 0 <= vUsedPowerCompressor(16) <= 2.67780822512165
 0 <= vUsedPowerCompressor(17) <= 2.67780822512165
 0 <= vUsedPowerCompressor(18) <= 2.67780822512165
 0 <= vUsedPowerCompressor(19) <= 2.67780822512165
 0 <= vUsedPowerCompressor(20) <= 2.67780822512165
 0 <= vUsedPowerCompressor(21) <= 2.67780822512165
 0 <= vUsedPowerCompressor(22) <= 2.67780822512165
 0 <= vUsedPowerCompressor(23) <= 2.67780822512165
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
 0 <= vMassFlowCompressor(10) <= 2500
 0 <= vMassFlowCompressor(11) <= 2500
 0 <= vMassFlowCompressor(12) <= 2500
 0 <= vMassFlowCompressor(13) <= 2500
 0 <= vMassFlowCompressor(14) <= 2500
 0 <= vMassFlowCompressor(15) <= 2500
 0 <= vMassFlowCompressor(16) <= 2500
 0 <= vMassFlowCompressor(17) <= 2500
 0 <= vMassFlowCompressor(18) <= 2500
 0 <= vMassFlowCompressor(19) <= 2500
 0 <= vMassFlowCompressor(20) <= 2500
 0 <= vMassFlowCompressor(21) <= 2500
 0 <= vMassFlowCompressor(22) <= 2500
 0 <= vMassFlowCompressor(23) <= 2500
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
 0 <= vPOutCompCompressor(10) <= 10000
 0 <= vPOutCompCompressor(11) <= 10000
 0 <= vPOutCompCompressor(12) <= 10000
 0 <= vPOutCompCompressor(13) <= 10000
 0 <= vPOutCompCompressor(14) <= 10000
 0 <= vPOutCompCompressor(15) <= 10000
 0 <= vPOutCompCompressor(16) <= 10000
 0 <= vPOutCompCompressor(17) <= 10000
 0 <= vPOutCompCompressor(18) <= 10000
 0 <= vPOutCompCompressor(19) <= 10000
 0 <= vPOutCompCompressor(20) <= 10000
 0 <= vPOutCompCompressor(21) <= 10000
 0 <= vPOutCompCompressor(22) <= 10000
 0 <= vPOutCompCompressor(23) <= 10000
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
 0 <= vStateElecGrid(10) <= 1
 0 <= vStateElecGrid(11) <= 1
 0 <= vStateElecGrid(12) <= 1
 0 <= vStateElecGrid(13) <= 1
 0 <= vStateElecGrid(14) <= 1
 0 <= vStateElecGrid(15) <= 1
 0 <= vStateElecGrid(16) <= 1
 0 <= vStateElecGrid(17) <= 1
 0 <= vStateElecGrid(18) <= 1
 0 <= vStateElecGrid(19) <= 1
 0 <= vStateElecGrid(20) <= 1
 0 <= vStateElecGrid(21) <= 1
 0 <= vStateElecGrid(22) <= 1
 0 <= vStateElecGrid(23) <= 1
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
 0 <= vGFxElecGrid(10) <= 10000
 0 <= vGFxElecGrid(11) <= 10000
 0 <= vGFxElecGrid(12) <= 10000
 0 <= vGFxElecGrid(13) <= 10000
 0 <= vGFxElecGrid(14) <= 10000
 0 <= vGFxElecGrid(15) <= 10000
 0 <= vGFxElecGrid(16) <= 10000
 0 <= vGFxElecGrid(17) <= 10000
 0 <= vGFxElecGrid(18) <= 10000
 0 <= vGFxElecGrid(19) <= 10000
 0 <= vGFxElecGrid(20) <= 10000
 0 <= vGFxElecGrid(21) <= 10000
 0 <= vGFxElecGrid(22) <= 10000
 0 <= vGFxElecGrid(23) <= 10000
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
 0 <= vStateH2Grid(10) <= 1
 0 <= vStateH2Grid(11) <= 1
 0 <= vStateH2Grid(12) <= 1
 0 <= vStateH2Grid(13) <= 1
 0 <= vStateH2Grid(14) <= 1
 0 <= vStateH2Grid(15) <= 1
 0 <= vStateH2Grid(16) <= 1
 0 <= vStateH2Grid(17) <= 1
 0 <= vStateH2Grid(18) <= 1
 0 <= vStateH2Grid(19) <= 1
 0 <= vStateH2Grid(20) <= 1
 0 <= vStateH2Grid(21) <= 1
 0 <= vStateH2Grid(22) <= 1
 0 <= vStateH2Grid(23) <= 1
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
 0 <= vGFxH2Grid(10) <= 10000
 0 <= vGFxH2Grid(11) <= 10000
 0 <= vGFxH2Grid(12) <= 10000
 0 <= vGFxH2Grid(13) <= 10000
 0 <= vGFxH2Grid(14) <= 10000
 0 <= vGFxH2Grid(15) <= 10000
 0 <= vGFxH2Grid(16) <= 10000
 0 <= vGFxH2Grid(17) <= 10000
 0 <= vGFxH2Grid(18) <= 10000
 0 <= vGFxH2Grid(19) <= 10000
 0 <= vGFxH2Grid(20) <= 10000
 0 <= vGFxH2Grid(21) <= 10000
 0 <= vGFxH2Grid(22) <= 10000
 0 <= vGFxH2Grid(23) <= 10000
 0 <= vInstalledH2Load <= 1
 0 <= vWeightH2Load <= 1
Generals
 vInstalledCompressor  vStateCompressor(0)  vStateCompressor(1) 
 vStateCompressor(2)  vStateCompressor(3)  vStateCompressor(4) 
 vStateCompressor(5)  vStateCompressor(6)  vStateCompressor(7) 
 vStateCompressor(8)  vStateCompressor(9)  vStateCompressor(10) 
 vStateCompressor(11)  vStateCompressor(12)  vStateCompressor(13) 
 vStateCompressor(14)  vStateCompressor(15)  vStateCompressor(16) 
 vStateCompressor(17)  vStateCompressor(18)  vStateCompressor(19) 
 vStateCompressor(20)  vStateCompressor(21)  vStateCompressor(22) 
 vStateCompressor(23)  vInstalledElecGrid  vStateElecGrid(0) 
 vStateElecGrid(1)  vStateElecGrid(2)  vStateElecGrid(3)  vStateElecGrid(4) 
 vStateElecGrid(5)  vStateElecGrid(6)  vStateElecGrid(7)  vStateElecGrid(8) 
 vStateElecGrid(9)  vStateElecGrid(10)  vStateElecGrid(11)  vStateElecGrid(12) 
 vStateElecGrid(13)  vStateElecGrid(14)  vStateElecGrid(15) 
 vStateElecGrid(16)  vStateElecGrid(17)  vStateElecGrid(18) 
 vStateElecGrid(19)  vStateElecGrid(20)  vStateElecGrid(21) 
 vStateElecGrid(22)  vStateElecGrid(23)  vInstalledH2Grid  vStateH2Grid(0) 
 vStateH2Grid(1)  vStateH2Grid(2)  vStateH2Grid(3)  vStateH2Grid(4) 
 vStateH2Grid(5)  vStateH2Grid(6)  vStateH2Grid(7)  vStateH2Grid(8) 
 vStateH2Grid(9)  vStateH2Grid(10)  vStateH2Grid(11)  vStateH2Grid(12) 
 vStateH2Grid(13)  vStateH2Grid(14)  vStateH2Grid(15)  vStateH2Grid(16) 
 vStateH2Grid(17)  vStateH2Grid(18)  vStateH2Grid(19)  vStateH2Grid(20) 
 vStateH2Grid(21)  vStateH2Grid(22)  vStateH2Grid(23)  vInstalledH2Load 
End
