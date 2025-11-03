\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 17520 vGFxCH4_Grid(0) + 17520 vGFxCH4_Grid(1) + 17520 vGFxCH4_Grid(2)
       + 17520 vGFxCH4_Grid(3) + 17520 vGFxCH4_Grid(4) + 17520 vGFxCH4_Grid(5)
       + 17520 vGFxCH4_Grid(6) + 17520 vGFxCH4_Grid(7) + 17520 vGFxCH4_Grid(8)
       + 17520 vGFxCH4_Grid(9) - 1752000 vGFxElec_Grid(0)
       - 1752000 vGFxElec_Grid(1) + 3504000 vGFxElec_Grid(2)
       - 1752000 vGFxElec_Grid(3) - 1752000 vGFxElec_Grid(4)
       - 1752000 vGFxElec_Grid(5) - 1752000 vGFxElec_Grid(6)
       - 1752000 vGFxElec_Grid(7) - 1752000 vGFxElec_Grid(8)
       - 1752000 vGFxElec_Grid(9) + 17520000 vStartUpMinStateTime(0)
       + 17520000 vStartUpMinStateTime(1) + 17520000 vStartUpMinStateTime(2)
       + 17520000 vStartUpMinStateTime(3) + 17520000 vStartUpMinStateTime(4)
       + 17520000 vStartUpMinStateTime(5) + 17520000 vStartUpMinStateTime(6)
       + 17520000 vStartUpMinStateTime(7) + 17520000 vStartUpMinStateTime(8)
       + 17520000 vStartUpMinStateTime(9) + 1752000 vShutDownMinStateTime(0)
       + 1752000 vShutDownMinStateTime(1) + 1752000 vShutDownMinStateTime(2)
       + 1752000 vShutDownMinStateTime(3) + 1752000 vShutDownMinStateTime(4)
       + 1752000 vShutDownMinStateTime(5) + 1752000 vShutDownMinStateTime(6)
       + 1752000 vShutDownMinStateTime(7) + 1752000 vShutDownMinStateTime(8)
       + 1752000 vShutDownMinStateTime(9)
Subject To
 csInstalledCH4_Grid0#0:         vInstalledCH4_Grid  = 1
 csMaxValCH4_Grid0#1:            vMaxFlowCH4_Grid  = 10000
 csBigMInstalledCH4_Grid0#2:     - 10000 vInstalledCH4_Grid + vMaxFlowCH4_Grid
                                 <= 0
 csMinSizeInstalledCH4_Grid0#3:  vMaxFlowCH4_Grid >= 0
 cNotRunningIfNotInstalledCH4_Grid0#4: - vInstalledCH4_Grid + vStateCH4_Grid(0)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid1#5: - vInstalledCH4_Grid + vStateCH4_Grid(1)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid2#6: - vInstalledCH4_Grid + vStateCH4_Grid(2)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid3#7: - vInstalledCH4_Grid + vStateCH4_Grid(3)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid4#8: - vInstalledCH4_Grid + vStateCH4_Grid(4)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid5#9: - vInstalledCH4_Grid + vStateCH4_Grid(5)
                                 <= 0
 cNotRunningIfNotInstalledCH4_Grid6#10: - vInstalledCH4_Grid
                                 + vStateCH4_Grid(6) <= 0
 cNotRunningIfNotInstalledCH4_Grid7#11: - vInstalledCH4_Grid
                                 + vStateCH4_Grid(7) <= 0
 cNotRunningIfNotInstalledCH4_Grid8#12: - vInstalledCH4_Grid
                                 + vStateCH4_Grid(8) <= 0
 cNotRunningIfNotInstalledCH4_Grid9#13: - vInstalledCH4_Grid
                                 + vStateCH4_Grid(9) <= 0
 cStateGridCH4_Grid0#14:         - 10000 vStateCH4_Grid(0) + vGFxCH4_Grid(0)
                                 <= 0
 cStateGridCH4_Grid1#15:         - 10000 vStateCH4_Grid(1) + vGFxCH4_Grid(1)
                                 <= 0
 cStateGridCH4_Grid2#16:         - 10000 vStateCH4_Grid(2) + vGFxCH4_Grid(2)
                                 <= 0
 cStateGridCH4_Grid3#17:         - 10000 vStateCH4_Grid(3) + vGFxCH4_Grid(3)
                                 <= 0
 cStateGridCH4_Grid4#18:         - 10000 vStateCH4_Grid(4) + vGFxCH4_Grid(4)
                                 <= 0
 cStateGridCH4_Grid5#19:         - 10000 vStateCH4_Grid(5) + vGFxCH4_Grid(5)
                                 <= 0
 cStateGridCH4_Grid6#20:         - 10000 vStateCH4_Grid(6) + vGFxCH4_Grid(6)
                                 <= 0
 cStateGridCH4_Grid7#21:         - 10000 vStateCH4_Grid(7) + vGFxCH4_Grid(7)
                                 <= 0
 cStateGridCH4_Grid8#22:         - 10000 vStateCH4_Grid(8) + vGFxCH4_Grid(8)
                                 <= 0
 cStateGridCH4_Grid9#23:         - 10000 vStateCH4_Grid(9) + vGFxCH4_Grid(9)
                                 <= 0
 cMaxGFxCH4_Grid0#24:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(0) <= 0
 cMaxGFxCH4_Grid1#25:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(1) <= 0
 cMaxGFxCH4_Grid2#26:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(2) <= 0
 cMaxGFxCH4_Grid3#27:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(3) <= 0
 cMaxGFxCH4_Grid4#28:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(4) <= 0
 cMaxGFxCH4_Grid5#29:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(5) <= 0
 cMaxGFxCH4_Grid6#30:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(6) <= 0
 cMaxGFxCH4_Grid7#31:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(7) <= 0
 cMaxGFxCH4_Grid8#32:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(8) <= 0
 cMaxGFxCH4_Grid9#33:            - vMaxFlowCH4_Grid + vGFxCH4_Grid(9) <= 0
 csInstalledConverter0#34:       vInstalledConverter  = 1
 csMaxValConverter0#35:          vMaxPowerConverter  = 10
 csBigMInstalledConverter0#36:   - 10 vInstalledConverter + vMaxPowerConverter
                                 <= 0
 csMinSizeInstalledConverter0#37: vMaxPowerConverter >= 0
 cNotRunningIfNotInstalledConverter0#38: - vInstalledConverter
                                 + vStateConverter(0) <= 0
 cNotRunningIfNotInstalledConverter1#39: - vInstalledConverter
                                 + vStateConverter(1) <= 0
 cNotRunningIfNotInstalledConverter2#40: - vInstalledConverter
                                 + vStateConverter(2) <= 0
 cNotRunningIfNotInstalledConverter3#41: - vInstalledConverter
                                 + vStateConverter(3) <= 0
 cNotRunningIfNotInstalledConverter4#42: - vInstalledConverter
                                 + vStateConverter(4) <= 0
 cNotRunningIfNotInstalledConverter5#43: - vInstalledConverter
                                 + vStateConverter(5) <= 0
 cNotRunningIfNotInstalledConverter6#44: - vInstalledConverter
                                 + vStateConverter(6) <= 0
 cNotRunningIfNotInstalledConverter7#45: - vInstalledConverter
                                 + vStateConverter(7) <= 0
 cNotRunningIfNotInstalledConverter8#46: - vInstalledConverter
                                 + vStateConverter(8) <= 0
 cNotRunningIfNotInstalledConverter9#47: - vInstalledConverter
                                 + vStateConverter(9) <= 0
 cPowerInMaxBoundConverter0#48:  vPowInConverter(0) <= 10
 cPowerInMaxBoundConverter0#49:  vPowInConverter(1) <= 10
 cPowerInMaxBoundConverter0#50:  vPowInConverter(2) <= 10
 cPowerInMaxBoundConverter0#51:  vPowInConverter(3) <= 10
 cPowerInMaxBoundConverter0#52:  vPowInConverter(4) <= 10
 cPowerInMaxBoundConverter0#53:  vPowInConverter(5) <= 10
 cPowerInMaxBoundConverter0#54:  vPowInConverter(6) <= 10
 cPowerInMaxBoundConverter0#55:  vPowInConverter(7) <= 10
 cPowerInMaxBoundConverter0#56:  vPowInConverter(8) <= 10
 cPowerInMaxBoundConverter0#57:  vPowInConverter(9) <= 10
 cPowMaxConverter0#58:           - 10 vStateConverter(0) + vPowInConverter(0)
                                 <= 0
 cDefPminConverter0#59:          0.2 vMaxPowerConverter - vPminConverter(0)
                                  = 0
 cPminConverter0#60:             - 2 vStateConverter(0) + vPowInConverter(0)
                                 - vPminConverter(0) >= -2
 cPowMaxConverter1#61:           - 10 vStateConverter(1) + vPowInConverter(1)
                                 <= 0
 cDefPminConverter1#62:          0.2 vMaxPowerConverter - vPminConverter(1)
                                  = 0
 cPminConverter1#63:             - 2 vStateConverter(1) + vPowInConverter(1)
                                 - vPminConverter(1) >= -2
 cPowMaxConverter2#64:           - 10 vStateConverter(2) + vPowInConverter(2)
                                 <= 0
 cDefPminConverter2#65:          0.2 vMaxPowerConverter - vPminConverter(2)
                                  = 0
 cPminConverter2#66:             - 2 vStateConverter(2) + vPowInConverter(2)
                                 - vPminConverter(2) >= -2
 cPowMaxConverter3#67:           - 10 vStateConverter(3) + vPowInConverter(3)
                                 <= 0
 cDefPminConverter3#68:          0.2 vMaxPowerConverter - vPminConverter(3)
                                  = 0
 cPminConverter3#69:             - 2 vStateConverter(3) + vPowInConverter(3)
                                 - vPminConverter(3) >= -2
 cPowMaxConverter4#70:           - 10 vStateConverter(4) + vPowInConverter(4)
                                 <= 0
 cDefPminConverter4#71:          0.2 vMaxPowerConverter - vPminConverter(4)
                                  = 0
 cPminConverter4#72:             - 2 vStateConverter(4) + vPowInConverter(4)
                                 - vPminConverter(4) >= -2
 cPowMaxConverter5#73:           - 10 vStateConverter(5) + vPowInConverter(5)
                                 <= 0
 cDefPminConverter5#74:          0.2 vMaxPowerConverter - vPminConverter(5)
                                  = 0
 cPminConverter5#75:             - 2 vStateConverter(5) + vPowInConverter(5)
                                 - vPminConverter(5) >= -2
 cPowMaxConverter6#76:           - 10 vStateConverter(6) + vPowInConverter(6)
                                 <= 0
 cDefPminConverter6#77:          0.2 vMaxPowerConverter - vPminConverter(6)
                                  = 0
 cPminConverter6#78:             - 2 vStateConverter(6) + vPowInConverter(6)
                                 - vPminConverter(6) >= -2
 cPowMaxConverter7#79:           - 10 vStateConverter(7) + vPowInConverter(7)
                                 <= 0
 cDefPminConverter7#80:          0.2 vMaxPowerConverter - vPminConverter(7)
                                  = 0
 cPminConverter7#81:             - 2 vStateConverter(7) + vPowInConverter(7)
                                 - vPminConverter(7) >= -2
 cPowMaxConverter8#82:           - 10 vStateConverter(8) + vPowInConverter(8)
                                 <= 0
 cDefPminConverter8#83:          0.2 vMaxPowerConverter - vPminConverter(8)
                                  = 0
 cPminConverter8#84:             - 2 vStateConverter(8) + vPowInConverter(8)
                                 - vPminConverter(8) >= -2
 cPowMaxConverter9#85:           - 10 vStateConverter(9) + vPowInConverter(9)
                                 <= 0
 cDefPminConverter9#86:          0.2 vMaxPowerConverter - vPminConverter(9)
                                  = 0
 cPminConverter9#87:             - 2 vStateConverter(9) + vPowInConverter(9)
                                 - vPminConverter(9) >= -2
 cPowEffConverter0#88:           vPowInConverter(0) - vPowOutConverter(0)  = 0
 cPowEffConverter1#89:           vPowInConverter(1) - vPowOutConverter(1)  = 0
 cPowEffConverter2#90:           vPowInConverter(2) - vPowOutConverter(2)  = 0
 cPowEffConverter3#91:           vPowInConverter(3) - vPowOutConverter(3)  = 0
 cPowEffConverter4#92:           vPowInConverter(4) - vPowOutConverter(4)  = 0
 cPowEffConverter5#93:           vPowInConverter(5) - vPowOutConverter(5)  = 0
 cPowEffConverter6#94:           vPowInConverter(6) - vPowOutConverter(6)  = 0
 cPowEffConverter7#95:           vPowInConverter(7) - vPowOutConverter(7)  = 0
 cPowEffConverter8#96:           vPowInConverter(8) - vPowOutConverter(8)  = 0
 cPowEffConverter9#97:           vPowInConverter(9) - vPowOutConverter(9)  = 0
 cPowVarConverter0#98:           - vMaxPowerConverter + vPowInConverter(0) <= 0
 cPowVarConverter1#99:           - vMaxPowerConverter + vPowInConverter(1) <= 0
 cPowVarConverter2#100:          - vMaxPowerConverter + vPowInConverter(2) <= 0
 cPowVarConverter3#101:          - vMaxPowerConverter + vPowInConverter(3) <= 0
 cPowVarConverter4#102:          - vMaxPowerConverter + vPowInConverter(4) <= 0
 cPowVarConverter5#103:          - vMaxPowerConverter + vPowInConverter(5) <= 0
 cPowVarConverter6#104:          - vMaxPowerConverter + vPowInConverter(6) <= 0
 cPowVarConverter7#105:          - vMaxPowerConverter + vPowInConverter(7) <= 0
 cPowVarConverter8#106:          - vMaxPowerConverter + vPowInConverter(8) <= 0
 cPowVarConverter9#107:          - vMaxPowerConverter + vPowInConverter(9) <= 0
 csInstalledElec_Grid0#108:      vInstalledElec_Grid  = 1
 csMaxValElec_Grid0#109:         vMaxFlowElec_Grid  = 10000
 csBigMInstalledElec_Grid0#110:  - 10000 vInstalledElec_Grid
                                 + vMaxFlowElec_Grid <= 0
 csMinSizeInstalledElec_Grid0#111: vMaxFlowElec_Grid >= 0
 cNotRunningIfNotInstalledElec_Grid0#112: - vInstalledElec_Grid
                                 + vStateElec_Grid(0) <= 0
 cNotRunningIfNotInstalledElec_Grid1#113: - vInstalledElec_Grid
                                 + vStateElec_Grid(1) <= 0
 cNotRunningIfNotInstalledElec_Grid2#114: - vInstalledElec_Grid
                                 + vStateElec_Grid(2) <= 0
 cNotRunningIfNotInstalledElec_Grid3#115: - vInstalledElec_Grid
                                 + vStateElec_Grid(3) <= 0
 cNotRunningIfNotInstalledElec_Grid4#116: - vInstalledElec_Grid
                                 + vStateElec_Grid(4) <= 0
 cNotRunningIfNotInstalledElec_Grid5#117: - vInstalledElec_Grid
                                 + vStateElec_Grid(5) <= 0
 cNotRunningIfNotInstalledElec_Grid6#118: - vInstalledElec_Grid
                                 + vStateElec_Grid(6) <= 0
 cNotRunningIfNotInstalledElec_Grid7#119: - vInstalledElec_Grid
                                 + vStateElec_Grid(7) <= 0
 cNotRunningIfNotInstalledElec_Grid8#120: - vInstalledElec_Grid
                                 + vStateElec_Grid(8) <= 0
 cNotRunningIfNotInstalledElec_Grid9#121: - vInstalledElec_Grid
                                 + vStateElec_Grid(9) <= 0
 cStateGridElec_Grid0#122:       - 10000 vStateElec_Grid(0) + vGFxElec_Grid(0)
                                 <= 0
 cStateGridElec_Grid1#123:       - 10000 vStateElec_Grid(1) + vGFxElec_Grid(1)
                                 <= 0
 cStateGridElec_Grid2#124:       - 10000 vStateElec_Grid(2) + vGFxElec_Grid(2)
                                 <= 0
 cStateGridElec_Grid3#125:       - 10000 vStateElec_Grid(3) + vGFxElec_Grid(3)
                                 <= 0
 cStateGridElec_Grid4#126:       - 10000 vStateElec_Grid(4) + vGFxElec_Grid(4)
                                 <= 0
 cStateGridElec_Grid5#127:       - 10000 vStateElec_Grid(5) + vGFxElec_Grid(5)
                                 <= 0
 cStateGridElec_Grid6#128:       - 10000 vStateElec_Grid(6) + vGFxElec_Grid(6)
                                 <= 0
 cStateGridElec_Grid7#129:       - 10000 vStateElec_Grid(7) + vGFxElec_Grid(7)
                                 <= 0
 cStateGridElec_Grid8#130:       - 10000 vStateElec_Grid(8) + vGFxElec_Grid(8)
                                 <= 0
 cStateGridElec_Grid9#131:       - 10000 vStateElec_Grid(9) + vGFxElec_Grid(9)
                                 <= 0
 cMaxGFxElec_Grid0#132:          - vMaxFlowElec_Grid + vGFxElec_Grid(0) <= 0
 cMaxGFxElec_Grid1#133:          - vMaxFlowElec_Grid + vGFxElec_Grid(1) <= 0
 cMaxGFxElec_Grid2#134:          - vMaxFlowElec_Grid + vGFxElec_Grid(2) <= 0
 cMaxGFxElec_Grid3#135:          - vMaxFlowElec_Grid + vGFxElec_Grid(3) <= 0
 cMaxGFxElec_Grid4#136:          - vMaxFlowElec_Grid + vGFxElec_Grid(4) <= 0
 cMaxGFxElec_Grid5#137:          - vMaxFlowElec_Grid + vGFxElec_Grid(5) <= 0
 cMaxGFxElec_Grid6#138:          - vMaxFlowElec_Grid + vGFxElec_Grid(6) <= 0
 cMaxGFxElec_Grid7#139:          - vMaxFlowElec_Grid + vGFxElec_Grid(7) <= 0
 cMaxGFxElec_Grid8#140:          - vMaxFlowElec_Grid + vGFxElec_Grid(8) <= 0
 cMaxGFxElec_Grid9#141:          - vMaxFlowElec_Grid + vGFxElec_Grid(9) <= 0
 cNotRunningIfNotInstalledMinStateTime0#142: vStateMinStateTime(0) <= 1
 cNotRunningIfNotInstalledMinStateTime1#143: vStateMinStateTime(1) <= 1
 cNotRunningIfNotInstalledMinStateTime2#144: vStateMinStateTime(2) <= 1
 cNotRunningIfNotInstalledMinStateTime3#145: vStateMinStateTime(3) <= 1
 cNotRunningIfNotInstalledMinStateTime4#146: vStateMinStateTime(4) <= 1
 cNotRunningIfNotInstalledMinStateTime5#147: vStateMinStateTime(5) <= 1
 cNotRunningIfNotInstalledMinStateTime6#148: vStateMinStateTime(6) <= 1
 cNotRunningIfNotInstalledMinStateTime7#149: vStateMinStateTime(7) <= 1
 cNotRunningIfNotInstalledMinStateTime8#150: vStateMinStateTime(8) <= 1
 cNotRunningIfNotInstalledMinStateTime9#151: vStateMinStateTime(9) <= 1
 cNoStartUpIfNotInstalledMinStateTime0#152: vStartUpMinStateTime(0) <= 1
 cNoShutDownIfNotInstalledMinStateTime0#153: vShutDownMinStateTime(0) <= 1
 cStateUpMinStateTime0#154:      - vStateMinStateTime(0)
                                 + vStartUpMinStateTime(0) <= 0
 cStateDownMinStateTime0#155:    vStateMinStateTime(0)
                                 + vShutDownMinStateTime(0) <= 1
 cStatesUpDownMinStateTime0#156: vStateMinStateTime(0)
                                 - vStartUpMinStateTime(0)
                                 + vShutDownMinStateTime(0)  = 0
 cNoStartUpIfNotInstalledMinStateTime1#157: vStartUpMinStateTime(1) <= 1
 cNoShutDownIfNotInstalledMinStateTime1#158: vShutDownMinStateTime(1) <= 1
 cStateUpMinStateTime1#159:      - vStateMinStateTime(1)
                                 + vStartUpMinStateTime(1) <= 0
 cStateDownMinStateTime1#160:    vStateMinStateTime(1)
                                 + vShutDownMinStateTime(1) <= 1
 cStatesUpDownMinStateTime1#161: - vStateMinStateTime(0)
                                 + vStateMinStateTime(1)
                                 - vStartUpMinStateTime(1)
                                 + vShutDownMinStateTime(1)  = 0
 cNoStartUpIfNotInstalledMinStateTime2#162: vStartUpMinStateTime(2) <= 1
 cNoShutDownIfNotInstalledMinStateTime2#163: vShutDownMinStateTime(2) <= 1
 cStateUpMinStateTime2#164:      - vStateMinStateTime(2)
                                 + vStartUpMinStateTime(2) <= 0
 cStateDownMinStateTime2#165:    vStateMinStateTime(2)
                                 + vShutDownMinStateTime(2) <= 1
 cStatesUpDownMinStateTime2#166: - vStateMinStateTime(1)
                                 + vStateMinStateTime(2)
                                 - vStartUpMinStateTime(2)
                                 + vShutDownMinStateTime(2)  = 0
 cNoStartUpIfNotInstalledMinStateTime3#167: vStartUpMinStateTime(3) <= 1
 cNoShutDownIfNotInstalledMinStateTime3#168: vShutDownMinStateTime(3) <= 1
 cStateUpMinStateTime3#169:      - vStateMinStateTime(3)
                                 + vStartUpMinStateTime(3) <= 0
 cStateDownMinStateTime3#170:    vStateMinStateTime(3)
                                 + vShutDownMinStateTime(3) <= 1
 cStatesUpDownMinStateTime3#171: - vStateMinStateTime(2)
                                 + vStateMinStateTime(3)
                                 - vStartUpMinStateTime(3)
                                 + vShutDownMinStateTime(3)  = 0
 cNoStartUpIfNotInstalledMinStateTime4#172: vStartUpMinStateTime(4) <= 1
 cNoShutDownIfNotInstalledMinStateTime4#173: vShutDownMinStateTime(4) <= 1
 cStateUpMinStateTime4#174:      - vStateMinStateTime(4)
                                 + vStartUpMinStateTime(4) <= 0
 cStateDownMinStateTime4#175:    vStateMinStateTime(4)
                                 + vShutDownMinStateTime(4) <= 1
 cStatesUpDownMinStateTime4#176: - vStateMinStateTime(3)
                                 + vStateMinStateTime(4)
                                 - vStartUpMinStateTime(4)
                                 + vShutDownMinStateTime(4)  = 0
 cNoStartUpIfNotInstalledMinStateTime5#177: vStartUpMinStateTime(5) <= 1
 cNoShutDownIfNotInstalledMinStateTime5#178: vShutDownMinStateTime(5) <= 1
 cStateUpMinStateTime5#179:      - vStateMinStateTime(5)
                                 + vStartUpMinStateTime(5) <= 0
 cStateDownMinStateTime5#180:    vStateMinStateTime(5)
                                 + vShutDownMinStateTime(5) <= 1
 cStatesUpDownMinStateTime5#181: - vStateMinStateTime(4)
                                 + vStateMinStateTime(5)
                                 - vStartUpMinStateTime(5)
                                 + vShutDownMinStateTime(5)  = 0
 cNoStartUpIfNotInstalledMinStateTime6#182: vStartUpMinStateTime(6) <= 1
 cNoShutDownIfNotInstalledMinStateTime6#183: vShutDownMinStateTime(6) <= 1
 cStateUpMinStateTime6#184:      - vStateMinStateTime(6)
                                 + vStartUpMinStateTime(6) <= 0
 cStateDownMinStateTime6#185:    vStateMinStateTime(6)
                                 + vShutDownMinStateTime(6) <= 1
 cStatesUpDownMinStateTime6#186: - vStateMinStateTime(5)
                                 + vStateMinStateTime(6)
                                 - vStartUpMinStateTime(6)
                                 + vShutDownMinStateTime(6)  = 0
 cNoStartUpIfNotInstalledMinStateTime7#187: vStartUpMinStateTime(7) <= 1
 cNoShutDownIfNotInstalledMinStateTime7#188: vShutDownMinStateTime(7) <= 1
 cStateUpMinStateTime7#189:      - vStateMinStateTime(7)
                                 + vStartUpMinStateTime(7) <= 0
 cStateDownMinStateTime7#190:    vStateMinStateTime(7)
                                 + vShutDownMinStateTime(7) <= 1
 cStatesUpDownMinStateTime7#191: - vStateMinStateTime(6)
                                 + vStateMinStateTime(7)
                                 - vStartUpMinStateTime(7)
                                 + vShutDownMinStateTime(7)  = 0
 cNoStartUpIfNotInstalledMinStateTime8#192: vStartUpMinStateTime(8) <= 1
 cNoShutDownIfNotInstalledMinStateTime8#193: vShutDownMinStateTime(8) <= 1
 cStateUpMinStateTime8#194:      - vStateMinStateTime(8)
                                 + vStartUpMinStateTime(8) <= 0
 cStateDownMinStateTime8#195:    vStateMinStateTime(8)
                                 + vShutDownMinStateTime(8) <= 1
 cStatesUpDownMinStateTime8#196: - vStateMinStateTime(7)
                                 + vStateMinStateTime(8)
                                 - vStartUpMinStateTime(8)
                                 + vShutDownMinStateTime(8)  = 0
 cNoStartUpIfNotInstalledMinStateTime9#197: vStartUpMinStateTime(9) <= 1
 cNoShutDownIfNotInstalledMinStateTime9#198: vShutDownMinStateTime(9) <= 1
 cStateUpMinStateTime9#199:      - vStateMinStateTime(9)
                                 + vStartUpMinStateTime(9) <= 0
 cStateDownMinStateTime9#200:    vStateMinStateTime(9)
                                 + vShutDownMinStateTime(9) <= 1
 cStatesUpDownMinStateTime9#201: - vStateMinStateTime(8)
                                 + vStateMinStateTime(9)
                                 - vStartUpMinStateTime(9)
                                 + vShutDownMinStateTime(9)  = 0
 cMinProdTimeMinStateTime0#202:  - vStateMinStateTime(0)
                                 + vStartUpMinStateTime(0) <= 0
 cMinProdTimeMinStateTime1#203:  - vStateMinStateTime(1)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1) <= 0
 cMinProdTimeMinStateTime2#204:  - vStateMinStateTime(2)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2) <= 0
 cMinProdTimeMinStateTime3#205:  - vStateMinStateTime(3)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3) <= 0
 cMinProdTimeMinStateTime4#206:  - vStateMinStateTime(4)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4) <= 0
 cMinProdTimeMinStateTime5#207:  - vStateMinStateTime(5)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4)
                                 + vStartUpMinStateTime(5) <= 0
 cMinProdTimeMinStateTime6#208:  - vStateMinStateTime(6)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4)
                                 + vStartUpMinStateTime(5)
                                 + vStartUpMinStateTime(6) <= 0
 cMinProdTimeMinStateTime7#209:  - vStateMinStateTime(7)
                                 + vStartUpMinStateTime(0)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4)
                                 + vStartUpMinStateTime(5)
                                 + vStartUpMinStateTime(6)
                                 + vStartUpMinStateTime(7) <= 0
 cMinProdTimeMinStateTime8#210:  - vStateMinStateTime(8)
                                 + vStartUpMinStateTime(1)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4)
                                 + vStartUpMinStateTime(5)
                                 + vStartUpMinStateTime(6)
                                 + vStartUpMinStateTime(7)
                                 + vStartUpMinStateTime(8) <= 0
 cMinProdTimeMinStateTime9#211:  - vStateMinStateTime(9)
                                 + vStartUpMinStateTime(2)
                                 + vStartUpMinStateTime(3)
                                 + vStartUpMinStateTime(4)
                                 + vStartUpMinStateTime(5)
                                 + vStartUpMinStateTime(6)
                                 + vStartUpMinStateTime(7)
                                 + vStartUpMinStateTime(8)
                                 + vStartUpMinStateTime(9) <= 0
 cMinStandByTimeMinStateTime0#212: vStateMinStateTime(0)
                                 + vShutDownMinStateTime(0) <= 1
 cMinStandByTimeMinStateTime1#213: vStateMinStateTime(1)
                                 + vShutDownMinStateTime(0)
                                 + vShutDownMinStateTime(1) <= 1
 cMinStandByTimeMinStateTime2#214: vStateMinStateTime(2)
                                 + vShutDownMinStateTime(0)
                                 + vShutDownMinStateTime(1)
                                 + vShutDownMinStateTime(2) <= 1
 cMinStandByTimeMinStateTime3#215: vStateMinStateTime(3)
                                 + vShutDownMinStateTime(0)
                                 + vShutDownMinStateTime(1)
                                 + vShutDownMinStateTime(2)
                                 + vShutDownMinStateTime(3) <= 1
 cMinStandByTimeMinStateTime4#216: vStateMinStateTime(4)
                                 + vShutDownMinStateTime(0)
                                 + vShutDownMinStateTime(1)
                                 + vShutDownMinStateTime(2)
                                 + vShutDownMinStateTime(3)
                                 + vShutDownMinStateTime(4) <= 1
 cMinStandByTimeMinStateTime5#217: vStateMinStateTime(5)
                                 + vShutDownMinStateTime(0)
                                 + vShutDownMinStateTime(1)
                                 + vShutDownMinStateTime(2)
                                 + vShutDownMinStateTime(3)
                                 + vShutDownMinStateTime(4)
                                 + vShutDownMinStateTime(5) <= 1
 cMinStandByTimeMinStateTime6#218: vStateMinStateTime(6)
                                 + vShutDownMinStateTime(1)
                                 + vShutDownMinStateTime(2)
                                 + vShutDownMinStateTime(3)
                                 + vShutDownMinStateTime(4)
                                 + vShutDownMinStateTime(5)
                                 + vShutDownMinStateTime(6) <= 1
 cMinStandByTimeMinStateTime7#219: vStateMinStateTime(7)
                                 + vShutDownMinStateTime(2)
                                 + vShutDownMinStateTime(3)
                                 + vShutDownMinStateTime(4)
                                 + vShutDownMinStateTime(5)
                                 + vShutDownMinStateTime(6)
                                 + vShutDownMinStateTime(7) <= 1
 cMinStandByTimeMinStateTime8#220: vStateMinStateTime(8)
                                 + vShutDownMinStateTime(3)
                                 + vShutDownMinStateTime(4)
                                 + vShutDownMinStateTime(5)
                                 + vShutDownMinStateTime(6)
                                 + vShutDownMinStateTime(7)
                                 + vShutDownMinStateTime(8) <= 1
 cMinStandByTimeMinStateTime9#221: vStateMinStateTime(9)
                                 + vShutDownMinStateTime(4)
                                 + vShutDownMinStateTime(5)
                                 + vShutDownMinStateTime(6)
                                 + vShutDownMinStateTime(7)
                                 + vShutDownMinStateTime(8)
                                 + vShutDownMinStateTime(9) <= 1
 cENodeEquality#10#222:          - vStateConverter(0)
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#223:          - vStateConverter(1)
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#224:          - vStateConverter(2)
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#225:          - vStateConverter(3)
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#226:          - vStateConverter(4)
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#227:          - vStateConverter(5)
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#228:          - vStateConverter(6)
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#229:          - vStateConverter(7)
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#230:          - vStateConverter(8)
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#231:          - vStateConverter(9)
                                 + vBusValueNodeEquality#1(9)  = 0
 cENodeEquality#10#232:          - vStateMinStateTime(0)
                                 + vBusValueNodeEquality#1(0)  = 0
 cENodeEquality#11#233:          - vStateMinStateTime(1)
                                 + vBusValueNodeEquality#1(1)  = 0
 cENodeEquality#12#234:          - vStateMinStateTime(2)
                                 + vBusValueNodeEquality#1(2)  = 0
 cENodeEquality#13#235:          - vStateMinStateTime(3)
                                 + vBusValueNodeEquality#1(3)  = 0
 cENodeEquality#14#236:          - vStateMinStateTime(4)
                                 + vBusValueNodeEquality#1(4)  = 0
 cENodeEquality#15#237:          - vStateMinStateTime(5)
                                 + vBusValueNodeEquality#1(5)  = 0
 cENodeEquality#16#238:          - vStateMinStateTime(6)
                                 + vBusValueNodeEquality#1(6)  = 0
 cENodeEquality#17#239:          - vStateMinStateTime(7)
                                 + vBusValueNodeEquality#1(7)  = 0
 cENodeEquality#18#240:          - vStateMinStateTime(8)
                                 + vBusValueNodeEquality#1(8)  = 0
 cENodeEquality#19#241:          - vStateMinStateTime(9)
                                 + vBusValueNodeEquality#1(9)  = 0
 cSNodeLaw#10#242:               vGFxCH4_Grid(0) - vPowInConverter(0)  = 0
 cSNodeLaw#11#243:               vGFxCH4_Grid(1) - vPowInConverter(1)  = 0
 cSNodeLaw#12#244:               vGFxCH4_Grid(2) - vPowInConverter(2)  = 0
 cSNodeLaw#13#245:               vGFxCH4_Grid(3) - vPowInConverter(3)  = 0
 cSNodeLaw#14#246:               vGFxCH4_Grid(4) - vPowInConverter(4)  = 0
 cSNodeLaw#15#247:               vGFxCH4_Grid(5) - vPowInConverter(5)  = 0
 cSNodeLaw#16#248:               vGFxCH4_Grid(6) - vPowInConverter(6)  = 0
 cSNodeLaw#17#249:               vGFxCH4_Grid(7) - vPowInConverter(7)  = 0
 cSNodeLaw#18#250:               vGFxCH4_Grid(8) - vPowInConverter(8)  = 0
 cSNodeLaw#19#251:               vGFxCH4_Grid(9) - vPowInConverter(9)  = 0
 cSNodeLaw#20#252:               vPowOutConverter(0) - vGFxElec_Grid(0)  = 0
 cSNodeLaw#21#253:               vPowOutConverter(1) - vGFxElec_Grid(1)  = 0
 cSNodeLaw#22#254:               vPowOutConverter(2) - vGFxElec_Grid(2)  = 0
 cSNodeLaw#23#255:               vPowOutConverter(3) - vGFxElec_Grid(3)  = 0
 cSNodeLaw#24#256:               vPowOutConverter(4) - vGFxElec_Grid(4)  = 0
 cSNodeLaw#25#257:               vPowOutConverter(5) - vGFxElec_Grid(5)  = 0
 cSNodeLaw#26#258:               vPowOutConverter(6) - vGFxElec_Grid(6)  = 0
 cSNodeLaw#27#259:               vPowOutConverter(7) - vGFxElec_Grid(7)  = 0
 cSNodeLaw#28#260:               vPowOutConverter(8) - vGFxElec_Grid(8)  = 0
 cSNodeLaw#29#261:               vPowOutConverter(9) - vGFxElec_Grid(9)  = 0
Bounds
 0 <= vInstalledCH4_Grid <= 1
 0 <= vMaxFlowCH4_Grid <= 10000
      vStateCH4_Grid(0) = 1
      vStateCH4_Grid(1) = 1
      vStateCH4_Grid(2) = 1
      vStateCH4_Grid(3) = 1
      vStateCH4_Grid(4) = 1
      vStateCH4_Grid(5) = 1
      vStateCH4_Grid(6) = 1
      vStateCH4_Grid(7) = 1
      vStateCH4_Grid(8) = 1
      vStateCH4_Grid(9) = 1
 0 <= vGFxCH4_Grid(0) <= 10000
 0 <= vGFxCH4_Grid(1) <= 10000
 0 <= vGFxCH4_Grid(2) <= 10000
 0 <= vGFxCH4_Grid(3) <= 10000
 0 <= vGFxCH4_Grid(4) <= 10000
 0 <= vGFxCH4_Grid(5) <= 10000
 0 <= vGFxCH4_Grid(6) <= 10000
 0 <= vGFxCH4_Grid(7) <= 10000
 0 <= vGFxCH4_Grid(8) <= 10000
 0 <= vGFxCH4_Grid(9) <= 10000
 0 <= vInstalledConverter <= 1
 0 <= vMaxPowerConverter <= 10
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
 0 <= vPowInConverter(0) <= 10
 0 <= vPowInConverter(1) <= 10
 0 <= vPowInConverter(2) <= 10
 0 <= vPowInConverter(3) <= 10
 0 <= vPowInConverter(4) <= 10
 0 <= vPowInConverter(5) <= 10
 0 <= vPowInConverter(6) <= 10
 0 <= vPowInConverter(7) <= 10
 0 <= vPowInConverter(8) <= 10
 0 <= vPowInConverter(9) <= 10
 0 <= vPowOutConverter(0) <= 10
 0 <= vPowOutConverter(1) <= 10
 0 <= vPowOutConverter(2) <= 10
 0 <= vPowOutConverter(3) <= 10
 0 <= vPowOutConverter(4) <= 10
 0 <= vPowOutConverter(5) <= 10
 0 <= vPowOutConverter(6) <= 10
 0 <= vPowOutConverter(7) <= 10
 0 <= vPowOutConverter(8) <= 10
 0 <= vPowOutConverter(9) <= 10
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
      vStateElec_Grid(0) = 1
      vStateElec_Grid(1) = 1
      vStateElec_Grid(2) = 1
      vStateElec_Grid(3) = 1
      vStateElec_Grid(4) = 1
      vStateElec_Grid(5) = 1
      vStateElec_Grid(6) = 1
      vStateElec_Grid(7) = 1
      vStateElec_Grid(8) = 1
      vStateElec_Grid(9) = 1
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
 0 <= vStateMinStateTime(0) <= 1
 0 <= vStateMinStateTime(1) <= 1
 0 <= vStateMinStateTime(2) <= 1
 0 <= vStateMinStateTime(3) <= 1
 0 <= vStateMinStateTime(4) <= 1
 0 <= vStateMinStateTime(5) <= 1
 0 <= vStateMinStateTime(6) <= 1
 0 <= vStateMinStateTime(7) <= 1
 0 <= vStateMinStateTime(8) <= 1
 0 <= vStateMinStateTime(9) <= 1
 0 <= vStartUpMinStateTime(0) <= 1
 0 <= vStartUpMinStateTime(1) <= 1
 0 <= vStartUpMinStateTime(2) <= 1
 0 <= vStartUpMinStateTime(3) <= 1
 0 <= vStartUpMinStateTime(4) <= 1
 0 <= vStartUpMinStateTime(5) <= 1
 0 <= vStartUpMinStateTime(6) <= 1
 0 <= vStartUpMinStateTime(7) <= 1
 0 <= vStartUpMinStateTime(8) <= 1
 0 <= vStartUpMinStateTime(9) <= 1
 0 <= vShutDownMinStateTime(0) <= 1
 0 <= vShutDownMinStateTime(1) <= 1
 0 <= vShutDownMinStateTime(2) <= 1
 0 <= vShutDownMinStateTime(3) <= 1
 0 <= vShutDownMinStateTime(4) <= 1
 0 <= vShutDownMinStateTime(5) <= 1
 0 <= vShutDownMinStateTime(6) <= 1
 0 <= vShutDownMinStateTime(7) <= 1
 0 <= vShutDownMinStateTime(8) <= 1
 0 <= vShutDownMinStateTime(9) <= 1
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
 vInstalledCH4_Grid  vInstalledConverter  vStateConverter(0) 
 vStateConverter(1)  vStateConverter(2)  vStateConverter(3) 
 vStateConverter(4)  vStateConverter(5)  vStateConverter(6) 
 vStateConverter(7)  vStateConverter(8)  vStateConverter(9) 
 vInstalledElec_Grid  vStateMinStateTime(0)  vStateMinStateTime(1) 
 vStateMinStateTime(2)  vStateMinStateTime(3)  vStateMinStateTime(4) 
 vStateMinStateTime(5)  vStateMinStateTime(6)  vStateMinStateTime(7) 
 vStateMinStateTime(8)  vStateMinStateTime(9)  vStartUpMinStateTime(0) 
 vStartUpMinStateTime(1)  vStartUpMinStateTime(2)  vStartUpMinStateTime(3) 
 vStartUpMinStateTime(4)  vStartUpMinStateTime(5)  vStartUpMinStateTime(6) 
 vStartUpMinStateTime(7)  vStartUpMinStateTime(8)  vStartUpMinStateTime(9) 
 vShutDownMinStateTime(0)  vShutDownMinStateTime(1)  vShutDownMinStateTime(2) 
 vShutDownMinStateTime(3)  vShutDownMinStateTime(4)  vShutDownMinStateTime(5) 
 vShutDownMinStateTime(6)  vShutDownMinStateTime(7)  vShutDownMinStateTime(8) 
 vShutDownMinStateTime(9) 
End
