\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 0 vCommonMinManualObjective#1 + vCommonMaxManualObjective#1
Subject To
 csInstalledGridFree#10#0:       vInstalledGridFree#1  = 1
 csMaxValGridFree#10#1:          vMaxFlowGridFree#1  = 10000
 csBigMInstalledGridFree#10#2:   - 10000 vInstalledGridFree#1
                                 + vMaxFlowGridFree#1 <= 0
 csMinSizeInstalledGridFree#10#3: vMaxFlowGridFree#1 >= 0
 cNotRunningIfNotInstalledGridFree#10#4: - vInstalledGridFree#1
                                 + vStateGridFree#1(0) <= 0
 cNotRunningIfNotInstalledGridFree#11#5: - vInstalledGridFree#1
                                 + vStateGridFree#1(1) <= 0
 cNotRunningIfNotInstalledGridFree#12#6: - vInstalledGridFree#1
                                 + vStateGridFree#1(2) <= 0
 cNotRunningIfNotInstalledGridFree#13#7: - vInstalledGridFree#1
                                 + vStateGridFree#1(3) <= 0
 cNotRunningIfNotInstalledGridFree#14#8: - vInstalledGridFree#1
                                 + vStateGridFree#1(4) <= 0
 cNotRunningIfNotInstalledGridFree#15#9: - vInstalledGridFree#1
                                 + vStateGridFree#1(5) <= 0
 cNotRunningIfNotInstalledGridFree#16#10: - vInstalledGridFree#1
                                 + vStateGridFree#1(6) <= 0
 cNotRunningIfNotInstalledGridFree#17#11: - vInstalledGridFree#1
                                 + vStateGridFree#1(7) <= 0
 cNotRunningIfNotInstalledGridFree#18#12: - vInstalledGridFree#1
                                 + vStateGridFree#1(8) <= 0
 cNotRunningIfNotInstalledGridFree#19#13: - vInstalledGridFree#1
                                 + vStateGridFree#1(9) <= 0
 cStateGridGridFree#10#14:       - 10000 vStateGridFree#1(0)
                                 + vGFxGridFree#1(0) <= 0
 cStateGridGridFree#11#15:       - 10000 vStateGridFree#1(1)
                                 + vGFxGridFree#1(1) <= 0
 cStateGridGridFree#12#16:       - 10000 vStateGridFree#1(2)
                                 + vGFxGridFree#1(2) <= 0
 cStateGridGridFree#13#17:       - 10000 vStateGridFree#1(3)
                                 + vGFxGridFree#1(3) <= 0
 cStateGridGridFree#14#18:       - 10000 vStateGridFree#1(4)
                                 + vGFxGridFree#1(4) <= 0
 cStateGridGridFree#15#19:       - 10000 vStateGridFree#1(5)
                                 + vGFxGridFree#1(5) <= 0
 cStateGridGridFree#16#20:       - 10000 vStateGridFree#1(6)
                                 + vGFxGridFree#1(6) <= 0
 cStateGridGridFree#17#21:       - 10000 vStateGridFree#1(7)
                                 + vGFxGridFree#1(7) <= 0
 cStateGridGridFree#18#22:       - 10000 vStateGridFree#1(8)
                                 + vGFxGridFree#1(8) <= 0
 cStateGridGridFree#19#23:       - 10000 vStateGridFree#1(9)
                                 + vGFxGridFree#1(9) <= 0
 cMaxGFxGridFree#10#24:          - vMaxFlowGridFree#1 + vGFxGridFree#1(0) <= 0
 cMaxGFxGridFree#11#25:          - vMaxFlowGridFree#1 + vGFxGridFree#1(1) <= 0
 cMaxGFxGridFree#12#26:          - vMaxFlowGridFree#1 + vGFxGridFree#1(2) <= 0
 cMaxGFxGridFree#13#27:          - vMaxFlowGridFree#1 + vGFxGridFree#1(3) <= 0
 cMaxGFxGridFree#14#28:          - vMaxFlowGridFree#1 + vGFxGridFree#1(4) <= 0
 cMaxGFxGridFree#15#29:          - vMaxFlowGridFree#1 + vGFxGridFree#1(5) <= 0
 cMaxGFxGridFree#16#30:          - vMaxFlowGridFree#1 + vGFxGridFree#1(6) <= 0
 cMaxGFxGridFree#17#31:          - vMaxFlowGridFree#1 + vGFxGridFree#1(7) <= 0
 cMaxGFxGridFree#18#32:          - vMaxFlowGridFree#1 + vGFxGridFree#1(8) <= 0
 cMaxGFxGridFree#19#33:          - vMaxFlowGridFree#1 + vGFxGridFree#1(9) <= 0
 csInstalledGridFree#20#34:      vInstalledGridFree#2  = 1
 csMaxValGridFree#20#35:         vMaxFlowGridFree#2  = 10000
 csBigMInstalledGridFree#20#36:  - 10000 vInstalledGridFree#2
                                 + vMaxFlowGridFree#2 <= 0
 csMinSizeInstalledGridFree#20#37: vMaxFlowGridFree#2 >= 0
 cNotRunningIfNotInstalledGridFree#20#38: - vInstalledGridFree#2
                                 + vStateGridFree#2(0) <= 0
 cNotRunningIfNotInstalledGridFree#21#39: - vInstalledGridFree#2
                                 + vStateGridFree#2(1) <= 0
 cNotRunningIfNotInstalledGridFree#22#40: - vInstalledGridFree#2
                                 + vStateGridFree#2(2) <= 0
 cNotRunningIfNotInstalledGridFree#23#41: - vInstalledGridFree#2
                                 + vStateGridFree#2(3) <= 0
 cNotRunningIfNotInstalledGridFree#24#42: - vInstalledGridFree#2
                                 + vStateGridFree#2(4) <= 0
 cNotRunningIfNotInstalledGridFree#25#43: - vInstalledGridFree#2
                                 + vStateGridFree#2(5) <= 0
 cNotRunningIfNotInstalledGridFree#26#44: - vInstalledGridFree#2
                                 + vStateGridFree#2(6) <= 0
 cNotRunningIfNotInstalledGridFree#27#45: - vInstalledGridFree#2
                                 + vStateGridFree#2(7) <= 0
 cNotRunningIfNotInstalledGridFree#28#46: - vInstalledGridFree#2
                                 + vStateGridFree#2(8) <= 0
 cNotRunningIfNotInstalledGridFree#29#47: - vInstalledGridFree#2
                                 + vStateGridFree#2(9) <= 0
 cStateGridGridFree#20#48:       - 10000 vStateGridFree#2(0)
                                 + vGFxGridFree#2(0) <= 0
 cStateGridGridFree#21#49:       - 10000 vStateGridFree#2(1)
                                 + vGFxGridFree#2(1) <= 0
 cStateGridGridFree#22#50:       - 10000 vStateGridFree#2(2)
                                 + vGFxGridFree#2(2) <= 0
 cStateGridGridFree#23#51:       - 10000 vStateGridFree#2(3)
                                 + vGFxGridFree#2(3) <= 0
 cStateGridGridFree#24#52:       - 10000 vStateGridFree#2(4)
                                 + vGFxGridFree#2(4) <= 0
 cStateGridGridFree#25#53:       - 10000 vStateGridFree#2(5)
                                 + vGFxGridFree#2(5) <= 0
 cStateGridGridFree#26#54:       - 10000 vStateGridFree#2(6)
                                 + vGFxGridFree#2(6) <= 0
 cStateGridGridFree#27#55:       - 10000 vStateGridFree#2(7)
                                 + vGFxGridFree#2(7) <= 0
 cStateGridGridFree#28#56:       - 10000 vStateGridFree#2(8)
                                 + vGFxGridFree#2(8) <= 0
 cStateGridGridFree#29#57:       - 10000 vStateGridFree#2(9)
                                 + vGFxGridFree#2(9) <= 0
 cMaxGFxGridFree#20#58:          - vMaxFlowGridFree#2 + vGFxGridFree#2(0) <= 0
 cMaxGFxGridFree#21#59:          - vMaxFlowGridFree#2 + vGFxGridFree#2(1) <= 0
 cMaxGFxGridFree#22#60:          - vMaxFlowGridFree#2 + vGFxGridFree#2(2) <= 0
 cMaxGFxGridFree#23#61:          - vMaxFlowGridFree#2 + vGFxGridFree#2(3) <= 0
 cMaxGFxGridFree#24#62:          - vMaxFlowGridFree#2 + vGFxGridFree#2(4) <= 0
 cMaxGFxGridFree#25#63:          - vMaxFlowGridFree#2 + vGFxGridFree#2(5) <= 0
 cMaxGFxGridFree#26#64:          - vMaxFlowGridFree#2 + vGFxGridFree#2(6) <= 0
 cMaxGFxGridFree#27#65:          - vMaxFlowGridFree#2 + vGFxGridFree#2(7) <= 0
 cMaxGFxGridFree#28#66:          - vMaxFlowGridFree#2 + vGFxGridFree#2(8) <= 0
 cMaxGFxGridFree#29#67:          - vMaxFlowGridFree#2 + vGFxGridFree#2(9) <= 0
 csInstalledSourceLoad#10#68:    vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10#69:       vWeightSourceLoad#1  = 1
 csBigMInstalledSourceLoad#10#70: - vInstalledSourceLoad#1
                                 + vWeightSourceLoad#1 <= 0
 csMinSizeInstalledSourceLoad#10#71: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10#72:          <= 10000
 cMSFxSourceLoad#11#73:          <= 10000
 cMSFxSourceLoad#12#74:          <= 10000
 cMSFxSourceLoad#13#75:          <= 10000
 cMSFxSourceLoad#14#76:          <= 10000
 cMSFxSourceLoad#15#77:          <= 10000
 cMSFxSourceLoad#16#78:          <= 10000
 cMSFxSourceLoad#17#79:          <= 10000
 cMSFxSourceLoad#18#80:          <= 10000
 cMSFxSourceLoad#19#81:          <= 10000
 csBigMInstalledSourceLoad#20#82: - 10 vInstalledSourceLoad#2
                                 + vWeightSourceLoad#2 <= 0
 csMinSizeInstalledSourceLoad#20#83: vWeightSourceLoad#2 >= 0
 cMSFxSourceLoad#20#84:          <= 10000
 cMSFxSourceLoad#21#85:          <= 10000
 cMSFxSourceLoad#22#86:          <= 10000
 cMSFxSourceLoad#23#87:          <= 10000
 cMSFxSourceLoad#24#88:          <= 10000
 cMSFxSourceLoad#25#89:          <= 10000
 cMSFxSourceLoad#26#90:          <= 10000
 cMSFxSourceLoad#27#91:          <= 10000
 cMSFxSourceLoad#28#92:          <= 10000
 cMSFxSourceLoad#29#93:          <= 10000
 csBigMInstalledSourceLoad#30#94: - 10 vInstalledSourceLoad#3
                                 + vWeightSourceLoad#3 <= 0
 csMinSizeInstalledSourceLoad#30#95: vWeightSourceLoad#3 >= 0
 cMSFxSourceLoad#30#96:          <= 10000
 cMSFxSourceLoad#31#97:          <= 10000
 cMSFxSourceLoad#32#98:          <= 10000
 cMSFxSourceLoad#33#99:          <= 10000
 cMSFxSourceLoad#34#100:         <= 10000
 cMSFxSourceLoad#35#101:         <= 10000
 cMSFxSourceLoad#36#102:         <= 10000
 cMSFxSourceLoad#37#103:         <= 10000
 cMSFxSourceLoad#38#104:         <= 10000
 cMSFxSourceLoad#39#105:         <= 10000
 cMaxConstraintManualObjective#10#106: vWeightSourceLoad#2
                                 + vCommonMaxManualObjective#1 >= 0
 cMaxConstraintManualObjective#10#107: vWeightSourceLoad#3
                                 + vCommonMaxManualObjective#1 >= 0
 cSNodeLaw#10#108:               vGFxGridFree#1(0) - 3 vGFxGridFree#2(0)
                                 + 1.845819 vWeightSourceLoad#2  = 0
 cSNodeLaw#11#109:               vGFxGridFree#1(1) - 3 vGFxGridFree#2(1)
                                 + 2.191294 vWeightSourceLoad#2  = 0
 cSNodeLaw#12#110:               vGFxGridFree#1(2) - 3 vGFxGridFree#2(2)
                                 + 2.326187 vWeightSourceLoad#2  = 0
 cSNodeLaw#13#111:               vGFxGridFree#1(3) - 3 vGFxGridFree#2(3)
                                 + 2.483921 vWeightSourceLoad#2  = 0
 cSNodeLaw#14#112:               vGFxGridFree#1(4) - 3 vGFxGridFree#2(4)
                                 + 2.67351 vWeightSourceLoad#2  = 0
 cSNodeLaw#15#113:               vGFxGridFree#1(5) - 3 vGFxGridFree#2(5)
                                 + 2.85778 vWeightSourceLoad#2  = 0
 cSNodeLaw#16#114:               vGFxGridFree#1(6) - 3 vGFxGridFree#2(6)
                                 + 2.98878 vWeightSourceLoad#2  = 0
 cSNodeLaw#17#115:               vGFxGridFree#1(7) - 3 vGFxGridFree#2(7)
                                 - 1.6752 vWeightSourceLoad#1
                                 + 3.033381 vWeightSourceLoad#2  = 0
 cSNodeLaw#18#116:               vGFxGridFree#1(8) - 3 vGFxGridFree#2(8)
                                 - 3.57848 vWeightSourceLoad#1
                                 + 2.807039 vWeightSourceLoad#2  = 0
 cSNodeLaw#19#117:               vGFxGridFree#1(9) - 3 vGFxGridFree#2(9)
                                 - 4.53728 vWeightSourceLoad#1
                                 + 2.311192 vWeightSourceLoad#2  = 0
 cMNodeLaw#20#118:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#21#119:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#22#120:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#23#121:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#24#122:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#25#123:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#26#124:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#27#125:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#28#126:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
 cMNodeLaw#29#127:               vWeightSourceLoad#2 + vWeightSourceLoad#3 <= 2
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
 0 <= vInstalledSourceLoad#2 <= 1
 0 <= vWeightSourceLoad#2 <= 10
 0 <= vInstalledSourceLoad#3 <= 1
 0 <= vWeightSourceLoad#3 <= 10
      vCommonMinManualObjective#1 Free
      vCommonMaxManualObjective#1 Free
Generals
 vInstalledGridFree#1  vStateGridFree#1(0)  vStateGridFree#1(1) 
 vStateGridFree#1(2)  vStateGridFree#1(3)  vStateGridFree#1(4) 
 vStateGridFree#1(5)  vStateGridFree#1(6)  vStateGridFree#1(7) 
 vStateGridFree#1(8)  vStateGridFree#1(9)  vInstalledGridFree#2 
 vStateGridFree#2(0)  vStateGridFree#2(1)  vStateGridFree#2(2) 
 vStateGridFree#2(3)  vStateGridFree#2(4)  vStateGridFree#2(5) 
 vStateGridFree#2(6)  vStateGridFree#2(7)  vStateGridFree#2(8) 
 vStateGridFree#2(9)  vInstalledSourceLoad#1  vInstalledSourceLoad#2 
 vInstalledSourceLoad#3 
End
