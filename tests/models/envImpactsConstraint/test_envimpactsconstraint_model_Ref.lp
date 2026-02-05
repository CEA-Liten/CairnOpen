\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 17520 vGFxElec_Grid_Cheap(0) + 17520 vGFxElec_Grid_Cheap(1)
       + 17520 vGFxElec_Grid_Cheap(2) + 17520 vGFxElec_Grid_Cheap(3)
       + 17520 vGFxElec_Grid_Cheap(4) + 17520 vGFxElec_Grid_Cheap(5)
       + 17520 vGFxElec_Grid_Cheap(6) + 17520 vGFxElec_Grid_Cheap(7)
       + 17520 vGFxElec_Grid_Cheap(8) + 17520 vGFxElec_Grid_Cheap(9)
       + 3504 vGFxElec_Grid_LowCO2(0) + 3504 vGFxElec_Grid_LowCO2(1)
       + 3504 vGFxElec_Grid_LowCO2(2) + 3504 vGFxElec_Grid_LowCO2(3)
       + 3504 vGFxElec_Grid_LowCO2(4) + 3504 vGFxElec_Grid_LowCO2(5)
       + 3504 vGFxElec_Grid_LowCO2(6) + 3504 vGFxElec_Grid_LowCO2(7)
       + 3504 vGFxElec_Grid_LowCO2(8) + 3504 vGFxElec_Grid_LowCO2(9)
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
 cEnvImpactMaxConstraintTecEcoAnalysis0: 17520 vGFxElec_Grid_Cheap(0)
                                 + 17520 vGFxElec_Grid_Cheap(1)
                                 + 17520 vGFxElec_Grid_Cheap(2)
                                 + 17520 vGFxElec_Grid_Cheap(3)
                                 + 17520 vGFxElec_Grid_Cheap(4)
                                 + 17520 vGFxElec_Grid_Cheap(5)
                                 + 17520 vGFxElec_Grid_Cheap(6)
                                 + 17520 vGFxElec_Grid_Cheap(7)
                                 + 17520 vGFxElec_Grid_Cheap(8)
                                 + 17520 vGFxElec_Grid_Cheap(9)
                                 + 3504 vGFxElec_Grid_LowCO2(0)
                                 + 3504 vGFxElec_Grid_LowCO2(1)
                                 + 3504 vGFxElec_Grid_LowCO2(2)
                                 + 3504 vGFxElec_Grid_LowCO2(3)
                                 + 3504 vGFxElec_Grid_LowCO2(4)
                                 + 3504 vGFxElec_Grid_LowCO2(5)
                                 + 3504 vGFxElec_Grid_LowCO2(6)
                                 + 3504 vGFxElec_Grid_LowCO2(7)
                                 + 3504 vGFxElec_Grid_LowCO2(8)
                                 + 3504 vGFxElec_Grid_LowCO2(9) <= 50000000
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
 0 <= vInstalledElec_Load <= 1
 0 <= vWeightElec_Load <= 1
Generals
 vInstalledElec_Grid_Cheap  vMaxFlowElec_Grid_Cheap  vStateElec_Grid_Cheap(0) 
 vStateElec_Grid_Cheap(1)  vStateElec_Grid_Cheap(2)  vStateElec_Grid_Cheap(3) 
 vStateElec_Grid_Cheap(4)  vStateElec_Grid_Cheap(5)  vStateElec_Grid_Cheap(6) 
 vStateElec_Grid_Cheap(7)  vStateElec_Grid_Cheap(8)  vStateElec_Grid_Cheap(9) 
 vInstalledElec_Grid_LowCO2  vMaxFlowElec_Grid_LowCO2  vInstalledElec_Load 
End
