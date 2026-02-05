\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 49649.907163036 vGFxElec_Grid(0) + 49649.907163036 vGFxElec_Grid(1)
       + 49649.907163036 vGFxElec_Grid(2) + 49649.907163036 vGFxElec_Grid(3)
       + 49649.907163036 vGFxElec_Grid(4) + 49649.907163036 vGFxElec_Grid(5)
       + 49649.907163036 vGFxElec_Grid(6) + 49649.907163036 vGFxElec_Grid(7)
       + 49649.907163036 vGFxElec_Grid(8) + 49649.907163036 vGFxElec_Grid(9)
       + 320847.470266889 vMaxPowerElectrolyzer
       + 9929.98143260721 vPowElectrolyzer(0)
       + 9929.98143260721 vPowElectrolyzer(1)
       + 9929.98143260721 vPowElectrolyzer(2)
       + 9929.98143260721 vPowElectrolyzer(3)
       + 9929.98143260721 vPowElectrolyzer(4)
       + 9929.98143260721 vPowElectrolyzer(5)
       + 9929.98143260721 vPowElectrolyzer(6)
       + 9929.98143260721 vPowElectrolyzer(7)
       + 9929.98143260721 vPowElectrolyzer(8)
       + 9929.98143260721 vPowElectrolyzer(9)
Subject To
 csInstalledElec_Grid0:          vInstalledElec_Grid  = 1
 csMaxValElec_Grid0:             vMaxFlowElec_Grid  = 10000
 csBigMInstalledElec_Grid0:      - 10000 vInstalledElec_Grid
                                 + vMaxFlowElec_Grid <= 0
 csMinSizeInstalledElec_Grid0:   vMaxFlowElec_Grid >= 0
 cNotRunningIfNotInstalledElec_Grid0: - vInstalledElec_Grid
                                 + vStateElec_Grid(0) <= 0
 cNotRunningIfNotInstalledElec_Grid1: - vInstalledElec_Grid
                                 + vStateElec_Grid(1) <= 0
 cNotRunningIfNotInstalledElec_Grid2: - vInstalledElec_Grid
                                 + vStateElec_Grid(2) <= 0
 cNotRunningIfNotInstalledElec_Grid3: - vInstalledElec_Grid
                                 + vStateElec_Grid(3) <= 0
 cNotRunningIfNotInstalledElec_Grid4: - vInstalledElec_Grid
                                 + vStateElec_Grid(4) <= 0
 cNotRunningIfNotInstalledElec_Grid5: - vInstalledElec_Grid
                                 + vStateElec_Grid(5) <= 0
 cNotRunningIfNotInstalledElec_Grid6: - vInstalledElec_Grid
                                 + vStateElec_Grid(6) <= 0
 cNotRunningIfNotInstalledElec_Grid7: - vInstalledElec_Grid
                                 + vStateElec_Grid(7) <= 0
 cNotRunningIfNotInstalledElec_Grid8: - vInstalledElec_Grid
                                 + vStateElec_Grid(8) <= 0
 cNotRunningIfNotInstalledElec_Grid9: - vInstalledElec_Grid
                                 + vStateElec_Grid(9) <= 0
 cStateGridElec_Grid0:           - 10000 vStateElec_Grid(0) + vGFxElec_Grid(0)
                                 <= 0
 cStateGridElec_Grid1:           - 10000 vStateElec_Grid(1) + vGFxElec_Grid(1)
                                 <= 0
 cStateGridElec_Grid2:           - 10000 vStateElec_Grid(2) + vGFxElec_Grid(2)
                                 <= 0
 cStateGridElec_Grid3:           - 10000 vStateElec_Grid(3) + vGFxElec_Grid(3)
                                 <= 0
 cStateGridElec_Grid4:           - 10000 vStateElec_Grid(4) + vGFxElec_Grid(4)
                                 <= 0
 cStateGridElec_Grid5:           - 10000 vStateElec_Grid(5) + vGFxElec_Grid(5)
                                 <= 0
 cStateGridElec_Grid6:           - 10000 vStateElec_Grid(6) + vGFxElec_Grid(6)
                                 <= 0
 cStateGridElec_Grid7:           - 10000 vStateElec_Grid(7) + vGFxElec_Grid(7)
                                 <= 0
 cStateGridElec_Grid8:           - 10000 vStateElec_Grid(8) + vGFxElec_Grid(8)
                                 <= 0
 cStateGridElec_Grid9:           - 10000 vStateElec_Grid(9) + vGFxElec_Grid(9)
                                 <= 0
 cMaxGFxElec_Grid0:              - vMaxFlowElec_Grid + vGFxElec_Grid(0) <= 0
 cMaxGFxElec_Grid1:              - vMaxFlowElec_Grid + vGFxElec_Grid(1) <= 0
 cMaxGFxElec_Grid2:              - vMaxFlowElec_Grid + vGFxElec_Grid(2) <= 0
 cMaxGFxElec_Grid3:              - vMaxFlowElec_Grid + vGFxElec_Grid(3) <= 0
 cMaxGFxElec_Grid4:              - vMaxFlowElec_Grid + vGFxElec_Grid(4) <= 0
 cMaxGFxElec_Grid5:              - vMaxFlowElec_Grid + vGFxElec_Grid(5) <= 0
 cMaxGFxElec_Grid6:              - vMaxFlowElec_Grid + vGFxElec_Grid(6) <= 0
 cMaxGFxElec_Grid7:              - vMaxFlowElec_Grid + vGFxElec_Grid(7) <= 0
 cMaxGFxElec_Grid8:              - vMaxFlowElec_Grid + vGFxElec_Grid(8) <= 0
 cMaxGFxElec_Grid9:              - vMaxFlowElec_Grid + vGFxElec_Grid(9) <= 0
 csBigMInstalledElectrolyzer0:   - 50 vInstalledElectrolyzer
                                 + vMaxPowerElectrolyzer <= 0
 csMinSizeInstalledElectrolyzer0: vMaxPowerElectrolyzer >= 0
 cNotRunningIfNotInstalledElectrolyzer0: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(0) <= 0
 cNotRunningIfNotInstalledElectrolyzer1: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(1) <= 0
 cNotRunningIfNotInstalledElectrolyzer2: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(2) <= 0
 cNotRunningIfNotInstalledElectrolyzer3: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(3) <= 0
 cNotRunningIfNotInstalledElectrolyzer4: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(4) <= 0
 cNotRunningIfNotInstalledElectrolyzer5: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(5) <= 0
 cNotRunningIfNotInstalledElectrolyzer6: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(6) <= 0
 cNotRunningIfNotInstalledElectrolyzer7: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(7) <= 0
 cNotRunningIfNotInstalledElectrolyzer8: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(8) <= 0
 cNotRunningIfNotInstalledElectrolyzer9: - vInstalledElectrolyzer
                                 + vStateElectrolyzer(9) <= 0
 cPowFlowElectrolyzer0:          vPowElectrolyzer(0)
                                 - 0.0555333333333333 vFlowElectrolyzer(0)  = 0
 cPowFlowElectrolyzer1:          vPowElectrolyzer(1)
                                 - 0.0555333333333333 vFlowElectrolyzer(1)  = 0
 cPowFlowElectrolyzer2:          vPowElectrolyzer(2)
                                 - 0.0555333333333333 vFlowElectrolyzer(2)  = 0
 cPowFlowElectrolyzer3:          vPowElectrolyzer(3)
                                 - 0.0555333333333333 vFlowElectrolyzer(3)  = 0
 cPowFlowElectrolyzer4:          vPowElectrolyzer(4)
                                 - 0.0555333333333333 vFlowElectrolyzer(4)  = 0
 cPowFlowElectrolyzer5:          vPowElectrolyzer(5)
                                 - 0.0555333333333333 vFlowElectrolyzer(5)  = 0
 cPowFlowElectrolyzer6:          vPowElectrolyzer(6)
                                 - 0.0555333333333333 vFlowElectrolyzer(6)  = 0
 cPowFlowElectrolyzer7:          vPowElectrolyzer(7)
                                 - 0.0555333333333333 vFlowElectrolyzer(7)  = 0
 cPowFlowElectrolyzer8:          vPowElectrolyzer(8)
                                 - 0.0555333333333333 vFlowElectrolyzer(8)  = 0
 cPowFlowElectrolyzer9:          vPowElectrolyzer(9)
                                 - 0.0555333333333333 vFlowElectrolyzer(9)  = 0
 cUsePowElectrolyzer0:           - vMaxPowerElectrolyzer
                                 + vUsablePowElectrolyzer  = 0
 cMaxTotalPowElectrolyzer0:      vPowElectrolyzer(0) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer1:      vPowElectrolyzer(1) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer2:      vPowElectrolyzer(2) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer3:      vPowElectrolyzer(3) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer4:      vPowElectrolyzer(4) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer5:      vPowElectrolyzer(5) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer6:      vPowElectrolyzer(6) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer7:      vPowElectrolyzer(7) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer8:      vPowElectrolyzer(8) - vUsablePowElectrolyzer
                                 <= 0
 cMaxTotalPowElectrolyzer9:      vPowElectrolyzer(9) - vUsablePowElectrolyzer
                                 <= 0
 cPowMaxElectrolyzer0:           - 50 vStateElectrolyzer(0)
                                 + vPowElectrolyzer(0) <= 0
 cDefPminElectrolyzer0:          - vPminElectrolyzer(0)  = 0
 cPminElectrolyzer0:             vPowElectrolyzer(0) - vPminElectrolyzer(0)
                                 >= 0
 cPowMaxElectrolyzer1:           - 50 vStateElectrolyzer(1)
                                 + vPowElectrolyzer(1) <= 0
 cDefPminElectrolyzer1:          - vPminElectrolyzer(1)  = 0
 cPminElectrolyzer1:             vPowElectrolyzer(1) - vPminElectrolyzer(1)
                                 >= 0
 cPowMaxElectrolyzer2:           - 50 vStateElectrolyzer(2)
                                 + vPowElectrolyzer(2) <= 0
 cDefPminElectrolyzer2:          - vPminElectrolyzer(2)  = 0
 cPminElectrolyzer2:             vPowElectrolyzer(2) - vPminElectrolyzer(2)
                                 >= 0
 cPowMaxElectrolyzer3:           - 50 vStateElectrolyzer(3)
                                 + vPowElectrolyzer(3) <= 0
 cDefPminElectrolyzer3:          - vPminElectrolyzer(3)  = 0
 cPminElectrolyzer3:             vPowElectrolyzer(3) - vPminElectrolyzer(3)
                                 >= 0
 cPowMaxElectrolyzer4:           - 50 vStateElectrolyzer(4)
                                 + vPowElectrolyzer(4) <= 0
 cDefPminElectrolyzer4:          - vPminElectrolyzer(4)  = 0
 cPminElectrolyzer4:             vPowElectrolyzer(4) - vPminElectrolyzer(4)
                                 >= 0
 cPowMaxElectrolyzer5:           - 50 vStateElectrolyzer(5)
                                 + vPowElectrolyzer(5) <= 0
 cDefPminElectrolyzer5:          - vPminElectrolyzer(5)  = 0
 cPminElectrolyzer5:             vPowElectrolyzer(5) - vPminElectrolyzer(5)
                                 >= 0
 cPowMaxElectrolyzer6:           - 50 vStateElectrolyzer(6)
                                 + vPowElectrolyzer(6) <= 0
 cDefPminElectrolyzer6:          - vPminElectrolyzer(6)  = 0
 cPminElectrolyzer6:             vPowElectrolyzer(6) - vPminElectrolyzer(6)
                                 >= 0
 cPowMaxElectrolyzer7:           - 50 vStateElectrolyzer(7)
                                 + vPowElectrolyzer(7) <= 0
 cDefPminElectrolyzer7:          - vPminElectrolyzer(7)  = 0
 cPminElectrolyzer7:             vPowElectrolyzer(7) - vPminElectrolyzer(7)
                                 >= 0
 cPowMaxElectrolyzer8:           - 50 vStateElectrolyzer(8)
                                 + vPowElectrolyzer(8) <= 0
 cDefPminElectrolyzer8:          - vPminElectrolyzer(8)  = 0
 cPminElectrolyzer8:             vPowElectrolyzer(8) - vPminElectrolyzer(8)
                                 >= 0
 cPowMaxElectrolyzer9:           - 50 vStateElectrolyzer(9)
                                 + vPowElectrolyzer(9) <= 0
 cDefPminElectrolyzer9:          - vPminElectrolyzer(9)  = 0
 cPminElectrolyzer9:             vPowElectrolyzer(9) - vPminElectrolyzer(9)
                                 >= 0
 cConverterUseElectrolyzer0:     - vMaxPowerElectrolyzer + vPowElectrolyzer(0)
                                 <= 0
 cConverterUseElectrolyzer1:     - vMaxPowerElectrolyzer + vPowElectrolyzer(1)
                                 <= 0
 cConverterUseElectrolyzer2:     - vMaxPowerElectrolyzer + vPowElectrolyzer(2)
                                 <= 0
 cConverterUseElectrolyzer3:     - vMaxPowerElectrolyzer + vPowElectrolyzer(3)
                                 <= 0
 cConverterUseElectrolyzer4:     - vMaxPowerElectrolyzer + vPowElectrolyzer(4)
                                 <= 0
 cConverterUseElectrolyzer5:     - vMaxPowerElectrolyzer + vPowElectrolyzer(5)
                                 <= 0
 cConverterUseElectrolyzer6:     - vMaxPowerElectrolyzer + vPowElectrolyzer(6)
                                 <= 0
 cConverterUseElectrolyzer7:     - vMaxPowerElectrolyzer + vPowElectrolyzer(7)
                                 <= 0
 cConverterUseElectrolyzer8:     - vMaxPowerElectrolyzer + vPowElectrolyzer(8)
                                 <= 0
 cConverterUseElectrolyzer9:     - vMaxPowerElectrolyzer + vPowElectrolyzer(9)
                                 <= 0
 csInstalledH2_Load0:            vInstalledH2_Load  = 1
 csWeightH2_Load0:               vWeightH2_Load  = 1
 csBigMInstalledH2_Load0:        - vInstalledH2_Load + vWeightH2_Load <= 0
 csMinSizeInstalledH2_Load0:     vWeightH2_Load >= 0
 cMSFxH2_Load0:                  <= 10000
 cMSFxH2_Load1:                  <= 10000
 cMSFxH2_Load2:                  <= 10000
 cMSFxH2_Load3:                  <= 10000
 cMSFxH2_Load4:                  <= 10000
 cMSFxH2_Load5:                  <= 10000
 cMSFxH2_Load6:                  <= 10000
 cMSFxH2_Load7:                  <= 10000
 cMSFxH2_Load8:                  <= 10000
 cMSFxH2_Load9:                  <= 10000
 cSNodeLaw#10:                   vGFxElec_Grid(0) - vPowElectrolyzer(0)  = 0
 cSNodeLaw#11:                   vGFxElec_Grid(1) - vPowElectrolyzer(1)  = 0
 cSNodeLaw#12:                   vGFxElec_Grid(2) - vPowElectrolyzer(2)  = 0
 cSNodeLaw#13:                   vGFxElec_Grid(3) - vPowElectrolyzer(3)  = 0
 cSNodeLaw#14:                   vGFxElec_Grid(4) - vPowElectrolyzer(4)  = 0
 cSNodeLaw#15:                   vGFxElec_Grid(5) - vPowElectrolyzer(5)  = 0
 cSNodeLaw#16:                   vGFxElec_Grid(6) - vPowElectrolyzer(6)  = 0
 cSNodeLaw#17:                   vGFxElec_Grid(7) - vPowElectrolyzer(7)  = 0
 cSNodeLaw#18:                   vGFxElec_Grid(8) - vPowElectrolyzer(8)  = 0
 cSNodeLaw#19:                   vGFxElec_Grid(9) - vPowElectrolyzer(9)  = 0
 cSNodeLaw#20:                   vFlowElectrolyzer(0) - 720 vWeightH2_Load  = 0
 cSNodeLaw#21:                   vFlowElectrolyzer(1) - 720 vWeightH2_Load  = 0
 cSNodeLaw#22:                   vFlowElectrolyzer(2) - 720 vWeightH2_Load  = 0
 cSNodeLaw#23:                   vFlowElectrolyzer(3) - 720 vWeightH2_Load  = 0
 cSNodeLaw#24:                   vFlowElectrolyzer(4) - 720 vWeightH2_Load  = 0
 cSNodeLaw#25:                   vFlowElectrolyzer(5) - 720 vWeightH2_Load  = 0
 cSNodeLaw#26:                   vFlowElectrolyzer(6) - 720 vWeightH2_Load  = 0
 cSNodeLaw#27:                   vFlowElectrolyzer(7) - 720 vWeightH2_Load  = 0
 cSNodeLaw#28:                   vFlowElectrolyzer(8) - 720 vWeightH2_Load  = 0
 cSNodeLaw#29:                   vFlowElectrolyzer(9) - 720 vWeightH2_Load  = 0
Bounds
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
 0 <= vInstalledElectrolyzer <= 1
 0 <= vMaxPowerElectrolyzer <= 50
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
 0 <= vPowElectrolyzer(0) <= 50
 0 <= vPowElectrolyzer(1) <= 50
 0 <= vPowElectrolyzer(2) <= 50
 0 <= vPowElectrolyzer(3) <= 50
 0 <= vPowElectrolyzer(4) <= 50
 0 <= vPowElectrolyzer(5) <= 50
 0 <= vPowElectrolyzer(6) <= 50
 0 <= vPowElectrolyzer(7) <= 50
 0 <= vPowElectrolyzer(8) <= 50
 0 <= vPowElectrolyzer(9) <= 50
 0 <= vFlowElectrolyzer(0) <= 900.360144057623
 0 <= vFlowElectrolyzer(1) <= 900.360144057623
 0 <= vFlowElectrolyzer(2) <= 900.360144057623
 0 <= vFlowElectrolyzer(3) <= 900.360144057623
 0 <= vFlowElectrolyzer(4) <= 900.360144057623
 0 <= vFlowElectrolyzer(5) <= 900.360144057623
 0 <= vFlowElectrolyzer(6) <= 900.360144057623
 0 <= vFlowElectrolyzer(7) <= 900.360144057623
 0 <= vFlowElectrolyzer(8) <= 900.360144057623
 0 <= vFlowElectrolyzer(9) <= 900.360144057623
 0 <= vUsablePowElectrolyzer <= 50
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
 0 <= vInstalledH2_Load <= 1
 0 <= vWeightH2_Load <= 1
Generals
 vInstalledElec_Grid  vStateElec_Grid(0)  vStateElec_Grid(1) 
 vStateElec_Grid(2)  vStateElec_Grid(3)  vStateElec_Grid(4) 
 vStateElec_Grid(5)  vStateElec_Grid(6)  vStateElec_Grid(7) 
 vStateElec_Grid(8)  vStateElec_Grid(9)  vInstalledElectrolyzer 
 vStateElectrolyzer(0)  vStateElectrolyzer(1)  vStateElectrolyzer(2) 
 vStateElectrolyzer(3)  vStateElectrolyzer(4)  vStateElectrolyzer(5) 
 vStateElectrolyzer(6)  vStateElectrolyzer(7)  vStateElectrolyzer(8) 
 vStateElectrolyzer(9)  vInstalledH2_Load 
End
