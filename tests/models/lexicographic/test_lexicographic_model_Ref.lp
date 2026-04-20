\ENCODING=ISO-8859-1
\Problem name: problem

Minimize multi-objectives
 ManualObjective#1: Priority=0 Weight=1 AbsTol=0.1 RelTol=0
                    vGFxGridFree#1(0) + vGFxGridFree#1(1) + vGFxGridFree#1(2)
                    + vGFxGridFree#1(3) + vGFxGridFree#1(4) + vGFxGridFree#1(5)
                    + vGFxGridFree#1(6) + vGFxGridFree#1(7) + vGFxGridFree#1(8)
                    + vGFxGridFree#1(9) + 0 vCommonMinManualObjective#1
                    + 0 vCommonMaxManualObjective#1
                    + 0 vCommonMinManualObjective#2
                    + 0 vCommonMaxManualObjective#2
 ManualObjective#2: Priority=1 Weight=1 AbsTol=0 RelTol=0.1
                    vGFxGridFree#2(0) + vGFxGridFree#2(1) + vGFxGridFree#2(2)
                    + vGFxGridFree#2(3) + vGFxGridFree#2(4) + vGFxGridFree#2(5)
                    + vGFxGridFree#2(6) + vGFxGridFree#2(7) + vGFxGridFree#2(8)
                    + vGFxGridFree#2(9)
 obj2: Priority=0 Weight=1 AbsTol=0 RelTol=0
                   
Subject To
 csInstalledGridFree#10:         vInstalledGridFree#1  = 1
 csMaxValGridFree#10:            vMaxFlowGridFree#1  = 10000
 csBigMInstalledGridFree#10:     - 10000 vInstalledGridFree#1
                                 + vMaxFlowGridFree#1 <= 0
 csMinSizeInstalledGridFree#10:  vMaxFlowGridFree#1 >= 0
 cNotRunningIfNotInstalledGridFree#10: - vInstalledGridFree#1
                                 + vStateGridFree#1(0) <= 0
 cNotRunningIfNotInstalledGridFree#11: - vInstalledGridFree#1
                                 + vStateGridFree#1(1) <= 0
 cNotRunningIfNotInstalledGridFree#12: - vInstalledGridFree#1
                                 + vStateGridFree#1(2) <= 0
 cNotRunningIfNotInstalledGridFree#13: - vInstalledGridFree#1
                                 + vStateGridFree#1(3) <= 0
 cNotRunningIfNotInstalledGridFree#14: - vInstalledGridFree#1
                                 + vStateGridFree#1(4) <= 0
 cNotRunningIfNotInstalledGridFree#15: - vInstalledGridFree#1
                                 + vStateGridFree#1(5) <= 0
 cNotRunningIfNotInstalledGridFree#16: - vInstalledGridFree#1
                                 + vStateGridFree#1(6) <= 0
 cNotRunningIfNotInstalledGridFree#17: - vInstalledGridFree#1
                                 + vStateGridFree#1(7) <= 0
 cNotRunningIfNotInstalledGridFree#18: - vInstalledGridFree#1
                                 + vStateGridFree#1(8) <= 0
 cNotRunningIfNotInstalledGridFree#19: - vInstalledGridFree#1
                                 + vStateGridFree#1(9) <= 0
 cStateGridGridFree#10:          - 10000 vStateGridFree#1(0)
                                 + vGFxGridFree#1(0) <= 0
 cStateGridGridFree#11:          - 10000 vStateGridFree#1(1)
                                 + vGFxGridFree#1(1) <= 0
 cStateGridGridFree#12:          - 10000 vStateGridFree#1(2)
                                 + vGFxGridFree#1(2) <= 0
 cStateGridGridFree#13:          - 10000 vStateGridFree#1(3)
                                 + vGFxGridFree#1(3) <= 0
 cStateGridGridFree#14:          - 10000 vStateGridFree#1(4)
                                 + vGFxGridFree#1(4) <= 0
 cStateGridGridFree#15:          - 10000 vStateGridFree#1(5)
                                 + vGFxGridFree#1(5) <= 0
 cStateGridGridFree#16:          - 10000 vStateGridFree#1(6)
                                 + vGFxGridFree#1(6) <= 0
 cStateGridGridFree#17:          - 10000 vStateGridFree#1(7)
                                 + vGFxGridFree#1(7) <= 0
 cStateGridGridFree#18:          - 10000 vStateGridFree#1(8)
                                 + vGFxGridFree#1(8) <= 0
 cStateGridGridFree#19:          - 10000 vStateGridFree#1(9)
                                 + vGFxGridFree#1(9) <= 0
 cMaxGFxGridFree#10:             - vMaxFlowGridFree#1 + vGFxGridFree#1(0) <= 0
 cMaxGFxGridFree#11:             - vMaxFlowGridFree#1 + vGFxGridFree#1(1) <= 0
 cMaxGFxGridFree#12:             - vMaxFlowGridFree#1 + vGFxGridFree#1(2) <= 0
 cMaxGFxGridFree#13:             - vMaxFlowGridFree#1 + vGFxGridFree#1(3) <= 0
 cMaxGFxGridFree#14:             - vMaxFlowGridFree#1 + vGFxGridFree#1(4) <= 0
 cMaxGFxGridFree#15:             - vMaxFlowGridFree#1 + vGFxGridFree#1(5) <= 0
 cMaxGFxGridFree#16:             - vMaxFlowGridFree#1 + vGFxGridFree#1(6) <= 0
 cMaxGFxGridFree#17:             - vMaxFlowGridFree#1 + vGFxGridFree#1(7) <= 0
 cMaxGFxGridFree#18:             - vMaxFlowGridFree#1 + vGFxGridFree#1(8) <= 0
 cMaxGFxGridFree#19:             - vMaxFlowGridFree#1 + vGFxGridFree#1(9) <= 0
 csInstalledGridFree#20:         vInstalledGridFree#2  = 1
 csMaxValGridFree#20:            vMaxFlowGridFree#2  = 10000
 csBigMInstalledGridFree#20:     - 10000 vInstalledGridFree#2
                                 + vMaxFlowGridFree#2 <= 0
 csMinSizeInstalledGridFree#20:  vMaxFlowGridFree#2 >= 0
 cNotRunningIfNotInstalledGridFree#20: - vInstalledGridFree#2
                                 + vStateGridFree#2(0) <= 0
 cNotRunningIfNotInstalledGridFree#21: - vInstalledGridFree#2
                                 + vStateGridFree#2(1) <= 0
 cNotRunningIfNotInstalledGridFree#22: - vInstalledGridFree#2
                                 + vStateGridFree#2(2) <= 0
 cNotRunningIfNotInstalledGridFree#23: - vInstalledGridFree#2
                                 + vStateGridFree#2(3) <= 0
 cNotRunningIfNotInstalledGridFree#24: - vInstalledGridFree#2
                                 + vStateGridFree#2(4) <= 0
 cNotRunningIfNotInstalledGridFree#25: - vInstalledGridFree#2
                                 + vStateGridFree#2(5) <= 0
 cNotRunningIfNotInstalledGridFree#26: - vInstalledGridFree#2
                                 + vStateGridFree#2(6) <= 0
 cNotRunningIfNotInstalledGridFree#27: - vInstalledGridFree#2
                                 + vStateGridFree#2(7) <= 0
 cNotRunningIfNotInstalledGridFree#28: - vInstalledGridFree#2
                                 + vStateGridFree#2(8) <= 0
 cNotRunningIfNotInstalledGridFree#29: - vInstalledGridFree#2
                                 + vStateGridFree#2(9) <= 0
 cStateGridGridFree#20:          - 10000 vStateGridFree#2(0)
                                 + vGFxGridFree#2(0) <= 0
 cStateGridGridFree#21:          - 10000 vStateGridFree#2(1)
                                 + vGFxGridFree#2(1) <= 0
 cStateGridGridFree#22:          - 10000 vStateGridFree#2(2)
                                 + vGFxGridFree#2(2) <= 0
 cStateGridGridFree#23:          - 10000 vStateGridFree#2(3)
                                 + vGFxGridFree#2(3) <= 0
 cStateGridGridFree#24:          - 10000 vStateGridFree#2(4)
                                 + vGFxGridFree#2(4) <= 0
 cStateGridGridFree#25:          - 10000 vStateGridFree#2(5)
                                 + vGFxGridFree#2(5) <= 0
 cStateGridGridFree#26:          - 10000 vStateGridFree#2(6)
                                 + vGFxGridFree#2(6) <= 0
 cStateGridGridFree#27:          - 10000 vStateGridFree#2(7)
                                 + vGFxGridFree#2(7) <= 0
 cStateGridGridFree#28:          - 10000 vStateGridFree#2(8)
                                 + vGFxGridFree#2(8) <= 0
 cStateGridGridFree#29:          - 10000 vStateGridFree#2(9)
                                 + vGFxGridFree#2(9) <= 0
 cMaxGFxGridFree#20:             - vMaxFlowGridFree#2 + vGFxGridFree#2(0) <= 0
 cMaxGFxGridFree#21:             - vMaxFlowGridFree#2 + vGFxGridFree#2(1) <= 0
 cMaxGFxGridFree#22:             - vMaxFlowGridFree#2 + vGFxGridFree#2(2) <= 0
 cMaxGFxGridFree#23:             - vMaxFlowGridFree#2 + vGFxGridFree#2(3) <= 0
 cMaxGFxGridFree#24:             - vMaxFlowGridFree#2 + vGFxGridFree#2(4) <= 0
 cMaxGFxGridFree#25:             - vMaxFlowGridFree#2 + vGFxGridFree#2(5) <= 0
 cMaxGFxGridFree#26:             - vMaxFlowGridFree#2 + vGFxGridFree#2(6) <= 0
 cMaxGFxGridFree#27:             - vMaxFlowGridFree#2 + vGFxGridFree#2(7) <= 0
 cMaxGFxGridFree#28:             - vMaxFlowGridFree#2 + vGFxGridFree#2(8) <= 0
 cMaxGFxGridFree#29:             - vMaxFlowGridFree#2 + vGFxGridFree#2(9) <= 0
 csInstalledSourceLoad#10:       vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10:          vWeightSourceLoad#1  = 1
 csBigMInstalledSourceLoad#10:   - vInstalledSourceLoad#1 + vWeightSourceLoad#1
                                 <= 0
 csMinSizeInstalledSourceLoad#10: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10:             <= 10000
 cMSFxSourceLoad#11:             <= 10000
 cMSFxSourceLoad#12:             <= 10000
 cMSFxSourceLoad#13:             <= 10000
 cMSFxSourceLoad#14:             <= 10000
 cMSFxSourceLoad#15:             <= 10000
 cMSFxSourceLoad#16:             <= 10000
 cMSFxSourceLoad#17:             <= 10000
 cMSFxSourceLoad#18:             <= 10000
 cMSFxSourceLoad#19:             <= 10000
 cSNodeLaw#10:                   vGFxGridFree#1(0) + vGFxGridFree#2(0)  = 0
 cSNodeLaw#11:                   vGFxGridFree#1(1) + vGFxGridFree#2(1)  = 0
 cSNodeLaw#12:                   vGFxGridFree#1(2) + vGFxGridFree#2(2)  = 0
 cSNodeLaw#13:                   vGFxGridFree#1(3) + vGFxGridFree#2(3)  = 0
 cSNodeLaw#14:                   vGFxGridFree#1(4) + vGFxGridFree#2(4)  = 0
 cSNodeLaw#15:                   vGFxGridFree#1(5) + vGFxGridFree#2(5)  = 0
 cSNodeLaw#16:                   vGFxGridFree#1(6) + vGFxGridFree#2(6)  = 0
 cSNodeLaw#17:                   vGFxGridFree#1(7) + vGFxGridFree#2(7)
                                 - 1.6752 vWeightSourceLoad#1  = 0
 cSNodeLaw#18:                   vGFxGridFree#1(8) + vGFxGridFree#2(8)
                                 - 3.57848 vWeightSourceLoad#1  = 0
 cSNodeLaw#19:                   vGFxGridFree#1(9) + vGFxGridFree#2(9)
                                 - 4.53728 vWeightSourceLoad#1  = 0
Bounds
 0 <= vInstalledGridFree#1 <= 1
 0 <= vMaxFlowGridFree#1 <= 10000
 0 <= vStateGridFree#1(0) <= 1
 0 <= vStateGridFree#1(1) <= 1
 0 <= vStateGridFree#1(2) <= 1
 0 <= vStateGridFree#1(3) <= 1
 0 <= vStateGridFree#1(4) <= 1
 0 <= vStateGridFree#1(5) <= 1
 0 <= vStateGridFree#1(6) <= 1
 0 <= vStateGridFree#1(7) <= 1
 0 <= vStateGridFree#1(8) <= 1
 0 <= vStateGridFree#1(9) <= 1
 0 <= vGFxGridFree#1(0) <= 10000
 0 <= vGFxGridFree#1(1) <= 10000
 0 <= vGFxGridFree#1(2) <= 10000
 0 <= vGFxGridFree#1(3) <= 10000
 0 <= vGFxGridFree#1(4) <= 10000
 0 <= vGFxGridFree#1(5) <= 10000
 0 <= vGFxGridFree#1(6) <= 10000
 0 <= vGFxGridFree#1(7) <= 10000
 0 <= vGFxGridFree#1(8) <= 10000
 0 <= vGFxGridFree#1(9) <= 10000
 0 <= vInstalledGridFree#2 <= 1
 0 <= vMaxFlowGridFree#2 <= 10000
 0 <= vStateGridFree#2(0) <= 1
 0 <= vStateGridFree#2(1) <= 1
 0 <= vStateGridFree#2(2) <= 1
 0 <= vStateGridFree#2(3) <= 1
 0 <= vStateGridFree#2(4) <= 1
 0 <= vStateGridFree#2(5) <= 1
 0 <= vStateGridFree#2(6) <= 1
 0 <= vStateGridFree#2(7) <= 1
 0 <= vStateGridFree#2(8) <= 1
 0 <= vStateGridFree#2(9) <= 1
 0 <= vGFxGridFree#2(0) <= 10000
 0 <= vGFxGridFree#2(1) <= 10000
 0 <= vGFxGridFree#2(2) <= 10000
 0 <= vGFxGridFree#2(3) <= 10000
 0 <= vGFxGridFree#2(4) <= 10000
 0 <= vGFxGridFree#2(5) <= 10000
 0 <= vGFxGridFree#2(6) <= 10000
 0 <= vGFxGridFree#2(7) <= 10000
 0 <= vGFxGridFree#2(8) <= 10000
 0 <= vGFxGridFree#2(9) <= 10000
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 1
      vCommonMinManualObjective#1 Free
      vCommonMaxManualObjective#1 Free
      vCommonMinManualObjective#2 Free
      vCommonMaxManualObjective#2 Free
Generals
 vInstalledGridFree#1  vStateGridFree#1(0)  vStateGridFree#1(1) 
 vStateGridFree#1(2)  vStateGridFree#1(3)  vStateGridFree#1(4) 
 vStateGridFree#1(5)  vStateGridFree#1(6)  vStateGridFree#1(7) 
 vStateGridFree#1(8)  vStateGridFree#1(9)  vInstalledGridFree#2 
 vStateGridFree#2(0)  vStateGridFree#2(1)  vStateGridFree#2(2) 
 vStateGridFree#2(3)  vStateGridFree#2(4)  vStateGridFree#2(5) 
 vStateGridFree#2(6)  vStateGridFree#2(7)  vStateGridFree#2(8) 
 vStateGridFree#2(9)  vInstalledSourceLoad#1 
End
