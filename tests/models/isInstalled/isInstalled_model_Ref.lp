\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 10 vMaxPowerConverter#2 + 10 vMaxPowerConverter#3
Subject To
 csBigMInstalledConverter#10#0:  - 10 vInstalledConverter#1
                                 + vMaxPowerConverter#1 <= 0
 csMinSizeInstalledConverter#10#1: vMaxPowerConverter#1 >= 0
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
 cPowerInMaxBoundConverter#10#12: vPowInConverter#1(0) <= 10
 cPowerInMaxBoundConverter#10#13: vPowInConverter#1(1) <= 10
 cPowerInMaxBoundConverter#10#14: vPowInConverter#1(2) <= 10
 cPowerInMaxBoundConverter#10#15: vPowInConverter#1(3) <= 10
 cPowerInMaxBoundConverter#10#16: vPowInConverter#1(4) <= 10
 cPowerInMaxBoundConverter#10#17: vPowInConverter#1(5) <= 10
 cPowerInMaxBoundConverter#10#18: vPowInConverter#1(6) <= 10
 cPowerInMaxBoundConverter#10#19: vPowInConverter#1(7) <= 10
 cPowerInMaxBoundConverter#10#20: vPowInConverter#1(8) <= 10
 cPowerInMaxBoundConverter#10#21: vPowInConverter#1(9) <= 10
 cPowMaxConverter#10#22:         - 10 vStateConverter#1(0)
                                 + vPowInConverter#1(0) <= 0
 cDefPminConverter#10#23:        - vPminConverter#1(0)  = 0
 cPminConverter#10#24:           vPowInConverter#1(0) - vPminConverter#1(0)
                                 >= 0
 cPowMaxConverter#11#25:         - 10 vStateConverter#1(1)
                                 + vPowInConverter#1(1) <= 0
 cDefPminConverter#11#26:        - vPminConverter#1(1)  = 0
 cPminConverter#11#27:           vPowInConverter#1(1) - vPminConverter#1(1)
                                 >= 0
 cPowMaxConverter#12#28:         - 10 vStateConverter#1(2)
                                 + vPowInConverter#1(2) <= 0
 cDefPminConverter#12#29:        - vPminConverter#1(2)  = 0
 cPminConverter#12#30:           vPowInConverter#1(2) - vPminConverter#1(2)
                                 >= 0
 cPowMaxConverter#13#31:         - 10 vStateConverter#1(3)
                                 + vPowInConverter#1(3) <= 0
 cDefPminConverter#13#32:        - vPminConverter#1(3)  = 0
 cPminConverter#13#33:           vPowInConverter#1(3) - vPminConverter#1(3)
                                 >= 0
 cPowMaxConverter#14#34:         - 10 vStateConverter#1(4)
                                 + vPowInConverter#1(4) <= 0
 cDefPminConverter#14#35:        - vPminConverter#1(4)  = 0
 cPminConverter#14#36:           vPowInConverter#1(4) - vPminConverter#1(4)
                                 >= 0
 cPowMaxConverter#15#37:         - 10 vStateConverter#1(5)
                                 + vPowInConverter#1(5) <= 0
 cDefPminConverter#15#38:        - vPminConverter#1(5)  = 0
 cPminConverter#15#39:           vPowInConverter#1(5) - vPminConverter#1(5)
                                 >= 0
 cPowMaxConverter#16#40:         - 10 vStateConverter#1(6)
                                 + vPowInConverter#1(6) <= 0
 cDefPminConverter#16#41:        - vPminConverter#1(6)  = 0
 cPminConverter#16#42:           vPowInConverter#1(6) - vPminConverter#1(6)
                                 >= 0
 cPowMaxConverter#17#43:         - 10 vStateConverter#1(7)
                                 + vPowInConverter#1(7) <= 0
 cDefPminConverter#17#44:        - vPminConverter#1(7)  = 0
 cPminConverter#17#45:           vPowInConverter#1(7) - vPminConverter#1(7)
                                 >= 0
 cPowMaxConverter#18#46:         - 10 vStateConverter#1(8)
                                 + vPowInConverter#1(8) <= 0
 cDefPminConverter#18#47:        - vPminConverter#1(8)  = 0
 cPminConverter#18#48:           vPowInConverter#1(8) - vPminConverter#1(8)
                                 >= 0
 cPowMaxConverter#19#49:         - 10 vStateConverter#1(9)
                                 + vPowInConverter#1(9) <= 0
 cDefPminConverter#19#50:        - vPminConverter#1(9)  = 0
 cPminConverter#19#51:           vPowInConverter#1(9) - vPminConverter#1(9)
                                 >= 0
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
 cPowVarConverter#10#62:         - vMaxPowerConverter#1 + vPowInConverter#1(0)
                                 <= 0
 cPowVarConverter#11#63:         - vMaxPowerConverter#1 + vPowInConverter#1(1)
                                 <= 0
 cPowVarConverter#12#64:         - vMaxPowerConverter#1 + vPowInConverter#1(2)
                                 <= 0
 cPowVarConverter#13#65:         - vMaxPowerConverter#1 + vPowInConverter#1(3)
                                 <= 0
 cPowVarConverter#14#66:         - vMaxPowerConverter#1 + vPowInConverter#1(4)
                                 <= 0
 cPowVarConverter#15#67:         - vMaxPowerConverter#1 + vPowInConverter#1(5)
                                 <= 0
 cPowVarConverter#16#68:         - vMaxPowerConverter#1 + vPowInConverter#1(6)
                                 <= 0
 cPowVarConverter#17#69:         - vMaxPowerConverter#1 + vPowInConverter#1(7)
                                 <= 0
 cPowVarConverter#18#70:         - vMaxPowerConverter#1 + vPowInConverter#1(8)
                                 <= 0
 cPowVarConverter#19#71:         - vMaxPowerConverter#1 + vPowInConverter#1(9)
                                 <= 0
 csMaxValConverter#20#72:        vMaxPowerConverter#2  = 0
 cnullInstalledSizeConverter#20#73: vInstalledConverter#2  = 0
 csBigMInstalledConverter#20#74: vMaxPowerConverter#2 <= 0
 csMinSizeInstalledConverter#20#75: vMaxPowerConverter#2 >= 0
 cNotRunningIfNotInstalledConverter#20#76: - vInstalledConverter#2
                                 + vStateConverter#2(0) <= 0
 cNotRunningIfNotInstalledConverter#21#77: - vInstalledConverter#2
                                 + vStateConverter#2(1) <= 0
 cNotRunningIfNotInstalledConverter#22#78: - vInstalledConverter#2
                                 + vStateConverter#2(2) <= 0
 cNotRunningIfNotInstalledConverter#23#79: - vInstalledConverter#2
                                 + vStateConverter#2(3) <= 0
 cNotRunningIfNotInstalledConverter#24#80: - vInstalledConverter#2
                                 + vStateConverter#2(4) <= 0
 cNotRunningIfNotInstalledConverter#25#81: - vInstalledConverter#2
                                 + vStateConverter#2(5) <= 0
 cNotRunningIfNotInstalledConverter#26#82: - vInstalledConverter#2
                                 + vStateConverter#2(6) <= 0
 cNotRunningIfNotInstalledConverter#27#83: - vInstalledConverter#2
                                 + vStateConverter#2(7) <= 0
 cNotRunningIfNotInstalledConverter#28#84: - vInstalledConverter#2
                                 + vStateConverter#2(8) <= 0
 cNotRunningIfNotInstalledConverter#29#85: - vInstalledConverter#2
                                 + vStateConverter#2(9) <= 0
 cPowerInMaxBoundConverter#20#86: vPowInConverter#2(0) <= 0
 cPowerInMaxBoundConverter#20#87: vPowInConverter#2(1) <= 0
 cPowerInMaxBoundConverter#20#88: vPowInConverter#2(2) <= 0
 cPowerInMaxBoundConverter#20#89: vPowInConverter#2(3) <= 0
 cPowerInMaxBoundConverter#20#90: vPowInConverter#2(4) <= 0
 cPowerInMaxBoundConverter#20#91: vPowInConverter#2(5) <= 0
 cPowerInMaxBoundConverter#20#92: vPowInConverter#2(6) <= 0
 cPowerInMaxBoundConverter#20#93: vPowInConverter#2(7) <= 0
 cPowerInMaxBoundConverter#20#94: vPowInConverter#2(8) <= 0
 cPowerInMaxBoundConverter#20#95: vPowInConverter#2(9) <= 0
 cPowMaxConverter#20#96:         vPowInConverter#2(0) <= 0
 cDefPminConverter#20#97:        - vPminConverter#2(0)  = 0
 cPminConverter#20#98:           vPowInConverter#2(0) - vPminConverter#2(0)
                                 >= 0
 cPowMaxConverter#21#99:         vPowInConverter#2(1) <= 0
 cDefPminConverter#21#100:       - vPminConverter#2(1)  = 0
 cPminConverter#21#101:          vPowInConverter#2(1) - vPminConverter#2(1)
                                 >= 0
 cPowMaxConverter#22#102:        vPowInConverter#2(2) <= 0
 cDefPminConverter#22#103:       - vPminConverter#2(2)  = 0
 cPminConverter#22#104:          vPowInConverter#2(2) - vPminConverter#2(2)
                                 >= 0
 cPowMaxConverter#23#105:        vPowInConverter#2(3) <= 0
 cDefPminConverter#23#106:       - vPminConverter#2(3)  = 0
 cPminConverter#23#107:          vPowInConverter#2(3) - vPminConverter#2(3)
                                 >= 0
 cPowMaxConverter#24#108:        vPowInConverter#2(4) <= 0
 cDefPminConverter#24#109:       - vPminConverter#2(4)  = 0
 cPminConverter#24#110:          vPowInConverter#2(4) - vPminConverter#2(4)
                                 >= 0
 cPowMaxConverter#25#111:        vPowInConverter#2(5) <= 0
 cDefPminConverter#25#112:       - vPminConverter#2(5)  = 0
 cPminConverter#25#113:          vPowInConverter#2(5) - vPminConverter#2(5)
                                 >= 0
 cPowMaxConverter#26#114:        vPowInConverter#2(6) <= 0
 cDefPminConverter#26#115:       - vPminConverter#2(6)  = 0
 cPminConverter#26#116:          vPowInConverter#2(6) - vPminConverter#2(6)
                                 >= 0
 cPowMaxConverter#27#117:        vPowInConverter#2(7) <= 0
 cDefPminConverter#27#118:       - vPminConverter#2(7)  = 0
 cPminConverter#27#119:          vPowInConverter#2(7) - vPminConverter#2(7)
                                 >= 0
 cPowMaxConverter#28#120:        vPowInConverter#2(8) <= 0
 cDefPminConverter#28#121:       - vPminConverter#2(8)  = 0
 cPminConverter#28#122:          vPowInConverter#2(8) - vPminConverter#2(8)
                                 >= 0
 cPowMaxConverter#29#123:        vPowInConverter#2(9) <= 0
 cDefPminConverter#29#124:       - vPminConverter#2(9)  = 0
 cPminConverter#29#125:          vPowInConverter#2(9) - vPminConverter#2(9)
                                 >= 0
 cPowEffConverter#20#126:        vPowInConverter#2(0) - vPowOutConverter#2(0)
                                  = 0
 cPowEffConverter#21#127:        vPowInConverter#2(1) - vPowOutConverter#2(1)
                                  = 0
 cPowEffConverter#22#128:        vPowInConverter#2(2) - vPowOutConverter#2(2)
                                  = 0
 cPowEffConverter#23#129:        vPowInConverter#2(3) - vPowOutConverter#2(3)
                                  = 0
 cPowEffConverter#24#130:        vPowInConverter#2(4) - vPowOutConverter#2(4)
                                  = 0
 cPowEffConverter#25#131:        vPowInConverter#2(5) - vPowOutConverter#2(5)
                                  = 0
 cPowEffConverter#26#132:        vPowInConverter#2(6) - vPowOutConverter#2(6)
                                  = 0
 cPowEffConverter#27#133:        vPowInConverter#2(7) - vPowOutConverter#2(7)
                                  = 0
 cPowEffConverter#28#134:        vPowInConverter#2(8) - vPowOutConverter#2(8)
                                  = 0
 cPowEffConverter#29#135:        vPowInConverter#2(9) - vPowOutConverter#2(9)
                                  = 0
 cPowVarConverter#20#136:        - vMaxPowerConverter#2 + vPowInConverter#2(0)
                                 <= 0
 cPowVarConverter#21#137:        - vMaxPowerConverter#2 + vPowInConverter#2(1)
                                 <= 0
 cPowVarConverter#22#138:        - vMaxPowerConverter#2 + vPowInConverter#2(2)
                                 <= 0
 cPowVarConverter#23#139:        - vMaxPowerConverter#2 + vPowInConverter#2(3)
                                 <= 0
 cPowVarConverter#24#140:        - vMaxPowerConverter#2 + vPowInConverter#2(4)
                                 <= 0
 cPowVarConverter#25#141:        - vMaxPowerConverter#2 + vPowInConverter#2(5)
                                 <= 0
 cPowVarConverter#26#142:        - vMaxPowerConverter#2 + vPowInConverter#2(6)
                                 <= 0
 cPowVarConverter#27#143:        - vMaxPowerConverter#2 + vPowInConverter#2(7)
                                 <= 0
 cPowVarConverter#28#144:        - vMaxPowerConverter#2 + vPowInConverter#2(8)
                                 <= 0
 cPowVarConverter#29#145:        - vMaxPowerConverter#2 + vPowInConverter#2(9)
                                 <= 0
 csBigMInstalledConverter#30#146: - 10 vInstalledConverter#3
                                 + vMaxPowerConverter#3 <= 0
 csMinSizeInstalledConverter#30#147: vMaxPowerConverter#3 >= 0
 cNotRunningIfNotInstalledConverter#30#148: - vInstalledConverter#3
                                 + vStateConverter#3(0) <= 0
 cNotRunningIfNotInstalledConverter#31#149: - vInstalledConverter#3
                                 + vStateConverter#3(1) <= 0
 cNotRunningIfNotInstalledConverter#32#150: - vInstalledConverter#3
                                 + vStateConverter#3(2) <= 0
 cNotRunningIfNotInstalledConverter#33#151: - vInstalledConverter#3
                                 + vStateConverter#3(3) <= 0
 cNotRunningIfNotInstalledConverter#34#152: - vInstalledConverter#3
                                 + vStateConverter#3(4) <= 0
 cNotRunningIfNotInstalledConverter#35#153: - vInstalledConverter#3
                                 + vStateConverter#3(5) <= 0
 cNotRunningIfNotInstalledConverter#36#154: - vInstalledConverter#3
                                 + vStateConverter#3(6) <= 0
 cNotRunningIfNotInstalledConverter#37#155: - vInstalledConverter#3
                                 + vStateConverter#3(7) <= 0
 cNotRunningIfNotInstalledConverter#38#156: - vInstalledConverter#3
                                 + vStateConverter#3(8) <= 0
 cNotRunningIfNotInstalledConverter#39#157: - vInstalledConverter#3
                                 + vStateConverter#3(9) <= 0
 cPowerInMaxBoundConverter#30#158: vPowInConverter#3(0) <= 10
 cPowerInMaxBoundConverter#30#159: vPowInConverter#3(1) <= 10
 cPowerInMaxBoundConverter#30#160: vPowInConverter#3(2) <= 10
 cPowerInMaxBoundConverter#30#161: vPowInConverter#3(3) <= 10
 cPowerInMaxBoundConverter#30#162: vPowInConverter#3(4) <= 10
 cPowerInMaxBoundConverter#30#163: vPowInConverter#3(5) <= 10
 cPowerInMaxBoundConverter#30#164: vPowInConverter#3(6) <= 10
 cPowerInMaxBoundConverter#30#165: vPowInConverter#3(7) <= 10
 cPowerInMaxBoundConverter#30#166: vPowInConverter#3(8) <= 10
 cPowerInMaxBoundConverter#30#167: vPowInConverter#3(9) <= 10
 cPowMaxConverter#30#168:        - 10 vStateConverter#3(0)
                                 + vPowInConverter#3(0) <= 0
 cDefPminConverter#30#169:       - vPminConverter#3(0)  = 0
 cPminConverter#30#170:          vPowInConverter#3(0) - vPminConverter#3(0)
                                 >= 0
 cPowMaxConverter#31#171:        - 10 vStateConverter#3(1)
                                 + vPowInConverter#3(1) <= 0
 cDefPminConverter#31#172:       - vPminConverter#3(1)  = 0
 cPminConverter#31#173:          vPowInConverter#3(1) - vPminConverter#3(1)
                                 >= 0
 cPowMaxConverter#32#174:        - 10 vStateConverter#3(2)
                                 + vPowInConverter#3(2) <= 0
 cDefPminConverter#32#175:       - vPminConverter#3(2)  = 0
 cPminConverter#32#176:          vPowInConverter#3(2) - vPminConverter#3(2)
                                 >= 0
 cPowMaxConverter#33#177:        - 10 vStateConverter#3(3)
                                 + vPowInConverter#3(3) <= 0
 cDefPminConverter#33#178:       - vPminConverter#3(3)  = 0
 cPminConverter#33#179:          vPowInConverter#3(3) - vPminConverter#3(3)
                                 >= 0
 cPowMaxConverter#34#180:        - 10 vStateConverter#3(4)
                                 + vPowInConverter#3(4) <= 0
 cDefPminConverter#34#181:       - vPminConverter#3(4)  = 0
 cPminConverter#34#182:          vPowInConverter#3(4) - vPminConverter#3(4)
                                 >= 0
 cPowMaxConverter#35#183:        - 10 vStateConverter#3(5)
                                 + vPowInConverter#3(5) <= 0
 cDefPminConverter#35#184:       - vPminConverter#3(5)  = 0
 cPminConverter#35#185:          vPowInConverter#3(5) - vPminConverter#3(5)
                                 >= 0
 cPowMaxConverter#36#186:        - 10 vStateConverter#3(6)
                                 + vPowInConverter#3(6) <= 0
 cDefPminConverter#36#187:       - vPminConverter#3(6)  = 0
 cPminConverter#36#188:          vPowInConverter#3(6) - vPminConverter#3(6)
                                 >= 0
 cPowMaxConverter#37#189:        - 10 vStateConverter#3(7)
                                 + vPowInConverter#3(7) <= 0
 cDefPminConverter#37#190:       - vPminConverter#3(7)  = 0
 cPminConverter#37#191:          vPowInConverter#3(7) - vPminConverter#3(7)
                                 >= 0
 cPowMaxConverter#38#192:        - 10 vStateConverter#3(8)
                                 + vPowInConverter#3(8) <= 0
 cDefPminConverter#38#193:       - vPminConverter#3(8)  = 0
 cPminConverter#38#194:          vPowInConverter#3(8) - vPminConverter#3(8)
                                 >= 0
 cPowMaxConverter#39#195:        - 10 vStateConverter#3(9)
                                 + vPowInConverter#3(9) <= 0
 cDefPminConverter#39#196:       - vPminConverter#3(9)  = 0
 cPminConverter#39#197:          vPowInConverter#3(9) - vPminConverter#3(9)
                                 >= 0
 cPowEffConverter#30#198:        vPowInConverter#3(0) - vPowOutConverter#3(0)
                                  = 0
 cPowEffConverter#31#199:        vPowInConverter#3(1) - vPowOutConverter#3(1)
                                  = 0
 cPowEffConverter#32#200:        vPowInConverter#3(2) - vPowOutConverter#3(2)
                                  = 0
 cPowEffConverter#33#201:        vPowInConverter#3(3) - vPowOutConverter#3(3)
                                  = 0
 cPowEffConverter#34#202:        vPowInConverter#3(4) - vPowOutConverter#3(4)
                                  = 0
 cPowEffConverter#35#203:        vPowInConverter#3(5) - vPowOutConverter#3(5)
                                  = 0
 cPowEffConverter#36#204:        vPowInConverter#3(6) - vPowOutConverter#3(6)
                                  = 0
 cPowEffConverter#37#205:        vPowInConverter#3(7) - vPowOutConverter#3(7)
                                  = 0
 cPowEffConverter#38#206:        vPowInConverter#3(8) - vPowOutConverter#3(8)
                                  = 0
 cPowEffConverter#39#207:        vPowInConverter#3(9) - vPowOutConverter#3(9)
                                  = 0
 cPowVarConverter#30#208:        - vMaxPowerConverter#3 + vPowInConverter#3(0)
                                 <= 0
 cPowVarConverter#31#209:        - vMaxPowerConverter#3 + vPowInConverter#3(1)
                                 <= 0
 cPowVarConverter#32#210:        - vMaxPowerConverter#3 + vPowInConverter#3(2)
                                 <= 0
 cPowVarConverter#33#211:        - vMaxPowerConverter#3 + vPowInConverter#3(3)
                                 <= 0
 cPowVarConverter#34#212:        - vMaxPowerConverter#3 + vPowInConverter#3(4)
                                 <= 0
 cPowVarConverter#35#213:        - vMaxPowerConverter#3 + vPowInConverter#3(5)
                                 <= 0
 cPowVarConverter#36#214:        - vMaxPowerConverter#3 + vPowInConverter#3(6)
                                 <= 0
 cPowVarConverter#37#215:        - vMaxPowerConverter#3 + vPowInConverter#3(7)
                                 <= 0
 cPowVarConverter#38#216:        - vMaxPowerConverter#3 + vPowInConverter#3(8)
                                 <= 0
 cPowVarConverter#39#217:        - vMaxPowerConverter#3 + vPowInConverter#3(9)
                                 <= 0
 csInstalledGridFree#10#218:     vInstalledGridFree#1  = 1
 csMaxValGridFree#10#219:        vMaxFlowGridFree#1  = 10000
 csBigMInstalledGridFree#10#220: - 10000 vInstalledGridFree#1
                                 + vMaxFlowGridFree#1 <= 0
 csMinSizeInstalledGridFree#10#221: vMaxFlowGridFree#1 >= 0
 cNotRunningIfNotInstalledGridFree#10#222: - vInstalledGridFree#1
                                 + vStateGridFree#1(0) <= 0
 cNotRunningIfNotInstalledGridFree#11#223: - vInstalledGridFree#1
                                 + vStateGridFree#1(1) <= 0
 cNotRunningIfNotInstalledGridFree#12#224: - vInstalledGridFree#1
                                 + vStateGridFree#1(2) <= 0
 cNotRunningIfNotInstalledGridFree#13#225: - vInstalledGridFree#1
                                 + vStateGridFree#1(3) <= 0
 cNotRunningIfNotInstalledGridFree#14#226: - vInstalledGridFree#1
                                 + vStateGridFree#1(4) <= 0
 cNotRunningIfNotInstalledGridFree#15#227: - vInstalledGridFree#1
                                 + vStateGridFree#1(5) <= 0
 cNotRunningIfNotInstalledGridFree#16#228: - vInstalledGridFree#1
                                 + vStateGridFree#1(6) <= 0
 cNotRunningIfNotInstalledGridFree#17#229: - vInstalledGridFree#1
                                 + vStateGridFree#1(7) <= 0
 cNotRunningIfNotInstalledGridFree#18#230: - vInstalledGridFree#1
                                 + vStateGridFree#1(8) <= 0
 cNotRunningIfNotInstalledGridFree#19#231: - vInstalledGridFree#1
                                 + vStateGridFree#1(9) <= 0
 cStateGridGridFree#10#232:      - 10000 vStateGridFree#1(0)
                                 + vGFxGridFree#1(0) <= 0
 cStateGridGridFree#11#233:      - 10000 vStateGridFree#1(1)
                                 + vGFxGridFree#1(1) <= 0
 cStateGridGridFree#12#234:      - 10000 vStateGridFree#1(2)
                                 + vGFxGridFree#1(2) <= 0
 cStateGridGridFree#13#235:      - 10000 vStateGridFree#1(3)
                                 + vGFxGridFree#1(3) <= 0
 cStateGridGridFree#14#236:      - 10000 vStateGridFree#1(4)
                                 + vGFxGridFree#1(4) <= 0
 cStateGridGridFree#15#237:      - 10000 vStateGridFree#1(5)
                                 + vGFxGridFree#1(5) <= 0
 cStateGridGridFree#16#238:      - 10000 vStateGridFree#1(6)
                                 + vGFxGridFree#1(6) <= 0
 cStateGridGridFree#17#239:      - 10000 vStateGridFree#1(7)
                                 + vGFxGridFree#1(7) <= 0
 cStateGridGridFree#18#240:      - 10000 vStateGridFree#1(8)
                                 + vGFxGridFree#1(8) <= 0
 cStateGridGridFree#19#241:      - 10000 vStateGridFree#1(9)
                                 + vGFxGridFree#1(9) <= 0
 cMaxGFxGridFree#10#242:         - vMaxFlowGridFree#1 + vGFxGridFree#1(0) <= 0
 cMaxGFxGridFree#11#243:         - vMaxFlowGridFree#1 + vGFxGridFree#1(1) <= 0
 cMaxGFxGridFree#12#244:         - vMaxFlowGridFree#1 + vGFxGridFree#1(2) <= 0
 cMaxGFxGridFree#13#245:         - vMaxFlowGridFree#1 + vGFxGridFree#1(3) <= 0
 cMaxGFxGridFree#14#246:         - vMaxFlowGridFree#1 + vGFxGridFree#1(4) <= 0
 cMaxGFxGridFree#15#247:         - vMaxFlowGridFree#1 + vGFxGridFree#1(5) <= 0
 cMaxGFxGridFree#16#248:         - vMaxFlowGridFree#1 + vGFxGridFree#1(6) <= 0
 cMaxGFxGridFree#17#249:         - vMaxFlowGridFree#1 + vGFxGridFree#1(7) <= 0
 cMaxGFxGridFree#18#250:         - vMaxFlowGridFree#1 + vGFxGridFree#1(8) <= 0
 cMaxGFxGridFree#19#251:         - vMaxFlowGridFree#1 + vGFxGridFree#1(9) <= 0
 csInstalledSourceLoad#10#252:   vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10#253:      vWeightSourceLoad#1  = 1
 csBigMInstalledSourceLoad#10#254: - vInstalledSourceLoad#1
                                 + vWeightSourceLoad#1 <= 0
 csMinSizeInstalledSourceLoad#10#255: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10#256:         <= 10000
 cMSFxSourceLoad#11#257:         <= 10000
 cMSFxSourceLoad#12#258:         <= 10000
 cMSFxSourceLoad#13#259:         <= 10000
 cMSFxSourceLoad#14#260:         <= 10000
 cMSFxSourceLoad#15#261:         <= 10000
 cMSFxSourceLoad#16#262:         <= 10000
 cMSFxSourceLoad#17#263:         <= 10000
 cMSFxSourceLoad#18#264:         <= 10000
 cMSFxSourceLoad#19#265:         <= 10000
 cSNodeLaw#10#266:               - vPowInConverter#1(0) - vPowInConverter#2(0)
                                 - vPowInConverter#3(0) + vGFxGridFree#1(0)
                                  = 0
 cSNodeLaw#11#267:               - vPowInConverter#1(1) - vPowInConverter#2(1)
                                 - vPowInConverter#3(1) + vGFxGridFree#1(1)
                                  = 0
 cSNodeLaw#12#268:               - vPowInConverter#1(2) - vPowInConverter#2(2)
                                 - vPowInConverter#3(2) + vGFxGridFree#1(2)
                                  = 0
 cSNodeLaw#13#269:               - vPowInConverter#1(3) - vPowInConverter#2(3)
                                 - vPowInConverter#3(3) + vGFxGridFree#1(3)
                                  = 0
 cSNodeLaw#14#270:               - vPowInConverter#1(4) - vPowInConverter#2(4)
                                 - vPowInConverter#3(4) + vGFxGridFree#1(4)
                                  = 0
 cSNodeLaw#15#271:               - vPowInConverter#1(5) - vPowInConverter#2(5)
                                 - vPowInConverter#3(5) + vGFxGridFree#1(5)
                                  = 0
 cSNodeLaw#16#272:               - vPowInConverter#1(6) - vPowInConverter#2(6)
                                 - vPowInConverter#3(6) + vGFxGridFree#1(6)
                                  = 0
 cSNodeLaw#17#273:               - vPowInConverter#1(7) - vPowInConverter#2(7)
                                 - vPowInConverter#3(7) + vGFxGridFree#1(7)
                                  = 0
 cSNodeLaw#18#274:               - vPowInConverter#1(8) - vPowInConverter#2(8)
                                 - vPowInConverter#3(8) + vGFxGridFree#1(8)
                                  = 0
 cSNodeLaw#19#275:               - vPowInConverter#1(9) - vPowInConverter#2(9)
                                 - vPowInConverter#3(9) + vGFxGridFree#1(9)
                                  = 0
 cSNodeLaw#20#276:               vPowOutConverter#1(0) + vPowOutConverter#2(0)
                                 + vPowOutConverter#3(0) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#21#277:               vPowOutConverter#1(1) + vPowOutConverter#2(1)
                                 + vPowOutConverter#3(1) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#22#278:               vPowOutConverter#1(2) + vPowOutConverter#2(2)
                                 + vPowOutConverter#3(2) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#23#279:               vPowOutConverter#1(3) + vPowOutConverter#2(3)
                                 + vPowOutConverter#3(3) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#24#280:               vPowOutConverter#1(4) + vPowOutConverter#2(4)
                                 + vPowOutConverter#3(4) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#25#281:               vPowOutConverter#1(5) + vPowOutConverter#2(5)
                                 + vPowOutConverter#3(5) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#26#282:               vPowOutConverter#1(6) + vPowOutConverter#2(6)
                                 + vPowOutConverter#3(6) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#27#283:               vPowOutConverter#1(7) + vPowOutConverter#2(7)
                                 + vPowOutConverter#3(7) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#28#284:               vPowOutConverter#1(8) + vPowOutConverter#2(8)
                                 + vPowOutConverter#3(8) - vWeightSourceLoad#1
                                  = 0
 cSNodeLaw#29#285:               vPowOutConverter#1(9) + vPowOutConverter#2(9)
                                 + vPowOutConverter#3(9) - vWeightSourceLoad#1
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
 0 <= vPowInConverter#1(0) <= 10
 0 <= vPowInConverter#1(1) <= 10
 0 <= vPowInConverter#1(2) <= 10
 0 <= vPowInConverter#1(3) <= 10
 0 <= vPowInConverter#1(4) <= 10
 0 <= vPowInConverter#1(5) <= 10
 0 <= vPowInConverter#1(6) <= 10
 0 <= vPowInConverter#1(7) <= 10
 0 <= vPowInConverter#1(8) <= 10
 0 <= vPowInConverter#1(9) <= 10
 0 <= vPowOutConverter#1(0) <= 10
 0 <= vPowOutConverter#1(1) <= 10
 0 <= vPowOutConverter#1(2) <= 10
 0 <= vPowOutConverter#1(3) <= 10
 0 <= vPowOutConverter#1(4) <= 10
 0 <= vPowOutConverter#1(5) <= 10
 0 <= vPowOutConverter#1(6) <= 10
 0 <= vPowOutConverter#1(7) <= 10
 0 <= vPowOutConverter#1(8) <= 10
 0 <= vPowOutConverter#1(9) <= 10
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
 0 <= vInstalledConverter#2 <= 1
      vMaxPowerConverter#2 = 0
 0 <= vStateConverter#2(0) <= 1
 0 <= vStateConverter#2(1) <= 1
 0 <= vStateConverter#2(2) <= 1
 0 <= vStateConverter#2(3) <= 1
 0 <= vStateConverter#2(4) <= 1
 0 <= vStateConverter#2(5) <= 1
 0 <= vStateConverter#2(6) <= 1
 0 <= vStateConverter#2(7) <= 1
 0 <= vStateConverter#2(8) <= 1
 0 <= vStateConverter#2(9) <= 1
      vPowInConverter#2(0) = 0
      vPowInConverter#2(1) = 0
      vPowInConverter#2(2) = 0
      vPowInConverter#2(3) = 0
      vPowInConverter#2(4) = 0
      vPowInConverter#2(5) = 0
      vPowInConverter#2(6) = 0
      vPowInConverter#2(7) = 0
      vPowInConverter#2(8) = 0
      vPowInConverter#2(9) = 0
      vPowOutConverter#2(0) = 0
      vPowOutConverter#2(1) = 0
      vPowOutConverter#2(2) = 0
      vPowOutConverter#2(3) = 0
      vPowOutConverter#2(4) = 0
      vPowOutConverter#2(5) = 0
      vPowOutConverter#2(6) = 0
      vPowOutConverter#2(7) = 0
      vPowOutConverter#2(8) = 0
      vPowOutConverter#2(9) = 0
      vPminConverter#2(0) Free
      vPminConverter#2(1) Free
      vPminConverter#2(2) Free
      vPminConverter#2(3) Free
      vPminConverter#2(4) Free
      vPminConverter#2(5) Free
      vPminConverter#2(6) Free
      vPminConverter#2(7) Free
      vPminConverter#2(8) Free
      vPminConverter#2(9) Free
 0 <= vInstalledConverter#3 <= 1
 0 <= vMaxPowerConverter#3 <= 10
 0 <= vStateConverter#3(0) <= 1
 0 <= vStateConverter#3(1) <= 1
 0 <= vStateConverter#3(2) <= 1
 0 <= vStateConverter#3(3) <= 1
 0 <= vStateConverter#3(4) <= 1
 0 <= vStateConverter#3(5) <= 1
 0 <= vStateConverter#3(6) <= 1
 0 <= vStateConverter#3(7) <= 1
 0 <= vStateConverter#3(8) <= 1
 0 <= vStateConverter#3(9) <= 1
 0 <= vPowInConverter#3(0) <= 10
 0 <= vPowInConverter#3(1) <= 10
 0 <= vPowInConverter#3(2) <= 10
 0 <= vPowInConverter#3(3) <= 10
 0 <= vPowInConverter#3(4) <= 10
 0 <= vPowInConverter#3(5) <= 10
 0 <= vPowInConverter#3(6) <= 10
 0 <= vPowInConverter#3(7) <= 10
 0 <= vPowInConverter#3(8) <= 10
 0 <= vPowInConverter#3(9) <= 10
 0 <= vPowOutConverter#3(0) <= 10
 0 <= vPowOutConverter#3(1) <= 10
 0 <= vPowOutConverter#3(2) <= 10
 0 <= vPowOutConverter#3(3) <= 10
 0 <= vPowOutConverter#3(4) <= 10
 0 <= vPowOutConverter#3(5) <= 10
 0 <= vPowOutConverter#3(6) <= 10
 0 <= vPowOutConverter#3(7) <= 10
 0 <= vPowOutConverter#3(8) <= 10
 0 <= vPowOutConverter#3(9) <= 10
      vPminConverter#3(0) Free
      vPminConverter#3(1) Free
      vPminConverter#3(2) Free
      vPminConverter#3(3) Free
      vPminConverter#3(4) Free
      vPminConverter#3(5) Free
      vPminConverter#3(6) Free
      vPminConverter#3(7) Free
      vPminConverter#3(8) Free
      vPminConverter#3(9) Free
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
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 1
Generals
 vInstalledConverter#1  vStateConverter#1(0)  vStateConverter#1(1) 
 vStateConverter#1(2)  vStateConverter#1(3)  vStateConverter#1(4) 
 vStateConverter#1(5)  vStateConverter#1(6)  vStateConverter#1(7) 
 vStateConverter#1(8)  vStateConverter#1(9)  vInstalledConverter#2 
 vStateConverter#2(0)  vStateConverter#2(1)  vStateConverter#2(2) 
 vStateConverter#2(3)  vStateConverter#2(4)  vStateConverter#2(5) 
 vStateConverter#2(6)  vStateConverter#2(7)  vStateConverter#2(8) 
 vStateConverter#2(9)  vInstalledConverter#3  vStateConverter#3(0) 
 vStateConverter#3(1)  vStateConverter#3(2)  vStateConverter#3(3) 
 vStateConverter#3(4)  vStateConverter#3(5)  vStateConverter#3(6) 
 vStateConverter#3(7)  vStateConverter#3(8)  vStateConverter#3(9) 
 vInstalledGridFree#1  vStateGridFree#1(0)  vStateGridFree#1(1) 
 vStateGridFree#1(2)  vStateGridFree#1(3)  vStateGridFree#1(4) 
 vStateGridFree#1(5)  vStateGridFree#1(6)  vStateGridFree#1(7) 
 vStateGridFree#1(8)  vStateGridFree#1(9)  vInstalledSourceLoad#1 
End
