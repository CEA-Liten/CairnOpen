\ File written by HiGHS .lp file handler
min
 obj: +697643.428659884 vMaxPowerELY_PEM +600168.077786779 vGFxElec_Grid(0) +600168.077786779 vGFxElec_Grid(1) +600168.077786779 vGFxElec_Grid(2) +600168.077786779 vGFxElec_Grid(3) +600168.077786779 vGFxElec_Grid(4) +600168.077786779 vGFxElec_Grid(5) +717937.657577501 vGFxElec_Grid(6) +717937.657577501 vGFxElec_Grid(7) +717937.657577501 vGFxElec_Grid(8) +717937.657577501 vGFxElec_Grid(9) +50 vMaxEstoH2_Tank 
st
 csBigMInstalledELY_PEM0: -30 vInstalledELY_PEM +1 vMaxPowerELY_PEM <= -0
 csMinSizeInstalledELY_PEM0: +1 vMaxPowerELY_PEM >= -0
 cNotRunningIfNotInstalledELY_PEM0: -1 vInstalledELY_PEM +1 vStateELY_PEM(0) <= -0
 cNotRunningIfNotInstalledELY_PEM1: -1 vInstalledELY_PEM +1 vStateELY_PEM(1) <= -0
 cNotRunningIfNotInstalledELY_PEM2: -1 vInstalledELY_PEM +1 vStateELY_PEM(2) <= -0
 cNotRunningIfNotInstalledELY_PEM3: -1 vInstalledELY_PEM +1 vStateELY_PEM(3) <= -0
 cNotRunningIfNotInstalledELY_PEM4: -1 vInstalledELY_PEM +1 vStateELY_PEM(4) <= -0
 cNotRunningIfNotInstalledELY_PEM5: -1 vInstalledELY_PEM +1 vStateELY_PEM(5) <= -0
 cNotRunningIfNotInstalledELY_PEM6: -1 vInstalledELY_PEM +1 vStateELY_PEM(6) <= -0
 cNotRunningIfNotInstalledELY_PEM7: -1 vInstalledELY_PEM +1 vStateELY_PEM(7) <= -0
 cNotRunningIfNotInstalledELY_PEM8: -1 vInstalledELY_PEM +1 vStateELY_PEM(8) <= -0
 cNotRunningIfNotInstalledELY_PEM9: -1 vInstalledELY_PEM +1 vStateELY_PEM(9) <= -0
 cPowFlowELY_PEM0: +1 vPowELY_PEM(0) -0.0499775011249438 vFlowELY_PEM(0) = +0
 cPowFlowELY_PEM1: +1 vPowELY_PEM(1) -0.0499775011249438 vFlowELY_PEM(1) = +0
 cPowFlowELY_PEM2: +1 vPowELY_PEM(2) -0.0499775011249438 vFlowELY_PEM(2) = +0
 cPowFlowELY_PEM3: +1 vPowELY_PEM(3) -0.0499775011249438 vFlowELY_PEM(3) = +0
 cPowFlowELY_PEM4: +1 vPowELY_PEM(4) -0.0499775011249438 vFlowELY_PEM(4) = +0
 cPowFlowELY_PEM5: +1 vPowELY_PEM(5) -0.0499775011249438 vFlowELY_PEM(5) = +0
 cPowFlowELY_PEM6: +1 vPowELY_PEM(6) -0.0499775011249438 vFlowELY_PEM(6) = +0
 cPowFlowELY_PEM7: +1 vPowELY_PEM(7) -0.0499775011249438 vFlowELY_PEM(7) = +0
 cPowFlowELY_PEM8: +1 vPowELY_PEM(8) -0.0499775011249438 vFlowELY_PEM(8) = +0
 cPowFlowELY_PEM9: +1 vPowELY_PEM(9) -0.0499775011249438 vFlowELY_PEM(9) = +0
 cUsePowELY_PEM0: -1 vMaxPowerELY_PEM +1 vUsablePowELY_PEM = +0
 cMaxTotalPowELY_PEM0: +1 vPowELY_PEM(0) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM1: +1 vPowELY_PEM(1) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM2: +1 vPowELY_PEM(2) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM3: +1 vPowELY_PEM(3) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM4: +1 vPowELY_PEM(4) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM5: +1 vPowELY_PEM(5) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM6: +1 vPowELY_PEM(6) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM7: +1 vPowELY_PEM(7) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM8: +1 vPowELY_PEM(8) -1 vUsablePowELY_PEM <= -0
 cMaxTotalPowELY_PEM9: +1 vPowELY_PEM(9) -1 vUsablePowELY_PEM <= -0
 cPowMaxELY_PEM0: -30 vStateELY_PEM(0) +1 vPowELY_PEM(0) <= +0
 cDefPminELY_PEM0: -1 vPminELY_PEM(0) = -0
 cPminELY_PEM0: +1 vPowELY_PEM(0) -1 vPminELY_PEM(0) >= -0
 cPowMaxELY_PEM1: -30 vStateELY_PEM(1) +1 vPowELY_PEM(1) <= +0
 cDefPminELY_PEM1: -1 vPminELY_PEM(1) = -0
 cPminELY_PEM1: +1 vPowELY_PEM(1) -1 vPminELY_PEM(1) >= -0
 cPowMaxELY_PEM2: -30 vStateELY_PEM(2) +1 vPowELY_PEM(2) <= +0
 cDefPminELY_PEM2: -1 vPminELY_PEM(2) = -0
 cPminELY_PEM2: +1 vPowELY_PEM(2) -1 vPminELY_PEM(2) >= -0
 cPowMaxELY_PEM3: -30 vStateELY_PEM(3) +1 vPowELY_PEM(3) <= +0
 cDefPminELY_PEM3: -1 vPminELY_PEM(3) = -0
 cPminELY_PEM3: +1 vPowELY_PEM(3) -1 vPminELY_PEM(3) >= -0
 cPowMaxELY_PEM4: -30 vStateELY_PEM(4) +1 vPowELY_PEM(4) <= +0
 cDefPminELY_PEM4: -1 vPminELY_PEM(4) = -0
 cPminELY_PEM4: +1 vPowELY_PEM(4) -1 vPminELY_PEM(4) >= -0
 cPowMaxELY_PEM5: -30 vStateELY_PEM(5) +1 vPowELY_PEM(5) <= +0
 cDefPminELY_PEM5: -1 vPminELY_PEM(5) = -0
 cPminELY_PEM5: +1 vPowELY_PEM(5) -1 vPminELY_PEM(5) >= -0
 cPowMaxELY_PEM6: -30 vStateELY_PEM(6) +1 vPowELY_PEM(6) <= +0
 cDefPminELY_PEM6: -1 vPminELY_PEM(6) = -0
 cPminELY_PEM6: +1 vPowELY_PEM(6) -1 vPminELY_PEM(6) >= -0
 cPowMaxELY_PEM7: -30 vStateELY_PEM(7) +1 vPowELY_PEM(7) <= +0
 cDefPminELY_PEM7: -1 vPminELY_PEM(7) = -0
 cPminELY_PEM7: +1 vPowELY_PEM(7) -1 vPminELY_PEM(7) >= -0
 cPowMaxELY_PEM8: -30 vStateELY_PEM(8) +1 vPowELY_PEM(8) <= +0
 cDefPminELY_PEM8: -1 vPminELY_PEM(8) = -0
 cPminELY_PEM8: +1 vPowELY_PEM(8) -1 vPminELY_PEM(8) >= -0
 cPowMaxELY_PEM9: -30 vStateELY_PEM(9) +1 vPowELY_PEM(9) <= +0
 cDefPminELY_PEM9: -1 vPminELY_PEM(9) = -0
 cPminELY_PEM9: +1 vPowELY_PEM(9) -1 vPminELY_PEM(9) >= -0
 cConverterUseELY_PEM0: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(0) <= +0
 cConverterUseELY_PEM1: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(1) <= +0
 cConverterUseELY_PEM2: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(2) <= +0
 cConverterUseELY_PEM3: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(3) <= +0
 cConverterUseELY_PEM4: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(4) <= +0
 cConverterUseELY_PEM5: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(5) <= +0
 cConverterUseELY_PEM6: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(6) <= +0
 cConverterUseELY_PEM7: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(7) <= +0
 cConverterUseELY_PEM8: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(8) <= +0
 cConverterUseELY_PEM9: -1 vMaxPowerELY_PEM +1 vPowELY_PEM(9) <= +0
 csInstalledElec_Grid0: +1 vInstalledElec_Grid = +1
 csMaxValElec_Grid0: +1 vMaxFlowElec_Grid = +400
 csBigMInstalledElec_Grid0: -400 vInstalledElec_Grid +1 vMaxFlowElec_Grid <= -0
 csMinSizeInstalledElec_Grid0: +1 vMaxFlowElec_Grid >= -0
 cNotRunningIfNotInstalledElec_Grid0: -1 vInstalledElec_Grid +1 vStateElec_Grid(0) <= -0
 cNotRunningIfNotInstalledElec_Grid1: -1 vInstalledElec_Grid +1 vStateElec_Grid(1) <= -0
 cNotRunningIfNotInstalledElec_Grid2: -1 vInstalledElec_Grid +1 vStateElec_Grid(2) <= -0
 cNotRunningIfNotInstalledElec_Grid3: -1 vInstalledElec_Grid +1 vStateElec_Grid(3) <= -0
 cNotRunningIfNotInstalledElec_Grid4: -1 vInstalledElec_Grid +1 vStateElec_Grid(4) <= -0
 cNotRunningIfNotInstalledElec_Grid5: -1 vInstalledElec_Grid +1 vStateElec_Grid(5) <= -0
 cNotRunningIfNotInstalledElec_Grid6: -1 vInstalledElec_Grid +1 vStateElec_Grid(6) <= -0
 cNotRunningIfNotInstalledElec_Grid7: -1 vInstalledElec_Grid +1 vStateElec_Grid(7) <= -0
 cNotRunningIfNotInstalledElec_Grid8: -1 vInstalledElec_Grid +1 vStateElec_Grid(8) <= -0
 cNotRunningIfNotInstalledElec_Grid9: -1 vInstalledElec_Grid +1 vStateElec_Grid(9) <= -0
 cStateGridElec_Grid0: -400 vStateElec_Grid(0) +1 vGFxElec_Grid(0) <= -0
 cStateGridElec_Grid1: -400 vStateElec_Grid(1) +1 vGFxElec_Grid(1) <= -0
 cStateGridElec_Grid2: -400 vStateElec_Grid(2) +1 vGFxElec_Grid(2) <= -0
 cStateGridElec_Grid3: -400 vStateElec_Grid(3) +1 vGFxElec_Grid(3) <= -0
 cStateGridElec_Grid4: -400 vStateElec_Grid(4) +1 vGFxElec_Grid(4) <= -0
 cStateGridElec_Grid5: -400 vStateElec_Grid(5) +1 vGFxElec_Grid(5) <= -0
 cStateGridElec_Grid6: -400 vStateElec_Grid(6) +1 vGFxElec_Grid(6) <= -0
 cStateGridElec_Grid7: -400 vStateElec_Grid(7) +1 vGFxElec_Grid(7) <= -0
 cStateGridElec_Grid8: -400 vStateElec_Grid(8) +1 vGFxElec_Grid(8) <= -0
 cStateGridElec_Grid9: -400 vStateElec_Grid(9) +1 vGFxElec_Grid(9) <= -0
 cMaxGFxElec_Grid0: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(0) <= -0
 cMaxGFxElec_Grid1: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(1) <= -0
 cMaxGFxElec_Grid2: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(2) <= -0
 cMaxGFxElec_Grid3: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(3) <= -0
 cMaxGFxElec_Grid4: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(4) <= -0
 cMaxGFxElec_Grid5: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(5) <= -0
 cMaxGFxElec_Grid6: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(6) <= -0
 cMaxGFxElec_Grid7: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(7) <= -0
 cMaxGFxElec_Grid8: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(8) <= -0
 cMaxGFxElec_Grid9: -1 vMaxFlowElec_Grid +1 vGFxElec_Grid(9) <= -0
 csInstalledElec_Grid_Inject0: +1 vInstalledElec_Grid_Inject = +1
 csMaxValElec_Grid_Inject0: +1 vMaxFlowElec_Grid_Inject = +1500000
 csBigMInstalledElec_Grid_Inject0: -1500000 vInstalledElec_Grid_Inject +1 vMaxFlowElec_Grid_Inject <= -0
 csMinSizeInstalledElec_Grid_Inject0: +1 vMaxFlowElec_Grid_Inject >= -0
 cNotRunningIfNotInstalledElec_Grid_Inject0: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(0) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject1: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(1) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject2: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(2) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject3: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(3) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject4: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(4) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject5: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(5) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject6: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(6) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject7: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(7) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject8: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(8) <= -0
 cNotRunningIfNotInstalledElec_Grid_Inject9: -1 vInstalledElec_Grid_Inject +1 vStateElec_Grid_Inject(9) <= -0
 cStateGridElec_Grid_Inject0: -1500000 vStateElec_Grid_Inject(0) +1 vGFxElec_Grid_Inject(0) <= -0
 cStateGridElec_Grid_Inject1: -1500000 vStateElec_Grid_Inject(1) +1 vGFxElec_Grid_Inject(1) <= -0
 cStateGridElec_Grid_Inject2: -1500000 vStateElec_Grid_Inject(2) +1 vGFxElec_Grid_Inject(2) <= -0
 cStateGridElec_Grid_Inject3: -1500000 vStateElec_Grid_Inject(3) +1 vGFxElec_Grid_Inject(3) <= -0
 cStateGridElec_Grid_Inject4: -1500000 vStateElec_Grid_Inject(4) +1 vGFxElec_Grid_Inject(4) <= -0
 cStateGridElec_Grid_Inject5: -1500000 vStateElec_Grid_Inject(5) +1 vGFxElec_Grid_Inject(5) <= -0
 cStateGridElec_Grid_Inject6: -1500000 vStateElec_Grid_Inject(6) +1 vGFxElec_Grid_Inject(6) <= -0
 cStateGridElec_Grid_Inject7: -1500000 vStateElec_Grid_Inject(7) +1 vGFxElec_Grid_Inject(7) <= -0
 cStateGridElec_Grid_Inject8: -1500000 vStateElec_Grid_Inject(8) +1 vGFxElec_Grid_Inject(8) <= -0
 cStateGridElec_Grid_Inject9: -1500000 vStateElec_Grid_Inject(9) +1 vGFxElec_Grid_Inject(9) <= -0
 cMaxGFxElec_Grid_Inject0: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(0) <= -0
 cMaxGFxElec_Grid_Inject1: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(1) <= -0
 cMaxGFxElec_Grid_Inject2: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(2) <= -0
 cMaxGFxElec_Grid_Inject3: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(3) <= -0
 cMaxGFxElec_Grid_Inject4: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(4) <= -0
 cMaxGFxElec_Grid_Inject5: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(5) <= -0
 cMaxGFxElec_Grid_Inject6: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(6) <= -0
 cMaxGFxElec_Grid_Inject7: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(7) <= -0
 cMaxGFxElec_Grid_Inject8: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(8) <= -0
 cMaxGFxElec_Grid_Inject9: -1 vMaxFlowElec_Grid_Inject +1 vGFxElec_Grid_Inject(9) <= -0
 csInstalledH2_Load0: +1 vInstalledH2_Load = +1
 csWeightH2_Load0: +1 vWeightH2_Load = +1
 csBigMInstalledH2_Load0: -1 vInstalledH2_Load +1 vWeightH2_Load <= -0
 csMinSizeInstalledH2_Load0: +1 vWeightH2_Load >= -0
 cMSFxH2_Load0: <= +1000
 cMSFxH2_Load1: <= +1000
 cMSFxH2_Load2: <= +1000
 cMSFxH2_Load3: <= +1000
 cMSFxH2_Load4: <= +1000
 cMSFxH2_Load5: <= +1000
 cMSFxH2_Load6: <= +1000
 cMSFxH2_Load7: <= +1000
 cMSFxH2_Load8: <= +1000
 cMSFxH2_Load9: <= +1000
 csBigMInstalledH2_Tank0: -20000000000 vInstalledH2_Tank +1 vMaxEstoH2_Tank <= -0
 csMinSizeInstalledH2_Tank0: +1 vMaxEstoH2_Tank >= -0
 cStoIni3H2_Tank0: +1 vStoIniH2_Tank = -0
 cMH2_Tank0: -1 vMaxEstoH2_Tank +1 vEH2_Tank(0) <= +0
 cStoH2_Tank0: +1 vEH2_Tank(0) -0.999 vQCH2_Tank(0) +1.001001001001 vQDH2_Tank(0) -1 vStoIniH2_Tank = +0
 cMH2_Tank1: -1 vMaxEstoH2_Tank +1 vEH2_Tank(1) <= +0
 cStoH2_Tank1: -1 vEH2_Tank(0) +1 vEH2_Tank(1) -0.999 vQCH2_Tank(1) +1.001001001001 vQDH2_Tank(1) = +0
 cMH2_Tank2: -1 vMaxEstoH2_Tank +1 vEH2_Tank(2) <= +0
 cStoH2_Tank2: -1 vEH2_Tank(1) +1 vEH2_Tank(2) -0.999 vQCH2_Tank(2) +1.001001001001 vQDH2_Tank(2) = +0
 cMH2_Tank3: -1 vMaxEstoH2_Tank +1 vEH2_Tank(3) <= +0
 cStoH2_Tank3: -1 vEH2_Tank(2) +1 vEH2_Tank(3) -0.999 vQCH2_Tank(3) +1.001001001001 vQDH2_Tank(3) = +0
 cMH2_Tank4: -1 vMaxEstoH2_Tank +1 vEH2_Tank(4) <= +0
 cStoH2_Tank4: -1 vEH2_Tank(3) +1 vEH2_Tank(4) -0.999 vQCH2_Tank(4) +1.001001001001 vQDH2_Tank(4) = +0
 cMH2_Tank5: -1 vMaxEstoH2_Tank +1 vEH2_Tank(5) <= +0
 cStoH2_Tank5: -1 vEH2_Tank(4) +1 vEH2_Tank(5) -0.999 vQCH2_Tank(5) +1.001001001001 vQDH2_Tank(5) = +0
 cMH2_Tank6: -1 vMaxEstoH2_Tank +1 vEH2_Tank(6) <= +0
 cStoH2_Tank6: -1 vEH2_Tank(5) +1 vEH2_Tank(6) -0.999 vQCH2_Tank(6) +1.001001001001 vQDH2_Tank(6) = +0
 cMH2_Tank7: -1 vMaxEstoH2_Tank +1 vEH2_Tank(7) <= +0
 cStoH2_Tank7: -1 vEH2_Tank(6) +1 vEH2_Tank(7) -0.999 vQCH2_Tank(7) +1.001001001001 vQDH2_Tank(7) = +0
 cMH2_Tank8: -1 vMaxEstoH2_Tank +1 vEH2_Tank(8) <= +0
 cStoH2_Tank8: -1 vEH2_Tank(7) +1 vEH2_Tank(8) -0.999 vQCH2_Tank(8) +1.001001001001 vQDH2_Tank(8) = +0
 cMH2_Tank9: -1 vMaxEstoH2_Tank +1 vEH2_Tank(9) <= +0
 cStoH2_Tank9: -1 vEH2_Tank(8) +1 vEH2_Tank(9) -0.999 vQCH2_Tank(9) +1.001001001001 vQDH2_Tank(9) = +0
 cMFCH2_Tank0: +1 vQCH2_Tank(0) -1100 vsensH2_Tank(0) <= +0
 cMFDH2_Tank0: +1 vQDH2_Tank(0) +1200 vsensH2_Tank(0) <= +1200
 cMFCH2_Tank1: +1 vQCH2_Tank(1) -1100 vsensH2_Tank(1) <= +0
 cMFDH2_Tank1: +1 vQDH2_Tank(1) +1200 vsensH2_Tank(1) <= +1200
 cMFCH2_Tank2: +1 vQCH2_Tank(2) -1100 vsensH2_Tank(2) <= +0
 cMFDH2_Tank2: +1 vQDH2_Tank(2) +1200 vsensH2_Tank(2) <= +1200
 cMFCH2_Tank3: +1 vQCH2_Tank(3) -1100 vsensH2_Tank(3) <= +0
 cMFDH2_Tank3: +1 vQDH2_Tank(3) +1200 vsensH2_Tank(3) <= +1200
 cMFCH2_Tank4: +1 vQCH2_Tank(4) -1100 vsensH2_Tank(4) <= +0
 cMFDH2_Tank4: +1 vQDH2_Tank(4) +1200 vsensH2_Tank(4) <= +1200
 cMFCH2_Tank5: +1 vQCH2_Tank(5) -1100 vsensH2_Tank(5) <= +0
 cMFDH2_Tank5: +1 vQDH2_Tank(5) +1200 vsensH2_Tank(5) <= +1200
 cMFCH2_Tank6: +1 vQCH2_Tank(6) -1100 vsensH2_Tank(6) <= +0
 cMFDH2_Tank6: +1 vQDH2_Tank(6) +1200 vsensH2_Tank(6) <= +1200
 cMFCH2_Tank7: +1 vQCH2_Tank(7) -1100 vsensH2_Tank(7) <= +0
 cMFDH2_Tank7: +1 vQDH2_Tank(7) +1200 vsensH2_Tank(7) <= +1200
 cMFCH2_Tank8: +1 vQCH2_Tank(8) -1100 vsensH2_Tank(8) <= +0
 cMFDH2_Tank8: +1 vQDH2_Tank(8) +1200 vsensH2_Tank(8) <= +1200
 cMFCH2_Tank9: +1 vQCH2_Tank(9) -1100 vsensH2_Tank(9) <= +0
 cMFDH2_Tank9: +1 vQDH2_Tank(9) +1200 vsensH2_Tank(9) <= +1200
 csBigMInstalledWind_farm0: -1 vInstalledWind_farm +1 vWeightWind_farm <= -0
 csMinSizeInstalledWind_farm0: +1 vWeightWind_farm >= -0
 cMSFxWind_farm0: <= +1000000
 cMSFxWind_farm1: <= +1000000
 cMSFxWind_farm2: <= +1000000
 cMSFxWind_farm3: <= +1000000
 cMSFxWind_farm4: <= +1000000
 cMSFxWind_farm5: <= +1000000
 cMSFxWind_farm6: <= +1000000
 cMSFxWind_farm7: <= +1000000
 cMSFxWind_farm8: <= +1000000
 cMSFxWind_farm9: <= +1000000
 cSElec_Bus0: -1 vPowELY_PEM(0) +1 vGFxElec_Grid(0) -1 vGFxElec_Grid_Inject(0) +1.845819 vWeightWind_farm = +0
 cSElec_Bus1: -1 vPowELY_PEM(1) +1 vGFxElec_Grid(1) -1 vGFxElec_Grid_Inject(1) +2.191294 vWeightWind_farm = +0
 cSElec_Bus2: -1 vPowELY_PEM(2) +1 vGFxElec_Grid(2) -1 vGFxElec_Grid_Inject(2) +2.326187 vWeightWind_farm = +0
 cSElec_Bus3: -1 vPowELY_PEM(3) +1 vGFxElec_Grid(3) -1 vGFxElec_Grid_Inject(3) +2.483921 vWeightWind_farm = +0
 cSElec_Bus4: -1 vPowELY_PEM(4) +1 vGFxElec_Grid(4) -1 vGFxElec_Grid_Inject(4) +2.67351 vWeightWind_farm = +0
 cSElec_Bus5: -1 vPowELY_PEM(5) +1 vGFxElec_Grid(5) -1 vGFxElec_Grid_Inject(5) +2.85778 vWeightWind_farm = +0
 cSElec_Bus6: -1 vPowELY_PEM(6) +1 vGFxElec_Grid(6) -1 vGFxElec_Grid_Inject(6) +2.98878 vWeightWind_farm = +0
 cSElec_Bus7: -1 vPowELY_PEM(7) +1 vGFxElec_Grid(7) -1 vGFxElec_Grid_Inject(7) +3.033381 vWeightWind_farm = +0
 cSElec_Bus8: -1 vPowELY_PEM(8) +1 vGFxElec_Grid(8) -1 vGFxElec_Grid_Inject(8) +2.807039 vWeightWind_farm = +0
 cSElec_Bus9: -1 vPowELY_PEM(9) +1 vGFxElec_Grid(9) -1 vGFxElec_Grid_Inject(9) +2.311192 vWeightWind_farm = +0
 cSH2_Bus0: +1 vFlowELY_PEM(0) -1 vQCH2_Tank(0) +1 vQDH2_Tank(0) = +0
 cSH2_Bus1: +1 vFlowELY_PEM(1) -1 vQCH2_Tank(1) +1 vQDH2_Tank(1) = +0
 cSH2_Bus2: +1 vFlowELY_PEM(2) -1 vQCH2_Tank(2) +1 vQDH2_Tank(2) = +0
 cSH2_Bus3: +1 vFlowELY_PEM(3) -1 vQCH2_Tank(3) +1 vQDH2_Tank(3) = +0
 cSH2_Bus4: +1 vFlowELY_PEM(4) -1 vQCH2_Tank(4) +1 vQDH2_Tank(4) = +0
 cSH2_Bus5: +1 vFlowELY_PEM(5) -1 vQCH2_Tank(5) +1 vQDH2_Tank(5) = +0
 cSH2_Bus6: +1 vFlowELY_PEM(6) -1 vQCH2_Tank(6) +1 vQDH2_Tank(6) = +0
 cSH2_Bus7: +1 vFlowELY_PEM(7) -1 vQCH2_Tank(7) +1 vQDH2_Tank(7) = +0
 cSH2_Bus8: +1 vFlowELY_PEM(8) -1 vQCH2_Tank(8) +1 vQDH2_Tank(8) = +0
 cSH2_Bus9: +1 vFlowELY_PEM(9) -1 vQCH2_Tank(9) +1 vQDH2_Tank(9) = +0
bounds
 vInstalledELY_PEM <= 1
 vMaxPowerELY_PEM <= 30
 vStateELY_PEM(0) <= 1
 vStateELY_PEM(1) <= 1
 vStateELY_PEM(2) <= 1
 vStateELY_PEM(3) <= 1
 vStateELY_PEM(4) <= 1
 vStateELY_PEM(5) <= 1
 vStateELY_PEM(6) <= 1
 vStateELY_PEM(7) <= 1
 vStateELY_PEM(8) <= 1
 vStateELY_PEM(9) <= 1
 vPowELY_PEM(0) <= 30
 vPowELY_PEM(1) <= 30
 vPowELY_PEM(2) <= 30
 vPowELY_PEM(3) <= 30
 vPowELY_PEM(4) <= 30
 vPowELY_PEM(5) <= 30
 vPowELY_PEM(6) <= 30
 vPowELY_PEM(7) <= 30
 vPowELY_PEM(8) <= 30
 vPowELY_PEM(9) <= 30
 vFlowELY_PEM(0) <= 600.270108043217
 vFlowELY_PEM(1) <= 600.270108043217
 vFlowELY_PEM(2) <= 600.270108043217
 vFlowELY_PEM(3) <= 600.270108043217
 vFlowELY_PEM(4) <= 600.270108043217
 vFlowELY_PEM(5) <= 600.270108043217
 vFlowELY_PEM(6) <= 600.270108043217
 vFlowELY_PEM(7) <= 600.270108043217
 vFlowELY_PEM(8) <= 600.270108043217
 vFlowELY_PEM(9) <= 600.270108043217
 vUsablePowELY_PEM <= 30
 vPminELY_PEM(0) free
 vPminELY_PEM(1) free
 vPminELY_PEM(2) free
 vPminELY_PEM(3) free
 vPminELY_PEM(4) free
 vPminELY_PEM(5) free
 vPminELY_PEM(6) free
 vPminELY_PEM(7) free
 vPminELY_PEM(8) free
 vPminELY_PEM(9) free
 vInstalledElec_Grid <= 1
 vMaxFlowElec_Grid <= 400
 vStateElec_Grid(0) <= 1
 vStateElec_Grid(1) <= 1
 vStateElec_Grid(2) <= 1
 vStateElec_Grid(3) <= 1
 vStateElec_Grid(4) <= 1
 vStateElec_Grid(5) <= 1
 vStateElec_Grid(6) <= 1
 vStateElec_Grid(7) <= 1
 vStateElec_Grid(8) <= 1
 vStateElec_Grid(9) <= 1
 vGFxElec_Grid(0) <= 400
 vGFxElec_Grid(1) <= 400
 vGFxElec_Grid(2) <= 400
 vGFxElec_Grid(3) <= 400
 vGFxElec_Grid(4) <= 400
 vGFxElec_Grid(5) <= 400
 vGFxElec_Grid(6) <= 400
 vGFxElec_Grid(7) <= 400
 vGFxElec_Grid(8) <= 400
 vGFxElec_Grid(9) <= 400
 vInstalledElec_Grid_Inject <= 1
 vMaxFlowElec_Grid_Inject <= 1500000
 vStateElec_Grid_Inject(0) <= 1
 vStateElec_Grid_Inject(1) <= 1
 vStateElec_Grid_Inject(2) <= 1
 vStateElec_Grid_Inject(3) <= 1
 vStateElec_Grid_Inject(4) <= 1
 vStateElec_Grid_Inject(5) <= 1
 vStateElec_Grid_Inject(6) <= 1
 vStateElec_Grid_Inject(7) <= 1
 vStateElec_Grid_Inject(8) <= 1
 vStateElec_Grid_Inject(9) <= 1
 vGFxElec_Grid_Inject(0) <= 1500000
 vGFxElec_Grid_Inject(1) <= 1500000
 vGFxElec_Grid_Inject(2) <= 1500000
 vGFxElec_Grid_Inject(3) <= 1500000
 vGFxElec_Grid_Inject(4) <= 1500000
 vGFxElec_Grid_Inject(5) <= 1500000
 vGFxElec_Grid_Inject(6) <= 1500000
 vGFxElec_Grid_Inject(7) <= 1500000
 vGFxElec_Grid_Inject(8) <= 1500000
 vGFxElec_Grid_Inject(9) <= 1500000
 vInstalledH2_Load <= 1
 vWeightH2_Load <= 1
 vInstalledH2_Tank <= 1
 vMaxEstoH2_Tank <= 20000000000
 vEH2_Tank(0) <= 20000000000
 vEH2_Tank(1) <= 20000000000
 vEH2_Tank(2) <= 20000000000
 vEH2_Tank(3) <= 20000000000
 vEH2_Tank(4) <= 20000000000
 vEH2_Tank(5) <= 20000000000
 vEH2_Tank(6) <= 20000000000
 vEH2_Tank(7) <= 20000000000
 vEH2_Tank(8) <= 20000000000
 vEH2_Tank(9) <= 20000000000
 vQCH2_Tank(0) <= 1100
 vQCH2_Tank(1) <= 1100
 vQCH2_Tank(2) <= 1100
 vQCH2_Tank(3) <= 1100
 vQCH2_Tank(4) <= 1100
 vQCH2_Tank(5) <= 1100
 vQCH2_Tank(6) <= 1100
 vQCH2_Tank(7) <= 1100
 vQCH2_Tank(8) <= 1100
 vQCH2_Tank(9) <= 1100
 vQDH2_Tank(0) <= 1200
 vQDH2_Tank(1) <= 1200
 vQDH2_Tank(2) <= 1200
 vQDH2_Tank(3) <= 1200
 vQDH2_Tank(4) <= 1200
 vQDH2_Tank(5) <= 1200
 vQDH2_Tank(6) <= 1200
 vQDH2_Tank(7) <= 1200
 vQDH2_Tank(8) <= 1200
 vQDH2_Tank(9) <= 1200
 vStoIniH2_Tank <= 20000000000
 vsensH2_Tank(0) <= 1
 vsensH2_Tank(1) <= 1
 vsensH2_Tank(2) <= 1
 vsensH2_Tank(3) <= 1
 vsensH2_Tank(4) <= 1
 vsensH2_Tank(5) <= 1
 vsensH2_Tank(6) <= 1
 vsensH2_Tank(7) <= 1
 vsensH2_Tank(8) <= 1
 vsensH2_Tank(9) <= 1
 vInstalledWind_farm <= 1
 vWeightWind_farm <= 1
bin
 vInstalledELY_PEM
 vStateELY_PEM(0)
 vStateELY_PEM(1)
 vStateELY_PEM(2)
 vStateELY_PEM(3)
 vStateELY_PEM(4)
 vStateELY_PEM(5)
 vStateELY_PEM(6)
 vStateELY_PEM(7)
 vStateELY_PEM(8)
 vStateELY_PEM(9)
 vInstalledElec_Grid
 vStateElec_Grid(0)
 vStateElec_Grid(1)
 vStateElec_Grid(2)
 vStateElec_Grid(3)
 vStateElec_Grid(4)
 vStateElec_Grid(5)
 vStateElec_Grid(6)
 vStateElec_Grid(7)
 vStateElec_Grid(8)
 vStateElec_Grid(9)
 vInstalledElec_Grid_Inject
 vStateElec_Grid_Inject(0)
 vStateElec_Grid_Inject(1)
 vStateElec_Grid_Inject(2)
 vStateElec_Grid_Inject(3)
 vStateElec_Grid_Inject(4)
 vStateElec_Grid_Inject(5)
 vStateElec_Grid_Inject(6)
 vStateElec_Grid_Inject(7)
 vStateElec_Grid_Inject(8)
 vStateElec_Grid_Inject(9)
 vInstalledH2_Load
 vInstalledH2_Tank
 vsensH2_Tank(0)
 vsensH2_Tank(1)
 vsensH2_Tank(2)
 vsensH2_Tank(3)
 vsensH2_Tank(4)
 vsensH2_Tank(5)
 vsensH2_Tank(6)
 vsensH2_Tank(7)
 vsensH2_Tank(8)
 vsensH2_Tank(9)
 vInstalledWind_farm
gen
semi
end
