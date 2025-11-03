\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: - 1752000 vGFxH2_Grid(0) - 1752000 vGFxH2_Grid(1)
       - 1752000 vGFxH2_Grid(2) - 1752000 vGFxH2_Grid(3)
       - 1752000 vGFxH2_Grid(4) - 1752000 vGFxH2_Grid(5)
       - 1752000 vGFxH2_Grid(6) - 1752000 vGFxH2_Grid(7)
       - 1752000 vGFxH2_Grid(8) - 1752000 vGFxH2_Grid(9)
Subject To
 csInstalledCurtailment0#0:      vInstalledCurtailment  = 1
 csMaxValCurtailment0#1:         vMaxFlowCurtailment  = 10000
 csBigMInstalledCurtailment0#2:  - 10000 vInstalledCurtailment
                                 + vMaxFlowCurtailment <= 0
 csMinSizeInstalledCurtailment0#3: vMaxFlowCurtailment >= 0
 cNotRunningIfNotInstalledCurtailment0#4: - vInstalledCurtailment
                                 + vStateCurtailment(0) <= 0
 cNotRunningIfNotInstalledCurtailment1#5: - vInstalledCurtailment
                                 + vStateCurtailment(1) <= 0
 cNotRunningIfNotInstalledCurtailment2#6: - vInstalledCurtailment
                                 + vStateCurtailment(2) <= 0
 cNotRunningIfNotInstalledCurtailment3#7: - vInstalledCurtailment
                                 + vStateCurtailment(3) <= 0
 cNotRunningIfNotInstalledCurtailment4#8: - vInstalledCurtailment
                                 + vStateCurtailment(4) <= 0
 cNotRunningIfNotInstalledCurtailment5#9: - vInstalledCurtailment
                                 + vStateCurtailment(5) <= 0
 cNotRunningIfNotInstalledCurtailment6#10: - vInstalledCurtailment
                                 + vStateCurtailment(6) <= 0
 cNotRunningIfNotInstalledCurtailment7#11: - vInstalledCurtailment
                                 + vStateCurtailment(7) <= 0
 cNotRunningIfNotInstalledCurtailment8#12: - vInstalledCurtailment
                                 + vStateCurtailment(8) <= 0
 cNotRunningIfNotInstalledCurtailment9#13: - vInstalledCurtailment
                                 + vStateCurtailment(9) <= 0
 cStateGridCurtailment0#14:      - 10000 vStateCurtailment(0)
                                 + vGFxCurtailment(0) <= 0
 cStateGridCurtailment1#15:      - 10000 vStateCurtailment(1)
                                 + vGFxCurtailment(1) <= 0
 cStateGridCurtailment2#16:      - 10000 vStateCurtailment(2)
                                 + vGFxCurtailment(2) <= 0
 cStateGridCurtailment3#17:      - 10000 vStateCurtailment(3)
                                 + vGFxCurtailment(3) <= 0
 cStateGridCurtailment4#18:      - 10000 vStateCurtailment(4)
                                 + vGFxCurtailment(4) <= 0
 cStateGridCurtailment5#19:      - 10000 vStateCurtailment(5)
                                 + vGFxCurtailment(5) <= 0
 cStateGridCurtailment6#20:      - 10000 vStateCurtailment(6)
                                 + vGFxCurtailment(6) <= 0
 cStateGridCurtailment7#21:      - 10000 vStateCurtailment(7)
                                 + vGFxCurtailment(7) <= 0
 cStateGridCurtailment8#22:      - 10000 vStateCurtailment(8)
                                 + vGFxCurtailment(8) <= 0
 cStateGridCurtailment9#23:      - 10000 vStateCurtailment(9)
                                 + vGFxCurtailment(9) <= 0
 cMaxGFxCurtailment0#24:         - vMaxFlowCurtailment + vGFxCurtailment(0)
                                 <= 0
 cMaxGFxCurtailment1#25:         - vMaxFlowCurtailment + vGFxCurtailment(1)
                                 <= 0
 cMaxGFxCurtailment2#26:         - vMaxFlowCurtailment + vGFxCurtailment(2)
                                 <= 0
 cMaxGFxCurtailment3#27:         - vMaxFlowCurtailment + vGFxCurtailment(3)
                                 <= 0
 cMaxGFxCurtailment4#28:         - vMaxFlowCurtailment + vGFxCurtailment(4)
                                 <= 0
 cMaxGFxCurtailment5#29:         - vMaxFlowCurtailment + vGFxCurtailment(5)
                                 <= 0
 cMaxGFxCurtailment6#30:         - vMaxFlowCurtailment + vGFxCurtailment(6)
                                 <= 0
 cMaxGFxCurtailment7#31:         - vMaxFlowCurtailment + vGFxCurtailment(7)
                                 <= 0
 cMaxGFxCurtailment8#32:         - vMaxFlowCurtailment + vGFxCurtailment(8)
                                 <= 0
 cMaxGFxCurtailment9#33:         - vMaxFlowCurtailment + vGFxCurtailment(9)
                                 <= 0
 csInstalledElec_source0#34:     vInstalledElec_source  = 1
 csWeightElec_source0#35:        vWeightElec_source  = 1
 csBigMInstalledElec_source0#36: - vInstalledElec_source + vWeightElec_source
                                 <= 0
 csMinSizeInstalledElec_source0#37: vWeightElec_source >= 0
 cMSFxElec_source0#38:           <= 10000
 cMSFxElec_source1#39:           <= 10000
 cMSFxElec_source2#40:           <= 10000
 cMSFxElec_source3#41:           <= 10000
 cMSFxElec_source4#42:           <= 10000
 cMSFxElec_source5#43:           <= 10000
 cMSFxElec_source6#44:           <= 10000
 cMSFxElec_source7#45:           <= 10000
 cMSFxElec_source8#46:           <= 10000
 cMSFxElec_source9#47:           <= 10000
 csInstalledElectrolyzer0#48:    vInstalledElectrolyzer  = 1
 csMaxValElectrolyzer0#49:       vMaxPowerElectrolyzer  = 400
 csBigMInstalledElectrolyzer0#50: - 400 vInstalledElectrolyzer
                                 + vMaxPowerElectrolyzer <= 0
 csMinSizeInstalledElectrolyzer0#51: vMaxPowerElectrolyzer >= 0
 cNotRunningIfNotInstalledElectrolyzer0#52: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(0) <= 0
 cNotRunningIfNotInstalledElectrolyzer1#53: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(1) <= 0
 cNotRunningIfNotInstalledElectrolyzer2#54: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(2) <= 0
 cNotRunningIfNotInstalledElectrolyzer3#55: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(3) <= 0
 cNotRunningIfNotInstalledElectrolyzer4#56: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(4) <= 0
 cNotRunningIfNotInstalledElectrolyzer5#57: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(5) <= 0
 cNotRunningIfNotInstalledElectrolyzer6#58: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(6) <= 0
 cNotRunningIfNotInstalledElectrolyzer7#59: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(7) <= 0
 cNotRunningIfNotInstalledElectrolyzer8#60: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(8) <= 0
 cNotRunningIfNotInstalledElectrolyzer9#61: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(9) <= 0
 cPowFlowElectrolyzer0#62:       vPowElectrolyzer(0)
                                 - 0.0555333333333333 vFlowElectrolyzer(0)  = 0
 cPowFlowElectrolyzer1#63:       vPowElectrolyzer(1)
                                 - 0.0555333333333333 vFlowElectrolyzer(1)  = 0
 cPowFlowElectrolyzer2#64:       vPowElectrolyzer(2)
                                 - 0.0555333333333333 vFlowElectrolyzer(2)  = 0
 cPowFlowElectrolyzer3#65:       vPowElectrolyzer(3)
                                 - 0.0555333333333333 vFlowElectrolyzer(3)  = 0
 cPowFlowElectrolyzer4#66:       vPowElectrolyzer(4)
                                 - 0.0555333333333333 vFlowElectrolyzer(4)  = 0
 cPowFlowElectrolyzer5#67:       vPowElectrolyzer(5)
                                 - 0.0555333333333333 vFlowElectrolyzer(5)  = 0
 cPowFlowElectrolyzer6#68:       vPowElectrolyzer(6)
                                 - 0.0555333333333333 vFlowElectrolyzer(6)  = 0
 cPowFlowElectrolyzer7#69:       vPowElectrolyzer(7)
                                 - 0.0555333333333333 vFlowElectrolyzer(7)  = 0
 cPowFlowElectrolyzer8#70:       vPowElectrolyzer(8)
                                 - 0.0555333333333333 vFlowElectrolyzer(8)  = 0
 cPowFlowElectrolyzer9#71:       vPowElectrolyzer(9)
                                 - 0.0555333333333333 vFlowElectrolyzer(9)  = 0
 cUsePowElectrolyzer0#72:        - vMaxPowerElectrolyzer
                                 + vUsablePowElectrolyzer  = 0
 cMaxTotalPowElectrolyzer0#73:   vPowElectrolyzer(0) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer1#74:   vPowElectrolyzer(1) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer2#75:   vPowElectrolyzer(2) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer3#76:   vPowElectrolyzer(3) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer4#77:   vPowElectrolyzer(4) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer5#78:   vPowElectrolyzer(5) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer6#79:   vPowElectrolyzer(6) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer7#80:   vPowElectrolyzer(7) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer8#81:   vPowElectrolyzer(8) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer9#82:   vPowElectrolyzer(9) - vUsablePowElectrolyzer
                                 <= 0
 cPowMaxElectrolyzer0#83:        - 400 vStateElectrolyzer(0)
                                 + vPowElectrolyzer(0) <= 0
 cDefPminElectrolyzer0#84:       - vPminElectrolyzer(0)  = 0
 cPminElectrolyzer0#85:          vPowElectrolyzer(0) - vPminElectrolyzer(0)
                                 >= 0
 cPowMaxElectrolyzer1#86:        - 400 vStateElectrolyzer(1)
                                 + vPowElectrolyzer(1) <= 0
 cDefPminElectrolyzer1#87:       - vPminElectrolyzer(1)  = 0
 cPminElectrolyzer1#88:          vPowElectrolyzer(1) - vPminElectrolyzer(1)
                                 >= 0
 cPowMaxElectrolyzer2#89:        - 400 vStateElectrolyzer(2)
                                 + vPowElectrolyzer(2) <= 0
 cDefPminElectrolyzer2#90:       - vPminElectrolyzer(2)  = 0
 cPminElectrolyzer2#91:          vPowElectrolyzer(2) - vPminElectrolyzer(2)
                                 >= 0
 cPowMaxElectrolyzer3#92:        - 400 vStateElectrolyzer(3)
                                 + vPowElectrolyzer(3) <= 0
 cDefPminElectrolyzer3#93:       - vPminElectrolyzer(3)  = 0
 cPminElectrolyzer3#94:          vPowElectrolyzer(3) - vPminElectrolyzer(3)
                                 >= 0
 cPowMaxElectrolyzer4#95:        - 400 vStateElectrolyzer(4)
                                 + vPowElectrolyzer(4) <= 0
 cDefPminElectrolyzer4#96:       - vPminElectrolyzer(4)  = 0
 cPminElectrolyzer4#97:          vPowElectrolyzer(4) - vPminElectrolyzer(4)
                                 >= 0
 cPowMaxElectrolyzer5#98:        - 400 vStateElectrolyzer(5)
                                 + vPowElectrolyzer(5) <= 0
 cDefPminElectrolyzer5#99:       - vPminElectrolyzer(5)  = 0
 cPminElectrolyzer5#100:         vPowElectrolyzer(5) - vPminElectrolyzer(5)
                                 >= 0
 cPowMaxElectrolyzer6#101:       - 400 vStateElectrolyzer(6)
                                 + vPowElectrolyzer(6) <= 0
 cDefPminElectrolyzer6#102:      - vPminElectrolyzer(6)  = 0
 cPminElectrolyzer6#103:         vPowElectrolyzer(6) - vPminElectrolyzer(6)
                                 >= 0
 cPowMaxElectrolyzer7#104:       - 400 vStateElectrolyzer(7)
                                 + vPowElectrolyzer(7) <= 0
 cDefPminElectrolyzer7#105:      - vPminElectrolyzer(7)  = 0
 cPminElectrolyzer7#106:         vPowElectrolyzer(7) - vPminElectrolyzer(7)
                                 >= 0
 cPowMaxElectrolyzer8#107:       - 400 vStateElectrolyzer(8)
                                 + vPowElectrolyzer(8) <= 0
 cDefPminElectrolyzer8#108:      - vPminElectrolyzer(8)  = 0
 cPminElectrolyzer8#109:         vPowElectrolyzer(8) - vPminElectrolyzer(8)
                                 >= 0
 cPowMaxElectrolyzer9#110:       - 400 vStateElectrolyzer(9)
                                 + vPowElectrolyzer(9) <= 0
 cDefPminElectrolyzer9#111:      - vPminElectrolyzer(9)  = 0
 cPminElectrolyzer9#112:         vPowElectrolyzer(9) - vPminElectrolyzer(9)
                                 >= 0
 cConverterUseElectrolyzer0#113: - vMaxPowerElectrolyzer + vPowElectrolyzer(0)
                                 <= 0
 cConverterUseElectrolyzer1#114: - vMaxPowerElectrolyzer + vPowElectrolyzer(1)
                                 <= 0
 cConverterUseElectrolyzer2#115: - vMaxPowerElectrolyzer + vPowElectrolyzer(2)
                                 <= 0
 cConverterUseElectrolyzer3#116: - vMaxPowerElectrolyzer + vPowElectrolyzer(3)
                                 <= 0
 cConverterUseElectrolyzer4#117: - vMaxPowerElectrolyzer + vPowElectrolyzer(4)
                                 <= 0
 cConverterUseElectrolyzer5#118: - vMaxPowerElectrolyzer + vPowElectrolyzer(5)
                                 <= 0
 cConverterUseElectrolyzer6#119: - vMaxPowerElectrolyzer + vPowElectrolyzer(6)
                                 <= 0
 cConverterUseElectrolyzer7#120: - vMaxPowerElectrolyzer + vPowElectrolyzer(7)
                                 <= 0
 cConverterUseElectrolyzer8#121: - vMaxPowerElectrolyzer + vPowElectrolyzer(8)
                                 <= 0
 cConverterUseElectrolyzer9#122: - vMaxPowerElectrolyzer + vPowElectrolyzer(9)
                                 <= 0
 csInstalledH2_Grid0#123:        vInstalledH2_Grid  = 1
 csMaxValH2_Grid0#124:           vMaxFlowH2_Grid  = 10000
 csBigMInstalledH2_Grid0#125:    - 10000 vInstalledH2_Grid + vMaxFlowH2_Grid
                                 <= 0
 csMinSizeInstalledH2_Grid0#126: vMaxFlowH2_Grid >= 0
 cNotRunningIfNotInstalledH2_Grid0#127: - vInstalledH2_Grid + vStateH2_Grid(0)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid1#128: - vInstalledH2_Grid + vStateH2_Grid(1)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid2#129: - vInstalledH2_Grid + vStateH2_Grid(2)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid3#130: - vInstalledH2_Grid + vStateH2_Grid(3)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid4#131: - vInstalledH2_Grid + vStateH2_Grid(4)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid5#132: - vInstalledH2_Grid + vStateH2_Grid(5)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid6#133: - vInstalledH2_Grid + vStateH2_Grid(6)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid7#134: - vInstalledH2_Grid + vStateH2_Grid(7)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid8#135: - vInstalledH2_Grid + vStateH2_Grid(8)
                                 <= 0
 cNotRunningIfNotInstalledH2_Grid9#136: - vInstalledH2_Grid + vStateH2_Grid(9)
                                 <= 0
 cStateGridH2_Grid0#137:         - 10000 vStateH2_Grid(0) + vGFxH2_Grid(0) <= 0
 cStateGridH2_Grid1#138:         - 10000 vStateH2_Grid(1) + vGFxH2_Grid(1) <= 0
 cStateGridH2_Grid2#139:         - 10000 vStateH2_Grid(2) + vGFxH2_Grid(2) <= 0
 cStateGridH2_Grid3#140:         - 10000 vStateH2_Grid(3) + vGFxH2_Grid(3) <= 0
 cStateGridH2_Grid4#141:         - 10000 vStateH2_Grid(4) + vGFxH2_Grid(4) <= 0
 cStateGridH2_Grid5#142:         - 10000 vStateH2_Grid(5) + vGFxH2_Grid(5) <= 0
 cStateGridH2_Grid6#143:         - 10000 vStateH2_Grid(6) + vGFxH2_Grid(6) <= 0
 cStateGridH2_Grid7#144:         - 10000 vStateH2_Grid(7) + vGFxH2_Grid(7) <= 0
 cStateGridH2_Grid8#145:         - 10000 vStateH2_Grid(8) + vGFxH2_Grid(8) <= 0
 cStateGridH2_Grid9#146:         - 10000 vStateH2_Grid(9) + vGFxH2_Grid(9) <= 0
 cMaxGFxH2_Grid0#147:            - vMaxFlowH2_Grid + vGFxH2_Grid(0) <= 0
 cMaxGFxH2_Grid1#148:            - vMaxFlowH2_Grid + vGFxH2_Grid(1) <= 0
 cMaxGFxH2_Grid2#149:            - vMaxFlowH2_Grid + vGFxH2_Grid(2) <= 0
 cMaxGFxH2_Grid3#150:            - vMaxFlowH2_Grid + vGFxH2_Grid(3) <= 0
 cMaxGFxH2_Grid4#151:            - vMaxFlowH2_Grid + vGFxH2_Grid(4) <= 0
 cMaxGFxH2_Grid5#152:            - vMaxFlowH2_Grid + vGFxH2_Grid(5) <= 0
 cMaxGFxH2_Grid6#153:            - vMaxFlowH2_Grid + vGFxH2_Grid(6) <= 0
 cMaxGFxH2_Grid7#154:            - vMaxFlowH2_Grid + vGFxH2_Grid(7) <= 0
 cMaxGFxH2_Grid8#155:            - vMaxFlowH2_Grid + vGFxH2_Grid(8) <= 0
 cMaxGFxH2_Grid9#156:            - vMaxFlowH2_Grid + vGFxH2_Grid(9) <= 0
 csMaxValRamp0#157:              vComponentSizeRamp  = 400
 csBigMInstalledRamp0#158:       vComponentSizeRamp <= 400
 csMinSizeInstalledRamp0#159:    vComponentSizeRamp >= 0
 cRelRampUpRamp0#160:            - 0.166666666666667 vComponentSizeRamp
                                 - vRampInputRamp(0) + vRampInputRamp(1) <= 0
 cRelRampDownRamp0#161:          - 0.166666666666667 vComponentSizeRamp
                                 + vRampInputRamp(0) - vRampInputRamp(1) <= 0
 cRelRampUpRamp0#162:            - 0.166666666666667 vComponentSizeRamp
                                 - vRampInputRamp(1) + vRampInputRamp(2) <= 0
 cRelRampDownRamp0#163:          - 0.166666666666667 vComponentSizeRamp
                                 + vRampInputRamp(1) - vRampInputRamp(2) <= 0
 cRelRampUpRamp0#164:            - 0.166666666666667 vComponentSizeRamp
                                 - vRampInputRamp(2) + vRampInputRamp(3) <= 0
 cRelRampDownRamp0#165:          - 0.166666666666667 vComponentSizeRamp
                                 + vRampInputRamp(2) - vRampInputRamp(3) <= 0
 cRelRampUpRamp0#166:            - 0.166666666666667 vComponentSizeRamp
                                 - vRampInputRamp(3) + vRampInputRamp(4) <= 0
 cRelRampDownRamp0#167:          - 0.166666666666667 vComponentSizeRamp
                                 + vRampInputRamp(3) - vRampInputRamp(4) <= 0
 cRelRampUpRamp0#168:            - 0.166666666666667 vComponentSizeRamp
                                 - vRampInputRamp(4) + vRampInputRamp(5) <= 0
 cRelRampDownRamp0#169:          - 0.166666666666667 vComponentSizeRamp
                                 + vRampInputRamp(4) - vRampInputRamp(5) <= 0
 cRelRampUpRamp0#170:            - 0.0416666666666667 vComponentSizeRamp
                                 - vRampInputRamp(5) + vRampInputRamp(6) <= 0
 cRelRampDownRamp0#171:          - 0.0833333333333333 vComponentSizeRamp
                                 + vRampInputRamp(5) - vRampInputRamp(6) <= 0
 cRelRampUpRamp0#172:            - 0.0416666666666667 vComponentSizeRamp
                                 - vRampInputRamp(6) + vRampInputRamp(7) <= 0
 cRelRampDownRamp0#173:          - 0.0833333333333333 vComponentSizeRamp
                                 + vRampInputRamp(6) - vRampInputRamp(7) <= 0
 cRelRampUpRamp0#174:            - 0.0416666666666667 vComponentSizeRamp
                                 - vRampInputRamp(7) + vRampInputRamp(8) <= 0
 cRelRampDownRamp0#175:          - 0.0833333333333333 vComponentSizeRamp
                                 + vRampInputRamp(7) - vRampInputRamp(8) <= 0
 cRelRampUpRamp0#176:            - 0.0416666666666667 vComponentSizeRamp
                                 - vRampInputRamp(8) + vRampInputRamp(9) <= 0
 cRelRampDownRamp0#177:          - 0.0833333333333333 vComponentSizeRamp
                                 + vRampInputRamp(8) - vRampInputRamp(9) <= 0
 cENodeEquality#10#178:          - vPowElectrolyzer(0)
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#179:          - vPowElectrolyzer(1)
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#180:          - vPowElectrolyzer(2)
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#181:          - vPowElectrolyzer(3)
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#182:          - vPowElectrolyzer(4)
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#183:          - vPowElectrolyzer(5)
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#184:          - vPowElectrolyzer(6)
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#185:          - vPowElectrolyzer(7)
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#186:          - vPowElectrolyzer(8)
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#187:          - vPowElectrolyzer(9)
                                 + vBusValueNodeEquality#1(9)  = 0
 cENodeEquality#10#188:          - vRampInputRamp(0)
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#189:          - vRampInputRamp(1)
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#190:          - vRampInputRamp(2)
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#191:          - vRampInputRamp(3)
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#192:          - vRampInputRamp(4)
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#193:          - vRampInputRamp(5)
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#194:          - vRampInputRamp(6)
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#195:          - vRampInputRamp(7)
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#196:          - vRampInputRamp(8)
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#197:          - vRampInputRamp(9)
                                 + vBusValueNodeEquality#1(9)  = 0
 cENodeEquality#20#198:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(0)  = 0
 cENodeEquality#21#199:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(1)  = 0
 cENodeEquality#22#200:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(2)  = 0
 cENodeEquality#23#201:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(3)  = 0
 cENodeEquality#24#202:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(4)  = 0
 cENodeEquality#25#203:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(5)  = 0
 cENodeEquality#26#204:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(6)  = 0
 cENodeEquality#27#205:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(7)  = 0
 cENodeEquality#28#206:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(8)  = 0
 cENodeEquality#29#207:          - vMaxPowerElectrolyzer
                                 + vBusValueNodeEquality#2(9)  = 0
 cENodeEquality#20#208:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(0)  = 0
 cENodeEquality#21#209:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(1)  = 0
 cENodeEquality#22#210:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(2)  = 0
 cENodeEquality#23#211:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(3)  = 0
 cENodeEquality#24#212:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(4)  = 0
 cENodeEquality#25#213:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(5)  = 0
 cENodeEquality#26#214:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(6)  = 0
 cENodeEquality#27#215:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(7)  = 0
 cENodeEquality#28#216:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(8)  = 0
 cENodeEquality#29#217:          - vComponentSizeRamp
                                 + vBusValueNodeEquality#2(9)  = 0
 cSNodeLaw#10#218:               - vGFxCurtailment(0) + 200 vWeightElec_source
                                 - vPowElectrolyzer(0)  = 0
 cSNodeLaw#11#219:               - vGFxCurtailment(1) + 300 vWeightElec_source
                                 - vPowElectrolyzer(1)  = 0
 cSNodeLaw#12#220:               - vGFxCurtailment(2) + 300 vWeightElec_source
                                 - vPowElectrolyzer(2)  = 0
 cSNodeLaw#13#221:               - vGFxCurtailment(3) + 300 vWeightElec_source
                                 - vPowElectrolyzer(3)  = 0
 cSNodeLaw#14#222:               - vGFxCurtailment(4) + 300 vWeightElec_source
                                 - vPowElectrolyzer(4)  = 0
 cSNodeLaw#15#223:               - vGFxCurtailment(5) + 300 vWeightElec_source
                                 - vPowElectrolyzer(5)  = 0
 cSNodeLaw#16#224:               - vGFxCurtailment(6) + 200 vWeightElec_source
                                 - vPowElectrolyzer(6)  = 0
 cSNodeLaw#17#225:               - vGFxCurtailment(7) + 200 vWeightElec_source
                                 - vPowElectrolyzer(7)  = 0
 cSNodeLaw#18#226:               - vGFxCurtailment(8) + 150 vWeightElec_source
                                 - vPowElectrolyzer(8)  = 0
 cSNodeLaw#19#227:               - vGFxCurtailment(9) + 200 vWeightElec_source
                                 - vPowElectrolyzer(9)  = 0
 cSNodeLaw#20#228:               vFlowElectrolyzer(0) - vGFxH2_Grid(0)  = 0
 cSNodeLaw#21#229:               vFlowElectrolyzer(1) - vGFxH2_Grid(1)  = 0
 cSNodeLaw#22#230:               vFlowElectrolyzer(2) - vGFxH2_Grid(2)  = 0
 cSNodeLaw#23#231:               vFlowElectrolyzer(3) - vGFxH2_Grid(3)  = 0
 cSNodeLaw#24#232:               vFlowElectrolyzer(4) - vGFxH2_Grid(4)  = 0
 cSNodeLaw#25#233:               vFlowElectrolyzer(5) - vGFxH2_Grid(5)  = 0
 cSNodeLaw#26#234:               vFlowElectrolyzer(6) - vGFxH2_Grid(6)  = 0
 cSNodeLaw#27#235:               vFlowElectrolyzer(7) - vGFxH2_Grid(7)  = 0
 cSNodeLaw#28#236:               vFlowElectrolyzer(8) - vGFxH2_Grid(8)  = 0
 cSNodeLaw#29#237:               vFlowElectrolyzer(9) - vGFxH2_Grid(9)  = 0
Bounds
 0 <= vInstalledCurtailment <= 1
 0 <= vMaxFlowCurtailment <= 10000
      vStateCurtailment(0) = 1
      vStateCurtailment(1) = 1
      vStateCurtailment(2) = 1
      vStateCurtailment(3) = 1
      vStateCurtailment(4) = 1
      vStateCurtailment(5) = 1
      vStateCurtailment(6) = 1
      vStateCurtailment(7) = 1
      vStateCurtailment(8) = 1
      vStateCurtailment(9) = 1
 0 <= vGFxCurtailment(0) <= 10000
 0 <= vGFxCurtailment(1) <= 10000
 0 <= vGFxCurtailment(2) <= 10000
 0 <= vGFxCurtailment(3) <= 10000
 0 <= vGFxCurtailment(4) <= 10000
 0 <= vGFxCurtailment(5) <= 10000
 0 <= vGFxCurtailment(6) <= 10000
 0 <= vGFxCurtailment(7) <= 10000
 0 <= vGFxCurtailment(8) <= 10000
 0 <= vGFxCurtailment(9) <= 10000
 0 <= vInstalledElec_source <= 1
 0 <= vWeightElec_source <= 1
 0 <= vInstalledElectrolyzer <= 1
 0 <= vMaxPowerElectrolyzer <= 400
 0 <= vStateElectrolyzer(0) <= 1
 0 <= vStateElectrolyzer(1) <= 1
 0 <= vStateElectrolyzer(2) <= 1
 0 <= vStateElectrolyzer(3) <= 1
 0 <= vStateElectrolyzer(4) <= 1
 0 <= vStateElectrolyzer(5) <= 1
 0 <= vStateElectrolyzer(6) <= 1
 0 <= vStateElectrolyzer(7) <= 1
 0 <= vStateElectrolyzer(8) <= 1
 0 <= vStateElectrolyzer(9) <= 1
 0 <= vPowElectrolyzer(0) <= 400
 0 <= vPowElectrolyzer(1) <= 400
 0 <= vPowElectrolyzer(2) <= 400
 0 <= vPowElectrolyzer(3) <= 400
 0 <= vPowElectrolyzer(4) <= 400
 0 <= vPowElectrolyzer(5) <= 400
 0 <= vPowElectrolyzer(6) <= 400
 0 <= vPowElectrolyzer(7) <= 400
 0 <= vPowElectrolyzer(8) <= 400
 0 <= vPowElectrolyzer(9) <= 400
 0 <= vFlowElectrolyzer(0) <= 7202.88115246098
 0 <= vFlowElectrolyzer(1) <= 7202.88115246098
 0 <= vFlowElectrolyzer(2) <= 7202.88115246098
 0 <= vFlowElectrolyzer(3) <= 7202.88115246098
 0 <= vFlowElectrolyzer(4) <= 7202.88115246098
 0 <= vFlowElectrolyzer(5) <= 7202.88115246098
 0 <= vFlowElectrolyzer(6) <= 7202.88115246098
 0 <= vFlowElectrolyzer(7) <= 7202.88115246098
 0 <= vFlowElectrolyzer(8) <= 7202.88115246098
 0 <= vFlowElectrolyzer(9) <= 7202.88115246098
 0 <= vUsablePowElectrolyzer <= 400
      vPminElectrolyzer(0) Free
      vPminElectrolyzer(1) Free
      vPminElectrolyzer(2) Free
      vPminElectrolyzer(3) Free
      vPminElectrolyzer(4) Free
      vPminElectrolyzer(5) Free
      vPminElectrolyzer(6) Free
      vPminElectrolyzer(7) Free
      vPminElectrolyzer(8) Free
      vPminElectrolyzer(9) Free
 0 <= vInstalledH2_Grid <= 1
 0 <= vMaxFlowH2_Grid <= 10000
      vStateH2_Grid(0) = 1
      vStateH2_Grid(1) = 1
      vStateH2_Grid(2) = 1
      vStateH2_Grid(3) = 1
      vStateH2_Grid(4) = 1
      vStateH2_Grid(5) = 1
      vStateH2_Grid(6) = 1
      vStateH2_Grid(7) = 1
      vStateH2_Grid(8) = 1
      vStateH2_Grid(9) = 1
 0 <= vGFxH2_Grid(0) <= 10000
 0 <= vGFxH2_Grid(1) <= 10000
 0 <= vGFxH2_Grid(2) <= 10000
 0 <= vGFxH2_Grid(3) <= 10000
 0 <= vGFxH2_Grid(4) <= 10000
 0 <= vGFxH2_Grid(5) <= 10000
 0 <= vGFxH2_Grid(6) <= 10000
 0 <= vGFxH2_Grid(7) <= 10000
 0 <= vGFxH2_Grid(8) <= 10000
 0 <= vGFxH2_Grid(9) <= 10000
 0 <= vComponentSizeRamp <= 400
      vRampInputRamp(0) Free
      vRampInputRamp(1) Free
      vRampInputRamp(2) Free
      vRampInputRamp(3) Free
      vRampInputRamp(4) Free
      vRampInputRamp(5) Free
      vRampInputRamp(6) Free
      vRampInputRamp(7) Free
      vRampInputRamp(8) Free
      vRampInputRamp(9) Free
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
 0 <= vBusValueNodeEquality#2(0) <= 1e+15
 0 <= vBusValueNodeEquality#2(1) <= 1e+15
 0 <= vBusValueNodeEquality#2(2) <= 1e+15
 0 <= vBusValueNodeEquality#2(3) <= 1e+15
 0 <= vBusValueNodeEquality#2(4) <= 1e+15
 0 <= vBusValueNodeEquality#2(5) <= 1e+15
 0 <= vBusValueNodeEquality#2(6) <= 1e+15
 0 <= vBusValueNodeEquality#2(7) <= 1e+15
 0 <= vBusValueNodeEquality#2(8) <= 1e+15
 0 <= vBusValueNodeEquality#2(9) <= 1e+15
Generals
 vInstalledCurtailment  vInstalledElec_source  vInstalledElectrolyzer 
 vStateElectrolyzer(0)  vStateElectrolyzer(1)  vStateElectrolyzer(2) 
 vStateElectrolyzer(3)  vStateElectrolyzer(4)  vStateElectrolyzer(5) 
 vStateElectrolyzer(6)  vStateElectrolyzer(7)  vStateElectrolyzer(8) 
 vStateElectrolyzer(9)  vInstalledH2_Grid 
End
