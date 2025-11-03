\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1: 19859.9628652144 vGFxGrid(0) + 19859.9628652144 vGFxGrid(1)
       + 19859.9628652144 vGFxGrid(2) + 19859.9628652144 vGFxGrid(3)
       + 19859.9628652144 vGFxGrid(4) + 19859.9628652144 vGFxGrid(5)
       + 19859.9628652144 vGFxGrid(6) + 19859.9628652144 vGFxGrid(7)
       + 19859.9628652144 vGFxGrid(8) + 19859.9628652144 vGFxGrid(9)
Subject To
 csInstalledFix_Shed0:           vInstalledFix_Shed  = 1
 csWeightFix_Shed0:              vWeightFix_Shed  = 1
 csBigMInstalledFix_Shed0:       - vInstalledFix_Shed + vWeightFix_Shed <= 0
 csMinSizeInstalledFix_Shed0:    vWeightFix_Shed >= 0
 cMSFxFix_Shed0:                 <= 10000
 cMSFxFix_Shed1:                 <= 10000
 cMSFxFix_Shed2:                 <= 10000
 cMSFxFix_Shed3:                 <= 10000
 cMSFxFix_Shed4:                 <= 10000
 cMSFxFix_Shed5:                 <= 10000
 cMSFxFix_Shed6:                 <= 10000
 cMSFxFix_Shed7:                 <= 10000
 cMSFxFix_Shed8:                 <= 10000
 cMSFxFix_Shed9:                 <= 10000
 cShedStateFix_Shed0:            vShedStateFix_Shed(0) - vShedOnFix_Shed(0)
                                 + vShedOffFix_Shed(0)  = 0
 cShedOnFix_Shed0:               - vShedStateFix_Shed(0) + vShedOnFix_Shed(0)
                                 <= 0
 cShedOffFix_Shed0:              vShedStateFix_Shed(0) + vShedOffFix_Shed(0)
                                 <= 1
 cShedStateFix_Shed1:            - vShedStateFix_Shed(0)
                                 + vShedStateFix_Shed(1) - vShedOnFix_Shed(1)
                                 + vShedOffFix_Shed(1)  = 0
 cShedOnFix_Shed1:               - vShedStateFix_Shed(1) + vShedOnFix_Shed(1)
                                 <= 0
 cShedOffFix_Shed1:              vShedStateFix_Shed(1) + vShedOffFix_Shed(1)
                                 <= 1
 cShedStateFix_Shed2:            - vShedStateFix_Shed(1)
                                 + vShedStateFix_Shed(2) - vShedOnFix_Shed(2)
                                 + vShedOffFix_Shed(2)  = 0
 cShedOnFix_Shed2:               - vShedStateFix_Shed(2) + vShedOnFix_Shed(2)
                                 <= 0
 cShedOffFix_Shed2:              vShedStateFix_Shed(2) + vShedOffFix_Shed(2)
                                 <= 1
 cShedStateFix_Shed3:            - vShedStateFix_Shed(2)
                                 + vShedStateFix_Shed(3) - vShedOnFix_Shed(3)
                                 + vShedOffFix_Shed(3)  = 0
 cShedOnFix_Shed3:               - vShedStateFix_Shed(3) + vShedOnFix_Shed(3)
                                 <= 0
 cShedOffFix_Shed3:              vShedStateFix_Shed(3) + vShedOffFix_Shed(3)
                                 <= 1
 cShedStateFix_Shed4:            - vShedStateFix_Shed(3)
                                 + vShedStateFix_Shed(4) - vShedOnFix_Shed(4)
                                 + vShedOffFix_Shed(4)  = 0
 cShedOnFix_Shed4:               - vShedStateFix_Shed(4) + vShedOnFix_Shed(4)
                                 <= 0
 cShedOffFix_Shed4:              vShedStateFix_Shed(4) + vShedOffFix_Shed(4)
                                 <= 1
 cShedStateFix_Shed5:            - vShedStateFix_Shed(4)
                                 + vShedStateFix_Shed(5) - vShedOnFix_Shed(5)
                                 + vShedOffFix_Shed(5)  = 0
 cShedOnFix_Shed5:               - vShedStateFix_Shed(5) + vShedOnFix_Shed(5)
                                 <= 0
 cShedOffFix_Shed5:              vShedStateFix_Shed(5) + vShedOffFix_Shed(5)
                                 <= 1
 cShedStateFix_Shed6:            - vShedStateFix_Shed(5)
                                 + vShedStateFix_Shed(6) - vShedOnFix_Shed(6)
                                 + vShedOffFix_Shed(6)  = 0
 cShedOnFix_Shed6:               - vShedStateFix_Shed(6) + vShedOnFix_Shed(6)
                                 <= 0
 cShedOffFix_Shed6:              vShedStateFix_Shed(6) + vShedOffFix_Shed(6)
                                 <= 1
 cShedStateFix_Shed7:            - vShedStateFix_Shed(6)
                                 + vShedStateFix_Shed(7) - vShedOnFix_Shed(7)
                                 + vShedOffFix_Shed(7)  = 0
 cShedOnFix_Shed7:               - vShedStateFix_Shed(7) + vShedOnFix_Shed(7)
                                 <= 0
 cShedOffFix_Shed7:              vShedStateFix_Shed(7) + vShedOffFix_Shed(7)
                                 <= 1
 cShedStateFix_Shed8:            - vShedStateFix_Shed(7)
                                 + vShedStateFix_Shed(8) - vShedOnFix_Shed(8)
                                 + vShedOffFix_Shed(8)  = 0
 cShedOnFix_Shed8:               - vShedStateFix_Shed(8) + vShedOnFix_Shed(8)
                                 <= 0
 cShedOffFix_Shed8:              vShedStateFix_Shed(8) + vShedOffFix_Shed(8)
                                 <= 1
 cShedStateFix_Shed9:            - vShedStateFix_Shed(8)
                                 + vShedStateFix_Shed(9) - vShedOnFix_Shed(9)
                                 + vShedOffFix_Shed(9)  = 0
 cShedOnFix_Shed9:               - vShedStateFix_Shed(9) + vShedOnFix_Shed(9)
                                 <= 0
 cShedOffFix_Shed9:              vShedStateFix_Shed(9) + vShedOffFix_Shed(9)
                                 <= 1
 cPositiveShedFix_Shed0:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(0) <= 0
 cMaxShedPowerFix_Shed0:         vPowerSheddingFix_Shed(0)
                                 - 0.01 vShedStateFix_Shed(0) <= 0
 cPositiveShedFix_Shed1:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(1) <= 0
 cMaxShedPowerFix_Shed1:         vPowerSheddingFix_Shed(1)
                                 - 0.01 vShedStateFix_Shed(1) <= 0
 cPositiveShedFix_Shed2:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(2) <= 0
 cMaxShedPowerFix_Shed2:         vPowerSheddingFix_Shed(2)
                                 - 0.01 vShedStateFix_Shed(2) <= 0
 cPositiveShedFix_Shed3:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(3) <= 0
 cMaxShedPowerFix_Shed3:         vPowerSheddingFix_Shed(3)
                                 - 0.01 vShedStateFix_Shed(3) <= 0
 cPositiveShedFix_Shed4:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(4) <= 0
 cMaxShedPowerFix_Shed4:         vPowerSheddingFix_Shed(4)
                                 - 0.01 vShedStateFix_Shed(4) <= 0
 cPositiveShedFix_Shed5:         - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(5) <= 0
 cMaxShedPowerFix_Shed5:         vPowerSheddingFix_Shed(5)
                                 - 0.01 vShedStateFix_Shed(5) <= 0
 cPositiveShedFix_Shed6:         - 0.08 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(6) <= 0
 cMaxShedPowerFix_Shed6:         vPowerSheddingFix_Shed(6)
                                 - 0.01 vShedStateFix_Shed(6) <= 0
 cPositiveShedFix_Shed7:         - 0.08 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(7) <= 0
 cMaxShedPowerFix_Shed7:         vPowerSheddingFix_Shed(7)
                                 - 0.01 vShedStateFix_Shed(7) <= 0
 cPositiveShedFix_Shed8:         - 0.1 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(8) <= 0
 cMaxShedPowerFix_Shed8:         vPowerSheddingFix_Shed(8)
                                 - 0.01 vShedStateFix_Shed(8) <= 0
 cPositiveShedFix_Shed9:         - 0.1 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(9) <= 0
 cMaxShedPowerFix_Shed9:         vPowerSheddingFix_Shed(9)
                                 - 0.01 vShedStateFix_Shed(9) <= 0
 cMaxShedTimeFix_Shed1:          - vShedStateFix_Shed(1) + vShedOnFix_Shed(0)
                                 + vShedOnFix_Shed(1) >= 0
 cMaxShedTimeFix_Shed2:          - vShedStateFix_Shed(2) + vShedOnFix_Shed(1)
                                 + vShedOnFix_Shed(2) >= 0
 cMaxShedTimeFix_Shed3:          - vShedStateFix_Shed(3) + vShedOnFix_Shed(2)
                                 + vShedOnFix_Shed(3) >= 0
 cMaxShedTimeFix_Shed4:          - vShedStateFix_Shed(4) + vShedOnFix_Shed(3)
                                 + vShedOnFix_Shed(4) >= 0
 cMaxShedTimeFix_Shed5:          - vShedStateFix_Shed(5) + vShedOnFix_Shed(4)
                                 + vShedOnFix_Shed(5) >= 0
 cMaxShedTimeFix_Shed6:          - vShedStateFix_Shed(6) + vShedOnFix_Shed(5)
                                 + vShedOnFix_Shed(6) >= 0
 cMaxShedTimeFix_Shed7:          - vShedStateFix_Shed(7) + vShedOnFix_Shed(6)
                                 + vShedOnFix_Shed(7) >= 0
 cMaxShedTimeFix_Shed8:          - vShedStateFix_Shed(8) + vShedOnFix_Shed(7)
                                 + vShedOnFix_Shed(8) >= 0
 cMaxShedTimeFix_Shed9:          - vShedStateFix_Shed(9) + vShedOnFix_Shed(8)
                                 + vShedOnFix_Shed(9) >= 0
 cMinShedStdByFix_Shed1:         vShedStateFix_Shed(1) + vShedOffFix_Shed(0)
                                 + vShedOffFix_Shed(1) <= 1
 cMinShedStdByFix_Shed2:         vShedStateFix_Shed(2) + vShedOffFix_Shed(1)
                                 + vShedOffFix_Shed(2) <= 1
 cMinShedStdByFix_Shed3:         vShedStateFix_Shed(3) + vShedOffFix_Shed(2)
                                 + vShedOffFix_Shed(3) <= 1
 cMinShedStdByFix_Shed4:         vShedStateFix_Shed(4) + vShedOffFix_Shed(3)
                                 + vShedOffFix_Shed(4) <= 1
 cMinShedStdByFix_Shed5:         vShedStateFix_Shed(5) + vShedOffFix_Shed(4)
                                 + vShedOffFix_Shed(5) <= 1
 cMinShedStdByFix_Shed6:         vShedStateFix_Shed(6) + vShedOffFix_Shed(5)
                                 + vShedOffFix_Shed(6) <= 1
 cMinShedStdByFix_Shed7:         vShedStateFix_Shed(7) + vShedOffFix_Shed(6)
                                 + vShedOffFix_Shed(7) <= 1
 cMinShedStdByFix_Shed8:         vShedStateFix_Shed(8) + vShedOffFix_Shed(7)
                                 + vShedOffFix_Shed(8) <= 1
 cMinShedStdByFix_Shed9:         vShedStateFix_Shed(9) + vShedOffFix_Shed(8)
                                 + vShedOffFix_Shed(9) <= 1
 csInstalledGrid0:               vInstalledGrid  = 1
 csMaxValGrid0:                  vMaxFlowGrid  = 10000
 csBigMInstalledGrid0:           - 10000 vInstalledGrid + vMaxFlowGrid <= 0
 csMinSizeInstalledGrid0:        vMaxFlowGrid >= 0
 cNotRunningIfNotInstalledGrid0: - vInstalledGrid + vStateGrid(0) <= 0
 cNotRunningIfNotInstalledGrid1: - vInstalledGrid + vStateGrid(1) <= 0
 cNotRunningIfNotInstalledGrid2: - vInstalledGrid + vStateGrid(2) <= 0
 cNotRunningIfNotInstalledGrid3: - vInstalledGrid + vStateGrid(3) <= 0
 cNotRunningIfNotInstalledGrid4: - vInstalledGrid + vStateGrid(4) <= 0
 cNotRunningIfNotInstalledGrid5: - vInstalledGrid + vStateGrid(5) <= 0
 cNotRunningIfNotInstalledGrid6: - vInstalledGrid + vStateGrid(6) <= 0
 cNotRunningIfNotInstalledGrid7: - vInstalledGrid + vStateGrid(7) <= 0
 cNotRunningIfNotInstalledGrid8: - vInstalledGrid + vStateGrid(8) <= 0
 cNotRunningIfNotInstalledGrid9: - vInstalledGrid + vStateGrid(9) <= 0
 cStateGridGrid0:                - 10000 vStateGrid(0) + vGFxGrid(0) <= 0
 cStateGridGrid1:                - 10000 vStateGrid(1) + vGFxGrid(1) <= 0
 cStateGridGrid2:                - 10000 vStateGrid(2) + vGFxGrid(2) <= 0
 cStateGridGrid3:                - 10000 vStateGrid(3) + vGFxGrid(3) <= 0
 cStateGridGrid4:                - 10000 vStateGrid(4) + vGFxGrid(4) <= 0
 cStateGridGrid5:                - 10000 vStateGrid(5) + vGFxGrid(5) <= 0
 cStateGridGrid6:                - 10000 vStateGrid(6) + vGFxGrid(6) <= 0
 cStateGridGrid7:                - 10000 vStateGrid(7) + vGFxGrid(7) <= 0
 cStateGridGrid8:                - 10000 vStateGrid(8) + vGFxGrid(8) <= 0
 cStateGridGrid9:                - 10000 vStateGrid(9) + vGFxGrid(9) <= 0
 cMaxGFxGrid0:                   - vMaxFlowGrid + vGFxGrid(0) <= 0
 cMaxGFxGrid1:                   - vMaxFlowGrid + vGFxGrid(1) <= 0
 cMaxGFxGrid2:                   - vMaxFlowGrid + vGFxGrid(2) <= 0
 cMaxGFxGrid3:                   - vMaxFlowGrid + vGFxGrid(3) <= 0
 cMaxGFxGrid4:                   - vMaxFlowGrid + vGFxGrid(4) <= 0
 cMaxGFxGrid5:                   - vMaxFlowGrid + vGFxGrid(5) <= 0
 cMaxGFxGrid6:                   - vMaxFlowGrid + vGFxGrid(6) <= 0
 cMaxGFxGrid7:                   - vMaxFlowGrid + vGFxGrid(7) <= 0
 cMaxGFxGrid8:                   - vMaxFlowGrid + vGFxGrid(8) <= 0
 cMaxGFxGrid9:                   - vMaxFlowGrid + vGFxGrid(9) <= 0
 csInstalledTimeseries_shed0:    vInstalledTimeseries_shed  = 1
 csWeightTimeseries_shed0:       vWeightTimeseries_shed  = 1
 csBigMInstalledTimeseries_shed0: - vInstalledTimeseries_shed
                                 + vWeightTimeseries_shed <= 0
 csMinSizeInstalledTimeseries_shed0: vWeightTimeseries_shed >= 0
 cMSFxTimeseries_shed0:          <= 10000
 cMSFxTimeseries_shed1:          <= 10000
 cMSFxTimeseries_shed2:          <= 10000
 cMSFxTimeseries_shed3:          <= 10000
 cMSFxTimeseries_shed4:          <= 10000
 cMSFxTimeseries_shed5:          <= 10000
 cMSFxTimeseries_shed6:          <= 10000
 cMSFxTimeseries_shed7:          <= 10000
 cMSFxTimeseries_shed8:          <= 10000
 cMSFxTimeseries_shed9:          <= 10000
 cShedStateTimeseries_shed0:     vShedStateTimeseries_shed(0)
                                 - vShedOnTimeseries_shed(0)
                                 + vShedOffTimeseries_shed(0)  = 0
 cShedOnTimeseries_shed0:        - vShedStateTimeseries_shed(0)
                                 + vShedOnTimeseries_shed(0) <= 0
 cShedOffTimeseries_shed0:       vShedStateTimeseries_shed(0)
                                 + vShedOffTimeseries_shed(0) <= 1
 cShedStateTimeseries_shed1:     - vShedStateTimeseries_shed(0)
                                 + vShedStateTimeseries_shed(1)
                                 - vShedOnTimeseries_shed(1)
                                 + vShedOffTimeseries_shed(1)  = 0
 cShedOnTimeseries_shed1:        - vShedStateTimeseries_shed(1)
                                 + vShedOnTimeseries_shed(1) <= 0
 cShedOffTimeseries_shed1:       vShedStateTimeseries_shed(1)
                                 + vShedOffTimeseries_shed(1) <= 1
 cShedStateTimeseries_shed2:     - vShedStateTimeseries_shed(1)
                                 + vShedStateTimeseries_shed(2)
                                 - vShedOnTimeseries_shed(2)
                                 + vShedOffTimeseries_shed(2)  = 0
 cShedOnTimeseries_shed2:        - vShedStateTimeseries_shed(2)
                                 + vShedOnTimeseries_shed(2) <= 0
 cShedOffTimeseries_shed2:       vShedStateTimeseries_shed(2)
                                 + vShedOffTimeseries_shed(2) <= 1
 cShedStateTimeseries_shed3:     - vShedStateTimeseries_shed(2)
                                 + vShedStateTimeseries_shed(3)
                                 - vShedOnTimeseries_shed(3)
                                 + vShedOffTimeseries_shed(3)  = 0
 cShedOnTimeseries_shed3:        - vShedStateTimeseries_shed(3)
                                 + vShedOnTimeseries_shed(3) <= 0
 cShedOffTimeseries_shed3:       vShedStateTimeseries_shed(3)
                                 + vShedOffTimeseries_shed(3) <= 1
 cShedStateTimeseries_shed4:     - vShedStateTimeseries_shed(3)
                                 + vShedStateTimeseries_shed(4)
                                 - vShedOnTimeseries_shed(4)
                                 + vShedOffTimeseries_shed(4)  = 0
 cShedOnTimeseries_shed4:        - vShedStateTimeseries_shed(4)
                                 + vShedOnTimeseries_shed(4) <= 0
 cShedOffTimeseries_shed4:       vShedStateTimeseries_shed(4)
                                 + vShedOffTimeseries_shed(4) <= 1
 cShedStateTimeseries_shed5:     - vShedStateTimeseries_shed(4)
                                 + vShedStateTimeseries_shed(5)
                                 - vShedOnTimeseries_shed(5)
                                 + vShedOffTimeseries_shed(5)  = 0
 cShedOnTimeseries_shed5:        - vShedStateTimeseries_shed(5)
                                 + vShedOnTimeseries_shed(5) <= 0
 cShedOffTimeseries_shed5:       vShedStateTimeseries_shed(5)
                                 + vShedOffTimeseries_shed(5) <= 1
 cShedStateTimeseries_shed6:     - vShedStateTimeseries_shed(5)
                                 + vShedStateTimeseries_shed(6)
                                 - vShedOnTimeseries_shed(6)
                                 + vShedOffTimeseries_shed(6)  = 0
 cShedOnTimeseries_shed6:        - vShedStateTimeseries_shed(6)
                                 + vShedOnTimeseries_shed(6) <= 0
 cShedOffTimeseries_shed6:       vShedStateTimeseries_shed(6)
                                 + vShedOffTimeseries_shed(6) <= 1
 cShedStateTimeseries_shed7:     - vShedStateTimeseries_shed(6)
                                 + vShedStateTimeseries_shed(7)
                                 - vShedOnTimeseries_shed(7)
                                 + vShedOffTimeseries_shed(7)  = 0
 cShedOnTimeseries_shed7:        - vShedStateTimeseries_shed(7)
                                 + vShedOnTimeseries_shed(7) <= 0
 cShedOffTimeseries_shed7:       vShedStateTimeseries_shed(7)
                                 + vShedOffTimeseries_shed(7) <= 1
 cShedStateTimeseries_shed8:     - vShedStateTimeseries_shed(7)
                                 + vShedStateTimeseries_shed(8)
                                 - vShedOnTimeseries_shed(8)
                                 + vShedOffTimeseries_shed(8)  = 0
 cShedOnTimeseries_shed8:        - vShedStateTimeseries_shed(8)
                                 + vShedOnTimeseries_shed(8) <= 0
 cShedOffTimeseries_shed8:       vShedStateTimeseries_shed(8)
                                 + vShedOffTimeseries_shed(8) <= 1
 cShedStateTimeseries_shed9:     - vShedStateTimeseries_shed(8)
                                 + vShedStateTimeseries_shed(9)
                                 - vShedOnTimeseries_shed(9)
                                 + vShedOffTimeseries_shed(9)  = 0
 cShedOnTimeseries_shed9:        - vShedStateTimeseries_shed(9)
                                 + vShedOnTimeseries_shed(9) <= 0
 cShedOffTimeseries_shed9:       vShedStateTimeseries_shed(9)
                                 + vShedOffTimeseries_shed(9) <= 1
 cPositiveShedTimeseries_shed0:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(0) <= 0
 cMaxShedPowerTimeseries_shed0:  vPowerSheddingTimeseries_shed(0)
                                 - 0.08 vShedStateTimeseries_shed(0) <= 0
 cPositiveShedTimeseries_shed1:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(1) <= 0
 cMaxShedPowerTimeseries_shed1:  vPowerSheddingTimeseries_shed(1)
                                 - 0.08 vShedStateTimeseries_shed(1) <= 0
 cPositiveShedTimeseries_shed2:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(2) <= 0
 cMaxShedPowerTimeseries_shed2:  vPowerSheddingTimeseries_shed(2)
                                 - 0.08 vShedStateTimeseries_shed(2) <= 0
 cPositiveShedTimeseries_shed3:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(3) <= 0
 cMaxShedPowerTimeseries_shed3:  vPowerSheddingTimeseries_shed(3)
                                 - 0.08 vShedStateTimeseries_shed(3) <= 0
 cPositiveShedTimeseries_shed4:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(4) <= 0
 cMaxShedPowerTimeseries_shed4:  vPowerSheddingTimeseries_shed(4)
                                 - 0.08 vShedStateTimeseries_shed(4) <= 0
 cPositiveShedTimeseries_shed5:  - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(5) <= 0
 cMaxShedPowerTimeseries_shed5:  vPowerSheddingTimeseries_shed(5)
                                 - 0.08 vShedStateTimeseries_shed(5) <= 0
 cPositiveShedTimeseries_shed6:  - 0.08 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(6) <= 0
 cMaxShedPowerTimeseries_shed6:  vPowerSheddingTimeseries_shed(6)
                                 - 0.08 vShedStateTimeseries_shed(6) <= 0
 cPositiveShedTimeseries_shed7:  - 0.08 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(7) <= 0
 cMaxShedPowerTimeseries_shed7:  vPowerSheddingTimeseries_shed(7)
                                 - 0.08 vShedStateTimeseries_shed(7) <= 0
 cPositiveShedTimeseries_shed8:  - 0.1 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(8) <= 0
 cMaxShedPowerTimeseries_shed8:  vPowerSheddingTimeseries_shed(8)
                                 - 0.08 vShedStateTimeseries_shed(8) <= 0
 cPositiveShedTimeseries_shed9:  - 0.1 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(9) <= 0
 cMaxShedPowerTimeseries_shed9:  vPowerSheddingTimeseries_shed(9)
                                 - 0.08 vShedStateTimeseries_shed(9) <= 0
 cSNode0:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(0) + vGFxGrid(0)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(0)  = 0
 cSNode1:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(1) + vGFxGrid(1)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(1)  = 0
 cSNode2:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(2) + vGFxGrid(2)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(2)  = 0
 cSNode3:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(3) + vGFxGrid(3)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(3)  = 0
 cSNode4:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(4) + vGFxGrid(4)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(4)  = 0
 cSNode5:                        - 0.06 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(5) + vGFxGrid(5)
                                 - 0.06 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(5)  = 0
 cSNode6:                        - 0.08 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(6) + vGFxGrid(6)
                                 - 0.08 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(6)  = 0
 cSNode7:                        - 0.08 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(7) + vGFxGrid(7)
                                 - 0.08 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(7)  = 0
 cSNode8:                        - 0.1 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(8) + vGFxGrid(8)
                                 - 0.1 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(8)  = 0
 cSNode9:                        - 0.1 vWeightFix_Shed
                                 + vPowerSheddingFix_Shed(9) + vGFxGrid(9)
                                 - 0.1 vWeightTimeseries_shed
                                 + vPowerSheddingTimeseries_shed(9)  = 0
Bounds
 0 <= vInstalledFix_Shed <= 1
 0 <= vWeightFix_Shed <= 1
 0 <= vPowerSheddingFix_Shed(0) <= 0.01
 0 <= vPowerSheddingFix_Shed(1) <= 0.01
 0 <= vPowerSheddingFix_Shed(2) <= 0.01
 0 <= vPowerSheddingFix_Shed(3) <= 0.01
 0 <= vPowerSheddingFix_Shed(4) <= 0.01
 0 <= vPowerSheddingFix_Shed(5) <= 0.01
 0 <= vPowerSheddingFix_Shed(6) <= 0.01
 0 <= vPowerSheddingFix_Shed(7) <= 0.01
 0 <= vPowerSheddingFix_Shed(8) <= 0.01
 0 <= vPowerSheddingFix_Shed(9) <= 0.01
 0 <= vShedStateFix_Shed(0) <= 1
 0 <= vShedStateFix_Shed(1) <= 1
 0 <= vShedStateFix_Shed(2) <= 1
 0 <= vShedStateFix_Shed(3) <= 1
 0 <= vShedStateFix_Shed(4) <= 1
 0 <= vShedStateFix_Shed(5) <= 1
 0 <= vShedStateFix_Shed(6) <= 1
 0 <= vShedStateFix_Shed(7) <= 1
 0 <= vShedStateFix_Shed(8) <= 1
 0 <= vShedStateFix_Shed(9) <= 1
 0 <= vShedOnFix_Shed(0) <= 1
 0 <= vShedOnFix_Shed(1) <= 1
 0 <= vShedOnFix_Shed(2) <= 1
 0 <= vShedOnFix_Shed(3) <= 1
 0 <= vShedOnFix_Shed(4) <= 1
 0 <= vShedOnFix_Shed(5) <= 1
 0 <= vShedOnFix_Shed(6) <= 1
 0 <= vShedOnFix_Shed(7) <= 1
 0 <= vShedOnFix_Shed(8) <= 1
 0 <= vShedOnFix_Shed(9) <= 1
 0 <= vShedOffFix_Shed(0) <= 1
 0 <= vShedOffFix_Shed(1) <= 1
 0 <= vShedOffFix_Shed(2) <= 1
 0 <= vShedOffFix_Shed(3) <= 1
 0 <= vShedOffFix_Shed(4) <= 1
 0 <= vShedOffFix_Shed(5) <= 1
 0 <= vShedOffFix_Shed(6) <= 1
 0 <= vShedOffFix_Shed(7) <= 1
 0 <= vShedOffFix_Shed(8) <= 1
 0 <= vShedOffFix_Shed(9) <= 1
 0 <= vInstalledGrid <= 1
 0 <= vMaxFlowGrid <= 10000
 0 <= vStateGrid(0) <= 1
 0 <= vStateGrid(1) <= 1
 0 <= vStateGrid(2) <= 1
 0 <= vStateGrid(3) <= 1
 0 <= vStateGrid(4) <= 1
 0 <= vStateGrid(5) <= 1
 0 <= vStateGrid(6) <= 1
 0 <= vStateGrid(7) <= 1
 0 <= vStateGrid(8) <= 1
 0 <= vStateGrid(9) <= 1
 0 <= vGFxGrid(0) <= 10000
 0 <= vGFxGrid(1) <= 10000
 0 <= vGFxGrid(2) <= 10000
 0 <= vGFxGrid(3) <= 10000
 0 <= vGFxGrid(4) <= 10000
 0 <= vGFxGrid(5) <= 10000
 0 <= vGFxGrid(6) <= 10000
 0 <= vGFxGrid(7) <= 10000
 0 <= vGFxGrid(8) <= 10000
 0 <= vGFxGrid(9) <= 10000
 0 <= vInstalledTimeseries_shed <= 1
 0 <= vWeightTimeseries_shed <= 1
 0 <= vPowerSheddingTimeseries_shed(0) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(1) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(2) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(3) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(4) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(5) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(6) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(7) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(8) <= 0.1
 0 <= vPowerSheddingTimeseries_shed(9) <= 0.1
 0 <= vShedStateTimeseries_shed(0) <= 1
 0 <= vShedStateTimeseries_shed(1) <= 1
 0 <= vShedStateTimeseries_shed(2) <= 1
 0 <= vShedStateTimeseries_shed(3) <= 1
 0 <= vShedStateTimeseries_shed(4) <= 1
 0 <= vShedStateTimeseries_shed(5) <= 1
 0 <= vShedStateTimeseries_shed(6) <= 1
 0 <= vShedStateTimeseries_shed(7) <= 1
 0 <= vShedStateTimeseries_shed(8) <= 1
 0 <= vShedStateTimeseries_shed(9) <= 1
 0 <= vShedOnTimeseries_shed(0) <= 1
 0 <= vShedOnTimeseries_shed(1) <= 1
 0 <= vShedOnTimeseries_shed(2) <= 1
 0 <= vShedOnTimeseries_shed(3) <= 1
 0 <= vShedOnTimeseries_shed(4) <= 1
 0 <= vShedOnTimeseries_shed(5) <= 1
 0 <= vShedOnTimeseries_shed(6) <= 1
 0 <= vShedOnTimeseries_shed(7) <= 1
 0 <= vShedOnTimeseries_shed(8) <= 1
 0 <= vShedOnTimeseries_shed(9) <= 1
 0 <= vShedOffTimeseries_shed(0) <= 1
 0 <= vShedOffTimeseries_shed(1) <= 1
 0 <= vShedOffTimeseries_shed(2) <= 1
 0 <= vShedOffTimeseries_shed(3) <= 1
 0 <= vShedOffTimeseries_shed(4) <= 1
 0 <= vShedOffTimeseries_shed(5) <= 1
 0 <= vShedOffTimeseries_shed(6) <= 1
 0 <= vShedOffTimeseries_shed(7) <= 1
 0 <= vShedOffTimeseries_shed(8) <= 1
 0 <= vShedOffTimeseries_shed(9) <= 1
Generals
 vInstalledFix_Shed  vShedStateFix_Shed(0)  vShedStateFix_Shed(1) 
 vShedStateFix_Shed(2)  vShedStateFix_Shed(3)  vShedStateFix_Shed(4) 
 vShedStateFix_Shed(5)  vShedStateFix_Shed(6)  vShedStateFix_Shed(7) 
 vShedStateFix_Shed(8)  vShedStateFix_Shed(9)  vShedOnFix_Shed(0) 
 vShedOnFix_Shed(1)  vShedOnFix_Shed(2)  vShedOnFix_Shed(3) 
 vShedOnFix_Shed(4)  vShedOnFix_Shed(5)  vShedOnFix_Shed(6) 
 vShedOnFix_Shed(7)  vShedOnFix_Shed(8)  vShedOnFix_Shed(9) 
 vShedOffFix_Shed(0)  vShedOffFix_Shed(1)  vShedOffFix_Shed(2) 
 vShedOffFix_Shed(3)  vShedOffFix_Shed(4)  vShedOffFix_Shed(5) 
 vShedOffFix_Shed(6)  vShedOffFix_Shed(7)  vShedOffFix_Shed(8) 
 vShedOffFix_Shed(9)  vInstalledGrid  vStateGrid(0)  vStateGrid(1) 
 vStateGrid(2)  vStateGrid(3)  vStateGrid(4)  vStateGrid(5)  vStateGrid(6) 
 vStateGrid(7)  vStateGrid(8)  vStateGrid(9)  vInstalledTimeseries_shed 
 vShedStateTimeseries_shed(0)  vShedStateTimeseries_shed(1) 
 vShedStateTimeseries_shed(2)  vShedStateTimeseries_shed(3) 
 vShedStateTimeseries_shed(4)  vShedStateTimeseries_shed(5) 
 vShedStateTimeseries_shed(6)  vShedStateTimeseries_shed(7) 
 vShedStateTimeseries_shed(8)  vShedStateTimeseries_shed(9) 
 vShedOnTimeseries_shed(0)  vShedOnTimeseries_shed(1) 
 vShedOnTimeseries_shed(2)  vShedOnTimeseries_shed(3) 
 vShedOnTimeseries_shed(4)  vShedOnTimeseries_shed(5) 
 vShedOnTimeseries_shed(6)  vShedOnTimeseries_shed(7) 
 vShedOnTimeseries_shed(8)  vShedOnTimeseries_shed(9) 
 vShedOffTimeseries_shed(0)  vShedOffTimeseries_shed(1) 
 vShedOffTimeseries_shed(2)  vShedOffTimeseries_shed(3) 
 vShedOffTimeseries_shed(4)  vShedOffTimeseries_shed(5) 
 vShedOffTimeseries_shed(6)  vShedOffTimeseries_shed(7) 
 vShedOffTimeseries_shed(8)  vShedOffTimeseries_shed(9) 
End
