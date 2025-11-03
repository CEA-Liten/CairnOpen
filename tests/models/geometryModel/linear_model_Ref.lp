\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 1133.55952427023 vMaxPowerElectrolyzer#1 + 1000 Discretized_SizeMax(1)
       + 1400 Discretized_SizeMax(2) + 14000 Discretized_SizeMax(3)
       + 600168.077786779 vGFxGridFree#1(0)
       + 600168.077786779 vGFxGridFree#1(1)
       + 600168.077786779 vGFxGridFree#1(2)
       + 600168.077786779 vGFxGridFree#1(3)
       + 600168.077786779 vGFxGridFree#1(4)
       + 600168.077786779 vGFxGridFree#1(5)
       + 717937.657577501 vGFxGridFree#1(6)
       + 717937.657577501 vGFxGridFree#1(7)
       + 717937.657577501 vGFxGridFree#1(8)
       + 717937.657577501 vGFxGridFree#1(9)
       + 11133.5595242702 vMaxEstoStorageGen#1 + 21335.5952427023 vWeightWind
Subject To
 csBigMInstalledElectrolyzer#10: - 1000 vInstalledElectrolyzer#1
                                 + vMaxPowerElectrolyzer#1 <= 0
 csMinSizeInstalledElectrolyzer#10: vMaxPowerElectrolyzer#1 >= 0
 cNotRunningIfNotInstalledElectrolyzer#10: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(0) <= 0
 cNotRunningIfNotInstalledElectrolyzer#11: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(1) <= 0
 cNotRunningIfNotInstalledElectrolyzer#12: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(2) <= 0
 cNotRunningIfNotInstalledElectrolyzer#13: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(3) <= 0
 cNotRunningIfNotInstalledElectrolyzer#14: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(4) <= 0
 cNotRunningIfNotInstalledElectrolyzer#15: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(5) <= 0
 cNotRunningIfNotInstalledElectrolyzer#16: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(6) <= 0
 cNotRunningIfNotInstalledElectrolyzer#17: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(7) <= 0
 cNotRunningIfNotInstalledElectrolyzer#18: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(8) <= 0
 cNotRunningIfNotInstalledElectrolyzer#19: - vInstalledElectrolyzer#1
                                 + vStateElectrolyzer#1(9) <= 0
 cPowFlowElectrolyzer#10:        vPowElectrolyzer#1(0)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(0)
                                  = 0
 cPowFlowElectrolyzer#11:        vPowElectrolyzer#1(1)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(1)
                                  = 0
 cPowFlowElectrolyzer#12:        vPowElectrolyzer#1(2)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(2)
                                  = 0
 cPowFlowElectrolyzer#13:        vPowElectrolyzer#1(3)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(3)
                                  = 0
 cPowFlowElectrolyzer#14:        vPowElectrolyzer#1(4)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(4)
                                  = 0
 cPowFlowElectrolyzer#15:        vPowElectrolyzer#1(5)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(5)
                                  = 0
 cPowFlowElectrolyzer#16:        vPowElectrolyzer#1(6)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(6)
                                  = 0
 cPowFlowElectrolyzer#17:        vPowElectrolyzer#1(7)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(7)
                                  = 0
 cPowFlowElectrolyzer#18:        vPowElectrolyzer#1(8)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(8)
                                  = 0
 cPowFlowElectrolyzer#19:        vPowElectrolyzer#1(9)
                                 - 0.0555333333333333 vFlowElectrolyzer#1(9)
                                  = 0
 cUsePowElectrolyzer#10:         - vMaxPowerElectrolyzer#1
                                 + vUsablePowElectrolyzer#1  = 0
 cMaxTotalPowElectrolyzer#10:    vPowElectrolyzer#1(0)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#11:    vPowElectrolyzer#1(1)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#12:    vPowElectrolyzer#1(2)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#13:    vPowElectrolyzer#1(3)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#14:    vPowElectrolyzer#1(4)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#15:    vPowElectrolyzer#1(5)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#16:    vPowElectrolyzer#1(6)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#17:    vPowElectrolyzer#1(7)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#18:    vPowElectrolyzer#1(8)
                                 - vUsablePowElectrolyzer#1 <= 0
 cMaxTotalPowElectrolyzer#19:    vPowElectrolyzer#1(9)
                                 - vUsablePowElectrolyzer#1 <= 0
 cPowMaxElectrolyzer#10:         - 1000 vStateElectrolyzer#1(0)
                                 + vPowElectrolyzer#1(0) <= 0
 cDefPminElectrolyzer#10:        - vPminElectrolyzer#1(0)  = 0
 cPminElectrolyzer#10:           vPowElectrolyzer#1(0) - vPminElectrolyzer#1(0)
                                 >= 0
 cPowMaxElectrolyzer#11:         - 1000 vStateElectrolyzer#1(1)
                                 + vPowElectrolyzer#1(1) <= 0
 cDefPminElectrolyzer#11:        - vPminElectrolyzer#1(1)  = 0
 cPminElectrolyzer#11:           vPowElectrolyzer#1(1) - vPminElectrolyzer#1(1)
                                 >= 0
 cPowMaxElectrolyzer#12:         - 1000 vStateElectrolyzer#1(2)
                                 + vPowElectrolyzer#1(2) <= 0
 cDefPminElectrolyzer#12:        - vPminElectrolyzer#1(2)  = 0
 cPminElectrolyzer#12:           vPowElectrolyzer#1(2) - vPminElectrolyzer#1(2)
                                 >= 0
 cPowMaxElectrolyzer#13:         - 1000 vStateElectrolyzer#1(3)
                                 + vPowElectrolyzer#1(3) <= 0
 cDefPminElectrolyzer#13:        - vPminElectrolyzer#1(3)  = 0
 cPminElectrolyzer#13:           vPowElectrolyzer#1(3) - vPminElectrolyzer#1(3)
                                 >= 0
 cPowMaxElectrolyzer#14:         - 1000 vStateElectrolyzer#1(4)
                                 + vPowElectrolyzer#1(4) <= 0
 cDefPminElectrolyzer#14:        - vPminElectrolyzer#1(4)  = 0
 cPminElectrolyzer#14:           vPowElectrolyzer#1(4) - vPminElectrolyzer#1(4)
                                 >= 0
 cPowMaxElectrolyzer#15:         - 1000 vStateElectrolyzer#1(5)
                                 + vPowElectrolyzer#1(5) <= 0
 cDefPminElectrolyzer#15:        - vPminElectrolyzer#1(5)  = 0
 cPminElectrolyzer#15:           vPowElectrolyzer#1(5) - vPminElectrolyzer#1(5)
                                 >= 0
 cPowMaxElectrolyzer#16:         - 1000 vStateElectrolyzer#1(6)
                                 + vPowElectrolyzer#1(6) <= 0
 cDefPminElectrolyzer#16:        - vPminElectrolyzer#1(6)  = 0
 cPminElectrolyzer#16:           vPowElectrolyzer#1(6) - vPminElectrolyzer#1(6)
                                 >= 0
 cPowMaxElectrolyzer#17:         - 1000 vStateElectrolyzer#1(7)
                                 + vPowElectrolyzer#1(7) <= 0
 cDefPminElectrolyzer#17:        - vPminElectrolyzer#1(7)  = 0
 cPminElectrolyzer#17:           vPowElectrolyzer#1(7) - vPminElectrolyzer#1(7)
                                 >= 0
 cPowMaxElectrolyzer#18:         - 1000 vStateElectrolyzer#1(8)
                                 + vPowElectrolyzer#1(8) <= 0
 cDefPminElectrolyzer#18:        - vPminElectrolyzer#1(8)  = 0
 cPminElectrolyzer#18:           vPowElectrolyzer#1(8) - vPminElectrolyzer#1(8)
                                 >= 0
 cPowMaxElectrolyzer#19:         - 1000 vStateElectrolyzer#1(9)
                                 + vPowElectrolyzer#1(9) <= 0
 cDefPminElectrolyzer#19:        - vPminElectrolyzer#1(9)  = 0
 cPminElectrolyzer#19:           vPowElectrolyzer#1(9) - vPminElectrolyzer#1(9)
                                 >= 0
 cConverterUseElectrolyzer#10:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(0) <= 0
 cConverterUseElectrolyzer#11:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(1) <= 0
 cConverterUseElectrolyzer#12:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(2) <= 0
 cConverterUseElectrolyzer#13:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(3) <= 0
 cConverterUseElectrolyzer#14:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(4) <= 0
 cConverterUseElectrolyzer#15:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(5) <= 0
 cConverterUseElectrolyzer#16:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(6) <= 0
 cConverterUseElectrolyzer#17:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(7) <= 0
 cConverterUseElectrolyzer#18:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(8) <= 0
 cConverterUseElectrolyzer#19:   - vMaxPowerElectrolyzer#1
                                 + vPowElectrolyzer#1(9) <= 0
 C_SizeMax:                      vMaxPowerElectrolyzer#1
                                 - 0.0001 Discretized_SizeMax(1)
                                 - Discretized_SizeMax(2)
                                 - 10 Discretized_SizeMax(3)  = 0
 c75:                            Discretized_SizeMax(0)
                                 + Discretized_SizeMax(1)
                                 + Discretized_SizeMax(2)
                                 + Discretized_SizeMax(3)  = 1
 csInstalledGridFree#10:         vInstalledGridFree#1  = 1
 csMaxValGridFree#10:            vMaxFlowGridFree#1  = 100000000
 csBigMInstalledGridFree#10:     - 100000000 vInstalledGridFree#1
                                 + vMaxFlowGridFree#1 <= 0
 csMinSizeInstalledGridFree#10:  vMaxFlowGridFree#1 >= 0
 cNotRunningIfNotInstalledGridFree#10: - vInstalledGridFree#1
                                 + vStateGridFree#1(0) <= 0
 cNotRunningIfNotInstalledGridFree#11: - vInstalledGridFree#1
                                 + vStateGridFree#1(1) <= 0
 cNotRunningIfNotInstalledGridFree#12: - vInstalledGridFree#1
                                 + vStateGridFree#1(2) <= 0
 cNotRunningIfNotInstalledGridFree#13: - vInstalledGridFree#1
                                 + vStateGridFree#1(3) <= 0
 cNotRunningIfNotInstalledGridFree#14: - vInstalledGridFree#1
                                 + vStateGridFree#1(4) <= 0
 cNotRunningIfNotInstalledGridFree#15: - vInstalledGridFree#1
                                 + vStateGridFree#1(5) <= 0
 cNotRunningIfNotInstalledGridFree#16: - vInstalledGridFree#1
                                 + vStateGridFree#1(6) <= 0
 cNotRunningIfNotInstalledGridFree#17: - vInstalledGridFree#1
                                 + vStateGridFree#1(7) <= 0
 cNotRunningIfNotInstalledGridFree#18: - vInstalledGridFree#1
                                 + vStateGridFree#1(8) <= 0
 cNotRunningIfNotInstalledGridFree#19: - vInstalledGridFree#1
                                 + vStateGridFree#1(9) <= 0
 cStateGridGridFree#10:          - 100000000 vStateGridFree#1(0)
                                 + vGFxGridFree#1(0) <= 0
 cStateGridGridFree#11:          - 100000000 vStateGridFree#1(1)
                                 + vGFxGridFree#1(1) <= 0
 cStateGridGridFree#12:          - 100000000 vStateGridFree#1(2)
                                 + vGFxGridFree#1(2) <= 0
 cStateGridGridFree#13:          - 100000000 vStateGridFree#1(3)
                                 + vGFxGridFree#1(3) <= 0
 cStateGridGridFree#14:          - 100000000 vStateGridFree#1(4)
                                 + vGFxGridFree#1(4) <= 0
 cStateGridGridFree#15:          - 100000000 vStateGridFree#1(5)
                                 + vGFxGridFree#1(5) <= 0
 cStateGridGridFree#16:          - 100000000 vStateGridFree#1(6)
                                 + vGFxGridFree#1(6) <= 0
 cStateGridGridFree#17:          - 100000000 vStateGridFree#1(7)
                                 + vGFxGridFree#1(7) <= 0
 cStateGridGridFree#18:          - 100000000 vStateGridFree#1(8)
                                 + vGFxGridFree#1(8) <= 0
 cStateGridGridFree#19:          - 100000000 vStateGridFree#1(9)
                                 + vGFxGridFree#1(9) <= 0
 cMaxGFxGridFree#10:             - vMaxFlowGridFree#1 + vGFxGridFree#1(0) <= 0
 cMaxGFxGridFree#11:             - vMaxFlowGridFree#1 + vGFxGridFree#1(1) <= 0
 cMaxGFxGridFree#12:             - vMaxFlowGridFree#1 + vGFxGridFree#1(2) <= 0
 cMaxGFxGridFree#13:             - vMaxFlowGridFree#1 + vGFxGridFree#1(3) <= 0
 cMaxGFxGridFree#14:             - vMaxFlowGridFree#1 + vGFxGridFree#1(4) <= 0
 cMaxGFxGridFree#15:             - vMaxFlowGridFree#1 + vGFxGridFree#1(5) <= 0
 cMaxGFxGridFree#16:             - vMaxFlowGridFree#1 + vGFxGridFree#1(6) <= 0
 cMaxGFxGridFree#17:             - vMaxFlowGridFree#1 + vGFxGridFree#1(7) <= 0
 cMaxGFxGridFree#18:             - vMaxFlowGridFree#1 + vGFxGridFree#1(8) <= 0
 cMaxGFxGridFree#19:             - vMaxFlowGridFree#1 + vGFxGridFree#1(9) <= 0
 csInstalledGridFree#20:         vInstalledGridFree#2  = 1
 csMaxValGridFree#20:            vMaxFlowGridFree#2  = 10000
 csBigMInstalledGridFree#20:     - 10000 vInstalledGridFree#2
                                 + vMaxFlowGridFree#2 <= 0
 csMinSizeInstalledGridFree#20:  vMaxFlowGridFree#2 >= 0
 cNotRunningIfNotInstalledGridFree#20: - vInstalledGridFree#2
                                 + vStateGridFree#2(0) <= 0
 cNotRunningIfNotInstalledGridFree#21: - vInstalledGridFree#2
                                 + vStateGridFree#2(1) <= 0
 cNotRunningIfNotInstalledGridFree#22: - vInstalledGridFree#2
                                 + vStateGridFree#2(2) <= 0
 cNotRunningIfNotInstalledGridFree#23: - vInstalledGridFree#2
                                 + vStateGridFree#2(3) <= 0
 cNotRunningIfNotInstalledGridFree#24: - vInstalledGridFree#2
                                 + vStateGridFree#2(4) <= 0
 cNotRunningIfNotInstalledGridFree#25: - vInstalledGridFree#2
                                 + vStateGridFree#2(5) <= 0
 cNotRunningIfNotInstalledGridFree#26: - vInstalledGridFree#2
                                 + vStateGridFree#2(6) <= 0
 cNotRunningIfNotInstalledGridFree#27: - vInstalledGridFree#2
                                 + vStateGridFree#2(7) <= 0
 cNotRunningIfNotInstalledGridFree#28: - vInstalledGridFree#2
                                 + vStateGridFree#2(8) <= 0
 cNotRunningIfNotInstalledGridFree#29: - vInstalledGridFree#2
                                 + vStateGridFree#2(9) <= 0
 cStateGridGridFree#20:          - 10000 vStateGridFree#2(0)
                                 + vGFxGridFree#2(0) <= 0
 cStateGridGridFree#21:          - 10000 vStateGridFree#2(1)
                                 + vGFxGridFree#2(1) <= 0
 cStateGridGridFree#22:          - 10000 vStateGridFree#2(2)
                                 + vGFxGridFree#2(2) <= 0
 cStateGridGridFree#23:          - 10000 vStateGridFree#2(3)
                                 + vGFxGridFree#2(3) <= 0
 cStateGridGridFree#24:          - 10000 vStateGridFree#2(4)
                                 + vGFxGridFree#2(4) <= 0
 cStateGridGridFree#25:          - 10000 vStateGridFree#2(5)
                                 + vGFxGridFree#2(5) <= 0
 cStateGridGridFree#26:          - 10000 vStateGridFree#2(6)
                                 + vGFxGridFree#2(6) <= 0
 cStateGridGridFree#27:          - 10000 vStateGridFree#2(7)
                                 + vGFxGridFree#2(7) <= 0
 cStateGridGridFree#28:          - 10000 vStateGridFree#2(8)
                                 + vGFxGridFree#2(8) <= 0
 cStateGridGridFree#29:          - 10000 vStateGridFree#2(9)
                                 + vGFxGridFree#2(9) <= 0
 cMaxGFxGridFree#20:             - vMaxFlowGridFree#2 + vGFxGridFree#2(0) <= 0
 cMaxGFxGridFree#21:             - vMaxFlowGridFree#2 + vGFxGridFree#2(1) <= 0
 cMaxGFxGridFree#22:             - vMaxFlowGridFree#2 + vGFxGridFree#2(2) <= 0
 cMaxGFxGridFree#23:             - vMaxFlowGridFree#2 + vGFxGridFree#2(3) <= 0
 cMaxGFxGridFree#24:             - vMaxFlowGridFree#2 + vGFxGridFree#2(4) <= 0
 cMaxGFxGridFree#25:             - vMaxFlowGridFree#2 + vGFxGridFree#2(5) <= 0
 cMaxGFxGridFree#26:             - vMaxFlowGridFree#2 + vGFxGridFree#2(6) <= 0
 cMaxGFxGridFree#27:             - vMaxFlowGridFree#2 + vGFxGridFree#2(7) <= 0
 cMaxGFxGridFree#28:             - vMaxFlowGridFree#2 + vGFxGridFree#2(8) <= 0
 cMaxGFxGridFree#29:             - vMaxFlowGridFree#2 + vGFxGridFree#2(9) <= 0
 csInstalledSourceLoad#10:       vInstalledSourceLoad#1  = 1
 csWeightSourceLoad#10:          vWeightSourceLoad#1  = 1
 csBigMInstalledSourceLoad#10:   - vInstalledSourceLoad#1 + vWeightSourceLoad#1
                                 <= 0
 csMinSizeInstalledSourceLoad#10: vWeightSourceLoad#1 >= 0
 cMSFxSourceLoad#10:             <= 100000000
 cMSFxSourceLoad#11:             <= 100000000
 cMSFxSourceLoad#12:             <= 100000000
 cMSFxSourceLoad#13:             <= 100000000
 cMSFxSourceLoad#14:             <= 100000000
 cMSFxSourceLoad#15:             <= 100000000
 cMSFxSourceLoad#16:             <= 100000000
 cMSFxSourceLoad#17:             <= 100000000
 cMSFxSourceLoad#18:             <= 100000000
 cMSFxSourceLoad#19:             <= 100000000
 csBigMInstalledStorageGen#10:   - 100 vInstalledStorageGen#1
                                 + vMaxEstoStorageGen#1 <= 0
 csMinSizeInstalledStorageGen#10: vMaxEstoStorageGen#1 >= 0
 cStoIni3StorageGen#10:          - 0.5 vMaxEstoStorageGen#1
                                 + vStoIniStorageGen#1  = 0
 cMStorageGen#10:                - vMaxEstoStorageGen#1 + vEStorageGen#1(0)
                                 <= 0
 cStoStorageGen#10:              vEStorageGen#1(0) - vQCStorageGen#1(0)
                                 + vQDStorageGen#1(0) - vStoIniStorageGen#1
                                  = 0
 cMStorageGen#11:                - vMaxEstoStorageGen#1 + vEStorageGen#1(1)
                                 <= 0
 cStoStorageGen#11:              - vEStorageGen#1(0) + vEStorageGen#1(1)
                                 - vQCStorageGen#1(1) + vQDStorageGen#1(1)  = 0
 cMStorageGen#12:                - vMaxEstoStorageGen#1 + vEStorageGen#1(2)
                                 <= 0
 cStoStorageGen#12:              - vEStorageGen#1(1) + vEStorageGen#1(2)
                                 - vQCStorageGen#1(2) + vQDStorageGen#1(2)  = 0
 cMStorageGen#13:                - vMaxEstoStorageGen#1 + vEStorageGen#1(3)
                                 <= 0
 cStoStorageGen#13:              - vEStorageGen#1(2) + vEStorageGen#1(3)
                                 - vQCStorageGen#1(3) + vQDStorageGen#1(3)  = 0
 cMStorageGen#14:                - vMaxEstoStorageGen#1 + vEStorageGen#1(4)
                                 <= 0
 cStoStorageGen#14:              - vEStorageGen#1(3) + vEStorageGen#1(4)
                                 - vQCStorageGen#1(4) + vQDStorageGen#1(4)  = 0
 cMStorageGen#15:                - vMaxEstoStorageGen#1 + vEStorageGen#1(5)
                                 <= 0
 cStoStorageGen#15:              - vEStorageGen#1(4) + vEStorageGen#1(5)
                                 - vQCStorageGen#1(5) + vQDStorageGen#1(5)  = 0
 cMStorageGen#16:                - vMaxEstoStorageGen#1 + vEStorageGen#1(6)
                                 <= 0
 cStoStorageGen#16:              - vEStorageGen#1(5) + vEStorageGen#1(6)
                                 - vQCStorageGen#1(6) + vQDStorageGen#1(6)  = 0
 cMStorageGen#17:                - vMaxEstoStorageGen#1 + vEStorageGen#1(7)
                                 <= 0
 cStoStorageGen#17:              - vEStorageGen#1(6) + vEStorageGen#1(7)
                                 - vQCStorageGen#1(7) + vQDStorageGen#1(7)  = 0
 cMStorageGen#18:                - vMaxEstoStorageGen#1 + vEStorageGen#1(8)
                                 <= 0
 cStoStorageGen#18:              - vEStorageGen#1(7) + vEStorageGen#1(8)
                                 - vQCStorageGen#1(8) + vQDStorageGen#1(8)  = 0
 cMStorageGen#19:                - vMaxEstoStorageGen#1 + vEStorageGen#1(9)
                                 <= 0
 cStoStorageGen#19:              - vEStorageGen#1(8) + vEStorageGen#1(9)
                                 - vQCStorageGen#1(9) + vQDStorageGen#1(9)  = 0
 cMinStoFinStorageGen#19:        vEStorageGen#1(9) - vStoIniStorageGen#1 >= 0
 cMFCStorageGen#10:              vQCStorageGen#1(0)
                                 - 100000000 vsensStorageGen#1(0) <= 0
 cMFDStorageGen#10:              vQDStorageGen#1(0)
                                 + 100000000 vsensStorageGen#1(0) <= 100000000
 cMFCStorageGen#11:              vQCStorageGen#1(1)
                                 - 100000000 vsensStorageGen#1(1) <= 0
 cMFDStorageGen#11:              vQDStorageGen#1(1)
                                 + 100000000 vsensStorageGen#1(1) <= 100000000
 cMFCStorageGen#12:              vQCStorageGen#1(2)
                                 - 100000000 vsensStorageGen#1(2) <= 0
 cMFDStorageGen#12:              vQDStorageGen#1(2)
                                 + 100000000 vsensStorageGen#1(2) <= 100000000
 cMFCStorageGen#13:              vQCStorageGen#1(3)
                                 - 100000000 vsensStorageGen#1(3) <= 0
 cMFDStorageGen#13:              vQDStorageGen#1(3)
                                 + 100000000 vsensStorageGen#1(3) <= 100000000
 cMFCStorageGen#14:              vQCStorageGen#1(4)
                                 - 100000000 vsensStorageGen#1(4) <= 0
 cMFDStorageGen#14:              vQDStorageGen#1(4)
                                 + 100000000 vsensStorageGen#1(4) <= 100000000
 cMFCStorageGen#15:              vQCStorageGen#1(5)
                                 - 100000000 vsensStorageGen#1(5) <= 0
 cMFDStorageGen#15:              vQDStorageGen#1(5)
                                 + 100000000 vsensStorageGen#1(5) <= 100000000
 cMFCStorageGen#16:              vQCStorageGen#1(6)
                                 - 100000000 vsensStorageGen#1(6) <= 0
 cMFDStorageGen#16:              vQDStorageGen#1(6)
                                 + 100000000 vsensStorageGen#1(6) <= 100000000
 cMFCStorageGen#17:              vQCStorageGen#1(7)
                                 - 100000000 vsensStorageGen#1(7) <= 0
 cMFDStorageGen#17:              vQDStorageGen#1(7)
                                 + 100000000 vsensStorageGen#1(7) <= 100000000
 cMFCStorageGen#18:              vQCStorageGen#1(8)
                                 - 100000000 vsensStorageGen#1(8) <= 0
 cMFDStorageGen#18:              vQDStorageGen#1(8)
                                 + 100000000 vsensStorageGen#1(8) <= 100000000
 cMFCStorageGen#19:              vQCStorageGen#1(9)
                                 - 100000000 vsensStorageGen#1(9) <= 0
 cMFDStorageGen#19:              vQDStorageGen#1(9)
                                 + 100000000 vsensStorageGen#1(9) <= 100000000
 csBigMInstalledWind0:           - 10 vInstalledWind + vWeightWind <= 0
 csMinSizeInstalledWind0:        vWeightWind >= 0
 cMSFxWind0:                     <= 10000
 cMSFxWind1:                     <= 10000
 cMSFxWind2:                     <= 10000
 cMSFxWind3:                     <= 10000
 cMSFxWind4:                     <= 10000
 cMSFxWind5:                     <= 10000
 cMSFxWind6:                     <= 10000
 cMSFxWind7:                     <= 10000
 cMSFxWind8:                     <= 10000
 cMSFxWind9:                     <= 10000
 cSNodeLaw#10:                   - vPowElectrolyzer#1(0) + vGFxGridFree#1(0)
                                 - vGFxGridFree#2(0) + 1.845819 vWeightWind
                                  = 0
 cSNodeLaw#11:                   - vPowElectrolyzer#1(1) + vGFxGridFree#1(1)
                                 - vGFxGridFree#2(1) + 2.191294 vWeightWind
                                  = 0
 cSNodeLaw#12:                   - vPowElectrolyzer#1(2) + vGFxGridFree#1(2)
                                 - vGFxGridFree#2(2) + 2.326187 vWeightWind
                                  = 0
 cSNodeLaw#13:                   - vPowElectrolyzer#1(3) + vGFxGridFree#1(3)
                                 - vGFxGridFree#2(3) + 2.483921 vWeightWind
                                  = 0
 cSNodeLaw#14:                   - vPowElectrolyzer#1(4) + vGFxGridFree#1(4)
                                 - vGFxGridFree#2(4) + 2.67351 vWeightWind  = 0
 cSNodeLaw#15:                   - vPowElectrolyzer#1(5) + vGFxGridFree#1(5)
                                 - vGFxGridFree#2(5) + 2.85778 vWeightWind  = 0
 cSNodeLaw#16:                   - vPowElectrolyzer#1(6) + vGFxGridFree#1(6)
                                 - vGFxGridFree#2(6) + 2.98878 vWeightWind  = 0
 cSNodeLaw#17:                   - vPowElectrolyzer#1(7) + vGFxGridFree#1(7)
                                 - vGFxGridFree#2(7) + 3.033381 vWeightWind
                                  = 0
 cSNodeLaw#18:                   - vPowElectrolyzer#1(8) + vGFxGridFree#1(8)
                                 - vGFxGridFree#2(8) + 2.807039 vWeightWind
                                  = 0
 cSNodeLaw#19:                   - vPowElectrolyzer#1(9) + vGFxGridFree#1(9)
                                 - vGFxGridFree#2(9) + 2.311192 vWeightWind
                                  = 0
 cSNodeLaw#20:                   vFlowElectrolyzer#1(0) - vQCStorageGen#1(0)
                                 + vQDStorageGen#1(0)  = 0
 cSNodeLaw#21:                   vFlowElectrolyzer#1(1) - vQCStorageGen#1(1)
                                 + vQDStorageGen#1(1)  = 0
 cSNodeLaw#22:                   vFlowElectrolyzer#1(2) - vQCStorageGen#1(2)
                                 + vQDStorageGen#1(2)  = 0
 cSNodeLaw#23:                   vFlowElectrolyzer#1(3) - vQCStorageGen#1(3)
                                 + vQDStorageGen#1(3)  = 0
 cSNodeLaw#24:                   vFlowElectrolyzer#1(4) - vQCStorageGen#1(4)
                                 + vQDStorageGen#1(4)  = 0
 cSNodeLaw#25:                   vFlowElectrolyzer#1(5) - vQCStorageGen#1(5)
                                 + vQDStorageGen#1(5)  = 0
 cSNodeLaw#26:                   vFlowElectrolyzer#1(6) - vQCStorageGen#1(6)
                                 + vQDStorageGen#1(6)  = 0
 cSNodeLaw#27:                   vFlowElectrolyzer#1(7) - vQCStorageGen#1(7)
                                 + vQDStorageGen#1(7)  = 0
 cSNodeLaw#28:                   vFlowElectrolyzer#1(8) - vQCStorageGen#1(8)
                                 + vQDStorageGen#1(8)  = 0
 cSNodeLaw#29:                   vFlowElectrolyzer#1(9) - vQCStorageGen#1(9)
                                 + vQDStorageGen#1(9)  = 0
Bounds
 0 <= vInstalledElectrolyzer#1 <= 1
 0 <= vMaxPowerElectrolyzer#1 <= 1000
 0 <= vStateElectrolyzer#1(0) <= 1
 0 <= vStateElectrolyzer#1(1) <= 1
 0 <= vStateElectrolyzer#1(2) <= 1
 0 <= vStateElectrolyzer#1(3) <= 1
 0 <= vStateElectrolyzer#1(4) <= 1
 0 <= vStateElectrolyzer#1(5) <= 1
 0 <= vStateElectrolyzer#1(6) <= 1
 0 <= vStateElectrolyzer#1(7) <= 1
 0 <= vStateElectrolyzer#1(8) <= 1
 0 <= vStateElectrolyzer#1(9) <= 1
 0 <= vPowElectrolyzer#1(0) <= 1000
 0 <= vPowElectrolyzer#1(1) <= 1000
 0 <= vPowElectrolyzer#1(2) <= 1000
 0 <= vPowElectrolyzer#1(3) <= 1000
 0 <= vPowElectrolyzer#1(4) <= 1000
 0 <= vPowElectrolyzer#1(5) <= 1000
 0 <= vPowElectrolyzer#1(6) <= 1000
 0 <= vPowElectrolyzer#1(7) <= 1000
 0 <= vPowElectrolyzer#1(8) <= 1000
 0 <= vPowElectrolyzer#1(9) <= 1000
 0 <= vFlowElectrolyzer#1(0) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(1) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(2) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(3) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(4) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(5) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(6) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(7) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(8) <= 18007.2028811525
 0 <= vFlowElectrolyzer#1(9) <= 18007.2028811525
 0 <= vUsablePowElectrolyzer#1 <= 1000
      vPminElectrolyzer#1(0) Free
      vPminElectrolyzer#1(1) Free
      vPminElectrolyzer#1(2) Free
      vPminElectrolyzer#1(3) Free
      vPminElectrolyzer#1(4) Free
      vPminElectrolyzer#1(5) Free
      vPminElectrolyzer#1(6) Free
      vPminElectrolyzer#1(7) Free
      vPminElectrolyzer#1(8) Free
      vPminElectrolyzer#1(9) Free
 0 <= Discretized_SizeMax(0) <= 1
 0 <= Discretized_SizeMax(1) <= 1
 0 <= Discretized_SizeMax(2) <= 1
 0 <= Discretized_SizeMax(3) <= 1
 0 <= vInstalledGridFree#1 <= 1
 0 <= vMaxFlowGridFree#1 <= 100000000
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
 0 <= vGFxGridFree#1(0) <= 100000000
 0 <= vGFxGridFree#1(1) <= 100000000
 0 <= vGFxGridFree#1(2) <= 100000000
 0 <= vGFxGridFree#1(3) <= 100000000
 0 <= vGFxGridFree#1(4) <= 100000000
 0 <= vGFxGridFree#1(5) <= 100000000
 0 <= vGFxGridFree#1(6) <= 100000000
 0 <= vGFxGridFree#1(7) <= 100000000
 0 <= vGFxGridFree#1(8) <= 100000000
 0 <= vGFxGridFree#1(9) <= 100000000
 0 <= vInstalledGridFree#2 <= 1
 0 <= vMaxFlowGridFree#2 <= 10000
 0 <= vStateGridFree#2(0) <= 1
 0 <= vStateGridFree#2(1) <= 1
 0 <= vStateGridFree#2(2) <= 1
 0 <= vStateGridFree#2(3) <= 1
 0 <= vStateGridFree#2(4) <= 1
 0 <= vStateGridFree#2(5) <= 1
 0 <= vStateGridFree#2(6) <= 1
 0 <= vStateGridFree#2(7) <= 1
 0 <= vStateGridFree#2(8) <= 1
 0 <= vStateGridFree#2(9) <= 1
 0 <= vGFxGridFree#2(0) <= 10000
 0 <= vGFxGridFree#2(1) <= 10000
 0 <= vGFxGridFree#2(2) <= 10000
 0 <= vGFxGridFree#2(3) <= 10000
 0 <= vGFxGridFree#2(4) <= 10000
 0 <= vGFxGridFree#2(5) <= 10000
 0 <= vGFxGridFree#2(6) <= 10000
 0 <= vGFxGridFree#2(7) <= 10000
 0 <= vGFxGridFree#2(8) <= 10000
 0 <= vGFxGridFree#2(9) <= 10000
 0 <= vInstalledSourceLoad#1 <= 1
 0 <= vWeightSourceLoad#1 <= 1
 0 <= vInstalledStorageGen#1 <= 1
 0 <= vMaxEstoStorageGen#1 <= 100
 0 <= vEStorageGen#1(0) <= 100
 0 <= vEStorageGen#1(1) <= 100
 0 <= vEStorageGen#1(2) <= 100
 0 <= vEStorageGen#1(3) <= 100
 0 <= vEStorageGen#1(4) <= 100
 0 <= vEStorageGen#1(5) <= 100
 0 <= vEStorageGen#1(6) <= 100
 0 <= vEStorageGen#1(7) <= 100
 0 <= vEStorageGen#1(8) <= 100
 0 <= vEStorageGen#1(9) <= 100
 0 <= vQCStorageGen#1(0) <= 100000000
 0 <= vQCStorageGen#1(1) <= 100000000
 0 <= vQCStorageGen#1(2) <= 100000000
 0 <= vQCStorageGen#1(3) <= 100000000
 0 <= vQCStorageGen#1(4) <= 100000000
 0 <= vQCStorageGen#1(5) <= 100000000
 0 <= vQCStorageGen#1(6) <= 100000000
 0 <= vQCStorageGen#1(7) <= 100000000
 0 <= vQCStorageGen#1(8) <= 100000000
 0 <= vQCStorageGen#1(9) <= 100000000
 0 <= vQDStorageGen#1(0) <= 100000000
 0 <= vQDStorageGen#1(1) <= 100000000
 0 <= vQDStorageGen#1(2) <= 100000000
 0 <= vQDStorageGen#1(3) <= 100000000
 0 <= vQDStorageGen#1(4) <= 100000000
 0 <= vQDStorageGen#1(5) <= 100000000
 0 <= vQDStorageGen#1(6) <= 100000000
 0 <= vQDStorageGen#1(7) <= 100000000
 0 <= vQDStorageGen#1(8) <= 100000000
 0 <= vQDStorageGen#1(9) <= 100000000
 0 <= vStoIniStorageGen#1 <= 100
 0 <= vsensStorageGen#1(0) <= 1
 0 <= vsensStorageGen#1(1) <= 1
 0 <= vsensStorageGen#1(2) <= 1
 0 <= vsensStorageGen#1(3) <= 1
 0 <= vsensStorageGen#1(4) <= 1
 0 <= vsensStorageGen#1(5) <= 1
 0 <= vsensStorageGen#1(6) <= 1
 0 <= vsensStorageGen#1(7) <= 1
 0 <= vsensStorageGen#1(8) <= 1
 0 <= vsensStorageGen#1(9) <= 1
 0 <= vInstalledWind <= 1
 0 <= vWeightWind <= 10
Generals
 vInstalledElectrolyzer#1  vStateElectrolyzer#1(0)  vStateElectrolyzer#1(1) 
 vStateElectrolyzer#1(2)  vStateElectrolyzer#1(3)  vStateElectrolyzer#1(4) 
 vStateElectrolyzer#1(5)  vStateElectrolyzer#1(6)  vStateElectrolyzer#1(7) 
 vStateElectrolyzer#1(8)  vStateElectrolyzer#1(9)  vInstalledGridFree#1 
 vStateGridFree#1(0)  vStateGridFree#1(1)  vStateGridFree#1(2) 
 vStateGridFree#1(3)  vStateGridFree#1(4)  vStateGridFree#1(5) 
 vStateGridFree#1(6)  vStateGridFree#1(7)  vStateGridFree#1(8) 
 vStateGridFree#1(9)  vInstalledGridFree#2  vStateGridFree#2(0) 
 vStateGridFree#2(1)  vStateGridFree#2(2)  vStateGridFree#2(3) 
 vStateGridFree#2(4)  vStateGridFree#2(5)  vStateGridFree#2(6) 
 vStateGridFree#2(7)  vStateGridFree#2(8)  vStateGridFree#2(9) 
 vInstalledSourceLoad#1  vInstalledStorageGen#1  vsensStorageGen#1(0) 
 vsensStorageGen#1(1)  vsensStorageGen#1(2)  vsensStorageGen#1(3) 
 vsensStorageGen#1(4)  vsensStorageGen#1(5)  vsensStorageGen#1(6) 
 vsensStorageGen#1(7)  vsensStorageGen#1(8)  vsensStorageGen#1(9) 
 vInstalledWind 
SOS
 s1: S2 ::  Discretized_SizeMax(0) : 0  Discretized_SizeMax(1) : 1
      Discretized_SizeMax(2) : 2  Discretized_SizeMax(3) : 3
End
