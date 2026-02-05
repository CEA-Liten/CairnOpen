\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 368.013571456214 vMaxPowerConverter + 992998.143260721 vGFxHeat_Grid(0)
       + 992998.143260721 vGFxHeat_Grid(1) + 992998.143260721 vGFxHeat_Grid(2)
       + 992998.143260721 vGFxHeat_Grid(3) + 992998.143260721 vGFxHeat_Grid(4)
       + 992998.143260721 vGFxHeat_Grid(5) + 992998.143260721 vGFxHeat_Grid(6)
       + 992998.143260721 vGFxHeat_Grid(7) + 992998.143260721 vGFxHeat_Grid(8)
       + 992998.143260721 vGFxHeat_Grid(9) + 100 vWeightSourceLoad#1
Subject To
 csBigMInstalledConverter0#0:    - 2000 vInstalledConverter
                                 + vMaxPowerConverter <= 0
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
 cPowerInMaxBoundConverter0#12:  vPowInConverter(0) <= 2000
 cPowerInMaxBoundConverter0#13:  vPowInConverter(1) <= 2000
 cPowerInMaxBoundConverter0#14:  vPowInConverter(2) <= 2000
 cPowerInMaxBoundConverter0#15:  vPowInConverter(3) <= 2000
 cPowerInMaxBoundConverter0#16:  vPowInConverter(4) <= 2000
 cPowerInMaxBoundConverter0#17:  vPowInConverter(5) <= 2000
 cPowerInMaxBoundConverter0#18:  vPowInConverter(6) <= 2000
 cPowerInMaxBoundConverter0#19:  vPowInConverter(7) <= 2000
 cPowerInMaxBoundConverter0#20:  vPowInConverter(8) <= 2000
 cPowerInMaxBoundConverter0#21:  vPowInConverter(9) <= 2000
 cPowMaxConverter0#22:           - 2000 vStateConverter(0) + vPowInConverter(0)
                                 <= 0
 cDefPminConverter0#23:          0.2 vMaxPowerConverter - vPminConverter(0)
                                  = 0
 cPminConverter0#24:             - 400 vStateConverter(0) + vPowInConverter(0)
                                 - vPminConverter(0) >= -400
 cPowMaxConverter1#25:           - 2000 vStateConverter(1) + vPowInConverter(1)
                                 <= 0
 cDefPminConverter1#26:          0.2 vMaxPowerConverter - vPminConverter(1)
                                  = 0
 cPminConverter1#27:             - 400 vStateConverter(1) + vPowInConverter(1)
                                 - vPminConverter(1) >= -400
 cPowMaxConverter2#28:           - 2000 vStateConverter(2) + vPowInConverter(2)
                                 <= 0
 cDefPminConverter2#29:          0.2 vMaxPowerConverter - vPminConverter(2)
                                  = 0
 cPminConverter2#30:             - 400 vStateConverter(2) + vPowInConverter(2)
                                 - vPminConverter(2) >= -400
 cPowMaxConverter3#31:           - 2000 vStateConverter(3) + vPowInConverter(3)
                                 <= 0
 cDefPminConverter3#32:          0.2 vMaxPowerConverter - vPminConverter(3)
                                  = 0
 cPminConverter3#33:             - 400 vStateConverter(3) + vPowInConverter(3)
                                 - vPminConverter(3) >= -400
 cPowMaxConverter4#34:           - 2000 vStateConverter(4) + vPowInConverter(4)
                                 <= 0
 cDefPminConverter4#35:          0.2 vMaxPowerConverter - vPminConverter(4)
                                  = 0
 cPminConverter4#36:             - 400 vStateConverter(4) + vPowInConverter(4)
                                 - vPminConverter(4) >= -400
 cPowMaxConverter5#37:           - 2000 vStateConverter(5) + vPowInConverter(5)
                                 <= 0
 cDefPminConverter5#38:          0.2 vMaxPowerConverter - vPminConverter(5)
                                  = 0
 cPminConverter5#39:             - 400 vStateConverter(5) + vPowInConverter(5)
                                 - vPminConverter(5) >= -400
 cPowMaxConverter6#40:           - 2000 vStateConverter(6) + vPowInConverter(6)
                                 <= 0
 cDefPminConverter6#41:          0.2 vMaxPowerConverter - vPminConverter(6)
                                  = 0
 cPminConverter6#42:             - 400 vStateConverter(6) + vPowInConverter(6)
                                 - vPminConverter(6) >= -400
 cPowMaxConverter7#43:           - 2000 vStateConverter(7) + vPowInConverter(7)
                                 <= 0
 cDefPminConverter7#44:          0.2 vMaxPowerConverter - vPminConverter(7)
                                  = 0
 cPminConverter7#45:             - 400 vStateConverter(7) + vPowInConverter(7)
                                 - vPminConverter(7) >= -400
 cPowMaxConverter8#46:           - 2000 vStateConverter(8) + vPowInConverter(8)
                                 <= 0
 cDefPminConverter8#47:          0.2 vMaxPowerConverter - vPminConverter(8)
                                  = 0
 cPminConverter8#48:             - 400 vStateConverter(8) + vPowInConverter(8)
                                 - vPminConverter(8) >= -400
 cPowMaxConverter9#49:           - 2000 vStateConverter(9) + vPowInConverter(9)
                                 <= 0
 cDefPminConverter9#50:          0.2 vMaxPowerConverter - vPminConverter(9)
                                  = 0
 cPminConverter9#51:             - 400 vStateConverter(9) + vPowInConverter(9)
                                 - vPminConverter(9) >= -400
 cPowEffConverter0#52:           0.8 vPowInConverter(0) - vPowOutConverter(0)
                                  = 0
 cPowEffConverter1#53:           0.8 vPowInConverter(1) - vPowOutConverter(1)
                                  = 0
 cPowEffConverter2#54:           0.8 vPowInConverter(2) - vPowOutConverter(2)
                                  = 0
 cPowEffConverter3#55:           0.8 vPowInConverter(3) - vPowOutConverter(3)
                                  = 0
 cPowEffConverter4#56:           0.8 vPowInConverter(4) - vPowOutConverter(4)
                                  = 0
 cPowEffConverter5#57:           0.8 vPowInConverter(5) - vPowOutConverter(5)
                                  = 0
 cPowEffConverter6#58:           0.8 vPowInConverter(6) - vPowOutConverter(6)
                                  = 0
 cPowEffConverter7#59:           0.8 vPowInConverter(7) - vPowOutConverter(7)
                                  = 0
 cPowEffConverter8#60:           0.8 vPowInConverter(8) - vPowOutConverter(8)
                                  = 0
 cPowEffConverter9#61:           0.8 vPowInConverter(9) - vPowOutConverter(9)
                                  = 0
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
 csInstalledElec_Load0#72:       vInstalledElec_Load  = 1
 csWeightElec_Load0#73:          vWeightElec_Load  = 1
 csBigMInstalledElec_Load0#74:   - vInstalledElec_Load + vWeightElec_Load <= 0
 csMinSizeInstalledElec_Load0#75: vWeightElec_Load >= 0
 cMSFxElec_Load0#76:             <= 10000
 cMSFxElec_Load1#77:             <= 10000
 cMSFxElec_Load2#78:             <= 10000
 cMSFxElec_Load3#79:             <= 10000
 cMSFxElec_Load4#80:             <= 10000
 cMSFxElec_Load5#81:             <= 10000
 cMSFxElec_Load6#82:             <= 10000
 cMSFxElec_Load7#83:             <= 10000
 cMSFxElec_Load8#84:             <= 10000
 cMSFxElec_Load9#85:             <= 10000
 csInstalledHeat_Grid0#86:       vInstalledHeat_Grid  = 1
 csMaxValHeat_Grid0#87:          vMaxFlowHeat_Grid  = 10000
 csBigMInstalledHeat_Grid0#88:   - 10000 vInstalledHeat_Grid
                                 + vMaxFlowHeat_Grid <= 0
 csMinSizeInstalledHeat_Grid0#89: vMaxFlowHeat_Grid >= 0
 cNotRunningIfNotInstalledHeat_Grid0#90: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(0) <= 0
 cNotRunningIfNotInstalledHeat_Grid1#91: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(1) <= 0
 cNotRunningIfNotInstalledHeat_Grid2#92: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(2) <= 0
 cNotRunningIfNotInstalledHeat_Grid3#93: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(3) <= 0
 cNotRunningIfNotInstalledHeat_Grid4#94: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(4) <= 0
 cNotRunningIfNotInstalledHeat_Grid5#95: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(5) <= 0
 cNotRunningIfNotInstalledHeat_Grid6#96: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(6) <= 0
 cNotRunningIfNotInstalledHeat_Grid7#97: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(7) <= 0
 cNotRunningIfNotInstalledHeat_Grid8#98: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(8) <= 0
 cNotRunningIfNotInstalledHeat_Grid9#99: - vInstalledHeat_Grid
                                 + vStateHeat_Grid(9) <= 0
 cStateGridHeat_Grid0#100:       - 10000 vStateHeat_Grid(0) + vGFxHeat_Grid(0)
                                 <= 0
 cStateGridHeat_Grid1#101:       - 10000 vStateHeat_Grid(1) + vGFxHeat_Grid(1)
                                 <= 0
 cStateGridHeat_Grid2#102:       - 10000 vStateHeat_Grid(2) + vGFxHeat_Grid(2)
                                 <= 0
 cStateGridHeat_Grid3#103:       - 10000 vStateHeat_Grid(3) + vGFxHeat_Grid(3)
                                 <= 0
 cStateGridHeat_Grid4#104:       - 10000 vStateHeat_Grid(4) + vGFxHeat_Grid(4)
                                 <= 0
 cStateGridHeat_Grid5#105:       - 10000 vStateHeat_Grid(5) + vGFxHeat_Grid(5)
                                 <= 0
 cStateGridHeat_Grid6#106:       - 10000 vStateHeat_Grid(6) + vGFxHeat_Grid(6)
                                 <= 0
 cStateGridHeat_Grid7#107:       - 10000 vStateHeat_Grid(7) + vGFxHeat_Grid(7)
                                 <= 0
 cStateGridHeat_Grid8#108:       - 10000 vStateHeat_Grid(8) + vGFxHeat_Grid(8)
                                 <= 0
 cStateGridHeat_Grid9#109:       - 10000 vStateHeat_Grid(9) + vGFxHeat_Grid(9)
                                 <= 0
 cMaxGFxHeat_Grid0#110:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(0) <= 0
 cMaxGFxHeat_Grid1#111:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(1) <= 0
 cMaxGFxHeat_Grid2#112:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(2) <= 0
 cMaxGFxHeat_Grid3#113:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(3) <= 0
 cMaxGFxHeat_Grid4#114:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(4) <= 0
 cMaxGFxHeat_Grid5#115:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(5) <= 0
 cMaxGFxHeat_Grid6#116:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(6) <= 0
 cMaxGFxHeat_Grid7#117:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(7) <= 0
 cMaxGFxHeat_Grid8#118:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(8) <= 0
 cMaxGFxHeat_Grid9#119:          - vMaxFlowHeat_Grid + vGFxHeat_Grid(9) <= 0
 csBigMInstalledSourceLoad#10#120: - 1000 vInstalledSourceLoad#1
                                 + vWeightSourceLoad#1 <= 0
 csMinSizeInstalledSourceLoad#10#121: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10#122:         <= 10000
 cMSFxSourceLoad#11#123:         <= 10000
 cMSFxSourceLoad#12#124:         <= 10000
 cMSFxSourceLoad#13#125:         <= 10000
 cMSFxSourceLoad#14#126:         <= 10000
 cMSFxSourceLoad#15#127:         <= 10000
 cMSFxSourceLoad#16#128:         <= 10000
 cMSFxSourceLoad#17#129:         <= 10000
 cMSFxSourceLoad#18#130:         <= 10000
 cMSFxSourceLoad#19#131:         <= 10000
 csBigMInstalledStorageGen#10#132: - 100000000 vInstalledStorageGen#1
                                 + vMaxEstoStorageGen#1 <= 0
 csMinSizeInstalledStorageGen#10#133: vMaxEstoStorageGen#1 >= 0
 cStoIni3StorageGen#10#134:      - 0.5 vMaxEstoStorageGen#1
                                 + vStoIniStorageGen#1  = 0
 cMStorageGen#10#135:            - vMaxEstoStorageGen#1 + vEStorageGen#1(0)
                                 <= 0
 cStoStorageGen#10#136:          vEStorageGen#1(0) - vQCStorageGen#1(0)
                                 + vQDStorageGen#1(0) - vStoIniStorageGen#1
                                  = 0
 cMStorageGen#11#137:            - vMaxEstoStorageGen#1 + vEStorageGen#1(1)
                                 <= 0
 cStoStorageGen#11#138:          - vEStorageGen#1(0) + vEStorageGen#1(1)
                                 - vQCStorageGen#1(1) + vQDStorageGen#1(1)  = 0
 cMStorageGen#12#139:            - vMaxEstoStorageGen#1 + vEStorageGen#1(2)
                                 <= 0
 cStoStorageGen#12#140:          - vEStorageGen#1(1) + vEStorageGen#1(2)
                                 - vQCStorageGen#1(2) + vQDStorageGen#1(2)  = 0
 cMStorageGen#13#141:            - vMaxEstoStorageGen#1 + vEStorageGen#1(3)
                                 <= 0
 cStoStorageGen#13#142:          - vEStorageGen#1(2) + vEStorageGen#1(3)
                                 - vQCStorageGen#1(3) + vQDStorageGen#1(3)  = 0
 cMStorageGen#14#143:            - vMaxEstoStorageGen#1 + vEStorageGen#1(4)
                                 <= 0
 cStoStorageGen#14#144:          - vEStorageGen#1(3) + vEStorageGen#1(4)
                                 - vQCStorageGen#1(4) + vQDStorageGen#1(4)  = 0
 cMStorageGen#15#145:            - vMaxEstoStorageGen#1 + vEStorageGen#1(5)
                                 <= 0
 cStoStorageGen#15#146:          - vEStorageGen#1(4) + vEStorageGen#1(5)
                                 - vQCStorageGen#1(5) + vQDStorageGen#1(5)  = 0
 cMStorageGen#16#147:            - vMaxEstoStorageGen#1 + vEStorageGen#1(6)
                                 <= 0
 cStoStorageGen#16#148:          - vEStorageGen#1(5) + vEStorageGen#1(6)
                                 - vQCStorageGen#1(6) + vQDStorageGen#1(6)  = 0
 cMStorageGen#17#149:            - vMaxEstoStorageGen#1 + vEStorageGen#1(7)
                                 <= 0
 cStoStorageGen#17#150:          - vEStorageGen#1(6) + vEStorageGen#1(7)
                                 - vQCStorageGen#1(7) + vQDStorageGen#1(7)  = 0
 cMStorageGen#18#151:            - vMaxEstoStorageGen#1 + vEStorageGen#1(8)
                                 <= 0
 cStoStorageGen#18#152:          - vEStorageGen#1(7) + vEStorageGen#1(8)
                                 - vQCStorageGen#1(8) + vQDStorageGen#1(8)  = 0
 cMStorageGen#19#153:            - vMaxEstoStorageGen#1 + vEStorageGen#1(9)
                                 <= 0
 cStoStorageGen#19#154:          - vEStorageGen#1(8) + vEStorageGen#1(9)
                                 - vQCStorageGen#1(9) + vQDStorageGen#1(9)  = 0
 cMinStoFinStorageGen#19#155:    vEStorageGen#1(9) - vStoIniStorageGen#1 >= 0
 cMFCStorageGen#10#156:          vQCStorageGen#1(0)
                                 - 100000000 vsensStorageGen#1(0) <= 0
 cMFDStorageGen#10#157:          vQDStorageGen#1(0)
                                 + 100000000 vsensStorageGen#1(0) <= 100000000
 cMFCStorageGen#11#158:          vQCStorageGen#1(1)
                                 - 100000000 vsensStorageGen#1(1) <= 0
 cMFDStorageGen#11#159:          vQDStorageGen#1(1)
                                 + 100000000 vsensStorageGen#1(1) <= 100000000
 cMFCStorageGen#12#160:          vQCStorageGen#1(2)
                                 - 100000000 vsensStorageGen#1(2) <= 0
 cMFDStorageGen#12#161:          vQDStorageGen#1(2)
                                 + 100000000 vsensStorageGen#1(2) <= 100000000
 cMFCStorageGen#13#162:          vQCStorageGen#1(3)
                                 - 100000000 vsensStorageGen#1(3) <= 0
 cMFDStorageGen#13#163:          vQDStorageGen#1(3)
                                 + 100000000 vsensStorageGen#1(3) <= 100000000
 cMFCStorageGen#14#164:          vQCStorageGen#1(4)
                                 - 100000000 vsensStorageGen#1(4) <= 0
 cMFDStorageGen#14#165:          vQDStorageGen#1(4)
                                 + 100000000 vsensStorageGen#1(4) <= 100000000
 cMFCStorageGen#15#166:          vQCStorageGen#1(5)
                                 - 100000000 vsensStorageGen#1(5) <= 0
 cMFDStorageGen#15#167:          vQDStorageGen#1(5)
                                 + 100000000 vsensStorageGen#1(5) <= 100000000
 cMFCStorageGen#16#168:          vQCStorageGen#1(6)
                                 - 100000000 vsensStorageGen#1(6) <= 0
 cMFDStorageGen#16#169:          vQDStorageGen#1(6)
                                 + 100000000 vsensStorageGen#1(6) <= 100000000
 cMFCStorageGen#17#170:          vQCStorageGen#1(7)
                                 - 100000000 vsensStorageGen#1(7) <= 0
 cMFDStorageGen#17#171:          vQDStorageGen#1(7)
                                 + 100000000 vsensStorageGen#1(7) <= 100000000
 cMFCStorageGen#18#172:          vQCStorageGen#1(8)
                                 - 100000000 vsensStorageGen#1(8) <= 0
 cMFDStorageGen#18#173:          vQDStorageGen#1(8)
                                 + 100000000 vsensStorageGen#1(8) <= 100000000
 cMFCStorageGen#19#174:          vQCStorageGen#1(9)
                                 - 100000000 vsensStorageGen#1(9) <= 0
 cMFDStorageGen#19#175:          vQDStorageGen#1(9)
                                 + 100000000 vsensStorageGen#1(9) <= 100000000
 cSElec_Bus0#176:                vPowOutConverter(0) - 450 vWeightElec_Load
                                 - vQCStorageGen#1(0) + vQDStorageGen#1(0)  = 0
 cSElec_Bus1#177:                vPowOutConverter(1) - 500 vWeightElec_Load
                                 - vQCStorageGen#1(1) + vQDStorageGen#1(1)  = 0
 cSElec_Bus2#178:                vPowOutConverter(2) - 510 vWeightElec_Load
                                 - vQCStorageGen#1(2) + vQDStorageGen#1(2)  = 0
 cSElec_Bus3#179:                vPowOutConverter(3) - 560 vWeightElec_Load
                                 - vQCStorageGen#1(3) + vQDStorageGen#1(3)  = 0
 cSElec_Bus4#180:                vPowOutConverter(4) - 600 vWeightElec_Load
                                 - vQCStorageGen#1(4) + vQDStorageGen#1(4)  = 0
 cSElec_Bus5#181:                vPowOutConverter(5) - 540 vWeightElec_Load
                                 - vQCStorageGen#1(5) + vQDStorageGen#1(5)  = 0
 cSElec_Bus6#182:                vPowOutConverter(6) - 700 vWeightElec_Load
                                 - vQCStorageGen#1(6) + vQDStorageGen#1(6)  = 0
 cSElec_Bus7#183:                vPowOutConverter(7) - 450 vWeightElec_Load
                                 - vQCStorageGen#1(7) + vQDStorageGen#1(7)  = 0
 cSElec_Bus8#184:                vPowOutConverter(8) - 600 vWeightElec_Load
                                 - vQCStorageGen#1(8) + vQDStorageGen#1(8)  = 0
 cSElec_Bus9#185:                vPowOutConverter(9) - 200 vWeightElec_Load
                                 - vQCStorageGen#1(9) + vQDStorageGen#1(9)  = 0
 cSHeat_Bus0#186:                - vPowInConverter(0) + vGFxHeat_Grid(0)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus1#187:                - vPowInConverter(1) + vGFxHeat_Grid(1)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus2#188:                - vPowInConverter(2) + vGFxHeat_Grid(2)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus3#189:                - vPowInConverter(3) + vGFxHeat_Grid(3)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus4#190:                - vPowInConverter(4) + vGFxHeat_Grid(4)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus5#191:                - vPowInConverter(5) + vGFxHeat_Grid(5)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus6#192:                - vPowInConverter(6) + vGFxHeat_Grid(6)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus7#193:                - vPowInConverter(7) + vGFxHeat_Grid(7)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus8#194:                - vPowInConverter(8) + vGFxHeat_Grid(8)
                                 + vWeightSourceLoad#1  = 0
 cSHeat_Bus9#195:                - vPowInConverter(9) + vGFxHeat_Grid(9)
                                 + vWeightSourceLoad#1  = 0
 cENodeEquality#10#196:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#197:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#198:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#199:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#200:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#201:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#202:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#203:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#204:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#205:          - 300 vMaxPowerConverter
                                 - 100 vWeightSourceLoad#1
                                 + vBusValueNodeEquality#1(9)  = 0
Bounds
 0 <= vInstalledConverter <= 1
 0 <= vMaxPowerConverter <= 2000
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
 0 <= vPowInConverter(0) <= 2000
 0 <= vPowInConverter(1) <= 2000
 0 <= vPowInConverter(2) <= 2000
 0 <= vPowInConverter(3) <= 2000
 0 <= vPowInConverter(4) <= 2000
 0 <= vPowInConverter(5) <= 2000
 0 <= vPowInConverter(6) <= 2000
 0 <= vPowInConverter(7) <= 2000
 0 <= vPowInConverter(8) <= 2000
 0 <= vPowInConverter(9) <= 2000
 0 <= vPowOutConverter(0) <= 1600
 0 <= vPowOutConverter(1) <= 1600
 0 <= vPowOutConverter(2) <= 1600
 0 <= vPowOutConverter(3) <= 1600
 0 <= vPowOutConverter(4) <= 1600
 0 <= vPowOutConverter(5) <= 1600
 0 <= vPowOutConverter(6) <= 1600
 0 <= vPowOutConverter(7) <= 1600
 0 <= vPowOutConverter(8) <= 1600
 0 <= vPowOutConverter(9) <= 1600
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
 0 <= vInstalledElec_Load <= 1
 0 <= vWeightElec_Load <= 1
 0 <= vInstalledHeat_Grid <= 1
 0 <= vMaxFlowHeat_Grid <= 10000
 0 <= vStateHeat_Grid(0) <= 1
 0 <= vStateHeat_Grid(1) <= 1
 0 <= vStateHeat_Grid(2) <= 1
 0 <= vStateHeat_Grid(3) <= 1
 0 <= vStateHeat_Grid(4) <= 1
 0 <= vStateHeat_Grid(5) <= 1
 0 <= vStateHeat_Grid(6) <= 1
 0 <= vStateHeat_Grid(7) <= 1
 0 <= vStateHeat_Grid(8) <= 1
 0 <= vStateHeat_Grid(9) <= 1
 0 <= vGFxHeat_Grid(0) <= 10000
 0 <= vGFxHeat_Grid(1) <= 10000
 0 <= vGFxHeat_Grid(2) <= 10000
 0 <= vGFxHeat_Grid(3) <= 10000
 0 <= vGFxHeat_Grid(4) <= 10000
 0 <= vGFxHeat_Grid(5) <= 10000
 0 <= vGFxHeat_Grid(6) <= 10000
 0 <= vGFxHeat_Grid(7) <= 10000
 0 <= vGFxHeat_Grid(8) <= 10000
 0 <= vGFxHeat_Grid(9) <= 10000
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 1000
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
 0 <= vBusValueNodeEquality#1(0) <= 200000
 0 <= vBusValueNodeEquality#1(1) <= 200000
 0 <= vBusValueNodeEquality#1(2) <= 200000
 0 <= vBusValueNodeEquality#1(3) <= 200000
 0 <= vBusValueNodeEquality#1(4) <= 200000
 0 <= vBusValueNodeEquality#1(5) <= 200000
 0 <= vBusValueNodeEquality#1(6) <= 200000
 0 <= vBusValueNodeEquality#1(7) <= 200000
 0 <= vBusValueNodeEquality#1(8) <= 200000
 0 <= vBusValueNodeEquality#1(9) <= 200000
Generals
 vInstalledConverter  vStateConverter(0)  vStateConverter(1) 
 vStateConverter(2)  vStateConverter(3)  vStateConverter(4) 
 vStateConverter(5)  vStateConverter(6)  vStateConverter(7) 
 vStateConverter(8)  vStateConverter(9)  vInstalledElec_Load 
 vInstalledHeat_Grid  vStateHeat_Grid(0)  vStateHeat_Grid(1) 
 vStateHeat_Grid(2)  vStateHeat_Grid(3)  vStateHeat_Grid(4) 
 vStateHeat_Grid(5)  vStateHeat_Grid(6)  vStateHeat_Grid(7) 
 vStateHeat_Grid(8)  vStateHeat_Grid(9)  vInstalledSourceLoad#1 
 vInstalledStorageGen#1  vsensStorageGen#1(0)  vsensStorageGen#1(1) 
 vsensStorageGen#1(2)  vsensStorageGen#1(3)  vsensStorageGen#1(4) 
 vsensStorageGen#1(5)  vsensStorageGen#1(6)  vsensStorageGen#1(7) 
 vsensStorageGen#1(8)  vsensStorageGen#1(9) 
End
