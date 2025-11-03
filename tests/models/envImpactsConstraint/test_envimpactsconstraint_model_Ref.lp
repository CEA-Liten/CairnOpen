\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 7300 vGFxElec_Grid_Cheap(0) + 7300 vGFxElec_Grid_Cheap(1)
       + 7300 vGFxElec_Grid_Cheap(2) + 7300 vGFxElec_Grid_Cheap(3)
       + 7300 vGFxElec_Grid_Cheap(4) + 7300 vGFxElec_Grid_Cheap(5)
       + 7300 vGFxElec_Grid_Cheap(6) + 7300 vGFxElec_Grid_Cheap(7)
       + 7300 vGFxElec_Grid_Cheap(8) + 7300 vGFxElec_Grid_Cheap(9)
       + 7300 vGFxElec_Grid_Cheap(10) + 7300 vGFxElec_Grid_Cheap(11)
       + 7300 vGFxElec_Grid_Cheap(12) + 7300 vGFxElec_Grid_Cheap(13)
       + 7300 vGFxElec_Grid_Cheap(14) + 7300 vGFxElec_Grid_Cheap(15)
       + 7300 vGFxElec_Grid_Cheap(16) + 7300 vGFxElec_Grid_Cheap(17)
       + 7300 vGFxElec_Grid_Cheap(18) + 7300 vGFxElec_Grid_Cheap(19)
       + 7300 vGFxElec_Grid_Cheap(20) + 7300 vGFxElec_Grid_Cheap(21)
       + 7300 vGFxElec_Grid_Cheap(22) + 7300 vGFxElec_Grid_Cheap(23)
       + 1460 vGFxElec_Grid_LowCO2(0) + 1460 vGFxElec_Grid_LowCO2(1)
       + 1460 vGFxElec_Grid_LowCO2(2) + 1460 vGFxElec_Grid_LowCO2(3)
       + 1460 vGFxElec_Grid_LowCO2(4) + 1460 vGFxElec_Grid_LowCO2(5)
       + 1460 vGFxElec_Grid_LowCO2(6) + 1460 vGFxElec_Grid_LowCO2(7)
       + 1460 vGFxElec_Grid_LowCO2(8) + 1460 vGFxElec_Grid_LowCO2(9)
       + 1460 vGFxElec_Grid_LowCO2(10) + 1460 vGFxElec_Grid_LowCO2(11)
       + 1460 vGFxElec_Grid_LowCO2(12) + 1460 vGFxElec_Grid_LowCO2(13)
       + 1460 vGFxElec_Grid_LowCO2(14) + 1460 vGFxElec_Grid_LowCO2(15)
       + 1460 vGFxElec_Grid_LowCO2(16) + 1460 vGFxElec_Grid_LowCO2(17)
       + 1460 vGFxElec_Grid_LowCO2(18) + 1460 vGFxElec_Grid_LowCO2(19)
       + 1460 vGFxElec_Grid_LowCO2(20) + 1460 vGFxElec_Grid_LowCO2(21)
       + 1460 vGFxElec_Grid_LowCO2(22) + 1460 vGFxElec_Grid_LowCO2(23)
Subject To
 csBigMInstalledElec_Grid_Cheap0: - 400 vInstalledElec_Grid_Cheap
                                 + 400 vMaxFlowElec_Grid_Cheap <= 0
 csMinSizeInstalledElec_Grid_Cheap0: 400 vMaxFlowElec_Grid_Cheap >= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap0: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(0) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap1: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(1) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap2: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(2) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap3: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(3) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap4: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(4) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap5: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(5) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap6: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(6) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap7: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(7) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap8: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(8) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap9: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(9) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap10: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(10) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap11: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(11) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap12: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(12) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap13: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(13) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap14: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(14) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap15: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(15) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap16: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(16) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap17: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(17) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap18: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(18) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap19: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(19) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap20: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(20) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap21: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(21) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap22: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(22) <= 0
 cNotRunningIfNotInstalledElec_Grid_Cheap23: - vInstalledElec_Grid_Cheap
                                 + vStateElec_Grid_Cheap(23) <= 0
 cStateGridElec_Grid_Cheap0:     - 400 vStateElec_Grid_Cheap(0)
                                 + vGFxElec_Grid_Cheap(0) <= 0
 cStateGridElec_Grid_Cheap1:     - 400 vStateElec_Grid_Cheap(1)
                                 + vGFxElec_Grid_Cheap(1) <= 0
 cStateGridElec_Grid_Cheap2:     - 400 vStateElec_Grid_Cheap(2)
                                 + vGFxElec_Grid_Cheap(2) <= 0
 cStateGridElec_Grid_Cheap3:     - 400 vStateElec_Grid_Cheap(3)
                                 + vGFxElec_Grid_Cheap(3) <= 0
 cStateGridElec_Grid_Cheap4:     - 400 vStateElec_Grid_Cheap(4)
                                 + vGFxElec_Grid_Cheap(4) <= 0
 cStateGridElec_Grid_Cheap5:     - 400 vStateElec_Grid_Cheap(5)
                                 + vGFxElec_Grid_Cheap(5) <= 0
 cStateGridElec_Grid_Cheap6:     - 400 vStateElec_Grid_Cheap(6)
                                 + vGFxElec_Grid_Cheap(6) <= 0
 cStateGridElec_Grid_Cheap7:     - 400 vStateElec_Grid_Cheap(7)
                                 + vGFxElec_Grid_Cheap(7) <= 0
 cStateGridElec_Grid_Cheap8:     - 400 vStateElec_Grid_Cheap(8)
                                 + vGFxElec_Grid_Cheap(8) <= 0
 cStateGridElec_Grid_Cheap9:     - 400 vStateElec_Grid_Cheap(9)
                                 + vGFxElec_Grid_Cheap(9) <= 0
 cStateGridElec_Grid_Cheap10:    - 400 vStateElec_Grid_Cheap(10)
                                 + vGFxElec_Grid_Cheap(10) <= 0
 cStateGridElec_Grid_Cheap11:    - 400 vStateElec_Grid_Cheap(11)
                                 + vGFxElec_Grid_Cheap(11) <= 0
 cStateGridElec_Grid_Cheap12:    - 400 vStateElec_Grid_Cheap(12)
                                 + vGFxElec_Grid_Cheap(12) <= 0
 cStateGridElec_Grid_Cheap13:    - 400 vStateElec_Grid_Cheap(13)
                                 + vGFxElec_Grid_Cheap(13) <= 0
 cStateGridElec_Grid_Cheap14:    - 400 vStateElec_Grid_Cheap(14)
                                 + vGFxElec_Grid_Cheap(14) <= 0
 cStateGridElec_Grid_Cheap15:    - 400 vStateElec_Grid_Cheap(15)
                                 + vGFxElec_Grid_Cheap(15) <= 0
 cStateGridElec_Grid_Cheap16:    - 400 vStateElec_Grid_Cheap(16)
                                 + vGFxElec_Grid_Cheap(16) <= 0
 cStateGridElec_Grid_Cheap17:    - 400 vStateElec_Grid_Cheap(17)
                                 + vGFxElec_Grid_Cheap(17) <= 0
 cStateGridElec_Grid_Cheap18:    - 400 vStateElec_Grid_Cheap(18)
                                 + vGFxElec_Grid_Cheap(18) <= 0
 cStateGridElec_Grid_Cheap19:    - 400 vStateElec_Grid_Cheap(19)
                                 + vGFxElec_Grid_Cheap(19) <= 0
 cStateGridElec_Grid_Cheap20:    - 400 vStateElec_Grid_Cheap(20)
                                 + vGFxElec_Grid_Cheap(20) <= 0
 cStateGridElec_Grid_Cheap21:    - 400 vStateElec_Grid_Cheap(21)
                                 + vGFxElec_Grid_Cheap(21) <= 0
 cStateGridElec_Grid_Cheap22:    - 400 vStateElec_Grid_Cheap(22)
                                 + vGFxElec_Grid_Cheap(22) <= 0
 cStateGridElec_Grid_Cheap23:    - 400 vStateElec_Grid_Cheap(23)
                                 + vGFxElec_Grid_Cheap(23) <= 0
 cMaxGFxElec_Grid_Cheap0:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(0) <= 0
 cMaxGFxElec_Grid_Cheap1:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(1) <= 0
 cMaxGFxElec_Grid_Cheap2:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(2) <= 0
 cMaxGFxElec_Grid_Cheap3:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(3) <= 0
 cMaxGFxElec_Grid_Cheap4:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(4) <= 0
 cMaxGFxElec_Grid_Cheap5:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(5) <= 0
 cMaxGFxElec_Grid_Cheap6:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(6) <= 0
 cMaxGFxElec_Grid_Cheap7:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(7) <= 0
 cMaxGFxElec_Grid_Cheap8:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(8) <= 0
 cMaxGFxElec_Grid_Cheap9:        - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(9) <= 0
 cMaxGFxElec_Grid_Cheap10:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(10) <= 0
 cMaxGFxElec_Grid_Cheap11:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(11) <= 0
 cMaxGFxElec_Grid_Cheap12:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(12) <= 0
 cMaxGFxElec_Grid_Cheap13:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(13) <= 0
 cMaxGFxElec_Grid_Cheap14:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(14) <= 0
 cMaxGFxElec_Grid_Cheap15:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(15) <= 0
 cMaxGFxElec_Grid_Cheap16:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(16) <= 0
 cMaxGFxElec_Grid_Cheap17:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(17) <= 0
 cMaxGFxElec_Grid_Cheap18:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(18) <= 0
 cMaxGFxElec_Grid_Cheap19:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(19) <= 0
 cMaxGFxElec_Grid_Cheap20:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(20) <= 0
 cMaxGFxElec_Grid_Cheap21:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(21) <= 0
 cMaxGFxElec_Grid_Cheap22:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(22) <= 0
 cMaxGFxElec_Grid_Cheap23:       - 400 vMaxFlowElec_Grid_Cheap
                                 + vGFxElec_Grid_Cheap(23) <= 0
 csBigMInstalledElec_Grid_LowCO20: - 400 vInstalledElec_Grid_LowCO2
                                 + 400 vMaxFlowElec_Grid_LowCO2 <= 0
 csMinSizeInstalledElec_Grid_LowCO20: 400 vMaxFlowElec_Grid_LowCO2 >= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO20: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(0) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO21: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(1) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO22: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(2) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO23: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(3) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO24: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(4) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO25: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(5) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO26: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(6) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO27: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(7) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO28: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(8) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO29: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(9) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO210: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(10) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO211: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(11) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO212: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(12) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO213: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(13) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO214: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(14) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO215: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(15) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO216: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(16) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO217: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(17) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO218: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(18) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO219: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(19) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO220: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(20) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO221: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(21) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO222: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(22) <= 0
 cNotRunningIfNotInstalledElec_Grid_LowCO223: - vInstalledElec_Grid_LowCO2
                                 + vStateElec_Grid_LowCO2(23) <= 0
 cStateGridElec_Grid_LowCO20:    - 400 vStateElec_Grid_LowCO2(0)
                                 + vGFxElec_Grid_LowCO2(0) <= 0
 cStateGridElec_Grid_LowCO21:    - 400 vStateElec_Grid_LowCO2(1)
                                 + vGFxElec_Grid_LowCO2(1) <= 0
 cStateGridElec_Grid_LowCO22:    - 400 vStateElec_Grid_LowCO2(2)
                                 + vGFxElec_Grid_LowCO2(2) <= 0
 cStateGridElec_Grid_LowCO23:    - 400 vStateElec_Grid_LowCO2(3)
                                 + vGFxElec_Grid_LowCO2(3) <= 0
 cStateGridElec_Grid_LowCO24:    - 400 vStateElec_Grid_LowCO2(4)
                                 + vGFxElec_Grid_LowCO2(4) <= 0
 cStateGridElec_Grid_LowCO25:    - 400 vStateElec_Grid_LowCO2(5)
                                 + vGFxElec_Grid_LowCO2(5) <= 0
 cStateGridElec_Grid_LowCO26:    - 400 vStateElec_Grid_LowCO2(6)
                                 + vGFxElec_Grid_LowCO2(6) <= 0
 cStateGridElec_Grid_LowCO27:    - 400 vStateElec_Grid_LowCO2(7)
                                 + vGFxElec_Grid_LowCO2(7) <= 0
 cStateGridElec_Grid_LowCO28:    - 400 vStateElec_Grid_LowCO2(8)
                                 + vGFxElec_Grid_LowCO2(8) <= 0
 cStateGridElec_Grid_LowCO29:    - 400 vStateElec_Grid_LowCO2(9)
                                 + vGFxElec_Grid_LowCO2(9) <= 0
 cStateGridElec_Grid_LowCO210:   - 400 vStateElec_Grid_LowCO2(10)
                                 + vGFxElec_Grid_LowCO2(10) <= 0
 cStateGridElec_Grid_LowCO211:   - 400 vStateElec_Grid_LowCO2(11)
                                 + vGFxElec_Grid_LowCO2(11) <= 0
 cStateGridElec_Grid_LowCO212:   - 400 vStateElec_Grid_LowCO2(12)
                                 + vGFxElec_Grid_LowCO2(12) <= 0
 cStateGridElec_Grid_LowCO213:   - 400 vStateElec_Grid_LowCO2(13)
                                 + vGFxElec_Grid_LowCO2(13) <= 0
 cStateGridElec_Grid_LowCO214:   - 400 vStateElec_Grid_LowCO2(14)
                                 + vGFxElec_Grid_LowCO2(14) <= 0
 cStateGridElec_Grid_LowCO215:   - 400 vStateElec_Grid_LowCO2(15)
                                 + vGFxElec_Grid_LowCO2(15) <= 0
 cStateGridElec_Grid_LowCO216:   - 400 vStateElec_Grid_LowCO2(16)
                                 + vGFxElec_Grid_LowCO2(16) <= 0
 cStateGridElec_Grid_LowCO217:   - 400 vStateElec_Grid_LowCO2(17)
                                 + vGFxElec_Grid_LowCO2(17) <= 0
 cStateGridElec_Grid_LowCO218:   - 400 vStateElec_Grid_LowCO2(18)
                                 + vGFxElec_Grid_LowCO2(18) <= 0
 cStateGridElec_Grid_LowCO219:   - 400 vStateElec_Grid_LowCO2(19)
                                 + vGFxElec_Grid_LowCO2(19) <= 0
 cStateGridElec_Grid_LowCO220:   - 400 vStateElec_Grid_LowCO2(20)
                                 + vGFxElec_Grid_LowCO2(20) <= 0
 cStateGridElec_Grid_LowCO221:   - 400 vStateElec_Grid_LowCO2(21)
                                 + vGFxElec_Grid_LowCO2(21) <= 0
 cStateGridElec_Grid_LowCO222:   - 400 vStateElec_Grid_LowCO2(22)
                                 + vGFxElec_Grid_LowCO2(22) <= 0
 cStateGridElec_Grid_LowCO223:   - 400 vStateElec_Grid_LowCO2(23)
                                 + vGFxElec_Grid_LowCO2(23) <= 0
 cMaxGFxElec_Grid_LowCO20:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(0) <= 0
 cMaxGFxElec_Grid_LowCO21:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(1) <= 0
 cMaxGFxElec_Grid_LowCO22:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(2) <= 0
 cMaxGFxElec_Grid_LowCO23:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(3) <= 0
 cMaxGFxElec_Grid_LowCO24:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(4) <= 0
 cMaxGFxElec_Grid_LowCO25:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(5) <= 0
 cMaxGFxElec_Grid_LowCO26:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(6) <= 0
 cMaxGFxElec_Grid_LowCO27:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(7) <= 0
 cMaxGFxElec_Grid_LowCO28:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(8) <= 0
 cMaxGFxElec_Grid_LowCO29:       - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(9) <= 0
 cMaxGFxElec_Grid_LowCO210:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(10) <= 0
 cMaxGFxElec_Grid_LowCO211:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(11) <= 0
 cMaxGFxElec_Grid_LowCO212:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(12) <= 0
 cMaxGFxElec_Grid_LowCO213:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(13) <= 0
 cMaxGFxElec_Grid_LowCO214:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(14) <= 0
 cMaxGFxElec_Grid_LowCO215:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(15) <= 0
 cMaxGFxElec_Grid_LowCO216:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(16) <= 0
 cMaxGFxElec_Grid_LowCO217:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(17) <= 0
 cMaxGFxElec_Grid_LowCO218:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(18) <= 0
 cMaxGFxElec_Grid_LowCO219:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(19) <= 0
 cMaxGFxElec_Grid_LowCO220:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(20) <= 0
 cMaxGFxElec_Grid_LowCO221:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(21) <= 0
 cMaxGFxElec_Grid_LowCO222:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(22) <= 0
 cMaxGFxElec_Grid_LowCO223:      - 400 vMaxFlowElec_Grid_LowCO2
                                 + vGFxElec_Grid_LowCO2(23) <= 0
 csInstalledElec_Load0:          vInstalledElec_Load  = 1
 csWeightElec_Load0:             vWeightElec_Load  = 1
 csBigMInstalledElec_Load0:      - vInstalledElec_Load + vWeightElec_Load <= 0
 csMinSizeInstalledElec_Load0:   vWeightElec_Load >= 0
 cMSFxElec_Load0:                <= 10000
 cMSFxElec_Load1:                <= 10000
 cMSFxElec_Load2:                <= 10000
 cMSFxElec_Load3:                <= 10000
 cMSFxElec_Load4:                <= 10000
 cMSFxElec_Load5:                <= 10000
 cMSFxElec_Load6:                <= 10000
 cMSFxElec_Load7:                <= 10000
 cMSFxElec_Load8:                <= 10000
 cMSFxElec_Load9:                <= 10000
 cMSFxElec_Load10:               <= 10000
 cMSFxElec_Load11:               <= 10000
 cMSFxElec_Load12:               <= 10000
 cMSFxElec_Load13:               <= 10000
 cMSFxElec_Load14:               <= 10000
 cMSFxElec_Load15:               <= 10000
 cMSFxElec_Load16:               <= 10000
 cMSFxElec_Load17:               <= 10000
 cMSFxElec_Load18:               <= 10000
 cMSFxElec_Load19:               <= 10000
 cMSFxElec_Load20:               <= 10000
 cMSFxElec_Load21:               <= 10000
 cMSFxElec_Load22:               <= 10000
 cMSFxElec_Load23:               <= 10000
 cSNodeLaw#10:                   vGFxElec_Grid_Cheap(0)
                                 + vGFxElec_Grid_LowCO2(0)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#11:                   vGFxElec_Grid_Cheap(1)
                                 + vGFxElec_Grid_LowCO2(1)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#12:                   vGFxElec_Grid_Cheap(2)
                                 + vGFxElec_Grid_LowCO2(2)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#13:                   vGFxElec_Grid_Cheap(3)
                                 + vGFxElec_Grid_LowCO2(3)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#14:                   vGFxElec_Grid_Cheap(4)
                                 + vGFxElec_Grid_LowCO2(4)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#15:                   vGFxElec_Grid_Cheap(5)
                                 + vGFxElec_Grid_LowCO2(5)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#16:                   vGFxElec_Grid_Cheap(6)
                                 + vGFxElec_Grid_LowCO2(6)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#17:                   vGFxElec_Grid_Cheap(7)
                                 + vGFxElec_Grid_LowCO2(7)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#18:                   vGFxElec_Grid_Cheap(8)
                                 + vGFxElec_Grid_LowCO2(8)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#19:                   vGFxElec_Grid_Cheap(9)
                                 + vGFxElec_Grid_LowCO2(9)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#110:                  vGFxElec_Grid_Cheap(10)
                                 + vGFxElec_Grid_LowCO2(10)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#111:                  vGFxElec_Grid_Cheap(11)
                                 + vGFxElec_Grid_LowCO2(11)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#112:                  vGFxElec_Grid_Cheap(12)
                                 + vGFxElec_Grid_LowCO2(12)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#113:                  vGFxElec_Grid_Cheap(13)
                                 + vGFxElec_Grid_LowCO2(13)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#114:                  vGFxElec_Grid_Cheap(14)
                                 + vGFxElec_Grid_LowCO2(14)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#115:                  vGFxElec_Grid_Cheap(15)
                                 + vGFxElec_Grid_LowCO2(15)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#116:                  vGFxElec_Grid_Cheap(16)
                                 + vGFxElec_Grid_LowCO2(16)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#117:                  vGFxElec_Grid_Cheap(17)
                                 + vGFxElec_Grid_LowCO2(17)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#118:                  vGFxElec_Grid_Cheap(18)
                                 + vGFxElec_Grid_LowCO2(18)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#119:                  vGFxElec_Grid_Cheap(19)
                                 + vGFxElec_Grid_LowCO2(19)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#120:                  vGFxElec_Grid_Cheap(20)
                                 + vGFxElec_Grid_LowCO2(20)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#121:                  vGFxElec_Grid_Cheap(21)
                                 + vGFxElec_Grid_LowCO2(21)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#122:                  vGFxElec_Grid_Cheap(22)
                                 + vGFxElec_Grid_LowCO2(22)
                                 - 400 vWeightElec_Load  = 0
 cSNodeLaw#123:                  vGFxElec_Grid_Cheap(23)
                                 + vGFxElec_Grid_LowCO2(23)
                                 - 400 vWeightElec_Load  = 0
 cEnvImpactMaxConstraintTecEcoAnalysis0: 7300 vGFxElec_Grid_Cheap(0)
                                 + 7300 vGFxElec_Grid_Cheap(1)
                                 + 7300 vGFxElec_Grid_Cheap(2)
                                 + 7300 vGFxElec_Grid_Cheap(3)
                                 + 7300 vGFxElec_Grid_Cheap(4)
                                 + 7300 vGFxElec_Grid_Cheap(5)
                                 + 7300 vGFxElec_Grid_Cheap(6)
                                 + 7300 vGFxElec_Grid_Cheap(7)
                                 + 7300 vGFxElec_Grid_Cheap(8)
                                 + 7300 vGFxElec_Grid_Cheap(9)
                                 + 7300 vGFxElec_Grid_Cheap(10)
                                 + 7300 vGFxElec_Grid_Cheap(11)
                                 + 7300 vGFxElec_Grid_Cheap(12)
                                 + 7300 vGFxElec_Grid_Cheap(13)
                                 + 7300 vGFxElec_Grid_Cheap(14)
                                 + 7300 vGFxElec_Grid_Cheap(15)
                                 + 7300 vGFxElec_Grid_Cheap(16)
                                 + 7300 vGFxElec_Grid_Cheap(17)
                                 + 7300 vGFxElec_Grid_Cheap(18)
                                 + 7300 vGFxElec_Grid_Cheap(19)
                                 + 7300 vGFxElec_Grid_Cheap(20)
                                 + 7300 vGFxElec_Grid_Cheap(21)
                                 + 7300 vGFxElec_Grid_Cheap(22)
                                 + 7300 vGFxElec_Grid_Cheap(23)
                                 + 1460 vGFxElec_Grid_LowCO2(0)
                                 + 1460 vGFxElec_Grid_LowCO2(1)
                                 + 1460 vGFxElec_Grid_LowCO2(2)
                                 + 1460 vGFxElec_Grid_LowCO2(3)
                                 + 1460 vGFxElec_Grid_LowCO2(4)
                                 + 1460 vGFxElec_Grid_LowCO2(5)
                                 + 1460 vGFxElec_Grid_LowCO2(6)
                                 + 1460 vGFxElec_Grid_LowCO2(7)
                                 + 1460 vGFxElec_Grid_LowCO2(8)
                                 + 1460 vGFxElec_Grid_LowCO2(9)
                                 + 1460 vGFxElec_Grid_LowCO2(10)
                                 + 1460 vGFxElec_Grid_LowCO2(11)
                                 + 1460 vGFxElec_Grid_LowCO2(12)
                                 + 1460 vGFxElec_Grid_LowCO2(13)
                                 + 1460 vGFxElec_Grid_LowCO2(14)
                                 + 1460 vGFxElec_Grid_LowCO2(15)
                                 + 1460 vGFxElec_Grid_LowCO2(16)
                                 + 1460 vGFxElec_Grid_LowCO2(17)
                                 + 1460 vGFxElec_Grid_LowCO2(18)
                                 + 1460 vGFxElec_Grid_LowCO2(19)
                                 + 1460 vGFxElec_Grid_LowCO2(20)
                                 + 1460 vGFxElec_Grid_LowCO2(21)
                                 + 1460 vGFxElec_Grid_LowCO2(22)
                                 + 1460 vGFxElec_Grid_LowCO2(23) <= 50000000
Bounds
 0 <= vInstalledElec_Grid_Cheap <= 1
 0 <= vMaxFlowElec_Grid_Cheap <= 1
 0 <= vStateElec_Grid_Cheap(0) <= 1
 0 <= vStateElec_Grid_Cheap(1) <= 1
 0 <= vStateElec_Grid_Cheap(2) <= 1
 0 <= vStateElec_Grid_Cheap(3) <= 1
 0 <= vStateElec_Grid_Cheap(4) <= 1
 0 <= vStateElec_Grid_Cheap(5) <= 1
 0 <= vStateElec_Grid_Cheap(6) <= 1
 0 <= vStateElec_Grid_Cheap(7) <= 1
 0 <= vStateElec_Grid_Cheap(8) <= 1
 0 <= vStateElec_Grid_Cheap(9) <= 1
 0 <= vStateElec_Grid_Cheap(10) <= 1
 0 <= vStateElec_Grid_Cheap(11) <= 1
 0 <= vStateElec_Grid_Cheap(12) <= 1
 0 <= vStateElec_Grid_Cheap(13) <= 1
 0 <= vStateElec_Grid_Cheap(14) <= 1
 0 <= vStateElec_Grid_Cheap(15) <= 1
 0 <= vStateElec_Grid_Cheap(16) <= 1
 0 <= vStateElec_Grid_Cheap(17) <= 1
 0 <= vStateElec_Grid_Cheap(18) <= 1
 0 <= vStateElec_Grid_Cheap(19) <= 1
 0 <= vStateElec_Grid_Cheap(20) <= 1
 0 <= vStateElec_Grid_Cheap(21) <= 1
 0 <= vStateElec_Grid_Cheap(22) <= 1
 0 <= vStateElec_Grid_Cheap(23) <= 1
 0 <= vGFxElec_Grid_Cheap(0) <= 400
 0 <= vGFxElec_Grid_Cheap(1) <= 400
 0 <= vGFxElec_Grid_Cheap(2) <= 400
 0 <= vGFxElec_Grid_Cheap(3) <= 400
 0 <= vGFxElec_Grid_Cheap(4) <= 400
 0 <= vGFxElec_Grid_Cheap(5) <= 400
 0 <= vGFxElec_Grid_Cheap(6) <= 400
 0 <= vGFxElec_Grid_Cheap(7) <= 400
 0 <= vGFxElec_Grid_Cheap(8) <= 400
 0 <= vGFxElec_Grid_Cheap(9) <= 400
 0 <= vGFxElec_Grid_Cheap(10) <= 400
 0 <= vGFxElec_Grid_Cheap(11) <= 400
 0 <= vGFxElec_Grid_Cheap(12) <= 400
 0 <= vGFxElec_Grid_Cheap(13) <= 400
 0 <= vGFxElec_Grid_Cheap(14) <= 400
 0 <= vGFxElec_Grid_Cheap(15) <= 400
 0 <= vGFxElec_Grid_Cheap(16) <= 400
 0 <= vGFxElec_Grid_Cheap(17) <= 400
 0 <= vGFxElec_Grid_Cheap(18) <= 400
 0 <= vGFxElec_Grid_Cheap(19) <= 400
 0 <= vGFxElec_Grid_Cheap(20) <= 400
 0 <= vGFxElec_Grid_Cheap(21) <= 400
 0 <= vGFxElec_Grid_Cheap(22) <= 400
 0 <= vGFxElec_Grid_Cheap(23) <= 400
 0 <= vInstalledElec_Grid_LowCO2 <= 1
 0 <= vMaxFlowElec_Grid_LowCO2 <= 1
      vStateElec_Grid_LowCO2(0) = 1
      vStateElec_Grid_LowCO2(1) = 1
      vStateElec_Grid_LowCO2(2) = 1
      vStateElec_Grid_LowCO2(3) = 1
      vStateElec_Grid_LowCO2(4) = 1
      vStateElec_Grid_LowCO2(5) = 1
      vStateElec_Grid_LowCO2(6) = 1
      vStateElec_Grid_LowCO2(7) = 1
      vStateElec_Grid_LowCO2(8) = 1
      vStateElec_Grid_LowCO2(9) = 1
      vStateElec_Grid_LowCO2(10) = 1
      vStateElec_Grid_LowCO2(11) = 1
      vStateElec_Grid_LowCO2(12) = 1
      vStateElec_Grid_LowCO2(13) = 1
      vStateElec_Grid_LowCO2(14) = 1
      vStateElec_Grid_LowCO2(15) = 1
      vStateElec_Grid_LowCO2(16) = 1
      vStateElec_Grid_LowCO2(17) = 1
      vStateElec_Grid_LowCO2(18) = 1
      vStateElec_Grid_LowCO2(19) = 1
      vStateElec_Grid_LowCO2(20) = 1
      vStateElec_Grid_LowCO2(21) = 1
      vStateElec_Grid_LowCO2(22) = 1
      vStateElec_Grid_LowCO2(23) = 1
 0 <= vGFxElec_Grid_LowCO2(0) <= 400
 0 <= vGFxElec_Grid_LowCO2(1) <= 400
 0 <= vGFxElec_Grid_LowCO2(2) <= 400
 0 <= vGFxElec_Grid_LowCO2(3) <= 400
 0 <= vGFxElec_Grid_LowCO2(4) <= 400
 0 <= vGFxElec_Grid_LowCO2(5) <= 400
 0 <= vGFxElec_Grid_LowCO2(6) <= 400
 0 <= vGFxElec_Grid_LowCO2(7) <= 400
 0 <= vGFxElec_Grid_LowCO2(8) <= 400
 0 <= vGFxElec_Grid_LowCO2(9) <= 400
 0 <= vGFxElec_Grid_LowCO2(10) <= 400
 0 <= vGFxElec_Grid_LowCO2(11) <= 400
 0 <= vGFxElec_Grid_LowCO2(12) <= 400
 0 <= vGFxElec_Grid_LowCO2(13) <= 400
 0 <= vGFxElec_Grid_LowCO2(14) <= 400
 0 <= vGFxElec_Grid_LowCO2(15) <= 400
 0 <= vGFxElec_Grid_LowCO2(16) <= 400
 0 <= vGFxElec_Grid_LowCO2(17) <= 400
 0 <= vGFxElec_Grid_LowCO2(18) <= 400
 0 <= vGFxElec_Grid_LowCO2(19) <= 400
 0 <= vGFxElec_Grid_LowCO2(20) <= 400
 0 <= vGFxElec_Grid_LowCO2(21) <= 400
 0 <= vGFxElec_Grid_LowCO2(22) <= 400
 0 <= vGFxElec_Grid_LowCO2(23) <= 400
 0 <= vInstalledElec_Load <= 1
 0 <= vWeightElec_Load <= 1
Generals
 vInstalledElec_Grid_Cheap  vMaxFlowElec_Grid_Cheap  vStateElec_Grid_Cheap(0) 
 vStateElec_Grid_Cheap(1)  vStateElec_Grid_Cheap(2)  vStateElec_Grid_Cheap(3) 
 vStateElec_Grid_Cheap(4)  vStateElec_Grid_Cheap(5)  vStateElec_Grid_Cheap(6) 
 vStateElec_Grid_Cheap(7)  vStateElec_Grid_Cheap(8)  vStateElec_Grid_Cheap(9) 
 vStateElec_Grid_Cheap(10)  vStateElec_Grid_Cheap(11) 
 vStateElec_Grid_Cheap(12)  vStateElec_Grid_Cheap(13) 
 vStateElec_Grid_Cheap(14)  vStateElec_Grid_Cheap(15) 
 vStateElec_Grid_Cheap(16)  vStateElec_Grid_Cheap(17) 
 vStateElec_Grid_Cheap(18)  vStateElec_Grid_Cheap(19) 
 vStateElec_Grid_Cheap(20)  vStateElec_Grid_Cheap(21) 
 vStateElec_Grid_Cheap(22)  vStateElec_Grid_Cheap(23) 
 vInstalledElec_Grid_LowCO2  vMaxFlowElec_Grid_LowCO2  vInstalledElec_Load 
End
