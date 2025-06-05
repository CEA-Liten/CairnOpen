\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 697643.428659884 vMaxPowerELY_PEM + 1200336.12830005 vGFxElec_Grid(0)
       + 1200336.12830005 vGFxElec_Grid(1) + 1200336.12830005 vGFxElec_Grid(2)
       + 1200336.12830005 vGFxElec_Grid(3) + 1200336.12830005 vGFxElec_Grid(4)
       + 50 vMaxEStoH2_Tank
Subject To
 cStateUpELY_PEM0:               - vStateELY_PEM(0) + vStartUpELY_PEM(0) <= 0
 cStateDownELY_PEM0:             vStateELY_PEM(0) + vShutDownELY_PEM(0) <= 1
 cStatesUpDownELY_PEM0:          vStateELY_PEM(0) - vStartUpELY_PEM(0)
                                 + vShutDownELY_PEM(0)  = 0
 cStateUpELY_PEM1:               - vStateELY_PEM(1) + vStartUpELY_PEM(1) <= 0
 cStateDownELY_PEM1:             vStateELY_PEM(1) + vShutDownELY_PEM(1) <= 1
 cStatesUpDownELY_PEM1:          - vStateELY_PEM(0) + vStateELY_PEM(1)
                                 - vStartUpELY_PEM(1) + vShutDownELY_PEM(1)
                                  = 0
 cStateUpELY_PEM2:               - vStateELY_PEM(2) + vStartUpELY_PEM(2) <= 0
 cStateDownELY_PEM2:             vStateELY_PEM(2) + vShutDownELY_PEM(2) <= 1
 cStatesUpDownELY_PEM2:          - vStateELY_PEM(1) + vStateELY_PEM(2)
                                 - vStartUpELY_PEM(2) + vShutDownELY_PEM(2)
                                  = 0
 cStateUpELY_PEM3:               - vStateELY_PEM(3) + vStartUpELY_PEM(3) <= 0
 cStateDownELY_PEM3:             vStateELY_PEM(3) + vShutDownELY_PEM(3) <= 1
 cStatesUpDownELY_PEM3:          - vStateELY_PEM(2) + vStateELY_PEM(3)
                                 - vStartUpELY_PEM(3) + vShutDownELY_PEM(3)
                                  = 0
 cStateUpELY_PEM4:               - vStateELY_PEM(4) + vStartUpELY_PEM(4) <= 0
 cStateDownELY_PEM4:             vStateELY_PEM(4) + vShutDownELY_PEM(4) <= 1
 cStatesUpDownELY_PEM4:          - vStateELY_PEM(3) + vStateELY_PEM(4)
                                 - vStartUpELY_PEM(4) + vShutDownELY_PEM(4)
                                  = 0
 cPowFlowELY_PEM0:               vPowELY_PEM(0)
                                 - 0.0499775011249438 vFlowELY_PEM(0)  = 0
 cPowFlowELY_PEM1:               vPowELY_PEM(1)
                                 - 0.0499775011249438 vFlowELY_PEM(1)  = 0
 cPowFlowELY_PEM2:               vPowELY_PEM(2)
                                 - 0.0499775011249438 vFlowELY_PEM(2)  = 0
 cPowFlowELY_PEM3:               vPowELY_PEM(3)
                                 - 0.0499775011249438 vFlowELY_PEM(3)  = 0
 cPowFlowELY_PEM4:               vPowELY_PEM(4)
                                 - 0.0499775011249438 vFlowELY_PEM(4)  = 0
 ccUsePowELY_PEM0:               - vMaxPowerELY_PEM + vUsablePowELY_PEM  = 0
 ccMaxTotalPowELY_PEM0:          vPowELY_PEM(0) - vUsablePowELY_PEM <= 0
 ccMaxTotalPowELY_PEM1:          vPowELY_PEM(1) - vUsablePowELY_PEM <= 0
 ccMaxTotalPowELY_PEM2:          vPowELY_PEM(2) - vUsablePowELY_PEM <= 0
 ccMaxTotalPowELY_PEM3:          vPowELY_PEM(3) - vUsablePowELY_PEM <= 0
 ccMaxTotalPowELY_PEM4:          vPowELY_PEM(4) - vUsablePowELY_PEM <= 0
 cPowMaxELY_PEM0:                vPowELY_PEM(0) - 30 vStateELY_PEM(0) <= 0
 cPowMinZELY_PEM0:               vPowELY_PEM(0) - vPminELY_PEM(0) >= 0
 cDefZELY_PEM0:                  vPminELY_PEM(0) <= 0
 cZVarMinPELY_PEM0:              vPminELY_PEM(0) <= 0
 cLinearizedMinConstraintELY_PEM0: vPminELY_PEM(0) >= 0
 cPowMaxELY_PEM1:                vPowELY_PEM(1) - 30 vStateELY_PEM(1) <= 0
 cPowMinZELY_PEM1:               vPowELY_PEM(1) - vPminELY_PEM(1) >= 0
 cDefZELY_PEM1:                  vPminELY_PEM(1) <= 0
 cZVarMinPELY_PEM1:              vPminELY_PEM(1) <= 0
 cLinearizedMinConstraintELY_PEM1: vPminELY_PEM(1) >= 0
 cPowMaxELY_PEM2:                vPowELY_PEM(2) - 30 vStateELY_PEM(2) <= 0
 cPowMinZELY_PEM2:               vPowELY_PEM(2) - vPminELY_PEM(2) >= 0
 cDefZELY_PEM2:                  vPminELY_PEM(2) <= 0
 cZVarMinPELY_PEM2:              vPminELY_PEM(2) <= 0
 cLinearizedMinConstraintELY_PEM2: vPminELY_PEM(2) >= 0
 cPowMaxELY_PEM3:                vPowELY_PEM(3) - 30 vStateELY_PEM(3) <= 0
 cPowMinZELY_PEM3:               vPowELY_PEM(3) - vPminELY_PEM(3) >= 0
 cDefZELY_PEM3:                  vPminELY_PEM(3) <= 0
 cZVarMinPELY_PEM3:              vPminELY_PEM(3) <= 0
 cLinearizedMinConstraintELY_PEM3: vPminELY_PEM(3) >= 0
 cPowMaxELY_PEM4:                vPowELY_PEM(4) - 30 vStateELY_PEM(4) <= 0
 cPowMinZELY_PEM4:               vPowELY_PEM(4) - vPminELY_PEM(4) >= 0
 cDefZELY_PEM4:                  vPminELY_PEM(4) <= 0
 cZVarMinPELY_PEM4:              vPminELY_PEM(4) <= 0
 cLinearizedMinConstraintELY_PEM4: vPminELY_PEM(4) >= 0
 ccConverterUseELY_PEM0:         - vMaxPowerELY_PEM + vPowELY_PEM(0) <= 0
 ccConverterUseELY_PEM1:         - vMaxPowerELY_PEM + vPowELY_PEM(1) <= 0
 ccConverterUseELY_PEM2:         - vMaxPowerELY_PEM + vPowELY_PEM(2) <= 0
 ccConverterUseELY_PEM3:         - vMaxPowerELY_PEM + vPowELY_PEM(3) <= 0
 ccConverterUseELY_PEM4:         - vMaxPowerELY_PEM + vPowELY_PEM(4) <= 0
 csMaxValElec_Grid0:             vMaxFluxElec_Grid  = 400
 cStateGridElec_Grid0:           - vGFxElec_Grid(0) + 400 vStateElec_Grid(0)
                                 >= 0
 cStateGridElec_Grid1:           - vGFxElec_Grid(1) + 400 vStateElec_Grid(1)
                                 >= 0
 cStateGridElec_Grid2:           - vGFxElec_Grid(2) + 400 vStateElec_Grid(2)
                                 >= 0
 cStateGridElec_Grid3:           - vGFxElec_Grid(3) + 400 vStateElec_Grid(3)
                                 >= 0
 cStateGridElec_Grid4:           - vGFxElec_Grid(4) + 400 vStateElec_Grid(4)
                                 >= 0
 cMaxGFxElec_Grid0:              vMaxFluxElec_Grid - vGFxElec_Grid(0) >= 0
 cMaxGFxElec_Grid1:              vMaxFluxElec_Grid - vGFxElec_Grid(1) >= 0
 cMaxGFxElec_Grid2:              vMaxFluxElec_Grid - vGFxElec_Grid(2) >= 0
 cMaxGFxElec_Grid3:              vMaxFluxElec_Grid - vGFxElec_Grid(3) >= 0
 cMaxGFxElec_Grid4:              vMaxFluxElec_Grid - vGFxElec_Grid(4) >= 0
 csMaxValElec_Grid_Inject0:      vMaxFluxElec_Grid_Inject  = 1500000
 cStateGridElec_Grid_Inject0:    - vGFxElec_Grid_Inject(0)
                                 + 1500000 vStateElec_Grid_Inject(0) >= 0
 cStateGridElec_Grid_Inject1:    - vGFxElec_Grid_Inject(1)
                                 + 1500000 vStateElec_Grid_Inject(1) >= 0
 cStateGridElec_Grid_Inject2:    - vGFxElec_Grid_Inject(2)
                                 + 1500000 vStateElec_Grid_Inject(2) >= 0
 cStateGridElec_Grid_Inject3:    - vGFxElec_Grid_Inject(3)
                                 + 1500000 vStateElec_Grid_Inject(3) >= 0
 cStateGridElec_Grid_Inject4:    - vGFxElec_Grid_Inject(4)
                                 + 1500000 vStateElec_Grid_Inject(4) >= 0
 cMaxGFxElec_Grid_Inject0:       vMaxFluxElec_Grid_Inject
                                 - vGFxElec_Grid_Inject(0) >= 0
 cMaxGFxElec_Grid_Inject1:       vMaxFluxElec_Grid_Inject
                                 - vGFxElec_Grid_Inject(1) >= 0
 cMaxGFxElec_Grid_Inject2:       vMaxFluxElec_Grid_Inject
                                 - vGFxElec_Grid_Inject(2) >= 0
 cMaxGFxElec_Grid_Inject3:       vMaxFluxElec_Grid_Inject
                                 - vGFxElec_Grid_Inject(3) >= 0
 cMaxGFxElec_Grid_Inject4:       vMaxFluxElec_Grid_Inject
                                 - vGFxElec_Grid_Inject(4) >= 0
 csWeightH2_Load0:               vMWeightH2_Load  = 1
 cMSFxH2_Load0:                  <= 1000
 cMSFxH2_Load1:                  <= 1000
 cMSFxH2_Load2:                  <= 1000
 cMSFxH2_Load3:                  <= 1000
 cMSFxH2_Load4:                  <= 1000
 cStoIni3H2_Tank0:               vStoIniH2_Tank  = 0
 cMH2_Tank0:                     - vMaxEStoH2_Tank + vEH2_Tank(0) <= 0
 cStoH2_Tank0:                   vEH2_Tank(0) - 0.999 vQCH2_Tank(0)
                                 + 1.001001001001 vQDH2_Tank(0)
                                 - vStoIniH2_Tank  = 0
 cMH2_Tank1:                     - vMaxEStoH2_Tank + vEH2_Tank(1) <= 0
 cStoH2_Tank1:                   - vEH2_Tank(0) + vEH2_Tank(1)
                                 - 0.999 vQCH2_Tank(1)
                                 + 1.001001001001 vQDH2_Tank(1)  = 0
 cMH2_Tank2:                     - vMaxEStoH2_Tank + vEH2_Tank(2) <= 0
 cStoH2_Tank2:                   - vEH2_Tank(1) + vEH2_Tank(2)
                                 - 0.999 vQCH2_Tank(2)
                                 + 1.001001001001 vQDH2_Tank(2)  = 0
 cMH2_Tank3:                     - vMaxEStoH2_Tank + vEH2_Tank(3) <= 0
 cStoH2_Tank3:                   - vEH2_Tank(2) + vEH2_Tank(3)
                                 - 0.999 vQCH2_Tank(3)
                                 + 1.001001001001 vQDH2_Tank(3)  = 0
 cMH2_Tank4:                     - vMaxEStoH2_Tank + vEH2_Tank(4) <= 0
 cStoH2_Tank4:                   - vEH2_Tank(3) + vEH2_Tank(4)
                                 - 0.999 vQCH2_Tank(4)
                                 + 1.001001001001 vQDH2_Tank(4)  = 0
 cMFCH2_Tank0:                   vQCH2_Tank(0) - 1100 vsensH2_Tank(0) <= 0
 cMFDH2_Tank0:                   vQDH2_Tank(0) + 1200 vsensH2_Tank(0) <= 1200
 cMFCH2_Tank1:                   vQCH2_Tank(1) - 1100 vsensH2_Tank(1) <= 0
 cMFDH2_Tank1:                   vQDH2_Tank(1) + 1200 vsensH2_Tank(1) <= 1200
 cMFCH2_Tank2:                   vQCH2_Tank(2) - 1100 vsensH2_Tank(2) <= 0
 cMFDH2_Tank2:                   vQDH2_Tank(2) + 1200 vsensH2_Tank(2) <= 1200
 cMFCH2_Tank3:                   vQCH2_Tank(3) - 1100 vsensH2_Tank(3) <= 0
 cMFDH2_Tank3:                   vQDH2_Tank(3) + 1200 vsensH2_Tank(3) <= 1200
 cMFCH2_Tank4:                   vQCH2_Tank(4) - 1100 vsensH2_Tank(4) <= 0
 cMFDH2_Tank4:                   vQDH2_Tank(4) + 1200 vsensH2_Tank(4) <= 1200
 cMSFxWind_farm0:                1.84581899642944 vMWeightWind_farm <= 1000000
 cMSFxWind_farm1:                2.19129395484924 vMWeightWind_farm <= 1000000
 cMSFxWind_farm2:                2.32618689537048 vMWeightWind_farm <= 1000000
 cMSFxWind_farm3:                2.48392105102539 vMWeightWind_farm <= 1000000
 cMSFxWind_farm4:                2.67351007461548 vMWeightWind_farm <= 1000000
 cSElec_Bus0:                    - vPowELY_PEM(0) + vGFxElec_Grid(0)
                                 - vGFxElec_Grid_Inject(0)
                                 + 1.84581899642944 vMWeightWind_farm  = 0
 cSElec_Bus1:                    - vPowELY_PEM(1) + vGFxElec_Grid(1)
                                 - vGFxElec_Grid_Inject(1)
                                 + 2.19129395484924 vMWeightWind_farm  = 0
 cSElec_Bus2:                    - vPowELY_PEM(2) + vGFxElec_Grid(2)
                                 - vGFxElec_Grid_Inject(2)
                                 + 2.32618689537048 vMWeightWind_farm  = 0
 cSElec_Bus3:                    - vPowELY_PEM(3) + vGFxElec_Grid(3)
                                 - vGFxElec_Grid_Inject(3)
                                 + 2.48392105102539 vMWeightWind_farm  = 0
 cSElec_Bus4:                    - vPowELY_PEM(4) + vGFxElec_Grid(4)
                                 - vGFxElec_Grid_Inject(4)
                                 + 2.67351007461548 vMWeightWind_farm  = 0
 cSH2_Bus0:                      vFlowELY_PEM(0) - vQCH2_Tank(0)
                                 + vQDH2_Tank(0)  = 0
 cSH2_Bus1:                      vFlowELY_PEM(1) - vQCH2_Tank(1)
                                 + vQDH2_Tank(1)  = 0
 cSH2_Bus2:                      vFlowELY_PEM(2) - vQCH2_Tank(2)
                                 + vQDH2_Tank(2)  = 0
 cSH2_Bus3:                      vFlowELY_PEM(3) - vQCH2_Tank(3)
                                 + vQDH2_Tank(3)  = 0
 cSH2_Bus4:                      vFlowELY_PEM(4) - vQCH2_Tank(4)
                                 + vQDH2_Tank(4)  = 0
Bounds
 0 <= vMaxPowerELY_PEM <= 30
 0 <= vPowELY_PEM(0) <= 30
 0 <= vPowELY_PEM(1) <= 30
 0 <= vPowELY_PEM(2) <= 30
 0 <= vPowELY_PEM(3) <= 30
 0 <= vPowELY_PEM(4) <= 30
 0 <= vFlowELY_PEM(0) <= 600.270108043217
 0 <= vFlowELY_PEM(1) <= 600.270108043217
 0 <= vFlowELY_PEM(2) <= 600.270108043217
 0 <= vFlowELY_PEM(3) <= 600.270108043217
 0 <= vFlowELY_PEM(4) <= 600.270108043217
 0 <= vUsablePowELY_PEM <= 30
 0 <= vStateELY_PEM(0) <= 1
 0 <= vStateELY_PEM(1) <= 1
 0 <= vStateELY_PEM(2) <= 1
 0 <= vStateELY_PEM(3) <= 1
 0 <= vStateELY_PEM(4) <= 1
 0 <= vStartUpELY_PEM(0) <= 1
 0 <= vStartUpELY_PEM(1) <= 1
 0 <= vStartUpELY_PEM(2) <= 1
 0 <= vStartUpELY_PEM(3) <= 1
 0 <= vStartUpELY_PEM(4) <= 1
 0 <= vShutDownELY_PEM(0) <= 1
 0 <= vShutDownELY_PEM(1) <= 1
 0 <= vShutDownELY_PEM(2) <= 1
 0 <= vShutDownELY_PEM(3) <= 1
 0 <= vShutDownELY_PEM(4) <= 1
      vPminELY_PEM(0) Free
      vPminELY_PEM(1) Free
      vPminELY_PEM(2) Free
      vPminELY_PEM(3) Free
      vPminELY_PEM(4) Free
 0 <= vMaxFluxElec_Grid <= 400
 0 <= vGFxElec_Grid(0) <= 400
 0 <= vGFxElec_Grid(1) <= 400
 0 <= vGFxElec_Grid(2) <= 400
 0 <= vGFxElec_Grid(3) <= 400
 0 <= vGFxElec_Grid(4) <= 400
 0 <= vStateElec_Grid(0) <= 1
 0 <= vStateElec_Grid(1) <= 1
 0 <= vStateElec_Grid(2) <= 1
 0 <= vStateElec_Grid(3) <= 1
 0 <= vStateElec_Grid(4) <= 1
 0 <= vMaxFluxElec_Grid_Inject <= 1500000
 0 <= vGFxElec_Grid_Inject(0) <= 1500000
 0 <= vGFxElec_Grid_Inject(1) <= 1500000
 0 <= vGFxElec_Grid_Inject(2) <= 1500000
 0 <= vGFxElec_Grid_Inject(3) <= 1500000
 0 <= vGFxElec_Grid_Inject(4) <= 1500000
 0 <= vStateElec_Grid_Inject(0) <= 1
 0 <= vStateElec_Grid_Inject(1) <= 1
 0 <= vStateElec_Grid_Inject(2) <= 1
 0 <= vStateElec_Grid_Inject(3) <= 1
 0 <= vStateElec_Grid_Inject(4) <= 1
 0 <= vMWeightH2_Load <= 1
 0 <= vMaxEStoH2_Tank <= 20000000000
 0 <= vEH2_Tank(0) <= 20000000000
 0 <= vEH2_Tank(1) <= 20000000000
 0 <= vEH2_Tank(2) <= 20000000000
 0 <= vEH2_Tank(3) <= 20000000000
 0 <= vEH2_Tank(4) <= 20000000000
 0 <= vQCH2_Tank(0) <= 1100
 0 <= vQCH2_Tank(1) <= 1100
 0 <= vQCH2_Tank(2) <= 1100
 0 <= vQCH2_Tank(3) <= 1100
 0 <= vQCH2_Tank(4) <= 1100
 0 <= vQDH2_Tank(0) <= 1200
 0 <= vQDH2_Tank(1) <= 1200
 0 <= vQDH2_Tank(2) <= 1200
 0 <= vQDH2_Tank(3) <= 1200
 0 <= vQDH2_Tank(4) <= 1200
 0 <= vStoIniH2_Tank <= 20000000000
 0 <= vsensH2_Tank(0) <= 1
 0 <= vsensH2_Tank(1) <= 1
 0 <= vsensH2_Tank(2) <= 1
 0 <= vsensH2_Tank(3) <= 1
 0 <= vsensH2_Tank(4) <= 1
 0 <= vMWeightWind_farm <= 1
Generals
 vStateELY_PEM(0)  vStateELY_PEM(1)  vStateELY_PEM(2)  vStateELY_PEM(3) 
 vStateELY_PEM(4)  vStartUpELY_PEM(0)  vStartUpELY_PEM(1)  vStartUpELY_PEM(2) 
 vStartUpELY_PEM(3)  vStartUpELY_PEM(4)  vShutDownELY_PEM(0) 
 vShutDownELY_PEM(1)  vShutDownELY_PEM(2)  vShutDownELY_PEM(3) 
 vShutDownELY_PEM(4)  vStateElec_Grid(0)  vStateElec_Grid(1) 
 vStateElec_Grid(2)  vStateElec_Grid(3)  vStateElec_Grid(4) 
 vStateElec_Grid_Inject(0)  vStateElec_Grid_Inject(1) 
 vStateElec_Grid_Inject(2)  vStateElec_Grid_Inject(3) 
 vStateElec_Grid_Inject(4)  vsensH2_Tank(0)  vsensH2_Tank(1)  vsensH2_Tank(2) 
 vsensH2_Tank(3)  vsensH2_Tank(4) 
End
