\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 2 vMaxPowerConverter#1 + 99299.8143260721 vGFxGridFree#2(0)
       + 99299.8143260721 vGFxGridFree#2(1)
       + 99299.8143260721 vGFxGridFree#2(2)
       + 99299.8143260721 vGFxGridFree#2(3)
       + 99299.8143260721 vGFxGridFree#2(4)
       + 99299.8143260721 vGFxGridFree#2(5)
       + 99299.8143260721 vGFxGridFree#2(6)
       + 99299.8143260721 vGFxGridFree#2(7)
       + 99299.8143260721 vGFxGridFree#2(8)
       + 99299.8143260721 vGFxGridFree#2(9)
Subject To
 csBigMInstalledConverter#10#0:  - 20 vInstalledConverter#1
                                 + 2 vMaxPowerConverter#1 <= 0
 csMinSizeInstalledConverter#10#1: 2 vMaxPowerConverter#1 >= 0
 cNotRunningIfNotInstalledConverter#10#2: - vInstalledConverter#1
                                 + vStateConverter#1(0) <= 0
 cNotRunningIfNotInstalledConverter#11#3: - vInstalledConverter#1
                                 + vStateConverter#1(1) <= 0
 cNotRunningIfNotInstalledConverter#12#4: - vInstalledConverter#1
                                 + vStateConverter#1(2) <= 0
 cNotRunningIfNotInstalledConverter#13#5: - vInstalledConverter#1
                                 + vStateConverter#1(3) <= 0
 cNotRunningIfNotInstalledConverter#14#6: - vInstalledConverter#1
                                 + vStateConverter#1(4) <= 0
 cNotRunningIfNotInstalledConverter#15#7: - vInstalledConverter#1
                                 + vStateConverter#1(5) <= 0
 cNotRunningIfNotInstalledConverter#16#8: - vInstalledConverter#1
                                 + vStateConverter#1(6) <= 0
 cNotRunningIfNotInstalledConverter#17#9: - vInstalledConverter#1
                                 + vStateConverter#1(7) <= 0
 cNotRunningIfNotInstalledConverter#18#10: - vInstalledConverter#1
                                 + vStateConverter#1(8) <= 0
 cNotRunningIfNotInstalledConverter#19#11: - vInstalledConverter#1
                                 + vStateConverter#1(9) <= 0
 cPowerInMaxBoundConverter#10#12: vPowInConverter#1(0) <= 2
 cPowerInMaxBoundConverter#10#13: vPowInConverter#1(1) <= 2
 cPowerInMaxBoundConverter#10#14: vPowInConverter#1(2) <= 2
 cPowerInMaxBoundConverter#10#15: vPowInConverter#1(3) <= 2
 cPowerInMaxBoundConverter#10#16: vPowInConverter#1(4) <= 2
 cPowerInMaxBoundConverter#10#17: vPowInConverter#1(5) <= 2
 cPowerInMaxBoundConverter#10#18: vPowInConverter#1(6) <= 2
 cPowerInMaxBoundConverter#10#19: vPowInConverter#1(7) <= 2
 cPowerInMaxBoundConverter#10#20: vPowInConverter#1(8) <= 2
 cPowerInMaxBoundConverter#10#21: vPowInConverter#1(9) <= 2
 cPowMaxConverter#10#22:         - 2 vStateConverter#1(0)
                                 + vPowInConverter#1(0) <= 0
 cDefPminConverter#10#23:        vMaxPowerConverter#1 - vPminConverter#1(0)
                                  = 0
 cPminConverter#10#24:           - vStateConverter#1(0) + vPowInConverter#1(0)
                                 - vPminConverter#1(0) >= -1
 cPowMaxConverter#11#25:         - 2 vStateConverter#1(1)
                                 + vPowInConverter#1(1) <= 0
 cDefPminConverter#11#26:        vMaxPowerConverter#1 - vPminConverter#1(1)
                                  = 0
 cPminConverter#11#27:           - vStateConverter#1(1) + vPowInConverter#1(1)
                                 - vPminConverter#1(1) >= -1
 cPowMaxConverter#12#28:         - 2 vStateConverter#1(2)
                                 + vPowInConverter#1(2) <= 0
 cDefPminConverter#12#29:        vMaxPowerConverter#1 - vPminConverter#1(2)
                                  = 0
 cPminConverter#12#30:           - vStateConverter#1(2) + vPowInConverter#1(2)
                                 - vPminConverter#1(2) >= -1
 cPowMaxConverter#13#31:         - 2 vStateConverter#1(3)
                                 + vPowInConverter#1(3) <= 0
 cDefPminConverter#13#32:        vMaxPowerConverter#1 - vPminConverter#1(3)
                                  = 0
 cPminConverter#13#33:           - vStateConverter#1(3) + vPowInConverter#1(3)
                                 - vPminConverter#1(3) >= -1
 cPowMaxConverter#14#34:         - 2 vStateConverter#1(4)
                                 + vPowInConverter#1(4) <= 0
 cDefPminConverter#14#35:        vMaxPowerConverter#1 - vPminConverter#1(4)
                                  = 0
 cPminConverter#14#36:           - vStateConverter#1(4) + vPowInConverter#1(4)
                                 - vPminConverter#1(4) >= -1
 cPowMaxConverter#15#37:         - 2 vStateConverter#1(5)
                                 + vPowInConverter#1(5) <= 0
 cDefPminConverter#15#38:        vMaxPowerConverter#1 - vPminConverter#1(5)
                                  = 0
 cPminConverter#15#39:           - vStateConverter#1(5) + vPowInConverter#1(5)
                                 - vPminConverter#1(5) >= -1
 cPowMaxConverter#16#40:         - 2 vStateConverter#1(6)
                                 + vPowInConverter#1(6) <= 0
 cDefPminConverter#16#41:        vMaxPowerConverter#1 - vPminConverter#1(6)
                                  = 0
 cPminConverter#16#42:           - vStateConverter#1(6) + vPowInConverter#1(6)
                                 - vPminConverter#1(6) >= -1
 cPowMaxConverter#17#43:         - 2 vStateConverter#1(7)
                                 + vPowInConverter#1(7) <= 0
 cDefPminConverter#17#44:        vMaxPowerConverter#1 - vPminConverter#1(7)
                                  = 0
 cPminConverter#17#45:           - vStateConverter#1(7) + vPowInConverter#1(7)
                                 - vPminConverter#1(7) >= -1
 cPowMaxConverter#18#46:         - 2 vStateConverter#1(8)
                                 + vPowInConverter#1(8) <= 0
 cDefPminConverter#18#47:        vMaxPowerConverter#1 - vPminConverter#1(8)
                                  = 0
 cPminConverter#18#48:           - vStateConverter#1(8) + vPowInConverter#1(8)
                                 - vPminConverter#1(8) >= -1
 cPowMaxConverter#19#49:         - 2 vStateConverter#1(9)
                                 + vPowInConverter#1(9) <= 0
 cDefPminConverter#19#50:        vMaxPowerConverter#1 - vPminConverter#1(9)
                                  = 0
 cPminConverter#19#51:           - vStateConverter#1(9) + vPowInConverter#1(9)
                                 - vPminConverter#1(9) >= -1
 cPowEffConverter#10#52:         vPowInConverter#1(0) - vPowOutConverter#1(0)
                                  = 0
 cPowEffConverter#11#53:         vPowInConverter#1(1) - vPowOutConverter#1(1)
                                  = 0
 cPowEffConverter#12#54:         vPowInConverter#1(2) - vPowOutConverter#1(2)
                                  = 0
 cPowEffConverter#13#55:         vPowInConverter#1(3) - vPowOutConverter#1(3)
                                  = 0
 cPowEffConverter#14#56:         vPowInConverter#1(4) - vPowOutConverter#1(4)
                                  = 0
 cPowEffConverter#15#57:         vPowInConverter#1(5) - vPowOutConverter#1(5)
                                  = 0
 cPowEffConverter#16#58:         vPowInConverter#1(6) - vPowOutConverter#1(6)
                                  = 0
 cPowEffConverter#17#59:         vPowInConverter#1(7) - vPowOutConverter#1(7)
                                  = 0
 cPowEffConverter#18#60:         vPowInConverter#1(8) - vPowOutConverter#1(8)
                                  = 0
 cPowEffConverter#19#61:         vPowInConverter#1(9) - vPowOutConverter#1(9)
                                  = 0
 cPowVarConverter#10#62:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(0) <= 0
 cPowVarConverter#11#63:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(1) <= 0
 cPowVarConverter#12#64:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(2) <= 0
 cPowVarConverter#13#65:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(3) <= 0
 cPowVarConverter#14#66:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(4) <= 0
 cPowVarConverter#15#67:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(5) <= 0
 cPowVarConverter#16#68:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(6) <= 0
 cPowVarConverter#17#69:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(7) <= 0
 cPowVarConverter#18#70:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(8) <= 0
 cPowVarConverter#19#71:         - 2 vMaxPowerConverter#1
                                 + vPowInConverter#1(9) <= 0
 csInstalledGridFree#10#72:      vInstalledGridFree#1  = 1
 csMaxValGridFree#10#73:         vMaxFlowGridFree#1  = 10000
 csBigMInstalledGridFree#10#74:  - 10000 vInstalledGridFree#1
                                 + vMaxFlowGridFree#1 <= 0
 csMinSizeInstalledGridFree#10#75: vMaxFlowGridFree#1 >= 0
 cNotRunningIfNotInstalledGridFree#10#76: - vInstalledGridFree#1
                                 + vStateGridFree#1(0) <= 0
 cNotRunningIfNotInstalledGridFree#11#77: - vInstalledGridFree#1
                                 + vStateGridFree#1(1) <= 0
 cNotRunningIfNotInstalledGridFree#12#78: - vInstalledGridFree#1
                                 + vStateGridFree#1(2) <= 0
 cNotRunningIfNotInstalledGridFree#13#79: - vInstalledGridFree#1
                                 + vStateGridFree#1(3) <= 0
 cNotRunningIfNotInstalledGridFree#14#80: - vInstalledGridFree#1
                                 + vStateGridFree#1(4) <= 0
 cNotRunningIfNotInstalledGridFree#15#81: - vInstalledGridFree#1
                                 + vStateGridFree#1(5) <= 0
 cNotRunningIfNotInstalledGridFree#16#82: - vInstalledGridFree#1
                                 + vStateGridFree#1(6) <= 0
 cNotRunningIfNotInstalledGridFree#17#83: - vInstalledGridFree#1
                                 + vStateGridFree#1(7) <= 0
 cNotRunningIfNotInstalledGridFree#18#84: - vInstalledGridFree#1
                                 + vStateGridFree#1(8) <= 0
 cNotRunningIfNotInstalledGridFree#19#85: - vInstalledGridFree#1
                                 + vStateGridFree#1(9) <= 0
 cStateGridGridFree#10#86:       - 10000 vStateGridFree#1(0)
                                 + vGFxGridFree#1(0) <= 0
 cStateGridGridFree#11#87:       - 10000 vStateGridFree#1(1)
                                 + vGFxGridFree#1(1) <= 0
 cStateGridGridFree#12#88:       - 10000 vStateGridFree#1(2)
                                 + vGFxGridFree#1(2) <= 0
 cStateGridGridFree#13#89:       - 10000 vStateGridFree#1(3)
                                 + vGFxGridFree#1(3) <= 0
 cStateGridGridFree#14#90:       - 10000 vStateGridFree#1(4)
                                 + vGFxGridFree#1(4) <= 0
 cStateGridGridFree#15#91:       - 10000 vStateGridFree#1(5)
                                 + vGFxGridFree#1(5) <= 0
 cStateGridGridFree#16#92:       - 10000 vStateGridFree#1(6)
                                 + vGFxGridFree#1(6) <= 0
 cStateGridGridFree#17#93:       - 10000 vStateGridFree#1(7)
                                 + vGFxGridFree#1(7) <= 0
 cStateGridGridFree#18#94:       - 10000 vStateGridFree#1(8)
                                 + vGFxGridFree#1(8) <= 0
 cStateGridGridFree#19#95:       - 10000 vStateGridFree#1(9)
                                 + vGFxGridFree#1(9) <= 0
 cMaxGFxGridFree#10#96:          - vMaxFlowGridFree#1 + vGFxGridFree#1(0) <= 0
 cMaxGFxGridFree#11#97:          - vMaxFlowGridFree#1 + vGFxGridFree#1(1) <= 0
 cMaxGFxGridFree#12#98:          - vMaxFlowGridFree#1 + vGFxGridFree#1(2) <= 0
 cMaxGFxGridFree#13#99:          - vMaxFlowGridFree#1 + vGFxGridFree#1(3) <= 0
 cMaxGFxGridFree#14#100:         - vMaxFlowGridFree#1 + vGFxGridFree#1(4) <= 0
 cMaxGFxGridFree#15#101:         - vMaxFlowGridFree#1 + vGFxGridFree#1(5) <= 0
 cMaxGFxGridFree#16#102:         - vMaxFlowGridFree#1 + vGFxGridFree#1(6) <= 0
 cMaxGFxGridFree#17#103:         - vMaxFlowGridFree#1 + vGFxGridFree#1(7) <= 0
 cMaxGFxGridFree#18#104:         - vMaxFlowGridFree#1 + vGFxGridFree#1(8) <= 0
 cMaxGFxGridFree#19#105:         - vMaxFlowGridFree#1 + vGFxGridFree#1(9) <= 0
 csInstalledGridFree#20#106:     vInstalledGridFree#2  = 1
 csMaxValGridFree#20#107:        vMaxFlowGridFree#2  = 10000
 csBigMInstalledGridFree#20#108: - 10000 vInstalledGridFree#2
                                 + vMaxFlowGridFree#2 <= 0
 csMinSizeInstalledGridFree#20#109: vMaxFlowGridFree#2 >= 0
 cNotRunningIfNotInstalledGridFree#20#110: - vInstalledGridFree#2
                                 + vStateGridFree#2(0) <= 0
 cNotRunningIfNotInstalledGridFree#21#111: - vInstalledGridFree#2
                                 + vStateGridFree#2(1) <= 0
 cNotRunningIfNotInstalledGridFree#22#112: - vInstalledGridFree#2
                                 + vStateGridFree#2(2) <= 0
 cNotRunningIfNotInstalledGridFree#23#113: - vInstalledGridFree#2
                                 + vStateGridFree#2(3) <= 0
 cNotRunningIfNotInstalledGridFree#24#114: - vInstalledGridFree#2
                                 + vStateGridFree#2(4) <= 0
 cNotRunningIfNotInstalledGridFree#25#115: - vInstalledGridFree#2
                                 + vStateGridFree#2(5) <= 0
 cNotRunningIfNotInstalledGridFree#26#116: - vInstalledGridFree#2
                                 + vStateGridFree#2(6) <= 0
 cNotRunningIfNotInstalledGridFree#27#117: - vInstalledGridFree#2
                                 + vStateGridFree#2(7) <= 0
 cNotRunningIfNotInstalledGridFree#28#118: - vInstalledGridFree#2
                                 + vStateGridFree#2(8) <= 0
 cNotRunningIfNotInstalledGridFree#29#119: - vInstalledGridFree#2
                                 + vStateGridFree#2(9) <= 0
 cStateGridGridFree#20#120:      - 10000 vStateGridFree#2(0)
                                 + vGFxGridFree#2(0) <= 0
 cStateGridGridFree#21#121:      - 10000 vStateGridFree#2(1)
                                 + vGFxGridFree#2(1) <= 0
 cStateGridGridFree#22#122:      - 10000 vStateGridFree#2(2)
                                 + vGFxGridFree#2(2) <= 0
 cStateGridGridFree#23#123:      - 10000 vStateGridFree#2(3)
                                 + vGFxGridFree#2(3) <= 0
 cStateGridGridFree#24#124:      - 10000 vStateGridFree#2(4)
                                 + vGFxGridFree#2(4) <= 0
 cStateGridGridFree#25#125:      - 10000 vStateGridFree#2(5)
                                 + vGFxGridFree#2(5) <= 0
 cStateGridGridFree#26#126:      - 10000 vStateGridFree#2(6)
                                 + vGFxGridFree#2(6) <= 0
 cStateGridGridFree#27#127:      - 10000 vStateGridFree#2(7)
                                 + vGFxGridFree#2(7) <= 0
 cStateGridGridFree#28#128:      - 10000 vStateGridFree#2(8)
                                 + vGFxGridFree#2(8) <= 0
 cStateGridGridFree#29#129:      - 10000 vStateGridFree#2(9)
                                 + vGFxGridFree#2(9) <= 0
 cMaxGFxGridFree#20#130:         - vMaxFlowGridFree#2 + vGFxGridFree#2(0) <= 0
 cMaxGFxGridFree#21#131:         - vMaxFlowGridFree#2 + vGFxGridFree#2(1) <= 0
 cMaxGFxGridFree#22#132:         - vMaxFlowGridFree#2 + vGFxGridFree#2(2) <= 0
 cMaxGFxGridFree#23#133:         - vMaxFlowGridFree#2 + vGFxGridFree#2(3) <= 0
 cMaxGFxGridFree#24#134:         - vMaxFlowGridFree#2 + vGFxGridFree#2(4) <= 0
 cMaxGFxGridFree#25#135:         - vMaxFlowGridFree#2 + vGFxGridFree#2(5) <= 0
 cMaxGFxGridFree#26#136:         - vMaxFlowGridFree#2 + vGFxGridFree#2(6) <= 0
 cMaxGFxGridFree#27#137:         - vMaxFlowGridFree#2 + vGFxGridFree#2(7) <= 0
 cMaxGFxGridFree#28#138:         - vMaxFlowGridFree#2 + vGFxGridFree#2(8) <= 0
 cMaxGFxGridFree#29#139:         - vMaxFlowGridFree#2 + vGFxGridFree#2(9) <= 0
 csInstalledGridFree#30#140:     vInstalledGridFree#3  = 1
 csMaxValGridFree#30#141:        vMaxFlowGridFree#3  = 10000
 csBigMInstalledGridFree#30#142: - 10000 vInstalledGridFree#3
                                 + vMaxFlowGridFree#3 <= 0
 csMinSizeInstalledGridFree#30#143: vMaxFlowGridFree#3 >= 0
 cNotRunningIfNotInstalledGridFree#30#144: - vInstalledGridFree#3
                                 + vStateGridFree#3(0) <= 0
 cNotRunningIfNotInstalledGridFree#31#145: - vInstalledGridFree#3
                                 + vStateGridFree#3(1) <= 0
 cNotRunningIfNotInstalledGridFree#32#146: - vInstalledGridFree#3
                                 + vStateGridFree#3(2) <= 0
 cNotRunningIfNotInstalledGridFree#33#147: - vInstalledGridFree#3
                                 + vStateGridFree#3(3) <= 0
 cNotRunningIfNotInstalledGridFree#34#148: - vInstalledGridFree#3
                                 + vStateGridFree#3(4) <= 0
 cNotRunningIfNotInstalledGridFree#35#149: - vInstalledGridFree#3
                                 + vStateGridFree#3(5) <= 0
 cNotRunningIfNotInstalledGridFree#36#150: - vInstalledGridFree#3
                                 + vStateGridFree#3(6) <= 0
 cNotRunningIfNotInstalledGridFree#37#151: - vInstalledGridFree#3
                                 + vStateGridFree#3(7) <= 0
 cNotRunningIfNotInstalledGridFree#38#152: - vInstalledGridFree#3
                                 + vStateGridFree#3(8) <= 0
 cNotRunningIfNotInstalledGridFree#39#153: - vInstalledGridFree#3
                                 + vStateGridFree#3(9) <= 0
 cStateGridGridFree#30#154:      - 10000 vStateGridFree#3(0)
                                 + vGFxGridFree#3(0) <= 0
 cStateGridGridFree#31#155:      - 10000 vStateGridFree#3(1)
                                 + vGFxGridFree#3(1) <= 0
 cStateGridGridFree#32#156:      - 10000 vStateGridFree#3(2)
                                 + vGFxGridFree#3(2) <= 0
 cStateGridGridFree#33#157:      - 10000 vStateGridFree#3(3)
                                 + vGFxGridFree#3(3) <= 0
 cStateGridGridFree#34#158:      - 10000 vStateGridFree#3(4)
                                 + vGFxGridFree#3(4) <= 0
 cStateGridGridFree#35#159:      - 10000 vStateGridFree#3(5)
                                 + vGFxGridFree#3(5) <= 0
 cStateGridGridFree#36#160:      - 10000 vStateGridFree#3(6)
                                 + vGFxGridFree#3(6) <= 0
 cStateGridGridFree#37#161:      - 10000 vStateGridFree#3(7)
                                 + vGFxGridFree#3(7) <= 0
 cStateGridGridFree#38#162:      - 10000 vStateGridFree#3(8)
                                 + vGFxGridFree#3(8) <= 0
 cStateGridGridFree#39#163:      - 10000 vStateGridFree#3(9)
                                 + vGFxGridFree#3(9) <= 0
 cMaxGFxGridFree#30#164:         - vMaxFlowGridFree#3 + vGFxGridFree#3(0) <= 0
 cMaxGFxGridFree#31#165:         - vMaxFlowGridFree#3 + vGFxGridFree#3(1) <= 0
 cMaxGFxGridFree#32#166:         - vMaxFlowGridFree#3 + vGFxGridFree#3(2) <= 0
 cMaxGFxGridFree#33#167:         - vMaxFlowGridFree#3 + vGFxGridFree#3(3) <= 0
 cMaxGFxGridFree#34#168:         - vMaxFlowGridFree#3 + vGFxGridFree#3(4) <= 0
 cMaxGFxGridFree#35#169:         - vMaxFlowGridFree#3 + vGFxGridFree#3(5) <= 0
 cMaxGFxGridFree#36#170:         - vMaxFlowGridFree#3 + vGFxGridFree#3(6) <= 0
 cMaxGFxGridFree#37#171:         - vMaxFlowGridFree#3 + vGFxGridFree#3(7) <= 0
 cMaxGFxGridFree#38#172:         - vMaxFlowGridFree#3 + vGFxGridFree#3(8) <= 0
 cMaxGFxGridFree#39#173:         - vMaxFlowGridFree#3 + vGFxGridFree#3(9) <= 0
 csInstalledSourceLoad#10#174:   vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10#175:      vWeightSourceLoad#1  = 5
 csBigMInstalledSourceLoad#10#176: - 5 vInstalledSourceLoad#1
                                 + vWeightSourceLoad#1 <= 0
 csMinSizeInstalledSourceLoad#10#177: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10#178:         <= 10000
 cMSFxSourceLoad#11#179:         <= 10000
 cMSFxSourceLoad#12#180:         <= 10000
 cMSFxSourceLoad#13#181:         <= 10000
 cMSFxSourceLoad#14#182:         <= 10000
 cMSFxSourceLoad#15#183:         <= 10000
 cMSFxSourceLoad#16#184:         <= 10000
 cMSFxSourceLoad#17#185:         <= 10000
 cMSFxSourceLoad#18#186:         <= 10000
 cMSFxSourceLoad#19#187:         <= 10000
 cSNodeLaw#10#188:               vPowOutConverter#1(0) + vGFxGridFree#2(0)
                                 - vGFxGridFree#3(0) - vWeightSourceLoad#1  = 0
 cSNodeLaw#11#189:               vPowOutConverter#1(1) + vGFxGridFree#2(1)
                                 - vGFxGridFree#3(1) - 0.9 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#12#190:               vPowOutConverter#1(2) + vGFxGridFree#2(2)
                                 - vGFxGridFree#3(2) - 0.8 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#13#191:               vPowOutConverter#1(3) + vGFxGridFree#2(3)
                                 - vGFxGridFree#3(3) - 0.7 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#14#192:               vPowOutConverter#1(4) + vGFxGridFree#2(4)
                                 - vGFxGridFree#3(4) - 0.6 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#15#193:               vPowOutConverter#1(5) + vGFxGridFree#2(5)
                                 - vGFxGridFree#3(5) - 0.5 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#16#194:               vPowOutConverter#1(6) + vGFxGridFree#2(6)
                                 - vGFxGridFree#3(6) - 0.4 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#17#195:               vPowOutConverter#1(7) + vGFxGridFree#2(7)
                                 - vGFxGridFree#3(7) - 0.3 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#18#196:               vPowOutConverter#1(8) + vGFxGridFree#2(8)
                                 - vGFxGridFree#3(8) - 0.2 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#19#197:               vPowOutConverter#1(9) + vGFxGridFree#2(9)
                                 - vGFxGridFree#3(9) - 0.1 vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#20#198:               - vPowInConverter#1(0) + vGFxGridFree#1(0)
                                  = 0
 cSNodeLaw#21#199:               - vPowInConverter#1(1) + vGFxGridFree#1(1)
                                  = 0
 cSNodeLaw#22#200:               - vPowInConverter#1(2) + vGFxGridFree#1(2)
                                  = 0
 cSNodeLaw#23#201:               - vPowInConverter#1(3) + vGFxGridFree#1(3)
                                  = 0
 cSNodeLaw#24#202:               - vPowInConverter#1(4) + vGFxGridFree#1(4)
                                  = 0
 cSNodeLaw#25#203:               - vPowInConverter#1(5) + vGFxGridFree#1(5)
                                  = 0
 cSNodeLaw#26#204:               - vPowInConverter#1(6) + vGFxGridFree#1(6)
                                  = 0
 cSNodeLaw#27#205:               - vPowInConverter#1(7) + vGFxGridFree#1(7)
                                  = 0
 cSNodeLaw#28#206:               - vPowInConverter#1(8) + vGFxGridFree#1(8)
                                  = 0
 cSNodeLaw#29#207:               - vPowInConverter#1(9) + vGFxGridFree#1(9)
                                  = 0
Bounds
 0 <= vInstalledConverter#1 <= 1
 0 <= vMaxPowerConverter#1 <= 10
 0 <= vStateConverter#1(0) <= 1
 0 <= vStateConverter#1(1) <= 1
 0 <= vStateConverter#1(2) <= 1
 0 <= vStateConverter#1(3) <= 1
 0 <= vStateConverter#1(4) <= 1
 0 <= vStateConverter#1(5) <= 1
 0 <= vStateConverter#1(6) <= 1
 0 <= vStateConverter#1(7) <= 1
 0 <= vStateConverter#1(8) <= 1
 0 <= vStateConverter#1(9) <= 1
 0 <= vPowInConverter#1(0) <= 2
 0 <= vPowInConverter#1(1) <= 2
 0 <= vPowInConverter#1(2) <= 2
 0 <= vPowInConverter#1(3) <= 2
 0 <= vPowInConverter#1(4) <= 2
 0 <= vPowInConverter#1(5) <= 2
 0 <= vPowInConverter#1(6) <= 2
 0 <= vPowInConverter#1(7) <= 2
 0 <= vPowInConverter#1(8) <= 2
 0 <= vPowInConverter#1(9) <= 2
 0 <= vPowOutConverter#1(0) <= 2
 0 <= vPowOutConverter#1(1) <= 2
 0 <= vPowOutConverter#1(2) <= 2
 0 <= vPowOutConverter#1(3) <= 2
 0 <= vPowOutConverter#1(4) <= 2
 0 <= vPowOutConverter#1(5) <= 2
 0 <= vPowOutConverter#1(6) <= 2
 0 <= vPowOutConverter#1(7) <= 2
 0 <= vPowOutConverter#1(8) <= 2
 0 <= vPowOutConverter#1(9) <= 2
      vPminConverter#1(0) Free
      vPminConverter#1(1) Free
      vPminConverter#1(2) Free
      vPminConverter#1(3) Free
      vPminConverter#1(4) Free
      vPminConverter#1(5) Free
      vPminConverter#1(6) Free
      vPminConverter#1(7) Free
      vPminConverter#1(8) Free
      vPminConverter#1(9) Free
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
 0 <= vInstalledGridFree#3 <= 1
 0 <= vMaxFlowGridFree#3 <= 10000
 0 <= vStateGridFree#3(0) <= 1
 0 <= vStateGridFree#3(1) <= 1
 0 <= vStateGridFree#3(2) <= 1
 0 <= vStateGridFree#3(3) <= 1
 0 <= vStateGridFree#3(4) <= 1
 0 <= vStateGridFree#3(5) <= 1
 0 <= vStateGridFree#3(6) <= 1
 0 <= vStateGridFree#3(7) <= 1
 0 <= vStateGridFree#3(8) <= 1
 0 <= vStateGridFree#3(9) <= 1
 0 <= vGFxGridFree#3(0) <= 10000
 0 <= vGFxGridFree#3(1) <= 10000
 0 <= vGFxGridFree#3(2) <= 10000
 0 <= vGFxGridFree#3(3) <= 10000
 0 <= vGFxGridFree#3(4) <= 10000
 0 <= vGFxGridFree#3(5) <= 10000
 0 <= vGFxGridFree#3(6) <= 10000
 0 <= vGFxGridFree#3(7) <= 10000
 0 <= vGFxGridFree#3(8) <= 10000
 0 <= vGFxGridFree#3(9) <= 10000
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 5
Generals
 vInstalledConverter#1  vMaxPowerConverter#1  vStateConverter#1(0) 
 vStateConverter#1(1)  vStateConverter#1(2)  vStateConverter#1(3) 
 vStateConverter#1(4)  vStateConverter#1(5)  vStateConverter#1(6) 
 vStateConverter#1(7)  vStateConverter#1(8)  vStateConverter#1(9) 
 vInstalledGridFree#1  vStateGridFree#1(0)  vStateGridFree#1(1) 
 vStateGridFree#1(2)  vStateGridFree#1(3)  vStateGridFree#1(4) 
 vStateGridFree#1(5)  vStateGridFree#1(6)  vStateGridFree#1(7) 
 vStateGridFree#1(8)  vStateGridFree#1(9)  vInstalledGridFree#2 
 vStateGridFree#2(0)  vStateGridFree#2(1)  vStateGridFree#2(2) 
 vStateGridFree#2(3)  vStateGridFree#2(4)  vStateGridFree#2(5) 
 vStateGridFree#2(6)  vStateGridFree#2(7)  vStateGridFree#2(8) 
 vStateGridFree#2(9)  vInstalledGridFree#3  vStateGridFree#3(0) 
 vStateGridFree#3(1)  vStateGridFree#3(2)  vStateGridFree#3(3) 
 vStateGridFree#3(4)  vStateGridFree#3(5)  vStateGridFree#3(6) 
 vStateGridFree#3(7)  vStateGridFree#3(8)  vStateGridFree#3(9) 
 vInstalledSourceLoad#1 
End
