\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 99299.8143260721 vGFxElec_grid(0) + 99299.8143260721 vGFxElec_grid(1)
       + 99299.8143260721 vGFxElec_grid(2) + 99299.8143260721 vGFxElec_grid(3)
       + 99299.8143260721 vGFxElec_grid(4) + 99299.8143260721 vGFxElec_grid(5)
       + 99299.8143260721 vGFxElec_grid(6) + 99299.8143260721 vGFxElec_grid(7)
       + 99299.8143260721 vGFxElec_grid(8) + 99299.8143260721 vGFxElec_grid(9)
Subject To
 csInstalledElec_grid0:          vInstalledElec_grid  = 1
 csMaxValElec_grid0:             vMaxFlowElec_grid  = 10000
 csBigMInstalledElec_grid0:      - 10000 vInstalledElec_grid
                                 + vMaxFlowElec_grid <= 0
 csMinSizeInstalledElec_grid0:   vMaxFlowElec_grid >= 0
 cNotRunningIfNotInstalledElec_grid0: - vInstalledElec_grid
                                 + vStateElec_grid(0) <= 0
 cNotRunningIfNotInstalledElec_grid1: - vInstalledElec_grid
                                 + vStateElec_grid(1) <= 0
 cNotRunningIfNotInstalledElec_grid2: - vInstalledElec_grid
                                 + vStateElec_grid(2) <= 0
 cNotRunningIfNotInstalledElec_grid3: - vInstalledElec_grid
                                 + vStateElec_grid(3) <= 0
 cNotRunningIfNotInstalledElec_grid4: - vInstalledElec_grid
                                 + vStateElec_grid(4) <= 0
 cNotRunningIfNotInstalledElec_grid5: - vInstalledElec_grid
                                 + vStateElec_grid(5) <= 0
 cNotRunningIfNotInstalledElec_grid6: - vInstalledElec_grid
                                 + vStateElec_grid(6) <= 0
 cNotRunningIfNotInstalledElec_grid7: - vInstalledElec_grid
                                 + vStateElec_grid(7) <= 0
 cNotRunningIfNotInstalledElec_grid8: - vInstalledElec_grid
                                 + vStateElec_grid(8) <= 0
 cNotRunningIfNotInstalledElec_grid9: - vInstalledElec_grid
                                 + vStateElec_grid(9) <= 0
 cStateGridElec_grid0:           - 10000 vStateElec_grid(0) + vGFxElec_grid(0)
                                 <= 0
 cStateGridElec_grid1:           - 10000 vStateElec_grid(1) + vGFxElec_grid(1)
                                 <= 0
 cStateGridElec_grid2:           - 10000 vStateElec_grid(2) + vGFxElec_grid(2)
                                 <= 0
 cStateGridElec_grid3:           - 10000 vStateElec_grid(3) + vGFxElec_grid(3)
                                 <= 0
 cStateGridElec_grid4:           - 10000 vStateElec_grid(4) + vGFxElec_grid(4)
                                 <= 0
 cStateGridElec_grid5:           - 10000 vStateElec_grid(5) + vGFxElec_grid(5)
                                 <= 0
 cStateGridElec_grid6:           - 10000 vStateElec_grid(6) + vGFxElec_grid(6)
                                 <= 0
 cStateGridElec_grid7:           - 10000 vStateElec_grid(7) + vGFxElec_grid(7)
                                 <= 0
 cStateGridElec_grid8:           - 10000 vStateElec_grid(8) + vGFxElec_grid(8)
                                 <= 0
 cStateGridElec_grid9:           - 10000 vStateElec_grid(9) + vGFxElec_grid(9)
                                 <= 0
 cMaxGFxElec_grid0:              - vMaxFlowElec_grid + vGFxElec_grid(0) <= 0
 cMaxGFxElec_grid1:              - vMaxFlowElec_grid + vGFxElec_grid(1) <= 0
 cMaxGFxElec_grid2:              - vMaxFlowElec_grid + vGFxElec_grid(2) <= 0
 cMaxGFxElec_grid3:              - vMaxFlowElec_grid + vGFxElec_grid(3) <= 0
 cMaxGFxElec_grid4:              - vMaxFlowElec_grid + vGFxElec_grid(4) <= 0
 cMaxGFxElec_grid5:              - vMaxFlowElec_grid + vGFxElec_grid(5) <= 0
 cMaxGFxElec_grid6:              - vMaxFlowElec_grid + vGFxElec_grid(6) <= 0
 cMaxGFxElec_grid7:              - vMaxFlowElec_grid + vGFxElec_grid(7) <= 0
 cMaxGFxElec_grid8:              - vMaxFlowElec_grid + vGFxElec_grid(8) <= 0
 cMaxGFxElec_grid9:              - vMaxFlowElec_grid + vGFxElec_grid(9) <= 0
 csInstalledH2O_grid0:           vInstalledH2O_grid  = 1
 csMaxValH2O_grid0:              vMaxFlowH2O_grid  = 10000
 csBigMInstalledH2O_grid0:       - 10000 vInstalledH2O_grid + vMaxFlowH2O_grid
                                 <= 0
 csMinSizeInstalledH2O_grid0:    vMaxFlowH2O_grid >= 0
 cNotRunningIfNotInstalledH2O_grid0: - vInstalledH2O_grid + vStateH2O_grid(0)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid1: - vInstalledH2O_grid + vStateH2O_grid(1)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid2: - vInstalledH2O_grid + vStateH2O_grid(2)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid3: - vInstalledH2O_grid + vStateH2O_grid(3)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid4: - vInstalledH2O_grid + vStateH2O_grid(4)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid5: - vInstalledH2O_grid + vStateH2O_grid(5)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid6: - vInstalledH2O_grid + vStateH2O_grid(6)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid7: - vInstalledH2O_grid + vStateH2O_grid(7)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid8: - vInstalledH2O_grid + vStateH2O_grid(8)
                                 <= 0
 cNotRunningIfNotInstalledH2O_grid9: - vInstalledH2O_grid + vStateH2O_grid(9)
                                 <= 0
 cStateGridH2O_grid0:            - 10000 vStateH2O_grid(0) + vGFxH2O_grid(0)
                                 <= 0
 cStateGridH2O_grid1:            - 10000 vStateH2O_grid(1) + vGFxH2O_grid(1)
                                 <= 0
 cStateGridH2O_grid2:            - 10000 vStateH2O_grid(2) + vGFxH2O_grid(2)
                                 <= 0
 cStateGridH2O_grid3:            - 10000 vStateH2O_grid(3) + vGFxH2O_grid(3)
                                 <= 0
 cStateGridH2O_grid4:            - 10000 vStateH2O_grid(4) + vGFxH2O_grid(4)
                                 <= 0
 cStateGridH2O_grid5:            - 10000 vStateH2O_grid(5) + vGFxH2O_grid(5)
                                 <= 0
 cStateGridH2O_grid6:            - 10000 vStateH2O_grid(6) + vGFxH2O_grid(6)
                                 <= 0
 cStateGridH2O_grid7:            - 10000 vStateH2O_grid(7) + vGFxH2O_grid(7)
                                 <= 0
 cStateGridH2O_grid8:            - 10000 vStateH2O_grid(8) + vGFxH2O_grid(8)
                                 <= 0
 cStateGridH2O_grid9:            - 10000 vStateH2O_grid(9) + vGFxH2O_grid(9)
                                 <= 0
 cMaxGFxH2O_grid0:               - vMaxFlowH2O_grid + vGFxH2O_grid(0) <= 0
 cMaxGFxH2O_grid1:               - vMaxFlowH2O_grid + vGFxH2O_grid(1) <= 0
 cMaxGFxH2O_grid2:               - vMaxFlowH2O_grid + vGFxH2O_grid(2) <= 0
 cMaxGFxH2O_grid3:               - vMaxFlowH2O_grid + vGFxH2O_grid(3) <= 0
 cMaxGFxH2O_grid4:               - vMaxFlowH2O_grid + vGFxH2O_grid(4) <= 0
 cMaxGFxH2O_grid5:               - vMaxFlowH2O_grid + vGFxH2O_grid(5) <= 0
 cMaxGFxH2O_grid6:               - vMaxFlowH2O_grid + vGFxH2O_grid(6) <= 0
 cMaxGFxH2O_grid7:               - vMaxFlowH2O_grid + vGFxH2O_grid(7) <= 0
 cMaxGFxH2O_grid8:               - vMaxFlowH2O_grid + vGFxH2O_grid(8) <= 0
 cMaxGFxH2O_grid9:               - vMaxFlowH2O_grid + vGFxH2O_grid(9) <= 0
 csInstalledH2_load0:            vInstalledH2_load  = 1
 csWeightH2_load0:               vWeightH2_load  = 1
 csBigMInstalledH2_load0:        - vInstalledH2_load + vWeightH2_load <= 0
 csMinSizeInstalledH2_load0:     vWeightH2_load >= 0
 cMSFxH2_load0:                  <= 10000
 cMSFxH2_load1:                  <= 10000
 cMSFxH2_load2:                  <= 10000
 cMSFxH2_load3:                  <= 10000
 cMSFxH2_load4:                  <= 10000
 cMSFxH2_load5:                  <= 10000
 cMSFxH2_load6:                  <= 10000
 cMSFxH2_load7:                  <= 10000
 cMSFxH2_load8:                  <= 10000
 cMSFxH2_load9:                  <= 10000
 csInstalledHeat_HT_grid0:       vInstalledHeat_HT_grid  = 1
 csMaxValHeat_HT_grid0:          vMaxFlowHeat_HT_grid  = 10000000
 csBigMInstalledHeat_HT_grid0:   - 10000000 vInstalledHeat_HT_grid
                                 + vMaxFlowHeat_HT_grid <= 0
 csMinSizeInstalledHeat_HT_grid0: vMaxFlowHeat_HT_grid >= 0
 cNotRunningIfNotInstalledHeat_HT_grid0: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(0) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid1: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(1) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid2: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(2) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid3: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(3) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid4: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(4) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid5: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(5) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid6: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(6) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid7: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(7) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid8: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(8) <= 0
 cNotRunningIfNotInstalledHeat_HT_grid9: - vInstalledHeat_HT_grid
                                 + vStateHeat_HT_grid(9) <= 0
 cStateGridHeat_HT_grid0:        - 10000000 vStateHeat_HT_grid(0)
                                 + vGFxHeat_HT_grid(0) <= 0
 cStateGridHeat_HT_grid1:        - 10000000 vStateHeat_HT_grid(1)
                                 + vGFxHeat_HT_grid(1) <= 0
 cStateGridHeat_HT_grid2:        - 10000000 vStateHeat_HT_grid(2)
                                 + vGFxHeat_HT_grid(2) <= 0
 cStateGridHeat_HT_grid3:        - 10000000 vStateHeat_HT_grid(3)
                                 + vGFxHeat_HT_grid(3) <= 0
 cStateGridHeat_HT_grid4:        - 10000000 vStateHeat_HT_grid(4)
                                 + vGFxHeat_HT_grid(4) <= 0
 cStateGridHeat_HT_grid5:        - 10000000 vStateHeat_HT_grid(5)
                                 + vGFxHeat_HT_grid(5) <= 0
 cStateGridHeat_HT_grid6:        - 10000000 vStateHeat_HT_grid(6)
                                 + vGFxHeat_HT_grid(6) <= 0
 cStateGridHeat_HT_grid7:        - 10000000 vStateHeat_HT_grid(7)
                                 + vGFxHeat_HT_grid(7) <= 0
 cStateGridHeat_HT_grid8:        - 10000000 vStateHeat_HT_grid(8)
                                 + vGFxHeat_HT_grid(8) <= 0
 cStateGridHeat_HT_grid9:        - 10000000 vStateHeat_HT_grid(9)
                                 + vGFxHeat_HT_grid(9) <= 0
 cMaxGFxHeat_HT_grid0:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(0)
                                 <= 0
 cMaxGFxHeat_HT_grid1:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(1)
                                 <= 0
 cMaxGFxHeat_HT_grid2:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(2)
                                 <= 0
 cMaxGFxHeat_HT_grid3:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(3)
                                 <= 0
 cMaxGFxHeat_HT_grid4:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(4)
                                 <= 0
 cMaxGFxHeat_HT_grid5:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(5)
                                 <= 0
 cMaxGFxHeat_HT_grid6:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(6)
                                 <= 0
 cMaxGFxHeat_HT_grid7:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(7)
                                 <= 0
 cMaxGFxHeat_HT_grid8:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(8)
                                 <= 0
 cMaxGFxHeat_HT_grid9:           - vMaxFlowHeat_HT_grid + vGFxHeat_HT_grid(9)
                                 <= 0
 csInstalledHeat_Low_temp0:      vInstalledHeat_Low_temp  = 1
 csMaxValHeat_Low_temp0:         vMaxFlowHeat_Low_temp  = 10000
 csBigMInstalledHeat_Low_temp0:  - 10000 vInstalledHeat_Low_temp
                                 + vMaxFlowHeat_Low_temp <= 0
 csMinSizeInstalledHeat_Low_temp0: vMaxFlowHeat_Low_temp >= 0
 cNotRunningIfNotInstalledHeat_Low_temp0: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(0) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp1: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(1) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp2: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(2) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp3: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(3) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp4: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(4) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp5: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(5) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp6: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(6) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp7: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(7) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp8: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(8) <= 0
 cNotRunningIfNotInstalledHeat_Low_temp9: - vInstalledHeat_Low_temp
                                 + vStateHeat_Low_temp(9) <= 0
 cStateGridHeat_Low_temp0:       - 10000 vStateHeat_Low_temp(0)
                                 + vGFxHeat_Low_temp(0) <= 0
 cStateGridHeat_Low_temp1:       - 10000 vStateHeat_Low_temp(1)
                                 + vGFxHeat_Low_temp(1) <= 0
 cStateGridHeat_Low_temp2:       - 10000 vStateHeat_Low_temp(2)
                                 + vGFxHeat_Low_temp(2) <= 0
 cStateGridHeat_Low_temp3:       - 10000 vStateHeat_Low_temp(3)
                                 + vGFxHeat_Low_temp(3) <= 0
 cStateGridHeat_Low_temp4:       - 10000 vStateHeat_Low_temp(4)
                                 + vGFxHeat_Low_temp(4) <= 0
 cStateGridHeat_Low_temp5:       - 10000 vStateHeat_Low_temp(5)
                                 + vGFxHeat_Low_temp(5) <= 0
 cStateGridHeat_Low_temp6:       - 10000 vStateHeat_Low_temp(6)
                                 + vGFxHeat_Low_temp(6) <= 0
 cStateGridHeat_Low_temp7:       - 10000 vStateHeat_Low_temp(7)
                                 + vGFxHeat_Low_temp(7) <= 0
 cStateGridHeat_Low_temp8:       - 10000 vStateHeat_Low_temp(8)
                                 + vGFxHeat_Low_temp(8) <= 0
 cStateGridHeat_Low_temp9:       - 10000 vStateHeat_Low_temp(9)
                                 + vGFxHeat_Low_temp(9) <= 0
 cMaxGFxHeat_Low_temp0:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(0)
                                 <= 0
 cMaxGFxHeat_Low_temp1:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(1)
                                 <= 0
 cMaxGFxHeat_Low_temp2:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(2)
                                 <= 0
 cMaxGFxHeat_Low_temp3:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(3)
                                 <= 0
 cMaxGFxHeat_Low_temp4:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(4)
                                 <= 0
 cMaxGFxHeat_Low_temp5:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(5)
                                 <= 0
 cMaxGFxHeat_Low_temp6:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(6)
                                 <= 0
 cMaxGFxHeat_Low_temp7:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(7)
                                 <= 0
 cMaxGFxHeat_Low_temp8:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(8)
                                 <= 0
 cMaxGFxHeat_Low_temp9:          - vMaxFlowHeat_Low_temp + vGFxHeat_Low_temp(9)
                                 <= 0
 csInstalledMultiConverter#10:   vInstalledMultiConverter#1  = 1
 csMaxValMultiConverter#10:      vMaxPowerMultiConverter#1  = 1000
 csBigMInstalledMultiConverter#10: - 1000 vInstalledMultiConverter#1
                                 + vMaxPowerMultiConverter#1 <= 0
 csMinSizeInstalledMultiConverter#10: vMaxPowerMultiConverter#1 >= 0
 ccEfficiency_0MultiConverter#10: vFluxin0MultiConverter#1(0)
                                 + vFluxin2MultiConverter#1(0)
                                 - 33.3 vFluxout_0MultiConverter#1(0)
                                 - vFluxout_1MultiConverter#1(0)  = 0
 ccEfficiency_0MultiConverter#11: vFluxin0MultiConverter#1(1)
                                 + vFluxin2MultiConverter#1(1)
                                 - 33.3 vFluxout_0MultiConverter#1(1)
                                 - vFluxout_1MultiConverter#1(1)  = 0
 ccEfficiency_0MultiConverter#12: vFluxin0MultiConverter#1(2)
                                 + vFluxin2MultiConverter#1(2)
                                 - 33.3 vFluxout_0MultiConverter#1(2)
                                 - vFluxout_1MultiConverter#1(2)  = 0
 ccEfficiency_0MultiConverter#13: vFluxin0MultiConverter#1(3)
                                 + vFluxin2MultiConverter#1(3)
                                 - 33.3 vFluxout_0MultiConverter#1(3)
                                 - vFluxout_1MultiConverter#1(3)  = 0
 ccEfficiency_0MultiConverter#14: vFluxin0MultiConverter#1(4)
                                 + vFluxin2MultiConverter#1(4)
                                 - 33.3 vFluxout_0MultiConverter#1(4)
                                 - vFluxout_1MultiConverter#1(4)  = 0
 ccEfficiency_0MultiConverter#15: vFluxin0MultiConverter#1(5)
                                 + vFluxin2MultiConverter#1(5)
                                 - 33.3 vFluxout_0MultiConverter#1(5)
                                 - vFluxout_1MultiConverter#1(5)  = 0
 ccEfficiency_0MultiConverter#16: vFluxin0MultiConverter#1(6)
                                 + vFluxin2MultiConverter#1(6)
                                 - 33.3 vFluxout_0MultiConverter#1(6)
                                 - vFluxout_1MultiConverter#1(6)  = 0
 ccEfficiency_0MultiConverter#17: vFluxin0MultiConverter#1(7)
                                 + vFluxin2MultiConverter#1(7)
                                 - 33.3 vFluxout_0MultiConverter#1(7)
                                 - vFluxout_1MultiConverter#1(7)  = 0
 ccEfficiency_0MultiConverter#18: vFluxin0MultiConverter#1(8)
                                 + vFluxin2MultiConverter#1(8)
                                 - 33.3 vFluxout_0MultiConverter#1(8)
                                 - vFluxout_1MultiConverter#1(8)  = 0
 ccEfficiency_0MultiConverter#19: vFluxin0MultiConverter#1(9)
                                 + vFluxin2MultiConverter#1(9)
                                 - 33.3 vFluxout_0MultiConverter#1(9)
                                 - vFluxout_1MultiConverter#1(9)  = 0
 ccEfficiency_1MultiConverter#10: vFluxin0MultiConverter#1(0)
                                 + 0.5 vFluxin2MultiConverter#1(0)
                                 - 19.8 vFluxout_0MultiConverter#1(0)  = 0
 ccEfficiency_1MultiConverter#11: vFluxin0MultiConverter#1(1)
                                 + 0.5 vFluxin2MultiConverter#1(1)
                                 - 19.8 vFluxout_0MultiConverter#1(1)  = 0
 ccEfficiency_1MultiConverter#12: vFluxin0MultiConverter#1(2)
                                 + 0.5 vFluxin2MultiConverter#1(2)
                                 - 19.8 vFluxout_0MultiConverter#1(2)  = 0
 ccEfficiency_1MultiConverter#13: vFluxin0MultiConverter#1(3)
                                 + 0.5 vFluxin2MultiConverter#1(3)
                                 - 19.8 vFluxout_0MultiConverter#1(3)  = 0
 ccEfficiency_1MultiConverter#14: vFluxin0MultiConverter#1(4)
                                 + 0.5 vFluxin2MultiConverter#1(4)
                                 - 19.8 vFluxout_0MultiConverter#1(4)  = 0
 ccEfficiency_1MultiConverter#15: vFluxin0MultiConverter#1(5)
                                 + 0.5 vFluxin2MultiConverter#1(5)
                                 - 19.8 vFluxout_0MultiConverter#1(5)  = 0
 ccEfficiency_1MultiConverter#16: vFluxin0MultiConverter#1(6)
                                 + 0.5 vFluxin2MultiConverter#1(6)
                                 - 19.8 vFluxout_0MultiConverter#1(6)  = 0
 ccEfficiency_1MultiConverter#17: vFluxin0MultiConverter#1(7)
                                 + 0.5 vFluxin2MultiConverter#1(7)
                                 - 19.8 vFluxout_0MultiConverter#1(7)  = 0
 ccEfficiency_1MultiConverter#18: vFluxin0MultiConverter#1(8)
                                 + 0.5 vFluxin2MultiConverter#1(8)
                                 - 19.8 vFluxout_0MultiConverter#1(8)  = 0
 ccEfficiency_1MultiConverter#19: vFluxin0MultiConverter#1(9)
                                 + 0.5 vFluxin2MultiConverter#1(9)
                                 - 19.8 vFluxout_0MultiConverter#1(9)  = 0
 ccEfficiency_2MultiConverter#10: - 31.99 vFluxout_0MultiConverter#1(0)
                                 + 4.032 vFluxout_2MultiConverter#1(0)  = 0
 ccEfficiency_2MultiConverter#11: - 31.99 vFluxout_0MultiConverter#1(1)
                                 + 4.032 vFluxout_2MultiConverter#1(1)  = 0
 ccEfficiency_2MultiConverter#12: - 31.99 vFluxout_0MultiConverter#1(2)
                                 + 4.032 vFluxout_2MultiConverter#1(2)  = 0
 ccEfficiency_2MultiConverter#13: - 31.99 vFluxout_0MultiConverter#1(3)
                                 + 4.032 vFluxout_2MultiConverter#1(3)  = 0
 ccEfficiency_2MultiConverter#14: - 31.99 vFluxout_0MultiConverter#1(4)
                                 + 4.032 vFluxout_2MultiConverter#1(4)  = 0
 ccEfficiency_2MultiConverter#15: - 31.99 vFluxout_0MultiConverter#1(5)
                                 + 4.032 vFluxout_2MultiConverter#1(5)  = 0
 ccEfficiency_2MultiConverter#16: - 31.99 vFluxout_0MultiConverter#1(6)
                                 + 4.032 vFluxout_2MultiConverter#1(6)  = 0
 ccEfficiency_2MultiConverter#17: - 31.99 vFluxout_0MultiConverter#1(7)
                                 + 4.032 vFluxout_2MultiConverter#1(7)  = 0
 ccEfficiency_2MultiConverter#18: - 31.99 vFluxout_0MultiConverter#1(8)
                                 + 4.032 vFluxout_2MultiConverter#1(8)  = 0
 ccEfficiency_2MultiConverter#19: - 31.99 vFluxout_0MultiConverter#1(9)
                                 + 4.032 vFluxout_2MultiConverter#1(9)  = 0
 ccEfficiency_3MultiConverter#10: 4.032 vFluxin1MultiConverter#1(0)
                                 - 36 vFluxout_0MultiConverter#1(0)  = 0
 ccEfficiency_3MultiConverter#11: 4.032 vFluxin1MultiConverter#1(1)
                                 - 36 vFluxout_0MultiConverter#1(1)  = 0
 ccEfficiency_3MultiConverter#12: 4.032 vFluxin1MultiConverter#1(2)
                                 - 36 vFluxout_0MultiConverter#1(2)  = 0
 ccEfficiency_3MultiConverter#13: 4.032 vFluxin1MultiConverter#1(3)
                                 - 36 vFluxout_0MultiConverter#1(3)  = 0
 ccEfficiency_3MultiConverter#14: 4.032 vFluxin1MultiConverter#1(4)
                                 - 36 vFluxout_0MultiConverter#1(4)  = 0
 ccEfficiency_3MultiConverter#15: 4.032 vFluxin1MultiConverter#1(5)
                                 - 36 vFluxout_0MultiConverter#1(5)  = 0
 ccEfficiency_3MultiConverter#16: 4.032 vFluxin1MultiConverter#1(6)
                                 - 36 vFluxout_0MultiConverter#1(6)  = 0
 ccEfficiency_3MultiConverter#17: 4.032 vFluxin1MultiConverter#1(7)
                                 - 36 vFluxout_0MultiConverter#1(7)  = 0
 ccEfficiency_3MultiConverter#18: 4.032 vFluxin1MultiConverter#1(8)
                                 - 36 vFluxout_0MultiConverter#1(8)  = 0
 ccEfficiency_3MultiConverter#19: 4.032 vFluxin1MultiConverter#1(9)
                                 - 36 vFluxout_0MultiConverter#1(9)  = 0
 ccEfficiency_4MultiConverter#10:  = 0
 ccEfficiency_4MultiConverter#11:  = 0
 ccEfficiency_4MultiConverter#12:  = 0
 ccEfficiency_4MultiConverter#13:  = 0
 ccEfficiency_4MultiConverter#14:  = 0
 ccEfficiency_4MultiConverter#15:  = 0
 ccEfficiency_4MultiConverter#16:  = 0
 ccEfficiency_4MultiConverter#17:  = 0
 ccEfficiency_4MultiConverter#18:  = 0
 ccEfficiency_4MultiConverter#19:  = 0
 ccEfficiency_5MultiConverter#10:  = 0
 ccEfficiency_5MultiConverter#11:  = 0
 ccEfficiency_5MultiConverter#12:  = 0
 ccEfficiency_5MultiConverter#13:  = 0
 ccEfficiency_5MultiConverter#14:  = 0
 ccEfficiency_5MultiConverter#15:  = 0
 ccEfficiency_5MultiConverter#16:  = 0
 ccEfficiency_5MultiConverter#17:  = 0
 ccEfficiency_5MultiConverter#18:  = 0
 ccEfficiency_5MultiConverter#19:  = 0
 csInstalledO2_prod0:            vInstalledO2_prod  = 1
 csMaxValO2_prod0:               vMaxFlowO2_prod  = 10000
 csBigMInstalledO2_prod0:        - 10000 vInstalledO2_prod + vMaxFlowO2_prod
                                 <= 0
 csMinSizeInstalledO2_prod0:     vMaxFlowO2_prod >= 0
 cNotRunningIfNotInstalledO2_prod0: - vInstalledO2_prod + vStateO2_prod(0) <= 0
 cNotRunningIfNotInstalledO2_prod1: - vInstalledO2_prod + vStateO2_prod(1) <= 0
 cNotRunningIfNotInstalledO2_prod2: - vInstalledO2_prod + vStateO2_prod(2) <= 0
 cNotRunningIfNotInstalledO2_prod3: - vInstalledO2_prod + vStateO2_prod(3) <= 0
 cNotRunningIfNotInstalledO2_prod4: - vInstalledO2_prod + vStateO2_prod(4) <= 0
 cNotRunningIfNotInstalledO2_prod5: - vInstalledO2_prod + vStateO2_prod(5) <= 0
 cNotRunningIfNotInstalledO2_prod6: - vInstalledO2_prod + vStateO2_prod(6) <= 0
 cNotRunningIfNotInstalledO2_prod7: - vInstalledO2_prod + vStateO2_prod(7) <= 0
 cNotRunningIfNotInstalledO2_prod8: - vInstalledO2_prod + vStateO2_prod(8) <= 0
 cNotRunningIfNotInstalledO2_prod9: - vInstalledO2_prod + vStateO2_prod(9) <= 0
 cStateGridO2_prod0:             - 10000 vStateO2_prod(0) + vGFxO2_prod(0) <= 0
 cStateGridO2_prod1:             - 10000 vStateO2_prod(1) + vGFxO2_prod(1) <= 0
 cStateGridO2_prod2:             - 10000 vStateO2_prod(2) + vGFxO2_prod(2) <= 0
 cStateGridO2_prod3:             - 10000 vStateO2_prod(3) + vGFxO2_prod(3) <= 0
 cStateGridO2_prod4:             - 10000 vStateO2_prod(4) + vGFxO2_prod(4) <= 0
 cStateGridO2_prod5:             - 10000 vStateO2_prod(5) + vGFxO2_prod(5) <= 0
 cStateGridO2_prod6:             - 10000 vStateO2_prod(6) + vGFxO2_prod(6) <= 0
 cStateGridO2_prod7:             - 10000 vStateO2_prod(7) + vGFxO2_prod(7) <= 0
 cStateGridO2_prod8:             - 10000 vStateO2_prod(8) + vGFxO2_prod(8) <= 0
 cStateGridO2_prod9:             - 10000 vStateO2_prod(9) + vGFxO2_prod(9) <= 0
 cMaxGFxO2_prod0:                - vMaxFlowO2_prod + vGFxO2_prod(0) <= 0
 cMaxGFxO2_prod1:                - vMaxFlowO2_prod + vGFxO2_prod(1) <= 0
 cMaxGFxO2_prod2:                - vMaxFlowO2_prod + vGFxO2_prod(2) <= 0
 cMaxGFxO2_prod3:                - vMaxFlowO2_prod + vGFxO2_prod(3) <= 0
 cMaxGFxO2_prod4:                - vMaxFlowO2_prod + vGFxO2_prod(4) <= 0
 cMaxGFxO2_prod5:                - vMaxFlowO2_prod + vGFxO2_prod(5) <= 0
 cMaxGFxO2_prod6:                - vMaxFlowO2_prod + vGFxO2_prod(6) <= 0
 cMaxGFxO2_prod7:                - vMaxFlowO2_prod + vGFxO2_prod(7) <= 0
 cMaxGFxO2_prod8:                - vMaxFlowO2_prod + vGFxO2_prod(8) <= 0
 cMaxGFxO2_prod9:                - vMaxFlowO2_prod + vGFxO2_prod(9) <= 0
 cSNodeLaw#10:                   vGFxElec_grid(0) - vFluxin0MultiConverter#1(0)
                                  = 0
 cSNodeLaw#11:                   vGFxElec_grid(1) - vFluxin0MultiConverter#1(1)
                                  = 0
 cSNodeLaw#12:                   vGFxElec_grid(2) - vFluxin0MultiConverter#1(2)
                                  = 0
 cSNodeLaw#13:                   vGFxElec_grid(3) - vFluxin0MultiConverter#1(3)
                                  = 0
 cSNodeLaw#14:                   vGFxElec_grid(4) - vFluxin0MultiConverter#1(4)
                                  = 0
 cSNodeLaw#15:                   vGFxElec_grid(5) - vFluxin0MultiConverter#1(5)
                                  = 0
 cSNodeLaw#16:                   vGFxElec_grid(6) - vFluxin0MultiConverter#1(6)
                                  = 0
 cSNodeLaw#17:                   vGFxElec_grid(7) - vFluxin0MultiConverter#1(7)
                                  = 0
 cSNodeLaw#18:                   vGFxElec_grid(8) - vFluxin0MultiConverter#1(8)
                                  = 0
 cSNodeLaw#19:                   vGFxElec_grid(9) - vFluxin0MultiConverter#1(9)
                                  = 0
 cSNodeLaw#20:                   vGFxH2O_grid(0) - vFluxin1MultiConverter#1(0)
                                  = 0
 cSNodeLaw#21:                   vGFxH2O_grid(1) - vFluxin1MultiConverter#1(1)
                                  = 0
 cSNodeLaw#22:                   vGFxH2O_grid(2) - vFluxin1MultiConverter#1(2)
                                  = 0
 cSNodeLaw#23:                   vGFxH2O_grid(3) - vFluxin1MultiConverter#1(3)
                                  = 0
 cSNodeLaw#24:                   vGFxH2O_grid(4) - vFluxin1MultiConverter#1(4)
                                  = 0
 cSNodeLaw#25:                   vGFxH2O_grid(5) - vFluxin1MultiConverter#1(5)
                                  = 0
 cSNodeLaw#26:                   vGFxH2O_grid(6) - vFluxin1MultiConverter#1(6)
                                  = 0
 cSNodeLaw#27:                   vGFxH2O_grid(7) - vFluxin1MultiConverter#1(7)
                                  = 0
 cSNodeLaw#28:                   vGFxH2O_grid(8) - vFluxin1MultiConverter#1(8)
                                  = 0
 cSNodeLaw#29:                   vGFxH2O_grid(9) - vFluxin1MultiConverter#1(9)
                                  = 0
 cSNodeLaw#30:                   vGFxHeat_HT_grid(0)
                                 - vFluxin2MultiConverter#1(0)  = 0
 cSNodeLaw#31:                   vGFxHeat_HT_grid(1)
                                 - vFluxin2MultiConverter#1(1)  = 0
 cSNodeLaw#32:                   vGFxHeat_HT_grid(2)
                                 - vFluxin2MultiConverter#1(2)  = 0
 cSNodeLaw#33:                   vGFxHeat_HT_grid(3)
                                 - vFluxin2MultiConverter#1(3)  = 0
 cSNodeLaw#34:                   vGFxHeat_HT_grid(4)
                                 - vFluxin2MultiConverter#1(4)  = 0
 cSNodeLaw#35:                   vGFxHeat_HT_grid(5)
                                 - vFluxin2MultiConverter#1(5)  = 0
 cSNodeLaw#36:                   vGFxHeat_HT_grid(6)
                                 - vFluxin2MultiConverter#1(6)  = 0
 cSNodeLaw#37:                   vGFxHeat_HT_grid(7)
                                 - vFluxin2MultiConverter#1(7)  = 0
 cSNodeLaw#38:                   vGFxHeat_HT_grid(8)
                                 - vFluxin2MultiConverter#1(8)  = 0
 cSNodeLaw#39:                   vGFxHeat_HT_grid(9)
                                 - vFluxin2MultiConverter#1(9)  = 0
 cSNodeLaw#40:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(0)  = 0
 cSNodeLaw#41:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(1)  = 0
 cSNodeLaw#42:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(2)  = 0
 cSNodeLaw#43:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(3)  = 0
 cSNodeLaw#44:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(4)  = 0
 cSNodeLaw#45:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(5)  = 0
 cSNodeLaw#46:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(6)  = 0
 cSNodeLaw#47:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(7)  = 0
 cSNodeLaw#48:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(8)  = 0
 cSNodeLaw#49:                   - vWeightH2_load
                                 + vFluxout_0MultiConverter#1(9)  = 0
 cSNodeLaw#50:                   - vGFxHeat_Low_temp(0)
                                 + vFluxout_1MultiConverter#1(0)  = 0
 cSNodeLaw#51:                   - vGFxHeat_Low_temp(1)
                                 + vFluxout_1MultiConverter#1(1)  = 0
 cSNodeLaw#52:                   - vGFxHeat_Low_temp(2)
                                 + vFluxout_1MultiConverter#1(2)  = 0
 cSNodeLaw#53:                   - vGFxHeat_Low_temp(3)
                                 + vFluxout_1MultiConverter#1(3)  = 0
 cSNodeLaw#54:                   - vGFxHeat_Low_temp(4)
                                 + vFluxout_1MultiConverter#1(4)  = 0
 cSNodeLaw#55:                   - vGFxHeat_Low_temp(5)
                                 + vFluxout_1MultiConverter#1(5)  = 0
 cSNodeLaw#56:                   - vGFxHeat_Low_temp(6)
                                 + vFluxout_1MultiConverter#1(6)  = 0
 cSNodeLaw#57:                   - vGFxHeat_Low_temp(7)
                                 + vFluxout_1MultiConverter#1(7)  = 0
 cSNodeLaw#58:                   - vGFxHeat_Low_temp(8)
                                 + vFluxout_1MultiConverter#1(8)  = 0
 cSNodeLaw#59:                   - vGFxHeat_Low_temp(9)
                                 + vFluxout_1MultiConverter#1(9)  = 0
 cSNodeLaw#60:                   vFluxout_2MultiConverter#1(0) - vGFxO2_prod(0)
                                  = 0
 cSNodeLaw#61:                   vFluxout_2MultiConverter#1(1) - vGFxO2_prod(1)
                                  = 0
 cSNodeLaw#62:                   vFluxout_2MultiConverter#1(2) - vGFxO2_prod(2)
                                  = 0
 cSNodeLaw#63:                   vFluxout_2MultiConverter#1(3) - vGFxO2_prod(3)
                                  = 0
 cSNodeLaw#64:                   vFluxout_2MultiConverter#1(4) - vGFxO2_prod(4)
                                  = 0
 cSNodeLaw#65:                   vFluxout_2MultiConverter#1(5) - vGFxO2_prod(5)
                                  = 0
 cSNodeLaw#66:                   vFluxout_2MultiConverter#1(6) - vGFxO2_prod(6)
                                  = 0
 cSNodeLaw#67:                   vFluxout_2MultiConverter#1(7) - vGFxO2_prod(7)
                                  = 0
 cSNodeLaw#68:                   vFluxout_2MultiConverter#1(8) - vGFxO2_prod(8)
                                  = 0
 cSNodeLaw#69:                   vFluxout_2MultiConverter#1(9) - vGFxO2_prod(9)
                                  = 0
Bounds
 0 <= vInstalledElec_grid <= 1
 0 <= vMaxFlowElec_grid <= 10000
 0 <= vStateElec_grid(0) <= 1
 0 <= vStateElec_grid(1) <= 1
 0 <= vStateElec_grid(2) <= 1
 0 <= vStateElec_grid(3) <= 1
 0 <= vStateElec_grid(4) <= 1
 0 <= vStateElec_grid(5) <= 1
 0 <= vStateElec_grid(6) <= 1
 0 <= vStateElec_grid(7) <= 1
 0 <= vStateElec_grid(8) <= 1
 0 <= vStateElec_grid(9) <= 1
 0 <= vGFxElec_grid(0) <= 10000
 0 <= vGFxElec_grid(1) <= 10000
 0 <= vGFxElec_grid(2) <= 10000
 0 <= vGFxElec_grid(3) <= 10000
 0 <= vGFxElec_grid(4) <= 10000
 0 <= vGFxElec_grid(5) <= 10000
 0 <= vGFxElec_grid(6) <= 10000
 0 <= vGFxElec_grid(7) <= 10000
 0 <= vGFxElec_grid(8) <= 10000
 0 <= vGFxElec_grid(9) <= 10000
 0 <= vInstalledH2O_grid <= 1
 0 <= vMaxFlowH2O_grid <= 10000
 0 <= vStateH2O_grid(0) <= 1
 0 <= vStateH2O_grid(1) <= 1
 0 <= vStateH2O_grid(2) <= 1
 0 <= vStateH2O_grid(3) <= 1
 0 <= vStateH2O_grid(4) <= 1
 0 <= vStateH2O_grid(5) <= 1
 0 <= vStateH2O_grid(6) <= 1
 0 <= vStateH2O_grid(7) <= 1
 0 <= vStateH2O_grid(8) <= 1
 0 <= vStateH2O_grid(9) <= 1
 0 <= vGFxH2O_grid(0) <= 10000
 0 <= vGFxH2O_grid(1) <= 10000
 0 <= vGFxH2O_grid(2) <= 10000
 0 <= vGFxH2O_grid(3) <= 10000
 0 <= vGFxH2O_grid(4) <= 10000
 0 <= vGFxH2O_grid(5) <= 10000
 0 <= vGFxH2O_grid(6) <= 10000
 0 <= vGFxH2O_grid(7) <= 10000
 0 <= vGFxH2O_grid(8) <= 10000
 0 <= vGFxH2O_grid(9) <= 10000
 0 <= vInstalledH2_load <= 1
 0 <= vWeightH2_load <= 1
 0 <= vInstalledHeat_HT_grid <= 1
 0 <= vMaxFlowHeat_HT_grid <= 10000000
 0 <= vStateHeat_HT_grid(0) <= 1
 0 <= vStateHeat_HT_grid(1) <= 1
 0 <= vStateHeat_HT_grid(2) <= 1
 0 <= vStateHeat_HT_grid(3) <= 1
 0 <= vStateHeat_HT_grid(4) <= 1
 0 <= vStateHeat_HT_grid(5) <= 1
 0 <= vStateHeat_HT_grid(6) <= 1
 0 <= vStateHeat_HT_grid(7) <= 1
 0 <= vStateHeat_HT_grid(8) <= 1
 0 <= vStateHeat_HT_grid(9) <= 1
 0 <= vGFxHeat_HT_grid(0) <= 10000000
 0 <= vGFxHeat_HT_grid(1) <= 10000000
 0 <= vGFxHeat_HT_grid(2) <= 10000000
 0 <= vGFxHeat_HT_grid(3) <= 10000000
 0 <= vGFxHeat_HT_grid(4) <= 10000000
 0 <= vGFxHeat_HT_grid(5) <= 10000000
 0 <= vGFxHeat_HT_grid(6) <= 10000000
 0 <= vGFxHeat_HT_grid(7) <= 10000000
 0 <= vGFxHeat_HT_grid(8) <= 10000000
 0 <= vGFxHeat_HT_grid(9) <= 10000000
 0 <= vInstalledHeat_Low_temp <= 1
 0 <= vMaxFlowHeat_Low_temp <= 10000
 0 <= vStateHeat_Low_temp(0) <= 1
 0 <= vStateHeat_Low_temp(1) <= 1
 0 <= vStateHeat_Low_temp(2) <= 1
 0 <= vStateHeat_Low_temp(3) <= 1
 0 <= vStateHeat_Low_temp(4) <= 1
 0 <= vStateHeat_Low_temp(5) <= 1
 0 <= vStateHeat_Low_temp(6) <= 1
 0 <= vStateHeat_Low_temp(7) <= 1
 0 <= vStateHeat_Low_temp(8) <= 1
 0 <= vStateHeat_Low_temp(9) <= 1
 0 <= vGFxHeat_Low_temp(0) <= 10000
 0 <= vGFxHeat_Low_temp(1) <= 10000
 0 <= vGFxHeat_Low_temp(2) <= 10000
 0 <= vGFxHeat_Low_temp(3) <= 10000
 0 <= vGFxHeat_Low_temp(4) <= 10000
 0 <= vGFxHeat_Low_temp(5) <= 10000
 0 <= vGFxHeat_Low_temp(6) <= 10000
 0 <= vGFxHeat_Low_temp(7) <= 10000
 0 <= vGFxHeat_Low_temp(8) <= 10000
 0 <= vGFxHeat_Low_temp(9) <= 10000
 0 <= vInstalledMultiConverter#1 <= 1
 0 <= vMaxPowerMultiConverter#1 <= 1000
 0 <= vFluxin0MultiConverter#1(0) <= 180244
 0 <= vFluxin0MultiConverter#1(1) <= 180244
 0 <= vFluxin0MultiConverter#1(2) <= 180244
 0 <= vFluxin0MultiConverter#1(3) <= 180244
 0 <= vFluxin0MultiConverter#1(4) <= 180244
 0 <= vFluxin0MultiConverter#1(5) <= 180244
 0 <= vFluxin0MultiConverter#1(6) <= 180244
 0 <= vFluxin0MultiConverter#1(7) <= 180244
 0 <= vFluxin0MultiConverter#1(8) <= 180244
 0 <= vFluxin0MultiConverter#1(9) <= 180244
 0 <= vFluxin1MultiConverter#1(0) <= 180244
 0 <= vFluxin1MultiConverter#1(1) <= 180244
 0 <= vFluxin1MultiConverter#1(2) <= 180244
 0 <= vFluxin1MultiConverter#1(3) <= 180244
 0 <= vFluxin1MultiConverter#1(4) <= 180244
 0 <= vFluxin1MultiConverter#1(5) <= 180244
 0 <= vFluxin1MultiConverter#1(6) <= 180244
 0 <= vFluxin1MultiConverter#1(7) <= 180244
 0 <= vFluxin1MultiConverter#1(8) <= 180244
 0 <= vFluxin1MultiConverter#1(9) <= 180244
 0 <= vFluxin2MultiConverter#1(0) <= 180244
 0 <= vFluxin2MultiConverter#1(1) <= 180244
 0 <= vFluxin2MultiConverter#1(2) <= 180244
 0 <= vFluxin2MultiConverter#1(3) <= 180244
 0 <= vFluxin2MultiConverter#1(4) <= 180244
 0 <= vFluxin2MultiConverter#1(5) <= 180244
 0 <= vFluxin2MultiConverter#1(6) <= 180244
 0 <= vFluxin2MultiConverter#1(7) <= 180244
 0 <= vFluxin2MultiConverter#1(8) <= 180244
 0 <= vFluxin2MultiConverter#1(9) <= 180244
 0 <= vFluxout_0MultiConverter#1(0) <= 267308
 0 <= vFluxout_0MultiConverter#1(1) <= 267308
 0 <= vFluxout_0MultiConverter#1(2) <= 267308
 0 <= vFluxout_0MultiConverter#1(3) <= 267308
 0 <= vFluxout_0MultiConverter#1(4) <= 267308
 0 <= vFluxout_0MultiConverter#1(5) <= 267308
 0 <= vFluxout_0MultiConverter#1(6) <= 267308
 0 <= vFluxout_0MultiConverter#1(7) <= 267308
 0 <= vFluxout_0MultiConverter#1(8) <= 267308
 0 <= vFluxout_0MultiConverter#1(9) <= 267308
 0 <= vFluxout_1MultiConverter#1(0) <= 267308
 0 <= vFluxout_1MultiConverter#1(1) <= 267308
 0 <= vFluxout_1MultiConverter#1(2) <= 267308
 0 <= vFluxout_1MultiConverter#1(3) <= 267308
 0 <= vFluxout_1MultiConverter#1(4) <= 267308
 0 <= vFluxout_1MultiConverter#1(5) <= 267308
 0 <= vFluxout_1MultiConverter#1(6) <= 267308
 0 <= vFluxout_1MultiConverter#1(7) <= 267308
 0 <= vFluxout_1MultiConverter#1(8) <= 267308
 0 <= vFluxout_1MultiConverter#1(9) <= 267308
 0 <= vFluxout_2MultiConverter#1(0) <= 267308
 0 <= vFluxout_2MultiConverter#1(1) <= 267308
 0 <= vFluxout_2MultiConverter#1(2) <= 267308
 0 <= vFluxout_2MultiConverter#1(3) <= 267308
 0 <= vFluxout_2MultiConverter#1(4) <= 267308
 0 <= vFluxout_2MultiConverter#1(5) <= 267308
 0 <= vFluxout_2MultiConverter#1(6) <= 267308
 0 <= vFluxout_2MultiConverter#1(7) <= 267308
 0 <= vFluxout_2MultiConverter#1(8) <= 267308
 0 <= vFluxout_2MultiConverter#1(9) <= 267308
 0 <= vInstalledO2_prod <= 1
 0 <= vMaxFlowO2_prod <= 10000
 0 <= vStateO2_prod(0) <= 1
 0 <= vStateO2_prod(1) <= 1
 0 <= vStateO2_prod(2) <= 1
 0 <= vStateO2_prod(3) <= 1
 0 <= vStateO2_prod(4) <= 1
 0 <= vStateO2_prod(5) <= 1
 0 <= vStateO2_prod(6) <= 1
 0 <= vStateO2_prod(7) <= 1
 0 <= vStateO2_prod(8) <= 1
 0 <= vStateO2_prod(9) <= 1
 0 <= vGFxO2_prod(0) <= 10000
 0 <= vGFxO2_prod(1) <= 10000
 0 <= vGFxO2_prod(2) <= 10000
 0 <= vGFxO2_prod(3) <= 10000
 0 <= vGFxO2_prod(4) <= 10000
 0 <= vGFxO2_prod(5) <= 10000
 0 <= vGFxO2_prod(6) <= 10000
 0 <= vGFxO2_prod(7) <= 10000
 0 <= vGFxO2_prod(8) <= 10000
 0 <= vGFxO2_prod(9) <= 10000
Generals
 vInstalledElec_grid  vStateElec_grid(0)  vStateElec_grid(1) 
 vStateElec_grid(2)  vStateElec_grid(3)  vStateElec_grid(4) 
 vStateElec_grid(5)  vStateElec_grid(6)  vStateElec_grid(7) 
 vStateElec_grid(8)  vStateElec_grid(9)  vInstalledH2O_grid  vStateH2O_grid(0) 
 vStateH2O_grid(1)  vStateH2O_grid(2)  vStateH2O_grid(3)  vStateH2O_grid(4) 
 vStateH2O_grid(5)  vStateH2O_grid(6)  vStateH2O_grid(7)  vStateH2O_grid(8) 
 vStateH2O_grid(9)  vInstalledH2_load  vInstalledHeat_HT_grid 
 vStateHeat_HT_grid(0)  vStateHeat_HT_grid(1)  vStateHeat_HT_grid(2) 
 vStateHeat_HT_grid(3)  vStateHeat_HT_grid(4)  vStateHeat_HT_grid(5) 
 vStateHeat_HT_grid(6)  vStateHeat_HT_grid(7)  vStateHeat_HT_grid(8) 
 vStateHeat_HT_grid(9)  vInstalledHeat_Low_temp  vStateHeat_Low_temp(0) 
 vStateHeat_Low_temp(1)  vStateHeat_Low_temp(2)  vStateHeat_Low_temp(3) 
 vStateHeat_Low_temp(4)  vStateHeat_Low_temp(5)  vStateHeat_Low_temp(6) 
 vStateHeat_Low_temp(7)  vStateHeat_Low_temp(8)  vStateHeat_Low_temp(9) 
 vInstalledMultiConverter#1  vInstalledO2_prod  vStateO2_prod(0) 
 vStateO2_prod(1)  vStateO2_prod(2)  vStateO2_prod(3)  vStateO2_prod(4) 
 vStateO2_prod(5)  vStateO2_prod(6)  vStateO2_prod(7)  vStateO2_prod(8) 
 vStateO2_prod(9) 
End
