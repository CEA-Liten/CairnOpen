\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 10 vMaxEstoBattery + 99299.8143260721 vGFxGrid_Elec(0)
       + 99299.8143260721 vGFxGrid_Elec(1) + 99299.8143260721 vGFxGrid_Elec(2)
       + 99299.8143260721 vGFxGrid_Elec(3) + 99299.8143260721 vGFxGrid_Elec(4)
       + 99299.8143260721 vGFxGrid_Elec(5) + 99299.8143260721 vGFxGrid_Elec(6)
       + 99299.8143260721 vGFxGrid_Elec(7) + 99299.8143260721 vGFxGrid_Elec(8)
       + 99299.8143260721 vGFxGrid_Elec(9)
Subject To
 csBigMInstalledBattery0:        - 100 vInstalledBattery + vMaxEstoBattery <= 0
 csMinSizeInstalledBattery0:     vMaxEstoBattery >= 0
 cStoIni3Battery0:               vStoIniBattery  = 0
 cMBattery0:                     - vMaxEstoBattery + vEBattery(0) <= 0
 cStoBattery0:                   vEBattery(0) - 0.8 vQCBattery(0)
                                 + 1.25 vQDBattery(0) - vStoIniBattery  = 0
 cMBattery1:                     - vMaxEstoBattery + vEBattery(1) <= 0
 cStoBattery1:                   - vEBattery(0) + vEBattery(1)
                                 - 0.8 vQCBattery(1) + 1.25 vQDBattery(1)  = 0
 cMBattery2:                     - vMaxEstoBattery + vEBattery(2) <= 0
 cStoBattery2:                   - vEBattery(1) + vEBattery(2)
                                 - 0.8 vQCBattery(2) + 1.25 vQDBattery(2)  = 0
 cMBattery3:                     - vMaxEstoBattery + vEBattery(3) <= 0
 cStoBattery3:                   - vEBattery(2) + vEBattery(3)
                                 - 0.8 vQCBattery(3) + 1.25 vQDBattery(3)  = 0
 cMBattery4:                     - vMaxEstoBattery + vEBattery(4) <= 0
 cStoBattery4:                   - vEBattery(3) + vEBattery(4)
                                 - 0.8 vQCBattery(4) + 1.25 vQDBattery(4)  = 0
 cMBattery5:                     - vMaxEstoBattery + vEBattery(5) <= 0
 cStoBattery5:                   - vEBattery(4) + vEBattery(5)
                                 - 0.8 vQCBattery(5) + 1.25 vQDBattery(5)  = 0
 cMBattery6:                     - vMaxEstoBattery + vEBattery(6) <= 0
 cStoBattery6:                   - vEBattery(5) + vEBattery(6)
                                 - 0.8 vQCBattery(6) + 1.25 vQDBattery(6)  = 0
 cMBattery7:                     - vMaxEstoBattery + vEBattery(7) <= 0
 cStoBattery7:                   - vEBattery(6) + vEBattery(7)
                                 - 0.8 vQCBattery(7) + 1.25 vQDBattery(7)  = 0
 cMBattery8:                     - vMaxEstoBattery + vEBattery(8) <= 0
 cStoBattery8:                   - vEBattery(7) + vEBattery(8)
                                 - 0.8 vQCBattery(8) + 1.25 vQDBattery(8)  = 0
 cMBattery9:                     - vMaxEstoBattery + vEBattery(9) <= 0
 cStoBattery9:                   - vEBattery(8) + vEBattery(9)
                                 - 0.8 vQCBattery(9) + 1.25 vQDBattery(9)  = 0
 cMinStoFinBattery9:             vEBattery(9) - vStoIniBattery >= 0
 cMFCBattery0:                   vQCBattery(0) - 10 vsensBattery(0) <= 0
 cMFDBattery0:                   vQDBattery(0) + 10 vsensBattery(0) <= 10
 cMFCBattery1:                   vQCBattery(1) - 10 vsensBattery(1) <= 0
 cMFDBattery1:                   vQDBattery(1) + 10 vsensBattery(1) <= 10
 cMFCBattery2:                   vQCBattery(2) - 10 vsensBattery(2) <= 0
 cMFDBattery2:                   vQDBattery(2) + 10 vsensBattery(2) <= 10
 cMFCBattery3:                   vQCBattery(3) - 10 vsensBattery(3) <= 0
 cMFDBattery3:                   vQDBattery(3) + 10 vsensBattery(3) <= 10
 cMFCBattery4:                   vQCBattery(4) - 10 vsensBattery(4) <= 0
 cMFDBattery4:                   vQDBattery(4) + 10 vsensBattery(4) <= 10
 cMFCBattery5:                   vQCBattery(5) - 10 vsensBattery(5) <= 0
 cMFDBattery5:                   vQDBattery(5) + 10 vsensBattery(5) <= 10
 cMFCBattery6:                   vQCBattery(6) - 10 vsensBattery(6) <= 0
 cMFDBattery6:                   vQDBattery(6) + 10 vsensBattery(6) <= 10
 cMFCBattery7:                   vQCBattery(7) - 10 vsensBattery(7) <= 0
 cMFDBattery7:                   vQDBattery(7) + 10 vsensBattery(7) <= 10
 cMFCBattery8:                   vQCBattery(8) - 10 vsensBattery(8) <= 0
 cMFDBattery8:                   vQDBattery(8) + 10 vsensBattery(8) <= 10
 cMFCBattery9:                   vQCBattery(9) - 10 vsensBattery(9) <= 0
 cMFDBattery9:                   vQDBattery(9) + 10 vsensBattery(9) <= 10
 csInstalledDemande0:            vInstalledDemande  = 1
 csWeightDemande0:               vWeightDemande  = 1
 csBigMInstalledDemande0:        - vInstalledDemande + vWeightDemande <= 0
 csMinSizeInstalledDemande0:     vWeightDemande >= 0
 cMSFxDemande0:                  <= 10000
 cMSFxDemande1:                  <= 10000
 cMSFxDemande2:                  <= 10000
 cMSFxDemande3:                  <= 10000
 cMSFxDemande4:                  <= 10000
 cMSFxDemande5:                  <= 10000
 cMSFxDemande6:                  <= 10000
 cMSFxDemande7:                  <= 10000
 cMSFxDemande8:                  <= 10000
 cMSFxDemande9:                  <= 10000
 csInstalledGrid_Elec0:          vInstalledGrid_Elec  = 1
 csMaxValGrid_Elec0:             vMaxFlowGrid_Elec  = 7.8125
 csBigMInstalledGrid_Elec0:      - 7.8125 vInstalledGrid_Elec
                                 + vMaxFlowGrid_Elec <= 0
 csMinSizeInstalledGrid_Elec0:   vMaxFlowGrid_Elec >= 0
 cNotRunningIfNotInstalledGrid_Elec0: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(0) <= 0
 cNotRunningIfNotInstalledGrid_Elec1: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(1) <= 0
 cNotRunningIfNotInstalledGrid_Elec2: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(2) <= 0
 cNotRunningIfNotInstalledGrid_Elec3: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(3) <= 0
 cNotRunningIfNotInstalledGrid_Elec4: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(4) <= 0
 cNotRunningIfNotInstalledGrid_Elec5: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(5) <= 0
 cNotRunningIfNotInstalledGrid_Elec6: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(6) <= 0
 cNotRunningIfNotInstalledGrid_Elec7: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(7) <= 0
 cNotRunningIfNotInstalledGrid_Elec8: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(8) <= 0
 cNotRunningIfNotInstalledGrid_Elec9: - vInstalledGrid_Elec
                                 + vStateGrid_Elec(9) <= 0
 cStateGridGrid_Elec0:           - 7.8125 vStateGrid_Elec(0) + vGFxGrid_Elec(0)
                                 <= 0
 cStateGridGrid_Elec1:           - 7.8125 vStateGrid_Elec(1) + vGFxGrid_Elec(1)
                                 <= 0
 cStateGridGrid_Elec2:           - 7.8125 vStateGrid_Elec(2) + vGFxGrid_Elec(2)
                                 <= 0
 cStateGridGrid_Elec3:           - 7.8125 vStateGrid_Elec(3) + vGFxGrid_Elec(3)
                                 <= 0
 cStateGridGrid_Elec4:           - 7.8125 vStateGrid_Elec(4) + vGFxGrid_Elec(4)
                                 <= 0
 cStateGridGrid_Elec5:           - 7.8125 vStateGrid_Elec(5) + vGFxGrid_Elec(5)
                                 <= 0
 cStateGridGrid_Elec6:           - 7.8125 vStateGrid_Elec(6) + vGFxGrid_Elec(6)
                                 <= 0
 cStateGridGrid_Elec7:           - 7.8125 vStateGrid_Elec(7) + vGFxGrid_Elec(7)
                                 <= 0
 cStateGridGrid_Elec8:           - 7.8125 vStateGrid_Elec(8) + vGFxGrid_Elec(8)
                                 <= 0
 cStateGridGrid_Elec9:           - 7.8125 vStateGrid_Elec(9) + vGFxGrid_Elec(9)
                                 <= 0
 cMaxGFxGrid_Elec0:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(0) <= 0
 cMaxGFxGrid_Elec1:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(1) <= 0
 cMaxGFxGrid_Elec2:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(2) <= 0
 cMaxGFxGrid_Elec3:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(3) <= 0
 cMaxGFxGrid_Elec4:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(4) <= 0
 cMaxGFxGrid_Elec5:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(5) <= 0
 cMaxGFxGrid_Elec6:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(6) <= 0
 cMaxGFxGrid_Elec7:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(7) <= 0
 cMaxGFxGrid_Elec8:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(8) <= 0
 cMaxGFxGrid_Elec9:              - vMaxFlowGrid_Elec + vGFxGrid_Elec(9) <= 0
 cSNodeLaw#10:                   - vQCBattery(0) + vGFxGrid_Elec(0)  = 0
 cSNodeLaw#11:                   - vQCBattery(1) + vGFxGrid_Elec(1)  = 0
 cSNodeLaw#12:                   - vQCBattery(2) + vGFxGrid_Elec(2)  = 0
 cSNodeLaw#13:                   - vQCBattery(3) + vGFxGrid_Elec(3)  = 0
 cSNodeLaw#14:                   - vQCBattery(4) + vGFxGrid_Elec(4)  = 0
 cSNodeLaw#15:                   - vQCBattery(5) + vGFxGrid_Elec(5)  = 0
 cSNodeLaw#16:                   - vQCBattery(6) + vGFxGrid_Elec(6)  = 0
 cSNodeLaw#17:                   - vQCBattery(7) + vGFxGrid_Elec(7)  = 0
 cSNodeLaw#18:                   - vQCBattery(8) + vGFxGrid_Elec(8)  = 0
 cSNodeLaw#19:                   - vQCBattery(9) + vGFxGrid_Elec(9)  = 0
 cSNodeLaw#20:                   vQDBattery(0)  = 0
 cSNodeLaw#21:                   vQDBattery(1)  = 0
 cSNodeLaw#22:                   vQDBattery(2)  = 0
 cSNodeLaw#23:                   vQDBattery(3)  = 0
 cSNodeLaw#24:                   vQDBattery(4)  = 0
 cSNodeLaw#25:                   vQDBattery(5)  = 0
 cSNodeLaw#26:                   vQDBattery(6)  = 0
 cSNodeLaw#27:                   vQDBattery(7)  = 0
 cSNodeLaw#28:                   vQDBattery(8)  = 0
 cSNodeLaw#29:                   vQDBattery(9)  = 0
Bounds
 0 <= vInstalledBattery <= 1
 0 <= vMaxEstoBattery <= 100
 0 <= vEBattery(0) <= 100
 0 <= vEBattery(1) <= 100
 0 <= vEBattery(2) <= 100
 0 <= vEBattery(3) <= 100
 0 <= vEBattery(4) <= 100
 0 <= vEBattery(5) <= 100
 0 <= vEBattery(6) <= 100
 0 <= vEBattery(7) <= 100
 0 <= vEBattery(8) <= 100
 0 <= vEBattery(9) <= 100
 0 <= vQCBattery(0) <= 10
 0 <= vQCBattery(1) <= 10
 0 <= vQCBattery(2) <= 10
 0 <= vQCBattery(3) <= 10
 0 <= vQCBattery(4) <= 10
 0 <= vQCBattery(5) <= 10
 0 <= vQCBattery(6) <= 10
 0 <= vQCBattery(7) <= 10
 0 <= vQCBattery(8) <= 10
 0 <= vQCBattery(9) <= 10
 0 <= vQDBattery(0) <= 10
 0 <= vQDBattery(1) <= 10
 0 <= vQDBattery(2) <= 10
 0 <= vQDBattery(3) <= 10
 0 <= vQDBattery(4) <= 10
 0 <= vQDBattery(5) <= 10
 0 <= vQDBattery(6) <= 10
 0 <= vQDBattery(7) <= 10
 0 <= vQDBattery(8) <= 10
 0 <= vQDBattery(9) <= 10
 0 <= vStoIniBattery <= 100
 0 <= vsensBattery(0) <= 1
 0 <= vsensBattery(1) <= 1
 0 <= vsensBattery(2) <= 1
 0 <= vsensBattery(3) <= 1
 0 <= vsensBattery(4) <= 1
 0 <= vsensBattery(5) <= 1
 0 <= vsensBattery(6) <= 1
 0 <= vsensBattery(7) <= 1
 0 <= vsensBattery(8) <= 1
 0 <= vsensBattery(9) <= 1
 0 <= vInstalledDemande <= 1
 0 <= vWeightDemande <= 1
 0 <= vInstalledGrid_Elec <= 1
 0 <= vMaxFlowGrid_Elec <= 7.8125
 0 <= vStateGrid_Elec(0) <= 1
 0 <= vStateGrid_Elec(1) <= 1
 0 <= vStateGrid_Elec(2) <= 1
 0 <= vStateGrid_Elec(3) <= 1
 0 <= vStateGrid_Elec(4) <= 1
 0 <= vStateGrid_Elec(5) <= 1
 0 <= vStateGrid_Elec(6) <= 1
 0 <= vStateGrid_Elec(7) <= 1
 0 <= vStateGrid_Elec(8) <= 1
 0 <= vStateGrid_Elec(9) <= 1
 0 <= vGFxGrid_Elec(0) <= 7.8125
 0 <= vGFxGrid_Elec(1) <= 7.8125
 0 <= vGFxGrid_Elec(2) <= 7.8125
 0 <= vGFxGrid_Elec(3) <= 7.8125
 0 <= vGFxGrid_Elec(4) <= 7.8125
 0 <= vGFxGrid_Elec(5) <= 7.8125
 0 <= vGFxGrid_Elec(6) <= 7.8125
 0 <= vGFxGrid_Elec(7) <= 7.8125
 0 <= vGFxGrid_Elec(8) <= 7.8125
 0 <= vGFxGrid_Elec(9) <= 7.8125
Generals
 vInstalledBattery  vsensBattery(0)  vsensBattery(1)  vsensBattery(2) 
 vsensBattery(3)  vsensBattery(4)  vsensBattery(5)  vsensBattery(6) 
 vsensBattery(7)  vsensBattery(8)  vsensBattery(9)  vInstalledDemande 
 vInstalledGrid_Elec  vStateGrid_Elec(0)  vStateGrid_Elec(1) 
 vStateGrid_Elec(2)  vStateGrid_Elec(3)  vStateGrid_Elec(4) 
 vStateGrid_Elec(5)  vStateGrid_Elec(6)  vStateGrid_Elec(7) 
 vStateGrid_Elec(8)  vStateGrid_Elec(9) 
End
