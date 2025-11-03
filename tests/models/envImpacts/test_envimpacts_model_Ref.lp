\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 52628.9015928182 vGFxElec_Grid(0) + 54614.8978793396 vGFxElec_Grid(1)
       + 55607.8960226004 vGFxElec_Grid(2) + 56600.8941658611 vGFxElec_Grid(3)
       + 57593.8923091218 vGFxElec_Grid(4) + 58586.8904523825 vGFxElec_Grid(5)
       + 54614.8978793396 vGFxElec_Grid(6) + 56600.8941658611 vGFxElec_Grid(7)
       + 58586.8904523825 vGFxElec_Grid(8) + 51635.9034495575 vGFxElec_Grid(9)
Subject To
 csBigMInstalledConverter0#0:    - 1000 vInstalledConverter
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
 cPowerInMaxBoundConverter0#12:  vPowInConverter(0) <= 1000
 cPowerInMaxBoundConverter0#13:  vPowInConverter(1) <= 1000
 cPowerInMaxBoundConverter0#14:  vPowInConverter(2) <= 1000
 cPowerInMaxBoundConverter0#15:  vPowInConverter(3) <= 1000
 cPowerInMaxBoundConverter0#16:  vPowInConverter(4) <= 1000
 cPowerInMaxBoundConverter0#17:  vPowInConverter(5) <= 1000
 cPowerInMaxBoundConverter0#18:  vPowInConverter(6) <= 1000
 cPowerInMaxBoundConverter0#19:  vPowInConverter(7) <= 1000
 cPowerInMaxBoundConverter0#20:  vPowInConverter(8) <= 1000
 cPowerInMaxBoundConverter0#21:  vPowInConverter(9) <= 1000
 cPowMaxConverter0#22:           - 1000 vStateConverter(0) + vPowInConverter(0)
                                 <= 0
 cDefPminConverter0#23:          - vPminConverter(0)  = 0
 cPminConverter0#24:             vPowInConverter(0) - vPminConverter(0) >= 0
 cPowMaxConverter1#25:           - 1000 vStateConverter(1) + vPowInConverter(1)
                                 <= 0
 cDefPminConverter1#26:          - vPminConverter(1)  = 0
 cPminConverter1#27:             vPowInConverter(1) - vPminConverter(1) >= 0
 cPowMaxConverter2#28:           - 1000 vStateConverter(2) + vPowInConverter(2)
                                 <= 0
 cDefPminConverter2#29:          - vPminConverter(2)  = 0
 cPminConverter2#30:             vPowInConverter(2) - vPminConverter(2) >= 0
 cPowMaxConverter3#31:           - 1000 vStateConverter(3) + vPowInConverter(3)
                                 <= 0
 cDefPminConverter3#32:          - vPminConverter(3)  = 0
 cPminConverter3#33:             vPowInConverter(3) - vPminConverter(3) >= 0
 cPowMaxConverter4#34:           - 1000 vStateConverter(4) + vPowInConverter(4)
                                 <= 0
 cDefPminConverter4#35:          - vPminConverter(4)  = 0
 cPminConverter4#36:             vPowInConverter(4) - vPminConverter(4) >= 0
 cPowMaxConverter5#37:           - 1000 vStateConverter(5) + vPowInConverter(5)
                                 <= 0
 cDefPminConverter5#38:          - vPminConverter(5)  = 0
 cPminConverter5#39:             vPowInConverter(5) - vPminConverter(5) >= 0
 cPowMaxConverter6#40:           - 1000 vStateConverter(6) + vPowInConverter(6)
                                 <= 0
 cDefPminConverter6#41:          - vPminConverter(6)  = 0
 cPminConverter6#42:             vPowInConverter(6) - vPminConverter(6) >= 0
 cPowMaxConverter7#43:           - 1000 vStateConverter(7) + vPowInConverter(7)
                                 <= 0
 cDefPminConverter7#44:          - vPminConverter(7)  = 0
 cPminConverter7#45:             vPowInConverter(7) - vPminConverter(7) >= 0
 cPowMaxConverter8#46:           - 1000 vStateConverter(8) + vPowInConverter(8)
                                 <= 0
 cDefPminConverter8#47:          - vPminConverter(8)  = 0
 cPminConverter8#48:             vPowInConverter(8) - vPminConverter(8) >= 0
 cPowMaxConverter9#49:           - 1000 vStateConverter(9) + vPowInConverter(9)
                                 <= 0
 cDefPminConverter9#50:          - vPminConverter(9)  = 0
 cPminConverter9#51:             vPowInConverter(9) - vPminConverter(9) >= 0
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
 csInstalledElec_Grid0#72:       vInstalledElec_Grid  = 1
 csMaxValElec_Grid0#73:          vMaxFlowElec_Grid  = 10000
 csBigMInstalledElec_Grid0#74:   - 10000 vInstalledElec_Grid
                                 + vMaxFlowElec_Grid <= 0
 csMinSizeInstalledElec_Grid0#75: vMaxFlowElec_Grid >= 0
 cNotRunningIfNotInstalledElec_Grid0#76: - vInstalledElec_Grid
                                 + vStateElec_Grid(0) <= 0
 cNotRunningIfNotInstalledElec_Grid1#77: - vInstalledElec_Grid
                                 + vStateElec_Grid(1) <= 0
 cNotRunningIfNotInstalledElec_Grid2#78: - vInstalledElec_Grid
                                 + vStateElec_Grid(2) <= 0
 cNotRunningIfNotInstalledElec_Grid3#79: - vInstalledElec_Grid
                                 + vStateElec_Grid(3) <= 0
 cNotRunningIfNotInstalledElec_Grid4#80: - vInstalledElec_Grid
                                 + vStateElec_Grid(4) <= 0
 cNotRunningIfNotInstalledElec_Grid5#81: - vInstalledElec_Grid
                                 + vStateElec_Grid(5) <= 0
 cNotRunningIfNotInstalledElec_Grid6#82: - vInstalledElec_Grid
                                 + vStateElec_Grid(6) <= 0
 cNotRunningIfNotInstalledElec_Grid7#83: - vInstalledElec_Grid
                                 + vStateElec_Grid(7) <= 0
 cNotRunningIfNotInstalledElec_Grid8#84: - vInstalledElec_Grid
                                 + vStateElec_Grid(8) <= 0
 cNotRunningIfNotInstalledElec_Grid9#85: - vInstalledElec_Grid
                                 + vStateElec_Grid(9) <= 0
 cStateGridElec_Grid0#86:        - 10000 vStateElec_Grid(0) + vGFxElec_Grid(0)
                                 <= 0
 cStateGridElec_Grid1#87:        - 10000 vStateElec_Grid(1) + vGFxElec_Grid(1)
                                 <= 0
 cStateGridElec_Grid2#88:        - 10000 vStateElec_Grid(2) + vGFxElec_Grid(2)
                                 <= 0
 cStateGridElec_Grid3#89:        - 10000 vStateElec_Grid(3) + vGFxElec_Grid(3)
                                 <= 0
 cStateGridElec_Grid4#90:        - 10000 vStateElec_Grid(4) + vGFxElec_Grid(4)
                                 <= 0
 cStateGridElec_Grid5#91:        - 10000 vStateElec_Grid(5) + vGFxElec_Grid(5)
                                 <= 0
 cStateGridElec_Grid6#92:        - 10000 vStateElec_Grid(6) + vGFxElec_Grid(6)
                                 <= 0
 cStateGridElec_Grid7#93:        - 10000 vStateElec_Grid(7) + vGFxElec_Grid(7)
                                 <= 0
 cStateGridElec_Grid8#94:        - 10000 vStateElec_Grid(8) + vGFxElec_Grid(8)
                                 <= 0
 cStateGridElec_Grid9#95:        - 10000 vStateElec_Grid(9) + vGFxElec_Grid(9)
                                 <= 0
 cMaxGFxElec_Grid0#96:           - vMaxFlowElec_Grid + vGFxElec_Grid(0) <= 0
 cMaxGFxElec_Grid1#97:           - vMaxFlowElec_Grid + vGFxElec_Grid(1) <= 0
 cMaxGFxElec_Grid2#98:           - vMaxFlowElec_Grid + vGFxElec_Grid(2) <= 0
 cMaxGFxElec_Grid3#99:           - vMaxFlowElec_Grid + vGFxElec_Grid(3) <= 0
 cMaxGFxElec_Grid4#100:          - vMaxFlowElec_Grid + vGFxElec_Grid(4) <= 0
 cMaxGFxElec_Grid5#101:          - vMaxFlowElec_Grid + vGFxElec_Grid(5) <= 0
 cMaxGFxElec_Grid6#102:          - vMaxFlowElec_Grid + vGFxElec_Grid(6) <= 0
 cMaxGFxElec_Grid7#103:          - vMaxFlowElec_Grid + vGFxElec_Grid(7) <= 0
 cMaxGFxElec_Grid8#104:          - vMaxFlowElec_Grid + vGFxElec_Grid(8) <= 0
 cMaxGFxElec_Grid9#105:          - vMaxFlowElec_Grid + vGFxElec_Grid(9) <= 0
 csInstalledElec_Load0#106:      vInstalledElec_Load  = 1
 csWeightElec_Load0#107:         vWeightElec_Load  = 1
 csBigMInstalledElec_Load0#108:  - vInstalledElec_Load + vWeightElec_Load <= 0
 csMinSizeInstalledElec_Load0#109: vWeightElec_Load >= 0
 cMSFxElec_Load0#110:            <= 10000
 cMSFxElec_Load1#111:            <= 10000
 cMSFxElec_Load2#112:            <= 10000
 cMSFxElec_Load3#113:            <= 10000
 cMSFxElec_Load4#114:            <= 10000
 cMSFxElec_Load5#115:            <= 10000
 cMSFxElec_Load6#116:            <= 10000
 cMSFxElec_Load7#117:            <= 10000
 cMSFxElec_Load8#118:            <= 10000
 cMSFxElec_Load9#119:            <= 10000
 cSNodeLaw#10#120:               - vPowInConverter(0) + vGFxElec_Grid(0)  = 0
 cSNodeLaw#11#121:               - vPowInConverter(1) + vGFxElec_Grid(1)  = 0
 cSNodeLaw#12#122:               - vPowInConverter(2) + vGFxElec_Grid(2)  = 0
 cSNodeLaw#13#123:               - vPowInConverter(3) + vGFxElec_Grid(3)  = 0
 cSNodeLaw#14#124:               - vPowInConverter(4) + vGFxElec_Grid(4)  = 0
 cSNodeLaw#15#125:               - vPowInConverter(5) + vGFxElec_Grid(5)  = 0
 cSNodeLaw#16#126:               - vPowInConverter(6) + vGFxElec_Grid(6)  = 0
 cSNodeLaw#17#127:               - vPowInConverter(7) + vGFxElec_Grid(7)  = 0
 cSNodeLaw#18#128:               - vPowInConverter(8) + vGFxElec_Grid(8)  = 0
 cSNodeLaw#19#129:               - vPowInConverter(9) + vGFxElec_Grid(9)  = 0
 cSNodeLaw#20#130:               vPowOutConverter(0) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#21#131:               vPowOutConverter(1) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#22#132:               vPowOutConverter(2) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#23#133:               vPowOutConverter(3) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#24#134:               vPowOutConverter(4) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#25#135:               vPowOutConverter(5) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#26#136:               vPowOutConverter(6) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#27#137:               vPowOutConverter(7) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#28#138:               vPowOutConverter(8) - 400 vWeightElec_Load
                                  = 0
 cSNodeLaw#29#139:               vPowOutConverter(9) - 400 vWeightElec_Load
                                  = 0
Bounds
 0 <= vInstalledConverter <= 1
 0 <= vMaxPowerConverter <= 1000
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
 0 <= vPowInConverter(0) <= 1000
 0 <= vPowInConverter(1) <= 1000
 0 <= vPowInConverter(2) <= 1000
 0 <= vPowInConverter(3) <= 1000
 0 <= vPowInConverter(4) <= 1000
 0 <= vPowInConverter(5) <= 1000
 0 <= vPowInConverter(6) <= 1000
 0 <= vPowInConverter(7) <= 1000
 0 <= vPowInConverter(8) <= 1000
 0 <= vPowInConverter(9) <= 1000
 0 <= vPowOutConverter(0) <= 800
 0 <= vPowOutConverter(1) <= 800
 0 <= vPowOutConverter(2) <= 800
 0 <= vPowOutConverter(3) <= 800
 0 <= vPowOutConverter(4) <= 800
 0 <= vPowOutConverter(5) <= 800
 0 <= vPowOutConverter(6) <= 800
 0 <= vPowOutConverter(7) <= 800
 0 <= vPowOutConverter(8) <= 800
 0 <= vPowOutConverter(9) <= 800
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
 0 <= vInstalledElec_Grid <= 1
 0 <= vMaxFlowElec_Grid <= 10000
 0 <= vStateElec_Grid(0) <= 1
 0 <= vStateElec_Grid(1) <= 1
 0 <= vStateElec_Grid(2) <= 1
 0 <= vStateElec_Grid(3) <= 1
 0 <= vStateElec_Grid(4) <= 1
 0 <= vStateElec_Grid(5) <= 1
 0 <= vStateElec_Grid(6) <= 1
 0 <= vStateElec_Grid(7) <= 1
 0 <= vStateElec_Grid(8) <= 1
 0 <= vStateElec_Grid(9) <= 1
 0 <= vGFxElec_Grid(0) <= 10000
 0 <= vGFxElec_Grid(1) <= 10000
 0 <= vGFxElec_Grid(2) <= 10000
 0 <= vGFxElec_Grid(3) <= 10000
 0 <= vGFxElec_Grid(4) <= 10000
 0 <= vGFxElec_Grid(5) <= 10000
 0 <= vGFxElec_Grid(6) <= 10000
 0 <= vGFxElec_Grid(7) <= 10000
 0 <= vGFxElec_Grid(8) <= 10000
 0 <= vGFxElec_Grid(9) <= 10000
 0 <= vInstalledElec_Load <= 1
 0 <= vWeightElec_Load <= 1
Generals
 vInstalledConverter  vStateConverter(0)  vStateConverter(1) 
 vStateConverter(2)  vStateConverter(3)  vStateConverter(4) 
 vStateConverter(5)  vStateConverter(6)  vStateConverter(7) 
 vStateConverter(8)  vStateConverter(9)  vInstalledElec_Grid 
 vStateElec_Grid(0)  vStateElec_Grid(1)  vStateElec_Grid(2) 
 vStateElec_Grid(3)  vStateElec_Grid(4)  vStateElec_Grid(5) 
 vStateElec_Grid(6)  vStateElec_Grid(7)  vStateElec_Grid(8) 
 vStateElec_Grid(9)  vInstalledElec_Load 
End
