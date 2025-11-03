\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 992998.143260721 vGFxGrid_Extract(0)
       + 992998.143260721 vGFxGrid_Extract(1)
       + 992998.143260721 vGFxGrid_Extract(2)
       + 992998.143260721 vGFxGrid_Extract(3)
       + 992998.143260721 vGFxGrid_Extract(4)
       + 992998.143260721 vGFxGrid_Extract(5)
       + 992998.143260721 vGFxGrid_Extract(6)
       + 992998.143260721 vGFxGrid_Extract(7)
       + 992998.143260721 vGFxGrid_Extract(8)
       + 992998.143260721 vGFxGrid_Extract(9)
       + 992998.143260721 vGFxGrid_Inject(0)
       + 992998.143260721 vGFxGrid_Inject(1)
       + 992998.143260721 vGFxGrid_Inject(2)
       + 992998.143260721 vGFxGrid_Inject(3)
       + 992998.143260721 vGFxGrid_Inject(4)
       + 992998.143260721 vGFxGrid_Inject(5)
       + 992998.143260721 vGFxGrid_Inject(6)
       + 992998.143260721 vGFxGrid_Inject(7)
       + 992998.143260721 vGFxGrid_Inject(8)
       + 992998.143260721 vGFxGrid_Inject(9)
       + 278.338988106756 vWeightLoad_PeakShaving
       + 32.6711904854046 vVarMaxEffectLoad_PeakShaving
Subject To
 csInstalledGrid_Extract0#0:     vInstalledGrid_Extract  = 1
 csMaxValGrid_Extract0#1:        vMaxFlowGrid_Extract  = 10000
 csBigMInstalledGrid_Extract0#2: - 10000 vInstalledGrid_Extract
                                 + vMaxFlowGrid_Extract <= 0
 csMinSizeInstalledGrid_Extract0#3: vMaxFlowGrid_Extract >= 0
 cNotRunningIfNotInstalledGrid_Extract0#4: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(0) <= 0
 cNotRunningIfNotInstalledGrid_Extract1#5: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(1) <= 0
 cNotRunningIfNotInstalledGrid_Extract2#6: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(2) <= 0
 cNotRunningIfNotInstalledGrid_Extract3#7: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(3) <= 0
 cNotRunningIfNotInstalledGrid_Extract4#8: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(4) <= 0
 cNotRunningIfNotInstalledGrid_Extract5#9: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(5) <= 0
 cNotRunningIfNotInstalledGrid_Extract6#10: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(6) <= 0
 cNotRunningIfNotInstalledGrid_Extract7#11: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(7) <= 0
 cNotRunningIfNotInstalledGrid_Extract8#12: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(8) <= 0
 cNotRunningIfNotInstalledGrid_Extract9#13: - vInstalledGrid_Extract
                                 + vStateGrid_Extract(9) <= 0
 cStateGridGrid_Extract0#14:     - 10000 vStateGrid_Extract(0)
                                 + vGFxGrid_Extract(0) <= 0
 cStateGridGrid_Extract1#15:     - 10000 vStateGrid_Extract(1)
                                 + vGFxGrid_Extract(1) <= 0
 cStateGridGrid_Extract2#16:     - 10000 vStateGrid_Extract(2)
                                 + vGFxGrid_Extract(2) <= 0
 cStateGridGrid_Extract3#17:     - 10000 vStateGrid_Extract(3)
                                 + vGFxGrid_Extract(3) <= 0
 cStateGridGrid_Extract4#18:     - 10000 vStateGrid_Extract(4)
                                 + vGFxGrid_Extract(4) <= 0
 cStateGridGrid_Extract5#19:     - 10000 vStateGrid_Extract(5)
                                 + vGFxGrid_Extract(5) <= 0
 cStateGridGrid_Extract6#20:     - 10000 vStateGrid_Extract(6)
                                 + vGFxGrid_Extract(6) <= 0
 cStateGridGrid_Extract7#21:     - 10000 vStateGrid_Extract(7)
                                 + vGFxGrid_Extract(7) <= 0
 cStateGridGrid_Extract8#22:     - 10000 vStateGrid_Extract(8)
                                 + vGFxGrid_Extract(8) <= 0
 cStateGridGrid_Extract9#23:     - 10000 vStateGrid_Extract(9)
                                 + vGFxGrid_Extract(9) <= 0
 cMaxGFxGrid_Extract0#24:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(0)
                                 <= 0
 cMaxGFxGrid_Extract1#25:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(1)
                                 <= 0
 cMaxGFxGrid_Extract2#26:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(2)
                                 <= 0
 cMaxGFxGrid_Extract3#27:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(3)
                                 <= 0
 cMaxGFxGrid_Extract4#28:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(4)
                                 <= 0
 cMaxGFxGrid_Extract5#29:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(5)
                                 <= 0
 cMaxGFxGrid_Extract6#30:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(6)
                                 <= 0
 cMaxGFxGrid_Extract7#31:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(7)
                                 <= 0
 cMaxGFxGrid_Extract8#32:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(8)
                                 <= 0
 cMaxGFxGrid_Extract9#33:        - vMaxFlowGrid_Extract + vGFxGrid_Extract(9)
                                 <= 0
 csInstalledGrid_Inject0#34:     vInstalledGrid_Inject  = 1
 csMaxValGrid_Inject0#35:        vMaxFlowGrid_Inject  = 10000
 csBigMInstalledGrid_Inject0#36: - 10000 vInstalledGrid_Inject
                                 + vMaxFlowGrid_Inject <= 0
 csMinSizeInstalledGrid_Inject0#37: vMaxFlowGrid_Inject >= 0
 cNotRunningIfNotInstalledGrid_Inject0#38: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(0) <= 0
 cNotRunningIfNotInstalledGrid_Inject1#39: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(1) <= 0
 cNotRunningIfNotInstalledGrid_Inject2#40: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(2) <= 0
 cNotRunningIfNotInstalledGrid_Inject3#41: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(3) <= 0
 cNotRunningIfNotInstalledGrid_Inject4#42: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(4) <= 0
 cNotRunningIfNotInstalledGrid_Inject5#43: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(5) <= 0
 cNotRunningIfNotInstalledGrid_Inject6#44: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(6) <= 0
 cNotRunningIfNotInstalledGrid_Inject7#45: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(7) <= 0
 cNotRunningIfNotInstalledGrid_Inject8#46: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(8) <= 0
 cNotRunningIfNotInstalledGrid_Inject9#47: - vInstalledGrid_Inject
                                 + vStateGrid_Inject(9) <= 0
 cStateGridGrid_Inject0#48:      - 10000 vStateGrid_Inject(0)
                                 + vGFxGrid_Inject(0) <= 0
 cStateGridGrid_Inject1#49:      - 10000 vStateGrid_Inject(1)
                                 + vGFxGrid_Inject(1) <= 0
 cStateGridGrid_Inject2#50:      - 10000 vStateGrid_Inject(2)
                                 + vGFxGrid_Inject(2) <= 0
 cStateGridGrid_Inject3#51:      - 10000 vStateGrid_Inject(3)
                                 + vGFxGrid_Inject(3) <= 0
 cStateGridGrid_Inject4#52:      - 10000 vStateGrid_Inject(4)
                                 + vGFxGrid_Inject(4) <= 0
 cStateGridGrid_Inject5#53:      - 10000 vStateGrid_Inject(5)
                                 + vGFxGrid_Inject(5) <= 0
 cStateGridGrid_Inject6#54:      - 10000 vStateGrid_Inject(6)
                                 + vGFxGrid_Inject(6) <= 0
 cStateGridGrid_Inject7#55:      - 10000 vStateGrid_Inject(7)
                                 + vGFxGrid_Inject(7) <= 0
 cStateGridGrid_Inject8#56:      - 10000 vStateGrid_Inject(8)
                                 + vGFxGrid_Inject(8) <= 0
 cStateGridGrid_Inject9#57:      - 10000 vStateGrid_Inject(9)
                                 + vGFxGrid_Inject(9) <= 0
 cMaxGFxGrid_Inject0#58:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(0)
                                 <= 0
 cMaxGFxGrid_Inject1#59:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(1)
                                 <= 0
 cMaxGFxGrid_Inject2#60:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(2)
                                 <= 0
 cMaxGFxGrid_Inject3#61:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(3)
                                 <= 0
 cMaxGFxGrid_Inject4#62:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(4)
                                 <= 0
 cMaxGFxGrid_Inject5#63:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(5)
                                 <= 0
 cMaxGFxGrid_Inject6#64:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(6)
                                 <= 0
 cMaxGFxGrid_Inject7#65:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(7)
                                 <= 0
 cMaxGFxGrid_Inject8#66:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(8)
                                 <= 0
 cMaxGFxGrid_Inject9#67:         - vMaxFlowGrid_Inject + vGFxGrid_Inject(9)
                                 <= 0
 csInstalledLoad_PeakShaving0#68: vInstalledLoad_PeakShaving  = 1
 csWeightLoad_PeakShaving0#69:   vWeightLoad_PeakShaving  = 1
 csBigMInstalledLoad_PeakShaving0#70: - vInstalledLoad_PeakShaving
                                 + vWeightLoad_PeakShaving <= 0
 csMinSizeInstalledLoad_PeakShaving0#71: vWeightLoad_PeakShaving >= 0
 cMSFxLoad_PeakShaving0#72:      <= 10
 cMSFxLoad_PeakShaving1#73:      <= 10
 cMSFxLoad_PeakShaving2#74:      <= 10
 cMSFxLoad_PeakShaving3#75:      <= 10
 cMSFxLoad_PeakShaving4#76:      <= 10
 cMSFxLoad_PeakShaving5#77:      <= 10
 cMSFxLoad_PeakShaving6#78:      <= 10
 cMSFxLoad_PeakShaving7#79:      <= 10
 cMSFxLoad_PeakShaving8#80:      <= 10
 cMSFxLoad_PeakShaving9#81:      <= 10
 cDefMaxEffectLoad_PeakShaving0#82: vVarMaxEffectLoad_PeakShaving  = 5
 cFluxGridBoundLoad_PeakShaving0#83: vVarPowerPeakShavingLoad_PeakShaving(0)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#84: vVarPowerPeakShavingLoad_PeakShaving(0)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#85: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(0)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#86: vVarPowerPeakShavingLoad_PeakShaving(1)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#87: vVarPowerPeakShavingLoad_PeakShaving(1)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#88: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(1)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#89: vVarPowerPeakShavingLoad_PeakShaving(2)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#90: vVarPowerPeakShavingLoad_PeakShaving(2)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#91: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(2)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#92: vVarPowerPeakShavingLoad_PeakShaving(3)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#93: vVarPowerPeakShavingLoad_PeakShaving(3)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#94: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(3)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#95: vVarPowerPeakShavingLoad_PeakShaving(4)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#96: vVarPowerPeakShavingLoad_PeakShaving(4)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#97: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(4)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#98: vVarPowerPeakShavingLoad_PeakShaving(5)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#99: vVarPowerPeakShavingLoad_PeakShaving(5)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#100: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(5)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#101: vVarPowerPeakShavingLoad_PeakShaving(6)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#102: vVarPowerPeakShavingLoad_PeakShaving(6)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#103: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(6)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#104: vVarPowerPeakShavingLoad_PeakShaving(7)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#105: vVarPowerPeakShavingLoad_PeakShaving(7)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#106: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(7)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#107: vVarPowerPeakShavingLoad_PeakShaving(8)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#108: vVarPowerPeakShavingLoad_PeakShaving(8)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#109: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(8)
                                 <= 15
 cFluxGridBoundLoad_PeakShaving0#110: vVarPowerPeakShavingLoad_PeakShaving(9)
                                 - vVarMaxEffectLoad_PeakShaving <= 0
 cFluxGridBound2Load_PeakShaving0#111: vVarPowerPeakShavingLoad_PeakShaving(9)
                                 + vVarMaxEffectLoad_PeakShaving >= 0
 cFluxGridMaxBoundLoad_PeakShaving0#112: 10 vWeightLoad_PeakShaving
                                 + vVarPowerPeakShavingLoad_PeakShaving(9)
                                 <= 15
 cSumsLoad_PeakShaving0#113:     vVarPowerPeakShavingLoad_PeakShaving(0)
                                 + vVarPowerPeakShavingLoad_PeakShaving(1)  = 0
 cSumsLoad_PeakShaving1#114:     vVarPowerPeakShavingLoad_PeakShaving(2)
                                 + vVarPowerPeakShavingLoad_PeakShaving(3)  = 0
 cSumsLoad_PeakShaving2#115:     vVarPowerPeakShavingLoad_PeakShaving(4)
                                 + vVarPowerPeakShavingLoad_PeakShaving(5)  = 0
 cSumsLoad_PeakShaving3#116:     vVarPowerPeakShavingLoad_PeakShaving(6)
                                 + vVarPowerPeakShavingLoad_PeakShaving(7)  = 0
 cSumsLoad_PeakShaving4#117:     vVarPowerPeakShavingLoad_PeakShaving(8)
                                 + vVarPowerPeakShavingLoad_PeakShaving(9)  = 0
 cSumsLoad_PeakShaving5#118:      = 0
 csInstalledVariable_Source0#119: vInstalledVariable_Source  = 1
 csWeightVariable_Source0#120:   vWeightVariable_Source  = 1
 csBigMInstalledVariable_Source0#121: - vInstalledVariable_Source
                                 + vWeightVariable_Source <= 0
 csMinSizeInstalledVariable_Source0#122: vWeightVariable_Source >= 0
 cMSFxVariable_Source0#123:      <= 10000
 cMSFxVariable_Source1#124:      <= 10000
 cMSFxVariable_Source2#125:      <= 10000
 cMSFxVariable_Source3#126:      <= 10000
 cMSFxVariable_Source4#127:      <= 10000
 cMSFxVariable_Source5#128:      <= 10000
 cMSFxVariable_Source6#129:      <= 10000
 cMSFxVariable_Source7#130:      <= 10000
 cMSFxVariable_Source8#131:      <= 10000
 cMSFxVariable_Source9#132:      <= 10000
 cSELec_Bus0#133:                vGFxGrid_Extract(0) - vGFxGrid_Inject(0)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(0)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus1#134:                vGFxGrid_Extract(1) - vGFxGrid_Inject(1)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(1)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus2#135:                vGFxGrid_Extract(2) - vGFxGrid_Inject(2)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(2)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus3#136:                vGFxGrid_Extract(3) - vGFxGrid_Inject(3)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(3)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus4#137:                vGFxGrid_Extract(4) - vGFxGrid_Inject(4)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(4)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus5#138:                vGFxGrid_Extract(5) - vGFxGrid_Inject(5)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(5)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus6#139:                vGFxGrid_Extract(6) - vGFxGrid_Inject(6)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(6)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus7#140:                vGFxGrid_Extract(7) - vGFxGrid_Inject(7)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(7)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus8#141:                vGFxGrid_Extract(8) - vGFxGrid_Inject(8)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(8)
                                 + 10 vWeightVariable_Source  = 0
 cSELec_Bus9#142:                vGFxGrid_Extract(9) - vGFxGrid_Inject(9)
                                 - 10 vWeightLoad_PeakShaving
                                 - vVarPowerPeakShavingLoad_PeakShaving(9)
                                 + 5 vWeightVariable_Source  = 0
Bounds
 0 <= vInstalledGrid_Extract <= 1
 0 <= vMaxFlowGrid_Extract <= 10000
 0 <= vStateGrid_Extract(0) <= 1
 0 <= vStateGrid_Extract(1) <= 1
 0 <= vStateGrid_Extract(2) <= 1
 0 <= vStateGrid_Extract(3) <= 1
 0 <= vStateGrid_Extract(4) <= 1
 0 <= vStateGrid_Extract(5) <= 1
 0 <= vStateGrid_Extract(6) <= 1
 0 <= vStateGrid_Extract(7) <= 1
 0 <= vStateGrid_Extract(8) <= 1
 0 <= vStateGrid_Extract(9) <= 1
 0 <= vGFxGrid_Extract(0) <= 10000
 0 <= vGFxGrid_Extract(1) <= 10000
 0 <= vGFxGrid_Extract(2) <= 10000
 0 <= vGFxGrid_Extract(3) <= 10000
 0 <= vGFxGrid_Extract(4) <= 10000
 0 <= vGFxGrid_Extract(5) <= 10000
 0 <= vGFxGrid_Extract(6) <= 10000
 0 <= vGFxGrid_Extract(7) <= 10000
 0 <= vGFxGrid_Extract(8) <= 10000
 0 <= vGFxGrid_Extract(9) <= 10000
 0 <= vInstalledGrid_Inject <= 1
 0 <= vMaxFlowGrid_Inject <= 10000
 0 <= vStateGrid_Inject(0) <= 1
 0 <= vStateGrid_Inject(1) <= 1
 0 <= vStateGrid_Inject(2) <= 1
 0 <= vStateGrid_Inject(3) <= 1
 0 <= vStateGrid_Inject(4) <= 1
 0 <= vStateGrid_Inject(5) <= 1
 0 <= vStateGrid_Inject(6) <= 1
 0 <= vStateGrid_Inject(7) <= 1
 0 <= vStateGrid_Inject(8) <= 1
 0 <= vStateGrid_Inject(9) <= 1
 0 <= vGFxGrid_Inject(0) <= 10000
 0 <= vGFxGrid_Inject(1) <= 10000
 0 <= vGFxGrid_Inject(2) <= 10000
 0 <= vGFxGrid_Inject(3) <= 10000
 0 <= vGFxGrid_Inject(4) <= 10000
 0 <= vGFxGrid_Inject(5) <= 10000
 0 <= vGFxGrid_Inject(6) <= 10000
 0 <= vGFxGrid_Inject(7) <= 10000
 0 <= vGFxGrid_Inject(8) <= 10000
 0 <= vGFxGrid_Inject(9) <= 10000
 0 <= vInstalledLoad_PeakShaving <= 1
 0 <= vWeightLoad_PeakShaving <= 1
-5 <= vVarPowerPeakShavingLoad_PeakShaving(0) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(1) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(2) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(3) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(4) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(5) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(6) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(7) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(8) <= 5
-5 <= vVarPowerPeakShavingLoad_PeakShaving(9) <= 5
 0 <= vVarMaxEffectLoad_PeakShaving <= 5
 0 <= vInstalledVariable_Source <= 1
 0 <= vWeightVariable_Source <= 1
Generals
 vInstalledGrid_Extract  vStateGrid_Extract(0)  vStateGrid_Extract(1) 
 vStateGrid_Extract(2)  vStateGrid_Extract(3)  vStateGrid_Extract(4) 
 vStateGrid_Extract(5)  vStateGrid_Extract(6)  vStateGrid_Extract(7) 
 vStateGrid_Extract(8)  vStateGrid_Extract(9)  vInstalledGrid_Inject 
 vStateGrid_Inject(0)  vStateGrid_Inject(1)  vStateGrid_Inject(2) 
 vStateGrid_Inject(3)  vStateGrid_Inject(4)  vStateGrid_Inject(5) 
 vStateGrid_Inject(6)  vStateGrid_Inject(7)  vStateGrid_Inject(8) 
 vStateGrid_Inject(9)  vInstalledLoad_PeakShaving  vInstalledVariable_Source 
End
