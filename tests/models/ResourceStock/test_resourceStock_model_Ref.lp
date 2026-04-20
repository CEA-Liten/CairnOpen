\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 1226.71190485405 vMaxPowerConverter + 992998.143260721 vGFxGridFree#1(0)
       + 992998.143260721 vGFxGridFree#1(1)
       + 992998.143260721 vGFxGridFree#1(2)
       + 992998.143260721 vGFxGridFree#1(3)
       + 992998.143260721 vGFxGridFree#1(4)
       + 992998.143260721 vGFxGridFree#1(5)
       + 992998.143260721 vGFxGridFree#1(6)
       + 992998.143260721 vGFxGridFree#1(7)
       + 992998.143260721 vGFxGridFree#1(8)
       + 992998.143260721 vGFxGridFree#1(9) + 10 Discretized_SizeMax(1)
       + 150 Discretized_SizeMax(2) + 2000 Discretized_SizeMax(3)
       + 10 vMaxEstoStorageGen#1
Subject To
 csBigMInstalledConverter0#0:    - 20 vInstalledConverter + vMaxPowerConverter
                                 <= 0
 csMinSizeInstalledConverter0#1: vMaxPowerConverter >= 0
 cNotRunningIfNotInstalledConverter0#2: - vInstalledConverter
                                 + vStateConverter(0) <= 0
 cNotRunningIfNotInstalledConverter1#3: - vInstalledConverter
                                 + vStateConverter(1) <= 0
 cNotRunningIfNotInstalledConverter2#4: - vInstalledConverter
                                 + vStateConverter(2) <= 0
 cNotRunningIfNotInstalledConverter3#5: - vInstalledConverter
                                 + vStateConverter(3) <= 0
 cNotRunningIfNotInstalledConverter4#6: - vInstalledConverter
                                 + vStateConverter(4) <= 0
 cNotRunningIfNotInstalledConverter5#7: - vInstalledConverter
                                 + vStateConverter(5) <= 0
 cNotRunningIfNotInstalledConverter6#8: - vInstalledConverter
                                 + vStateConverter(6) <= 0
 cNotRunningIfNotInstalledConverter7#9: - vInstalledConverter
                                 + vStateConverter(7) <= 0
 cNotRunningIfNotInstalledConverter8#10: - vInstalledConverter
                                 + vStateConverter(8) <= 0
 cNotRunningIfNotInstalledConverter9#11: - vInstalledConverter
                                 + vStateConverter(9) <= 0
 cPowerInMaxBoundConverter0#12:  vPowInConverter(0) <= 20
 cPowerInMaxBoundConverter0#13:  vPowInConverter(1) <= 20
 cPowerInMaxBoundConverter0#14:  vPowInConverter(2) <= 20
 cPowerInMaxBoundConverter0#15:  vPowInConverter(3) <= 20
 cPowerInMaxBoundConverter0#16:  vPowInConverter(4) <= 20
 cPowerInMaxBoundConverter0#17:  vPowInConverter(5) <= 20
 cPowerInMaxBoundConverter0#18:  vPowInConverter(6) <= 20
 cPowerInMaxBoundConverter0#19:  vPowInConverter(7) <= 20
 cPowerInMaxBoundConverter0#20:  vPowInConverter(8) <= 20
 cPowerInMaxBoundConverter0#21:  vPowInConverter(9) <= 20
 cPowMaxConverter0#22:           - 20 vStateConverter(0) + vPowInConverter(0)
                                 <= 0
 cDefPminConverter0#23:          - vPminConverter(0)  = 0
 cPminConverter0#24:             vPowInConverter(0) - vPminConverter(0) >= 0
 cPowMaxConverter1#25:           - 20 vStateConverter(1) + vPowInConverter(1)
                                 <= 0
 cDefPminConverter1#26:          - vPminConverter(1)  = 0
 cPminConverter1#27:             vPowInConverter(1) - vPminConverter(1) >= 0
 cPowMaxConverter2#28:           - 20 vStateConverter(2) + vPowInConverter(2)
                                 <= 0
 cDefPminConverter2#29:          - vPminConverter(2)  = 0
 cPminConverter2#30:             vPowInConverter(2) - vPminConverter(2) >= 0
 cPowMaxConverter3#31:           - 20 vStateConverter(3) + vPowInConverter(3)
                                 <= 0
 cDefPminConverter3#32:          - vPminConverter(3)  = 0
 cPminConverter3#33:             vPowInConverter(3) - vPminConverter(3) >= 0
 cPowMaxConverter4#34:           - 20 vStateConverter(4) + vPowInConverter(4)
                                 <= 0
 cDefPminConverter4#35:          - vPminConverter(4)  = 0
 cPminConverter4#36:             vPowInConverter(4) - vPminConverter(4) >= 0
 cPowMaxConverter5#37:           - 20 vStateConverter(5) + vPowInConverter(5)
                                 <= 0
 cDefPminConverter5#38:          - vPminConverter(5)  = 0
 cPminConverter5#39:             vPowInConverter(5) - vPminConverter(5) >= 0
 cPowMaxConverter6#40:           - 20 vStateConverter(6) + vPowInConverter(6)
                                 <= 0
 cDefPminConverter6#41:          - vPminConverter(6)  = 0
 cPminConverter6#42:             vPowInConverter(6) - vPminConverter(6) >= 0
 cPowMaxConverter7#43:           - 20 vStateConverter(7) + vPowInConverter(7)
                                 <= 0
 cDefPminConverter7#44:          - vPminConverter(7)  = 0
 cPminConverter7#45:             vPowInConverter(7) - vPminConverter(7) >= 0
 cPowMaxConverter8#46:           - 20 vStateConverter(8) + vPowInConverter(8)
                                 <= 0
 cDefPminConverter8#47:          - vPminConverter(8)  = 0
 cPminConverter8#48:             vPowInConverter(8) - vPminConverter(8) >= 0
 cPowMaxConverter9#49:           - 20 vStateConverter(9) + vPowInConverter(9)
                                 <= 0
 cDefPminConverter9#50:          - vPminConverter(9)  = 0
 cPminConverter9#51:             vPowInConverter(9) - vPminConverter(9) >= 0
 cPowEffConverter0#52:           vPowInConverter(0) - vPowOutConverter(0)  = 0
 cPowEffConverter1#53:           vPowInConverter(1) - vPowOutConverter(1)  = 0
 cPowEffConverter2#54:           vPowInConverter(2) - vPowOutConverter(2)  = 0
 cPowEffConverter3#55:           vPowInConverter(3) - vPowOutConverter(3)  = 0
 cPowEffConverter4#56:           vPowInConverter(4) - vPowOutConverter(4)  = 0
 cPowEffConverter5#57:           vPowInConverter(5) - vPowOutConverter(5)  = 0
 cPowEffConverter6#58:           vPowInConverter(6) - vPowOutConverter(6)  = 0
 cPowEffConverter7#59:           vPowInConverter(7) - vPowOutConverter(7)  = 0
 cPowEffConverter8#60:           vPowInConverter(8) - vPowOutConverter(8)  = 0
 cPowEffConverter9#61:           vPowInConverter(9) - vPowOutConverter(9)  = 0
 cPowVarConverter0#62:           - vMaxPowerConverter + vPowInConverter(0) <= 0
 cPowVarConverter1#63:           - vMaxPowerConverter + vPowInConverter(1) <= 0
 cPowVarConverter2#64:           - vMaxPowerConverter + vPowInConverter(2) <= 0
 cPowVarConverter3#65:           - vMaxPowerConverter + vPowInConverter(3) <= 0
 cPowVarConverter4#66:           - vMaxPowerConverter + vPowInConverter(4) <= 0
 cPowVarConverter5#67:           - vMaxPowerConverter + vPowInConverter(5) <= 0
 cPowVarConverter6#68:           - vMaxPowerConverter + vPowInConverter(6) <= 0
 cPowVarConverter7#69:           - vMaxPowerConverter + vPowInConverter(7) <= 0
 cPowVarConverter8#70:           - vMaxPowerConverter + vPowInConverter(8) <= 0
 cPowVarConverter9#71:           - vMaxPowerConverter + vPowInConverter(9) <= 0
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
 csBigMInstalledResourceStock10#106: - 1000 vInstalledResourceStock1
                                 + vResourceUsedResourceStock1 <= 0
 csMinSizeInstalledResourceStock10#107: vResourceUsedResourceStock1 >= 0
 C_SizeMax#108:                  vResourceUsedResourceStock1
                                 - 10 Discretized_SizeMax(1)
                                 - 15 Discretized_SizeMax(2)
                                 - 20 Discretized_SizeMax(3)  = 0
 c110#109:                       Discretized_SizeMax(0)
                                 + Discretized_SizeMax(1)
                                 + Discretized_SizeMax(2)
                                 + Discretized_SizeMax(3)  = 1
 csInstalledSourceLoad#10#110:   vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10#111:      vWeightSourceLoad#1  = 1
 csBigMInstalledSourceLoad#10#112: - vInstalledSourceLoad#1
                                 + vWeightSourceLoad#1 <= 0
 csMinSizeInstalledSourceLoad#10#113: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10#114:         <= 10000
 cMSFxSourceLoad#11#115:         <= 10000
 cMSFxSourceLoad#12#116:         <= 10000
 cMSFxSourceLoad#13#117:         <= 10000
 cMSFxSourceLoad#14#118:         <= 10000
 cMSFxSourceLoad#15#119:         <= 10000
 cMSFxSourceLoad#16#120:         <= 10000
 cMSFxSourceLoad#17#121:         <= 10000
 cMSFxSourceLoad#18#122:         <= 10000
 cMSFxSourceLoad#19#123:         <= 10000
 csBigMInstalledStorageGen#10#124: - 100000000 vInstalledStorageGen#1
                                 + vMaxEstoStorageGen#1 <= 0
 csMinSizeInstalledStorageGen#10#125: vMaxEstoStorageGen#1 >= 0
 cStoIni3StorageGen#10#126:      - 0.5 vMaxEstoStorageGen#1
                                 + vStoIniStorageGen#1  = 0
 cMStorageGen#10#127:            - vMaxEstoStorageGen#1 + vEStorageGen#1(0)
                                 <= 0
 cStoStorageGen#10#128:          vEStorageGen#1(0) - vQCStorageGen#1(0)
                                 + vQDStorageGen#1(0) - vStoIniStorageGen#1
                                  = 0
 cMStorageGen#11#129:            - vMaxEstoStorageGen#1 + vEStorageGen#1(1)
                                 <= 0
 cStoStorageGen#11#130:          - vEStorageGen#1(0) + vEStorageGen#1(1)
                                 - vQCStorageGen#1(1) + vQDStorageGen#1(1)  = 0
 cMStorageGen#12#131:            - vMaxEstoStorageGen#1 + vEStorageGen#1(2)
                                 <= 0
 cStoStorageGen#12#132:          - vEStorageGen#1(1) + vEStorageGen#1(2)
                                 - vQCStorageGen#1(2) + vQDStorageGen#1(2)  = 0
 cMStorageGen#13#133:            - vMaxEstoStorageGen#1 + vEStorageGen#1(3)
                                 <= 0
 cStoStorageGen#13#134:          - vEStorageGen#1(2) + vEStorageGen#1(3)
                                 - vQCStorageGen#1(3) + vQDStorageGen#1(3)  = 0
 cMStorageGen#14#135:            - vMaxEstoStorageGen#1 + vEStorageGen#1(4)
                                 <= 0
 cStoStorageGen#14#136:          - vEStorageGen#1(3) + vEStorageGen#1(4)
                                 - vQCStorageGen#1(4) + vQDStorageGen#1(4)  = 0
 cMStorageGen#15#137:            - vMaxEstoStorageGen#1 + vEStorageGen#1(5)
                                 <= 0
 cStoStorageGen#15#138:          - vEStorageGen#1(4) + vEStorageGen#1(5)
                                 - vQCStorageGen#1(5) + vQDStorageGen#1(5)  = 0
 cMStorageGen#16#139:            - vMaxEstoStorageGen#1 + vEStorageGen#1(6)
                                 <= 0
 cStoStorageGen#16#140:          - vEStorageGen#1(5) + vEStorageGen#1(6)
                                 - vQCStorageGen#1(6) + vQDStorageGen#1(6)  = 0
 cMStorageGen#17#141:            - vMaxEstoStorageGen#1 + vEStorageGen#1(7)
                                 <= 0
 cStoStorageGen#17#142:          - vEStorageGen#1(6) + vEStorageGen#1(7)
                                 - vQCStorageGen#1(7) + vQDStorageGen#1(7)  = 0
 cMStorageGen#18#143:            - vMaxEstoStorageGen#1 + vEStorageGen#1(8)
                                 <= 0
 cStoStorageGen#18#144:          - vEStorageGen#1(7) + vEStorageGen#1(8)
                                 - vQCStorageGen#1(8) + vQDStorageGen#1(8)  = 0
 cMStorageGen#19#145:            - vMaxEstoStorageGen#1 + vEStorageGen#1(9)
                                 <= 0
 cStoStorageGen#19#146:          - vEStorageGen#1(8) + vEStorageGen#1(9)
                                 - vQCStorageGen#1(9) + vQDStorageGen#1(9)  = 0
 cMinStoFinStorageGen#19#147:    vEStorageGen#1(9) - vStoIniStorageGen#1 >= 0
 cMFCStorageGen#10#148:          vQCStorageGen#1(0)
                                 - 100000000 vsensStorageGen#1(0) <= 0
 cMFDStorageGen#10#149:          vQDStorageGen#1(0)
                                 + 100000000 vsensStorageGen#1(0) <= 100000000
 cMFCStorageGen#11#150:          vQCStorageGen#1(1)
                                 - 100000000 vsensStorageGen#1(1) <= 0
 cMFDStorageGen#11#151:          vQDStorageGen#1(1)
                                 + 100000000 vsensStorageGen#1(1) <= 100000000
 cMFCStorageGen#12#152:          vQCStorageGen#1(2)
                                 - 100000000 vsensStorageGen#1(2) <= 0
 cMFDStorageGen#12#153:          vQDStorageGen#1(2)
                                 + 100000000 vsensStorageGen#1(2) <= 100000000
 cMFCStorageGen#13#154:          vQCStorageGen#1(3)
                                 - 100000000 vsensStorageGen#1(3) <= 0
 cMFDStorageGen#13#155:          vQDStorageGen#1(3)
                                 + 100000000 vsensStorageGen#1(3) <= 100000000
 cMFCStorageGen#14#156:          vQCStorageGen#1(4)
                                 - 100000000 vsensStorageGen#1(4) <= 0
 cMFDStorageGen#14#157:          vQDStorageGen#1(4)
                                 + 100000000 vsensStorageGen#1(4) <= 100000000
 cMFCStorageGen#15#158:          vQCStorageGen#1(5)
                                 - 100000000 vsensStorageGen#1(5) <= 0
 cMFDStorageGen#15#159:          vQDStorageGen#1(5)
                                 + 100000000 vsensStorageGen#1(5) <= 100000000
 cMFCStorageGen#16#160:          vQCStorageGen#1(6)
                                 - 100000000 vsensStorageGen#1(6) <= 0
 cMFDStorageGen#16#161:          vQDStorageGen#1(6)
                                 + 100000000 vsensStorageGen#1(6) <= 100000000
 cMFCStorageGen#17#162:          vQCStorageGen#1(7)
                                 - 100000000 vsensStorageGen#1(7) <= 0
 cMFDStorageGen#17#163:          vQDStorageGen#1(7)
                                 + 100000000 vsensStorageGen#1(7) <= 100000000
 cMFCStorageGen#18#164:          vQCStorageGen#1(8)
                                 - 100000000 vsensStorageGen#1(8) <= 0
 cMFDStorageGen#18#165:          vQDStorageGen#1(8)
                                 + 100000000 vsensStorageGen#1(8) <= 100000000
 cMFCStorageGen#19#166:          vQCStorageGen#1(9)
                                 - 100000000 vsensStorageGen#1(9) <= 0
 cMFDStorageGen#19#167:          vQDStorageGen#1(9)
                                 + 100000000 vsensStorageGen#1(9) <= 100000000
 cENodeEquality#10#168:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#169:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#170:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#171:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#172:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#173:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#174:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#175:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#176:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#177:          - vMaxPowerConverter
                                 + vBusValueNodeEquality#1(9)  = 0
 cENodeEquality#10#178:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#179:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#180:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#181:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#182:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#183:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#184:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#185:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#186:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#187:          - vResourceUsedResourceStock1
                                 + vBusValueNodeEquality#1(9)  = 0
 cSNodeLaw#10#188:               - vPowInConverter(0) + vGFxGridFree#1(0)  = 0
 cSNodeLaw#11#189:               - vPowInConverter(1) + vGFxGridFree#1(1)  = 0
 cSNodeLaw#12#190:               - vPowInConverter(2) + vGFxGridFree#1(2)  = 0
 cSNodeLaw#13#191:               - vPowInConverter(3) + vGFxGridFree#1(3)  = 0
 cSNodeLaw#14#192:               - vPowInConverter(4) + vGFxGridFree#1(4)  = 0
 cSNodeLaw#15#193:               - vPowInConverter(5) + vGFxGridFree#1(5)  = 0
 cSNodeLaw#16#194:               - vPowInConverter(6) + vGFxGridFree#1(6)  = 0
 cSNodeLaw#17#195:               - vPowInConverter(7) + vGFxGridFree#1(7)  = 0
 cSNodeLaw#18#196:               - vPowInConverter(8) + vGFxGridFree#1(8)  = 0
 cSNodeLaw#19#197:               - vPowInConverter(9) + vGFxGridFree#1(9)  = 0
 cSNodeLaw#20#198:               vPowOutConverter(0) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(0) + vQDStorageGen#1(0)  = 0
 cSNodeLaw#21#199:               vPowOutConverter(1) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(1) + vQDStorageGen#1(1)  = 0
 cSNodeLaw#22#200:               vPowOutConverter(2) - 12 vWeightSourceLoad#1
                                 - vQCStorageGen#1(2) + vQDStorageGen#1(2)  = 0
 cSNodeLaw#23#201:               vPowOutConverter(3) - 16 vWeightSourceLoad#1
                                 - vQCStorageGen#1(3) + vQDStorageGen#1(3)  = 0
 cSNodeLaw#24#202:               vPowOutConverter(4) - 4 vWeightSourceLoad#1
                                 - vQCStorageGen#1(4) + vQDStorageGen#1(4)  = 0
 cSNodeLaw#25#203:               vPowOutConverter(5) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(5) + vQDStorageGen#1(5)  = 0
 cSNodeLaw#26#204:               vPowOutConverter(6) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(6) + vQDStorageGen#1(6)  = 0
 cSNodeLaw#27#205:               vPowOutConverter(7) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(7) + vQDStorageGen#1(7)  = 0
 cSNodeLaw#28#206:               vPowOutConverter(8) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(8) + vQDStorageGen#1(8)  = 0
 cSNodeLaw#29#207:               vPowOutConverter(9) - 11 vWeightSourceLoad#1
                                 - vQCStorageGen#1(9) + vQDStorageGen#1(9)  = 0
Bounds
 0 <= vInstalledConverter <= 1
 0 <= vMaxPowerConverter <= 20
 0 <= vStateConverter(0) <= 1
 0 <= vStateConverter(1) <= 1
 0 <= vStateConverter(2) <= 1
 0 <= vStateConverter(3) <= 1
 0 <= vStateConverter(4) <= 1
 0 <= vStateConverter(5) <= 1
 0 <= vStateConverter(6) <= 1
 0 <= vStateConverter(7) <= 1
 0 <= vStateConverter(8) <= 1
 0 <= vStateConverter(9) <= 1
 0 <= vPowInConverter(0) <= 20
 0 <= vPowInConverter(1) <= 20
 0 <= vPowInConverter(2) <= 20
 0 <= vPowInConverter(3) <= 20
 0 <= vPowInConverter(4) <= 20
 0 <= vPowInConverter(5) <= 20
 0 <= vPowInConverter(6) <= 20
 0 <= vPowInConverter(7) <= 20
 0 <= vPowInConverter(8) <= 20
 0 <= vPowInConverter(9) <= 20
 0 <= vPowOutConverter(0) <= 20
 0 <= vPowOutConverter(1) <= 20
 0 <= vPowOutConverter(2) <= 20
 0 <= vPowOutConverter(3) <= 20
 0 <= vPowOutConverter(4) <= 20
 0 <= vPowOutConverter(5) <= 20
 0 <= vPowOutConverter(6) <= 20
 0 <= vPowOutConverter(7) <= 20
 0 <= vPowOutConverter(8) <= 20
 0 <= vPowOutConverter(9) <= 20
      vPminConverter(0) Free
      vPminConverter(1) Free
      vPminConverter(2) Free
      vPminConverter(3) Free
      vPminConverter(4) Free
      vPminConverter(5) Free
      vPminConverter(6) Free
      vPminConverter(7) Free
      vPminConverter(8) Free
      vPminConverter(9) Free
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
 0 <= vInstalledResourceStock1 <= 1
 0 <= vResourceUsedResourceStock1 <= 1000
 0 <= Discretized_SizeMax(0) <= 1
 0 <= Discretized_SizeMax(1) <= 1
 0 <= Discretized_SizeMax(2) <= 1
 0 <= Discretized_SizeMax(3) <= 1
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 1
 0 <= vInstalledStorageGen#1 <= 1
 0 <= vMaxEstoStorageGen#1 <= 100000000
 0 <= vEStorageGen#1(0) <= 100000000
 0 <= vEStorageGen#1(1) <= 100000000
 0 <= vEStorageGen#1(2) <= 100000000
 0 <= vEStorageGen#1(3) <= 100000000
 0 <= vEStorageGen#1(4) <= 100000000
 0 <= vEStorageGen#1(5) <= 100000000
 0 <= vEStorageGen#1(6) <= 100000000
 0 <= vEStorageGen#1(7) <= 100000000
 0 <= vEStorageGen#1(8) <= 100000000
 0 <= vEStorageGen#1(9) <= 100000000
 0 <= vQCStorageGen#1(0) <= 100000000
 0 <= vQCStorageGen#1(1) <= 100000000
 0 <= vQCStorageGen#1(2) <= 100000000
 0 <= vQCStorageGen#1(3) <= 100000000
 0 <= vQCStorageGen#1(4) <= 100000000
 0 <= vQCStorageGen#1(5) <= 100000000
 0 <= vQCStorageGen#1(6) <= 100000000
 0 <= vQCStorageGen#1(7) <= 100000000
 0 <= vQCStorageGen#1(8) <= 100000000
 0 <= vQCStorageGen#1(9) <= 100000000
 0 <= vQDStorageGen#1(0) <= 100000000
 0 <= vQDStorageGen#1(1) <= 100000000
 0 <= vQDStorageGen#1(2) <= 100000000
 0 <= vQDStorageGen#1(3) <= 100000000
 0 <= vQDStorageGen#1(4) <= 100000000
 0 <= vQDStorageGen#1(5) <= 100000000
 0 <= vQDStorageGen#1(6) <= 100000000
 0 <= vQDStorageGen#1(7) <= 100000000
 0 <= vQDStorageGen#1(8) <= 100000000
 0 <= vQDStorageGen#1(9) <= 100000000
 0 <= vStoIniStorageGen#1 <= 100000000
 0 <= vsensStorageGen#1(0) <= 1
 0 <= vsensStorageGen#1(1) <= 1
 0 <= vsensStorageGen#1(2) <= 1
 0 <= vsensStorageGen#1(3) <= 1
 0 <= vsensStorageGen#1(4) <= 1
 0 <= vsensStorageGen#1(5) <= 1
 0 <= vsensStorageGen#1(6) <= 1
 0 <= vsensStorageGen#1(7) <= 1
 0 <= vsensStorageGen#1(8) <= 1
 0 <= vsensStorageGen#1(9) <= 1
 0 <= vBusValueNodeEquality#1(0) <= 1e+15
 0 <= vBusValueNodeEquality#1(1) <= 1e+15
 0 <= vBusValueNodeEquality#1(2) <= 1e+15
 0 <= vBusValueNodeEquality#1(3) <= 1e+15
 0 <= vBusValueNodeEquality#1(4) <= 1e+15
 0 <= vBusValueNodeEquality#1(5) <= 1e+15
 0 <= vBusValueNodeEquality#1(6) <= 1e+15
 0 <= vBusValueNodeEquality#1(7) <= 1e+15
 0 <= vBusValueNodeEquality#1(8) <= 1e+15
 0 <= vBusValueNodeEquality#1(9) <= 1e+15
Generals
 vInstalledConverter  vStateConverter(0)  vStateConverter(1) 
 vStateConverter(2)  vStateConverter(3)  vStateConverter(4) 
 vStateConverter(5)  vStateConverter(6)  vStateConverter(7) 
 vStateConverter(8)  vStateConverter(9)  vInstalledGridFree#1 
 vStateGridFree#1(0)  vStateGridFree#1(1)  vStateGridFree#1(2) 
 vStateGridFree#1(3)  vStateGridFree#1(4)  vStateGridFree#1(5) 
 vStateGridFree#1(6)  vStateGridFree#1(7)  vStateGridFree#1(8) 
 vStateGridFree#1(9)  vInstalledResourceStock1  vInstalledSourceLoad#1 
 vInstalledStorageGen#1  vsensStorageGen#1(0)  vsensStorageGen#1(1) 
 vsensStorageGen#1(2)  vsensStorageGen#1(3)  vsensStorageGen#1(4) 
 vsensStorageGen#1(5)  vsensStorageGen#1(6)  vsensStorageGen#1(7) 
 vsensStorageGen#1(8)  vsensStorageGen#1(9) 
End
