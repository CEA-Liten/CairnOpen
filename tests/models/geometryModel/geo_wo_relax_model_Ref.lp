\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 1000 Discretized_SizeMax(1)#56 + 1400 Discretized_SizeMax(2)#57
       + 14000 Discretized_SizeMax(3)#58
       + 600168.077786779 vGFxGridFree#1(0)#71
       + 600168.077786779 vGFxGridFree#1(1)#72
       + 600168.077786779 vGFxGridFree#1(2)#73
       + 600168.077786779 vGFxGridFree#1(3)#74
       + 600168.077786779 vGFxGridFree#1(4)#75
       + 600168.077786779 vGFxGridFree#1(5)#76
       + 717937.657577501 vGFxGridFree#1(6)#77
       + 717937.657577501 vGFxGridFree#1(7)#78
       + 717937.657577501 vGFxGridFree#1(8)#79
       + 717937.657577501 vGFxGridFree#1(9)#80
       + 11133.5595242702 vMaxEstoStorageGen#1#106
       + 21335.5952427023 vWeightWind#149
Subject To
 csBigMInstalledElectrolyzer#10#0: - 1000 vInstalledElectrolyzer#1#0
                                 + vMaxPowerElectrolyzer#1#1 <= 0
 csMinSizeInstalledElectrolyzer#10#1: vMaxPowerElectrolyzer#1#1 >= 0
 cNotRunningIfNotInstalledElectrolyzer#10#2: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(0)#2 <= 0
 cNotRunningIfNotInstalledElectrolyzer#11#3: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(1)#3 <= 0
 cNotRunningIfNotInstalledElectrolyzer#12#4: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(2)#4 <= 0
 cNotRunningIfNotInstalledElectrolyzer#13#5: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(3)#5 <= 0
 cNotRunningIfNotInstalledElectrolyzer#14#6: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(4)#6 <= 0
 cNotRunningIfNotInstalledElectrolyzer#15#7: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(5)#7 <= 0
 cNotRunningIfNotInstalledElectrolyzer#16#8: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(6)#8 <= 0
 cNotRunningIfNotInstalledElectrolyzer#17#9: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(7)#9 <= 0
 cNotRunningIfNotInstalledElectrolyzer#18#10: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(8)#10 <= 0
 cNotRunningIfNotInstalledElectrolyzer#19#11: - vInstalledElectrolyzer#1#0
                                 + vStateElectrolyzer#1(9)#11 <= 0
 cPowFlowElectrolyzer#10#12:     vPowElectrolyzer#1(0)#12
                                 - 0.0555333333333333 vFlowElectrolyzer#1(0)#22
                                  = 0
 cPowFlowElectrolyzer#11#13:     vPowElectrolyzer#1(1)#13
                                 - 0.0555333333333333 vFlowElectrolyzer#1(1)#23
                                  = 0
 cPowFlowElectrolyzer#12#14:     vPowElectrolyzer#1(2)#14
                                 - 0.0555333333333333 vFlowElectrolyzer#1(2)#24
                                  = 0
 cPowFlowElectrolyzer#13#15:     vPowElectrolyzer#1(3)#15
                                 - 0.0555333333333333 vFlowElectrolyzer#1(3)#25
                                  = 0
 cPowFlowElectrolyzer#14#16:     vPowElectrolyzer#1(4)#16
                                 - 0.0555333333333333 vFlowElectrolyzer#1(4)#26
                                  = 0
 cPowFlowElectrolyzer#15#17:     vPowElectrolyzer#1(5)#17
                                 - 0.0555333333333333 vFlowElectrolyzer#1(5)#27
                                  = 0
 cPowFlowElectrolyzer#16#18:     vPowElectrolyzer#1(6)#18
                                 - 0.0555333333333333 vFlowElectrolyzer#1(6)#28
                                  = 0
 cPowFlowElectrolyzer#17#19:     vPowElectrolyzer#1(7)#19
                                 - 0.0555333333333333 vFlowElectrolyzer#1(7)#29
                                  = 0
 cPowFlowElectrolyzer#18#20:     vPowElectrolyzer#1(8)#20
                                 - 0.0555333333333333 vFlowElectrolyzer#1(8)#30
                                  = 0
 cPowFlowElectrolyzer#19#21:     vPowElectrolyzer#1(9)#21
                                 - 0.0555333333333333 vFlowElectrolyzer#1(9)#31
                                  = 0
 cUsePowElectrolyzer#10#22:      - vMaxPowerElectrolyzer#1#1
                                 + vUsablePowElectrolyzer#1#32  = 0
 cMaxTotalPowElectrolyzer#10#23: vPowElectrolyzer#1(0)#12
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#11#24: vPowElectrolyzer#1(1)#13
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#12#25: vPowElectrolyzer#1(2)#14
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#13#26: vPowElectrolyzer#1(3)#15
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#14#27: vPowElectrolyzer#1(4)#16
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#15#28: vPowElectrolyzer#1(5)#17
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#16#29: vPowElectrolyzer#1(6)#18
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#17#30: vPowElectrolyzer#1(7)#19
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#18#31: vPowElectrolyzer#1(8)#20
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cMaxTotalPowElectrolyzer#19#32: vPowElectrolyzer#1(9)#21
                                 - vUsablePowElectrolyzer#1#32 <= 0
 cPowMaxElectrolyzer#10#33:      - 1000 vStateElectrolyzer#1(0)#2
                                 + vPowElectrolyzer#1(0)#12 <= 0
 cDefPminElectrolyzer#10#34:     - vPminElectrolyzer#1(0)#33  = 0
 cPminElectrolyzer#10#35:        vPowElectrolyzer#1(0)#12
                                 - vPminElectrolyzer#1(0)#33 >= 0
 cPowMaxElectrolyzer#11#36:      - 1000 vStateElectrolyzer#1(1)#3
                                 + vPowElectrolyzer#1(1)#13 <= 0
 cDefPminElectrolyzer#11#37:     - vPminElectrolyzer#1(1)#34  = 0
 cPminElectrolyzer#11#38:        vPowElectrolyzer#1(1)#13
                                 - vPminElectrolyzer#1(1)#34 >= 0
 cPowMaxElectrolyzer#12#39:      - 1000 vStateElectrolyzer#1(2)#4
                                 + vPowElectrolyzer#1(2)#14 <= 0
 cDefPminElectrolyzer#12#40:     - vPminElectrolyzer#1(2)#35  = 0
 cPminElectrolyzer#12#41:        vPowElectrolyzer#1(2)#14
                                 - vPminElectrolyzer#1(2)#35 >= 0
 cPowMaxElectrolyzer#13#42:      - 1000 vStateElectrolyzer#1(3)#5
                                 + vPowElectrolyzer#1(3)#15 <= 0
 cDefPminElectrolyzer#13#43:     - vPminElectrolyzer#1(3)#36  = 0
 cPminElectrolyzer#13#44:        vPowElectrolyzer#1(3)#15
                                 - vPminElectrolyzer#1(3)#36 >= 0
 cPowMaxElectrolyzer#14#45:      - 1000 vStateElectrolyzer#1(4)#6
                                 + vPowElectrolyzer#1(4)#16 <= 0
 cDefPminElectrolyzer#14#46:     - vPminElectrolyzer#1(4)#37  = 0
 cPminElectrolyzer#14#47:        vPowElectrolyzer#1(4)#16
                                 - vPminElectrolyzer#1(4)#37 >= 0
 cPowMaxElectrolyzer#15#48:      - 1000 vStateElectrolyzer#1(5)#7
                                 + vPowElectrolyzer#1(5)#17 <= 0
 cDefPminElectrolyzer#15#49:     - vPminElectrolyzer#1(5)#38  = 0
 cPminElectrolyzer#15#50:        vPowElectrolyzer#1(5)#17
                                 - vPminElectrolyzer#1(5)#38 >= 0
 cPowMaxElectrolyzer#16#51:      - 1000 vStateElectrolyzer#1(6)#8
                                 + vPowElectrolyzer#1(6)#18 <= 0
 cDefPminElectrolyzer#16#52:     - vPminElectrolyzer#1(6)#39  = 0
 cPminElectrolyzer#16#53:        vPowElectrolyzer#1(6)#18
                                 - vPminElectrolyzer#1(6)#39 >= 0
 cPowMaxElectrolyzer#17#54:      - 1000 vStateElectrolyzer#1(7)#9
                                 + vPowElectrolyzer#1(7)#19 <= 0
 cDefPminElectrolyzer#17#55:     - vPminElectrolyzer#1(7)#40  = 0
 cPminElectrolyzer#17#56:        vPowElectrolyzer#1(7)#19
                                 - vPminElectrolyzer#1(7)#40 >= 0
 cPowMaxElectrolyzer#18#57:      - 1000 vStateElectrolyzer#1(8)#10
                                 + vPowElectrolyzer#1(8)#20 <= 0
 cDefPminElectrolyzer#18#58:     - vPminElectrolyzer#1(8)#41  = 0
 cPminElectrolyzer#18#59:        vPowElectrolyzer#1(8)#20
                                 - vPminElectrolyzer#1(8)#41 >= 0
 cPowMaxElectrolyzer#19#60:      - 1000 vStateElectrolyzer#1(9)#11
                                 + vPowElectrolyzer#1(9)#21 <= 0
 cDefPminElectrolyzer#19#61:     - vPminElectrolyzer#1(9)#42  = 0
 cPminElectrolyzer#19#62:        vPowElectrolyzer#1(9)#21
                                 - vPminElectrolyzer#1(9)#42 >= 0
 cConverterUseElectrolyzer#10#63: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(0)#12 <= 0
 cConverterUseElectrolyzer#11#64: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(1)#13 <= 0
 cConverterUseElectrolyzer#12#65: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(2)#14 <= 0
 cConverterUseElectrolyzer#13#66: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(3)#15 <= 0
 cConverterUseElectrolyzer#14#67: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(4)#16 <= 0
 cConverterUseElectrolyzer#15#68: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(5)#17 <= 0
 cConverterUseElectrolyzer#16#69: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(6)#18 <= 0
 cConverterUseElectrolyzer#17#70: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(7)#19 <= 0
 cConverterUseElectrolyzer#18#71: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(8)#20 <= 0
 cConverterUseElectrolyzer#19#72: - vMaxPowerElectrolyzer#1#1
                                 + vPowElectrolyzer#1(9)#21 <= 0
 C_SizeMax#73:                   vMaxPowerElectrolyzer#1#1
                                 - 0.0001 Discretized_SizeMax(1)#44
                                 - Discretized_SizeMax(2)#45
                                 - 10 Discretized_SizeMax(3)#46  = 0
 c75#74:                         Discretized_SizeMax(0)#43
                                 + Discretized_SizeMax(1)#44
                                 + Discretized_SizeMax(2)#45
                                 + Discretized_SizeMax(3)#46  = 1
 C_SizeMax#75:                   vMaxPowerElectrolyzer#1#1
                                 - 0.0001 Discretized_SizeMax(1)#48
                                 - Discretized_SizeMax(2)#49
                                 - 10 Discretized_SizeMax(3)#50  = 0
 c77#76:                         Discretized_SizeMax(0)#47
                                 + Discretized_SizeMax(1)#48
                                 + Discretized_SizeMax(2)#49
                                 + Discretized_SizeMax(3)#50  = 1
 C_SizeMax#77:                   vMaxPowerElectrolyzer#1#1
                                 - 0.0001 Discretized_SizeMax(1)#52
                                 - Discretized_SizeMax(2)#53
                                 - 10 Discretized_SizeMax(3)#54  = 0
 c79#78:                         Discretized_SizeMax(0)#51
                                 + Discretized_SizeMax(1)#52
                                 + Discretized_SizeMax(2)#53
                                 + Discretized_SizeMax(3)#54  = 1
 C_SizeMax#79:                   vMaxPowerElectrolyzer#1#1
                                 - 0.0001 Discretized_SizeMax(1)#56
                                 - Discretized_SizeMax(2)#57
                                 - 10 Discretized_SizeMax(3)#58  = 0
 c81#80:                         Discretized_SizeMax(0)#55
                                 + Discretized_SizeMax(1)#56
                                 + Discretized_SizeMax(2)#57
                                 + Discretized_SizeMax(3)#58  = 1
 csInstalledGridFree#10#81:      vInstalledGridFree#1#59  = 1
 csMaxValGridFree#10#82:         vMaxFlowGridFree#1#60  = 100000000
 csBigMInstalledGridFree#10#83:  - 100000000 vInstalledGridFree#1#59
                                 + vMaxFlowGridFree#1#60 <= 0
 csMinSizeInstalledGridFree#10#84: vMaxFlowGridFree#1#60 >= 0
 cNotRunningIfNotInstalledGridFree#10#85: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(0)#61 <= 0
 cNotRunningIfNotInstalledGridFree#11#86: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(1)#62 <= 0
 cNotRunningIfNotInstalledGridFree#12#87: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(2)#63 <= 0
 cNotRunningIfNotInstalledGridFree#13#88: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(3)#64 <= 0
 cNotRunningIfNotInstalledGridFree#14#89: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(4)#65 <= 0
 cNotRunningIfNotInstalledGridFree#15#90: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(5)#66 <= 0
 cNotRunningIfNotInstalledGridFree#16#91: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(6)#67 <= 0
 cNotRunningIfNotInstalledGridFree#17#92: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(7)#68 <= 0
 cNotRunningIfNotInstalledGridFree#18#93: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(8)#69 <= 0
 cNotRunningIfNotInstalledGridFree#19#94: - vInstalledGridFree#1#59
                                 + vStateGridFree#1(9)#70 <= 0
 cStateGridGridFree#10#95:       - 100000000 vStateGridFree#1(0)#61
                                 + vGFxGridFree#1(0)#71 <= 0
 cStateGridGridFree#11#96:       - 100000000 vStateGridFree#1(1)#62
                                 + vGFxGridFree#1(1)#72 <= 0
 cStateGridGridFree#12#97:       - 100000000 vStateGridFree#1(2)#63
                                 + vGFxGridFree#1(2)#73 <= 0
 cStateGridGridFree#13#98:       - 100000000 vStateGridFree#1(3)#64
                                 + vGFxGridFree#1(3)#74 <= 0
 cStateGridGridFree#14#99:       - 100000000 vStateGridFree#1(4)#65
                                 + vGFxGridFree#1(4)#75 <= 0
 cStateGridGridFree#15#100:      - 100000000 vStateGridFree#1(5)#66
                                 + vGFxGridFree#1(5)#76 <= 0
 cStateGridGridFree#16#101:      - 100000000 vStateGridFree#1(6)#67
                                 + vGFxGridFree#1(6)#77 <= 0
 cStateGridGridFree#17#102:      - 100000000 vStateGridFree#1(7)#68
                                 + vGFxGridFree#1(7)#78 <= 0
 cStateGridGridFree#18#103:      - 100000000 vStateGridFree#1(8)#69
                                 + vGFxGridFree#1(8)#79 <= 0
 cStateGridGridFree#19#104:      - 100000000 vStateGridFree#1(9)#70
                                 + vGFxGridFree#1(9)#80 <= 0
 cMaxGFxGridFree#10#105:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(0)#71
                                 <= 0
 cMaxGFxGridFree#11#106:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(1)#72
                                 <= 0
 cMaxGFxGridFree#12#107:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(2)#73
                                 <= 0
 cMaxGFxGridFree#13#108:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(3)#74
                                 <= 0
 cMaxGFxGridFree#14#109:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(4)#75
                                 <= 0
 cMaxGFxGridFree#15#110:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(5)#76
                                 <= 0
 cMaxGFxGridFree#16#111:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(6)#77
                                 <= 0
 cMaxGFxGridFree#17#112:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(7)#78
                                 <= 0
 cMaxGFxGridFree#18#113:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(8)#79
                                 <= 0
 cMaxGFxGridFree#19#114:         - vMaxFlowGridFree#1#60 + vGFxGridFree#1(9)#80
                                 <= 0
 csInstalledGridFree#20#115:     vInstalledGridFree#2#81  = 1
 csMaxValGridFree#20#116:        vMaxFlowGridFree#2#82  = 10000
 csBigMInstalledGridFree#20#117: - 10000 vInstalledGridFree#2#81
                                 + vMaxFlowGridFree#2#82 <= 0
 csMinSizeInstalledGridFree#20#118: vMaxFlowGridFree#2#82 >= 0
 cNotRunningIfNotInstalledGridFree#20#119: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(0)#83 <= 0
 cNotRunningIfNotInstalledGridFree#21#120: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(1)#84 <= 0
 cNotRunningIfNotInstalledGridFree#22#121: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(2)#85 <= 0
 cNotRunningIfNotInstalledGridFree#23#122: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(3)#86 <= 0
 cNotRunningIfNotInstalledGridFree#24#123: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(4)#87 <= 0
 cNotRunningIfNotInstalledGridFree#25#124: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(5)#88 <= 0
 cNotRunningIfNotInstalledGridFree#26#125: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(6)#89 <= 0
 cNotRunningIfNotInstalledGridFree#27#126: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(7)#90 <= 0
 cNotRunningIfNotInstalledGridFree#28#127: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(8)#91 <= 0
 cNotRunningIfNotInstalledGridFree#29#128: - vInstalledGridFree#2#81
                                 + vStateGridFree#2(9)#92 <= 0
 cStateGridGridFree#20#129:      - 10000 vStateGridFree#2(0)#83
                                 + vGFxGridFree#2(0)#93 <= 0
 cStateGridGridFree#21#130:      - 10000 vStateGridFree#2(1)#84
                                 + vGFxGridFree#2(1)#94 <= 0
 cStateGridGridFree#22#131:      - 10000 vStateGridFree#2(2)#85
                                 + vGFxGridFree#2(2)#95 <= 0
 cStateGridGridFree#23#132:      - 10000 vStateGridFree#2(3)#86
                                 + vGFxGridFree#2(3)#96 <= 0
 cStateGridGridFree#24#133:      - 10000 vStateGridFree#2(4)#87
                                 + vGFxGridFree#2(4)#97 <= 0
 cStateGridGridFree#25#134:      - 10000 vStateGridFree#2(5)#88
                                 + vGFxGridFree#2(5)#98 <= 0
 cStateGridGridFree#26#135:      - 10000 vStateGridFree#2(6)#89
                                 + vGFxGridFree#2(6)#99 <= 0
 cStateGridGridFree#27#136:      - 10000 vStateGridFree#2(7)#90
                                 + vGFxGridFree#2(7)#100 <= 0
 cStateGridGridFree#28#137:      - 10000 vStateGridFree#2(8)#91
                                 + vGFxGridFree#2(8)#101 <= 0
 cStateGridGridFree#29#138:      - 10000 vStateGridFree#2(9)#92
                                 + vGFxGridFree#2(9)#102 <= 0
 cMaxGFxGridFree#20#139:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(0)#93
                                 <= 0
 cMaxGFxGridFree#21#140:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(1)#94
                                 <= 0
 cMaxGFxGridFree#22#141:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(2)#95
                                 <= 0
 cMaxGFxGridFree#23#142:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(3)#96
                                 <= 0
 cMaxGFxGridFree#24#143:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(4)#97
                                 <= 0
 cMaxGFxGridFree#25#144:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(5)#98
                                 <= 0
 cMaxGFxGridFree#26#145:         - vMaxFlowGridFree#2#82 + vGFxGridFree#2(6)#99
                                 <= 0
 cMaxGFxGridFree#27#146:         - vMaxFlowGridFree#2#82
                                 + vGFxGridFree#2(7)#100 <= 0
 cMaxGFxGridFree#28#147:         - vMaxFlowGridFree#2#82
                                 + vGFxGridFree#2(8)#101 <= 0
 cMaxGFxGridFree#29#148:         - vMaxFlowGridFree#2#82
                                 + vGFxGridFree#2(9)#102 <= 0
 csInstalledSourceLoad#10#149:   vInstalledSourceLoad#1#103  = 1
 csWeightSourceLoad#10#150:      vWeightSourceLoad#1#104  = 1
 csBigMInstalledSourceLoad#10#151: - vInstalledSourceLoad#1#103
                                 + vWeightSourceLoad#1#104 <= 0
 csMinSizeInstalledSourceLoad#10#152: vWeightSourceLoad#1#104 >= 0
 cMSFxSourceLoad#10#153:         <= 100000000
 cMSFxSourceLoad#11#154:         <= 100000000
 cMSFxSourceLoad#12#155:         <= 100000000
 cMSFxSourceLoad#13#156:         <= 100000000
 cMSFxSourceLoad#14#157:         <= 100000000
 cMSFxSourceLoad#15#158:         <= 100000000
 cMSFxSourceLoad#16#159:         <= 100000000
 cMSFxSourceLoad#17#160:         <= 100000000
 cMSFxSourceLoad#18#161:         <= 100000000
 cMSFxSourceLoad#19#162:         <= 100000000
 csBigMInstalledStorageGen#10#163: - 100 vInstalledStorageGen#1#105
                                 + vMaxEstoStorageGen#1#106 <= 0
 csMinSizeInstalledStorageGen#10#164: vMaxEstoStorageGen#1#106 >= 0
 cStoIni3StorageGen#10#165:      - 0.5 vMaxEstoStorageGen#1#106
                                 + vStoIniStorageGen#1#137  = 0
 cMStorageGen#10#166:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(0)#107 <= 0
 cStoStorageGen#10#167:          vEStorageGen#1(0)#107 - vQCStorageGen#1(0)#117
                                 + vQDStorageGen#1(0)#127
                                 - vStoIniStorageGen#1#137  = 0
 cMStorageGen#11#168:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(1)#108 <= 0
 cStoStorageGen#11#169:          - vEStorageGen#1(0)#107
                                 + vEStorageGen#1(1)#108
                                 - vQCStorageGen#1(1)#118
                                 + vQDStorageGen#1(1)#128  = 0
 cMStorageGen#12#170:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(2)#109 <= 0
 cStoStorageGen#12#171:          - vEStorageGen#1(1)#108
                                 + vEStorageGen#1(2)#109
                                 - vQCStorageGen#1(2)#119
                                 + vQDStorageGen#1(2)#129  = 0
 cMStorageGen#13#172:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(3)#110 <= 0
 cStoStorageGen#13#173:          - vEStorageGen#1(2)#109
                                 + vEStorageGen#1(3)#110
                                 - vQCStorageGen#1(3)#120
                                 + vQDStorageGen#1(3)#130  = 0
 cMStorageGen#14#174:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(4)#111 <= 0
 cStoStorageGen#14#175:          - vEStorageGen#1(3)#110
                                 + vEStorageGen#1(4)#111
                                 - vQCStorageGen#1(4)#121
                                 + vQDStorageGen#1(4)#131  = 0
 cMStorageGen#15#176:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(5)#112 <= 0
 cStoStorageGen#15#177:          - vEStorageGen#1(4)#111
                                 + vEStorageGen#1(5)#112
                                 - vQCStorageGen#1(5)#122
                                 + vQDStorageGen#1(5)#132  = 0
 cMStorageGen#16#178:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(6)#113 <= 0
 cStoStorageGen#16#179:          - vEStorageGen#1(5)#112
                                 + vEStorageGen#1(6)#113
                                 - vQCStorageGen#1(6)#123
                                 + vQDStorageGen#1(6)#133  = 0
 cMStorageGen#17#180:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(7)#114 <= 0
 cStoStorageGen#17#181:          - vEStorageGen#1(6)#113
                                 + vEStorageGen#1(7)#114
                                 - vQCStorageGen#1(7)#124
                                 + vQDStorageGen#1(7)#134  = 0
 cMStorageGen#18#182:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(8)#115 <= 0
 cStoStorageGen#18#183:          - vEStorageGen#1(7)#114
                                 + vEStorageGen#1(8)#115
                                 - vQCStorageGen#1(8)#125
                                 + vQDStorageGen#1(8)#135  = 0
 cMStorageGen#19#184:            - vMaxEstoStorageGen#1#106
                                 + vEStorageGen#1(9)#116 <= 0
 cStoStorageGen#19#185:          - vEStorageGen#1(8)#115
                                 + vEStorageGen#1(9)#116
                                 - vQCStorageGen#1(9)#126
                                 + vQDStorageGen#1(9)#136  = 0
 cMinStoFinStorageGen#19#186:    vEStorageGen#1(9)#116
                                 - vStoIniStorageGen#1#137 >= 0
 cMFCStorageGen#10#187:          vQCStorageGen#1(0)#117
                                 - 100000000 vsensStorageGen#1(0)#138 <= 0
 cMFDStorageGen#10#188:          vQDStorageGen#1(0)#127
                                 + 100000000 vsensStorageGen#1(0)#138
                                 <= 100000000
 cMFCStorageGen#11#189:          vQCStorageGen#1(1)#118
                                 - 100000000 vsensStorageGen#1(1)#139 <= 0
 cMFDStorageGen#11#190:          vQDStorageGen#1(1)#128
                                 + 100000000 vsensStorageGen#1(1)#139
                                 <= 100000000
 cMFCStorageGen#12#191:          vQCStorageGen#1(2)#119
                                 - 100000000 vsensStorageGen#1(2)#140 <= 0
 cMFDStorageGen#12#192:          vQDStorageGen#1(2)#129
                                 + 100000000 vsensStorageGen#1(2)#140
                                 <= 100000000
 cMFCStorageGen#13#193:          vQCStorageGen#1(3)#120
                                 - 100000000 vsensStorageGen#1(3)#141 <= 0
 cMFDStorageGen#13#194:          vQDStorageGen#1(3)#130
                                 + 100000000 vsensStorageGen#1(3)#141
                                 <= 100000000
 cMFCStorageGen#14#195:          vQCStorageGen#1(4)#121
                                 - 100000000 vsensStorageGen#1(4)#142 <= 0
 cMFDStorageGen#14#196:          vQDStorageGen#1(4)#131
                                 + 100000000 vsensStorageGen#1(4)#142
                                 <= 100000000
 cMFCStorageGen#15#197:          vQCStorageGen#1(5)#122
                                 - 100000000 vsensStorageGen#1(5)#143 <= 0
 cMFDStorageGen#15#198:          vQDStorageGen#1(5)#132
                                 + 100000000 vsensStorageGen#1(5)#143
                                 <= 100000000
 cMFCStorageGen#16#199:          vQCStorageGen#1(6)#123
                                 - 100000000 vsensStorageGen#1(6)#144 <= 0
 cMFDStorageGen#16#200:          vQDStorageGen#1(6)#133
                                 + 100000000 vsensStorageGen#1(6)#144
                                 <= 100000000
 cMFCStorageGen#17#201:          vQCStorageGen#1(7)#124
                                 - 100000000 vsensStorageGen#1(7)#145 <= 0
 cMFDStorageGen#17#202:          vQDStorageGen#1(7)#134
                                 + 100000000 vsensStorageGen#1(7)#145
                                 <= 100000000
 cMFCStorageGen#18#203:          vQCStorageGen#1(8)#125
                                 - 100000000 vsensStorageGen#1(8)#146 <= 0
 cMFDStorageGen#18#204:          vQDStorageGen#1(8)#135
                                 + 100000000 vsensStorageGen#1(8)#146
                                 <= 100000000
 cMFCStorageGen#19#205:          vQCStorageGen#1(9)#126
                                 - 100000000 vsensStorageGen#1(9)#147 <= 0
 cMFDStorageGen#19#206:          vQDStorageGen#1(9)#136
                                 + 100000000 vsensStorageGen#1(9)#147
                                 <= 100000000
 csBigMInstalledWind0#207:       - 1000 vInstalledWind#148 + vWeightWind#149
                                 <= 0
 csMinSizeInstalledWind0#208:    vWeightWind#149 >= 0
 cMSFxWind0#209:                 <= 10000
 cMSFxWind1#210:                 <= 10000
 cMSFxWind2#211:                 <= 10000
 cMSFxWind3#212:                 <= 10000
 cMSFxWind4#213:                 <= 10000
 cMSFxWind5#214:                 <= 10000
 cMSFxWind6#215:                 <= 10000
 cMSFxWind7#216:                 <= 10000
 cMSFxWind8#217:                 <= 10000
 cMSFxWind9#218:                 <= 10000
 cSNodeLaw#10#219:               - vPowElectrolyzer#1(0)#12
                                 + vGFxGridFree#1(0)#71 - vGFxGridFree#2(0)#93
                                 + 1.845819 vWeightWind#149  = 0
 cSNodeLaw#11#220:               - vPowElectrolyzer#1(1)#13
                                 + vGFxGridFree#1(1)#72 - vGFxGridFree#2(1)#94
                                 + 2.191294 vWeightWind#149  = 0
 cSNodeLaw#12#221:               - vPowElectrolyzer#1(2)#14
                                 + vGFxGridFree#1(2)#73 - vGFxGridFree#2(2)#95
                                 + 2.326187 vWeightWind#149  = 0
 cSNodeLaw#13#222:               - vPowElectrolyzer#1(3)#15
                                 + vGFxGridFree#1(3)#74 - vGFxGridFree#2(3)#96
                                 + 2.483921 vWeightWind#149  = 0
 cSNodeLaw#14#223:               - vPowElectrolyzer#1(4)#16
                                 + vGFxGridFree#1(4)#75 - vGFxGridFree#2(4)#97
                                 + 2.67351 vWeightWind#149  = 0
 cSNodeLaw#15#224:               - vPowElectrolyzer#1(5)#17
                                 + vGFxGridFree#1(5)#76 - vGFxGridFree#2(5)#98
                                 + 2.85778 vWeightWind#149  = 0
 cSNodeLaw#16#225:               - vPowElectrolyzer#1(6)#18
                                 + vGFxGridFree#1(6)#77 - vGFxGridFree#2(6)#99
                                 + 2.98878 vWeightWind#149  = 0
 cSNodeLaw#17#226:               - vPowElectrolyzer#1(7)#19
                                 + vGFxGridFree#1(7)#78 - vGFxGridFree#2(7)#100
                                 + 3.033381 vWeightWind#149  = 0
 cSNodeLaw#18#227:               - vPowElectrolyzer#1(8)#20
                                 + vGFxGridFree#1(8)#79 - vGFxGridFree#2(8)#101
                                 + 2.807039 vWeightWind#149  = 0
 cSNodeLaw#19#228:               - vPowElectrolyzer#1(9)#21
                                 + vGFxGridFree#1(9)#80 - vGFxGridFree#2(9)#102
                                 + 2.311192 vWeightWind#149  = 0
 cSNodeLaw#20#229:               vFlowElectrolyzer#1(0)#22
                                 - vQCStorageGen#1(0)#117
                                 + vQDStorageGen#1(0)#127  = 0
 cSNodeLaw#21#230:               vFlowElectrolyzer#1(1)#23
                                 - vQCStorageGen#1(1)#118
                                 + vQDStorageGen#1(1)#128  = 0
 cSNodeLaw#22#231:               vFlowElectrolyzer#1(2)#24
                                 - vQCStorageGen#1(2)#119
                                 + vQDStorageGen#1(2)#129  = 0
 cSNodeLaw#23#232:               vFlowElectrolyzer#1(3)#25
                                 - vQCStorageGen#1(3)#120
                                 + vQDStorageGen#1(3)#130  = 0
 cSNodeLaw#24#233:               vFlowElectrolyzer#1(4)#26
                                 - vQCStorageGen#1(4)#121
                                 + vQDStorageGen#1(4)#131  = 0
 cSNodeLaw#25#234:               vFlowElectrolyzer#1(5)#27
                                 - vQCStorageGen#1(5)#122
                                 + vQDStorageGen#1(5)#132  = 0
 cSNodeLaw#26#235:               vFlowElectrolyzer#1(6)#28
                                 - vQCStorageGen#1(6)#123
                                 + vQDStorageGen#1(6)#133  = 0
 cSNodeLaw#27#236:               vFlowElectrolyzer#1(7)#29
                                 - vQCStorageGen#1(7)#124
                                 + vQDStorageGen#1(7)#134  = 0
 cSNodeLaw#28#237:               vFlowElectrolyzer#1(8)#30
                                 - vQCStorageGen#1(8)#125
                                 + vQDStorageGen#1(8)#135  = 0
 cSNodeLaw#29#238:               vFlowElectrolyzer#1(9)#31
                                 - vQCStorageGen#1(9)#126
                                 + vQDStorageGen#1(9)#136  = 0
Bounds
 0 <= vInstalledElectrolyzer#1#0 <= 1
 0 <= vMaxPowerElectrolyzer#1#1 <= 1000
 0 <= vStateElectrolyzer#1(0)#2 <= 1
 0 <= vStateElectrolyzer#1(1)#3 <= 1
 0 <= vStateElectrolyzer#1(2)#4 <= 1
 0 <= vStateElectrolyzer#1(3)#5 <= 1
 0 <= vStateElectrolyzer#1(4)#6 <= 1
 0 <= vStateElectrolyzer#1(5)#7 <= 1
 0 <= vStateElectrolyzer#1(6)#8 <= 1
 0 <= vStateElectrolyzer#1(7)#9 <= 1
 0 <= vStateElectrolyzer#1(8)#10 <= 1
 0 <= vStateElectrolyzer#1(9)#11 <= 1
 0 <= vPowElectrolyzer#1(0)#12 <= 1000
 0 <= vPowElectrolyzer#1(1)#13 <= 1000
 0 <= vPowElectrolyzer#1(2)#14 <= 1000
 0 <= vPowElectrolyzer#1(3)#15 <= 1000
 0 <= vPowElectrolyzer#1(4)#16 <= 1000
 0 <= vPowElectrolyzer#1(5)#17 <= 1000
 0 <= vPowElectrolyzer#1(6)#18 <= 1000
 0 <= vPowElectrolyzer#1(7)#19 <= 1000
 0 <= vPowElectrolyzer#1(8)#20 <= 1000
 0 <= vPowElectrolyzer#1(9)#21 <= 1000
 0 <= vFlowElectrolyzer#1(0)#22 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(1)#23 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(2)#24 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(3)#25 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(4)#26 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(5)#27 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(6)#28 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(7)#29 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(8)#30 <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(9)#31 <= 18007.2028811525
 0 <= vUsablePowElectrolyzer#1#32 <= 1000
      vPminElectrolyzer#1(0)#33 Free
      vPminElectrolyzer#1(1)#34 Free
      vPminElectrolyzer#1(2)#35 Free
      vPminElectrolyzer#1(3)#36 Free
      vPminElectrolyzer#1(4)#37 Free
      vPminElectrolyzer#1(5)#38 Free
      vPminElectrolyzer#1(6)#39 Free
      vPminElectrolyzer#1(7)#40 Free
      vPminElectrolyzer#1(8)#41 Free
      vPminElectrolyzer#1(9)#42 Free
 0 <= Discretized_SizeMax(0)#43 <= 1
 0 <= Discretized_SizeMax(1)#44 <= 1
 0 <= Discretized_SizeMax(2)#45 <= 1
 0 <= Discretized_SizeMax(3)#46 <= 1
 0 <= Discretized_SizeMax(0)#47 <= 1
 0 <= Discretized_SizeMax(1)#48 <= 1
 0 <= Discretized_SizeMax(2)#49 <= 1
 0 <= Discretized_SizeMax(3)#50 <= 1
 0 <= Discretized_SizeMax(0)#51 <= 1
 0 <= Discretized_SizeMax(1)#52 <= 1
 0 <= Discretized_SizeMax(2)#53 <= 1
 0 <= Discretized_SizeMax(3)#54 <= 1
 0 <= Discretized_SizeMax(0)#55 <= 1
 0 <= Discretized_SizeMax(1)#56 <= 1
 0 <= Discretized_SizeMax(2)#57 <= 1
 0 <= Discretized_SizeMax(3)#58 <= 1
 0 <= vInstalledGridFree#1#59 <= 1
 0 <= vMaxFlowGridFree#1#60 <= 100000000
 0 <= vStateGridFree#1(0)#61 <= 1
 0 <= vStateGridFree#1(1)#62 <= 1
 0 <= vStateGridFree#1(2)#63 <= 1
 0 <= vStateGridFree#1(3)#64 <= 1
 0 <= vStateGridFree#1(4)#65 <= 1
 0 <= vStateGridFree#1(5)#66 <= 1
 0 <= vStateGridFree#1(6)#67 <= 1
 0 <= vStateGridFree#1(7)#68 <= 1
 0 <= vStateGridFree#1(8)#69 <= 1
 0 <= vStateGridFree#1(9)#70 <= 1
 0 <= vGFxGridFree#1(0)#71 <= 100000000
 0 <= vGFxGridFree#1(1)#72 <= 100000000
 0 <= vGFxGridFree#1(2)#73 <= 100000000
 0 <= vGFxGridFree#1(3)#74 <= 100000000
 0 <= vGFxGridFree#1(4)#75 <= 100000000
 0 <= vGFxGridFree#1(5)#76 <= 100000000
 0 <= vGFxGridFree#1(6)#77 <= 100000000
 0 <= vGFxGridFree#1(7)#78 <= 100000000
 0 <= vGFxGridFree#1(8)#79 <= 100000000
 0 <= vGFxGridFree#1(9)#80 <= 100000000
 0 <= vInstalledGridFree#2#81 <= 1
 0 <= vMaxFlowGridFree#2#82 <= 10000
 0 <= vStateGridFree#2(0)#83 <= 1
 0 <= vStateGridFree#2(1)#84 <= 1
 0 <= vStateGridFree#2(2)#85 <= 1
 0 <= vStateGridFree#2(3)#86 <= 1
 0 <= vStateGridFree#2(4)#87 <= 1
 0 <= vStateGridFree#2(5)#88 <= 1
 0 <= vStateGridFree#2(6)#89 <= 1
 0 <= vStateGridFree#2(7)#90 <= 1
 0 <= vStateGridFree#2(8)#91 <= 1
 0 <= vStateGridFree#2(9)#92 <= 1
 0 <= vGFxGridFree#2(0)#93 <= 10000
 0 <= vGFxGridFree#2(1)#94 <= 10000
 0 <= vGFxGridFree#2(2)#95 <= 10000
 0 <= vGFxGridFree#2(3)#96 <= 10000
 0 <= vGFxGridFree#2(4)#97 <= 10000
 0 <= vGFxGridFree#2(5)#98 <= 10000
 0 <= vGFxGridFree#2(6)#99 <= 10000
 0 <= vGFxGridFree#2(7)#100 <= 10000
 0 <= vGFxGridFree#2(8)#101 <= 10000
 0 <= vGFxGridFree#2(9)#102 <= 10000
 0 <= vInstalledSourceLoad#1#103 <= 1
 0 <= vWeightSourceLoad#1#104 <= 1
 0 <= vInstalledStorageGen#1#105 <= 1
 0 <= vMaxEstoStorageGen#1#106 <= 100
 0 <= vEStorageGen#1(0)#107 <= 100
 0 <= vEStorageGen#1(1)#108 <= 100
 0 <= vEStorageGen#1(2)#109 <= 100
 0 <= vEStorageGen#1(3)#110 <= 100
 0 <= vEStorageGen#1(4)#111 <= 100
 0 <= vEStorageGen#1(5)#112 <= 100
 0 <= vEStorageGen#1(6)#113 <= 100
 0 <= vEStorageGen#1(7)#114 <= 100
 0 <= vEStorageGen#1(8)#115 <= 100
 0 <= vEStorageGen#1(9)#116 <= 100
 0 <= vQCStorageGen#1(0)#117 <= 100000000
 0 <= vQCStorageGen#1(1)#118 <= 100000000
 0 <= vQCStorageGen#1(2)#119 <= 100000000
 0 <= vQCStorageGen#1(3)#120 <= 100000000
 0 <= vQCStorageGen#1(4)#121 <= 100000000
 0 <= vQCStorageGen#1(5)#122 <= 100000000
 0 <= vQCStorageGen#1(6)#123 <= 100000000
 0 <= vQCStorageGen#1(7)#124 <= 100000000
 0 <= vQCStorageGen#1(8)#125 <= 100000000
 0 <= vQCStorageGen#1(9)#126 <= 100000000
 0 <= vQDStorageGen#1(0)#127 <= 100000000
 0 <= vQDStorageGen#1(1)#128 <= 100000000
 0 <= vQDStorageGen#1(2)#129 <= 100000000
 0 <= vQDStorageGen#1(3)#130 <= 100000000
 0 <= vQDStorageGen#1(4)#131 <= 100000000
 0 <= vQDStorageGen#1(5)#132 <= 100000000
 0 <= vQDStorageGen#1(6)#133 <= 100000000
 0 <= vQDStorageGen#1(7)#134 <= 100000000
 0 <= vQDStorageGen#1(8)#135 <= 100000000
 0 <= vQDStorageGen#1(9)#136 <= 100000000
 0 <= vStoIniStorageGen#1#137 <= 100
 0 <= vsensStorageGen#1(0)#138 <= 1
 0 <= vsensStorageGen#1(1)#139 <= 1
 0 <= vsensStorageGen#1(2)#140 <= 1
 0 <= vsensStorageGen#1(3)#141 <= 1
 0 <= vsensStorageGen#1(4)#142 <= 1
 0 <= vsensStorageGen#1(5)#143 <= 1
 0 <= vsensStorageGen#1(6)#144 <= 1
 0 <= vsensStorageGen#1(7)#145 <= 1
 0 <= vsensStorageGen#1(8)#146 <= 1
 0 <= vsensStorageGen#1(9)#147 <= 1
 0 <= vInstalledWind#148 <= 1
 0 <= vWeightWind#149 <= 1000
Generals
 vInstalledElectrolyzer#1#0  vStateElectrolyzer#1(0)#2 
 vStateElectrolyzer#1(1)#3  vStateElectrolyzer#1(2)#4 
 vStateElectrolyzer#1(3)#5  vStateElectrolyzer#1(4)#6 
 vStateElectrolyzer#1(5)#7  vStateElectrolyzer#1(6)#8 
 vStateElectrolyzer#1(7)#9  vStateElectrolyzer#1(8)#10 
 vStateElectrolyzer#1(9)#11  vInstalledGridFree#1#59  vStateGridFree#1(0)#61 
 vStateGridFree#1(1)#62  vStateGridFree#1(2)#63  vStateGridFree#1(3)#64 
 vStateGridFree#1(4)#65  vStateGridFree#1(5)#66  vStateGridFree#1(6)#67 
 vStateGridFree#1(7)#68  vStateGridFree#1(8)#69  vStateGridFree#1(9)#70 
 vInstalledGridFree#2#81  vStateGridFree#2(0)#83  vStateGridFree#2(1)#84 
 vStateGridFree#2(2)#85  vStateGridFree#2(3)#86  vStateGridFree#2(4)#87 
 vStateGridFree#2(5)#88  vStateGridFree#2(6)#89  vStateGridFree#2(7)#90 
 vStateGridFree#2(8)#91  vStateGridFree#2(9)#92  vInstalledSourceLoad#1#103 
 vInstalledStorageGen#1#105  vsensStorageGen#1(0)#138 
 vsensStorageGen#1(1)#139  vsensStorageGen#1(2)#140  vsensStorageGen#1(3)#141 
 vsensStorageGen#1(4)#142  vsensStorageGen#1(5)#143  vsensStorageGen#1(6)#144 
 vsensStorageGen#1(7)#145  vsensStorageGen#1(8)#146  vsensStorageGen#1(9)#147 
 vInstalledWind#148 
SOS
 s1: S2 ::  Discretized_SizeMax(0)#43 : 0  Discretized_SizeMax(1)#44 : 1
      Discretized_SizeMax(2)#45 : 2  Discretized_SizeMax(3)#46 : 3
 s2: S2 ::  Discretized_SizeMax(0)#47 : 0  Discretized_SizeMax(1)#48 : 1
      Discretized_SizeMax(2)#49 : 2  Discretized_SizeMax(3)#50 : 3
 s3: S2 ::  Discretized_SizeMax(0)#51 : 0  Discretized_SizeMax(1)#52 : 1
      Discretized_SizeMax(2)#53 : 2  Discretized_SizeMax(3)#54 : 3
 s4: S2 ::  Discretized_SizeMax(0)#55 : 0  Discretized_SizeMax(1)#56 : 1
      Discretized_SizeMax(2)#57 : 2  Discretized_SizeMax(3)#58 : 3
End
