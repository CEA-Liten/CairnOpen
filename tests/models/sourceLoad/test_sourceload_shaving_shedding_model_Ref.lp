\ENCODING=ISO-8859-1
\Problem name: problem

Minimize
 obj1:
Subject To
 csInstalledElec_load0#0:        vInstalledElec_load  = 1
 csWeightElec_load0#1:           vWeightElec_load  = 1
 csBigMInstalledElec_load0#2:    - vInstalledElec_load + vWeightElec_load <= 0
 csMinSizeInstalledElec_load0#3: vWeightElec_load >= 0
 cMSFxElec_load0#4:              <= 10000
 cMSFxElec_load1#5:              <= 10000
 cMSFxElec_load2#6:              <= 10000
 cMSFxElec_load3#7:              <= 10000
 cMSFxElec_load4#8:              <= 10000
 cMSFxElec_load5#9:              <= 10000
 cMSFxElec_load6#10:             <= 10000
 cMSFxElec_load7#11:             <= 10000
 cMSFxElec_load8#12:             <= 10000
 cMSFxElec_load9#13:             <= 10000
 cShedStateElec_load0#14:        vShedStateElec_load(0) - vShedOnElec_load(0)
                                 + vShedOffElec_load(0)  = 0
 cShedOnElec_load0#15:           - vShedStateElec_load(0) + vShedOnElec_load(0)
                                 <= 0
 cShedOffElec_load0#16:          vShedStateElec_load(0) + vShedOffElec_load(0)
                                 <= 1
 cShedStateElec_load1#17:        - vShedStateElec_load(0)
                                 + vShedStateElec_load(1) - vShedOnElec_load(1)
                                 + vShedOffElec_load(1)  = 0
 cShedOnElec_load1#18:           - vShedStateElec_load(1) + vShedOnElec_load(1)
                                 <= 0
 cShedOffElec_load1#19:          vShedStateElec_load(1) + vShedOffElec_load(1)
                                 <= 1
 cShedStateElec_load2#20:        - vShedStateElec_load(1)
                                 + vShedStateElec_load(2) - vShedOnElec_load(2)
                                 + vShedOffElec_load(2)  = 0
 cShedOnElec_load2#21:           - vShedStateElec_load(2) + vShedOnElec_load(2)
                                 <= 0
 cShedOffElec_load2#22:          vShedStateElec_load(2) + vShedOffElec_load(2)
                                 <= 1
 cShedStateElec_load3#23:        - vShedStateElec_load(2)
                                 + vShedStateElec_load(3) - vShedOnElec_load(3)
                                 + vShedOffElec_load(3)  = 0
 cShedOnElec_load3#24:           - vShedStateElec_load(3) + vShedOnElec_load(3)
                                 <= 0
 cShedOffElec_load3#25:          vShedStateElec_load(3) + vShedOffElec_load(3)
                                 <= 1
 cShedStateElec_load4#26:        - vShedStateElec_load(3)
                                 + vShedStateElec_load(4) - vShedOnElec_load(4)
                                 + vShedOffElec_load(4)  = 0
 cShedOnElec_load4#27:           - vShedStateElec_load(4) + vShedOnElec_load(4)
                                 <= 0
 cShedOffElec_load4#28:          vShedStateElec_load(4) + vShedOffElec_load(4)
                                 <= 1
 cShedStateElec_load5#29:        - vShedStateElec_load(4)
                                 + vShedStateElec_load(5) - vShedOnElec_load(5)
                                 + vShedOffElec_load(5)  = 0
 cShedOnElec_load5#30:           - vShedStateElec_load(5) + vShedOnElec_load(5)
                                 <= 0
 cShedOffElec_load5#31:          vShedStateElec_load(5) + vShedOffElec_load(5)
                                 <= 1
 cShedStateElec_load6#32:        - vShedStateElec_load(5)
                                 + vShedStateElec_load(6) - vShedOnElec_load(6)
                                 + vShedOffElec_load(6)  = 0
 cShedOnElec_load6#33:           - vShedStateElec_load(6) + vShedOnElec_load(6)
                                 <= 0
 cShedOffElec_load6#34:          vShedStateElec_load(6) + vShedOffElec_load(6)
                                 <= 1
 cShedStateElec_load7#35:        - vShedStateElec_load(6)
                                 + vShedStateElec_load(7) - vShedOnElec_load(7)
                                 + vShedOffElec_load(7)  = 0
 cShedOnElec_load7#36:           - vShedStateElec_load(7) + vShedOnElec_load(7)
                                 <= 0
 cShedOffElec_load7#37:          vShedStateElec_load(7) + vShedOffElec_load(7)
                                 <= 1
 cShedStateElec_load8#38:        - vShedStateElec_load(7)
                                 + vShedStateElec_load(8) - vShedOnElec_load(8)
                                 + vShedOffElec_load(8)  = 0
 cShedOnElec_load8#39:           - vShedStateElec_load(8) + vShedOnElec_load(8)
                                 <= 0
 cShedOffElec_load8#40:          vShedStateElec_load(8) + vShedOffElec_load(8)
                                 <= 1
 cShedStateElec_load9#41:        - vShedStateElec_load(8)
                                 + vShedStateElec_load(9) - vShedOnElec_load(9)
                                 + vShedOffElec_load(9)  = 0
 cShedOnElec_load9#42:           - vShedStateElec_load(9) + vShedOnElec_load(9)
                                 <= 0
 cShedOffElec_load9#43:          vShedStateElec_load(9) + vShedOffElec_load(9)
                                 <= 1
 cPositiveShedElec_load0#44:     - 10 vWeightElec_load
                                 + vPowerSheddingElec_load(0) <= 0
 cMaxShedPowerElec_load0#45:     vPowerSheddingElec_load(0)
                                 - 20 vShedStateElec_load(0) <= 0
 cPositiveShedElec_load1#46:     - 18 vWeightElec_load
                                 + vPowerSheddingElec_load(1) <= 0
 cMaxShedPowerElec_load1#47:     vPowerSheddingElec_load(1)
                                 - 20 vShedStateElec_load(1) <= 0
 cPositiveShedElec_load2#48:     - 25 vWeightElec_load
                                 + vPowerSheddingElec_load(2) <= 0
 cMaxShedPowerElec_load2#49:     vPowerSheddingElec_load(2)
                                 - 20 vShedStateElec_load(2) <= 0
 cPositiveShedElec_load3#50:     - 32 vWeightElec_load
                                 + vPowerSheddingElec_load(3) <= 0
 cMaxShedPowerElec_load3#51:     vPowerSheddingElec_load(3)
                                 - 20 vShedStateElec_load(3) <= 0
 cPositiveShedElec_load4#52:     - 34 vWeightElec_load
                                 + vPowerSheddingElec_load(4) <= 0
 cMaxShedPowerElec_load4#53:     vPowerSheddingElec_load(4)
                                 - 20 vShedStateElec_load(4) <= 0
 cPositiveShedElec_load5#54:     - 32 vWeightElec_load
                                 + vPowerSheddingElec_load(5) <= 0
 cMaxShedPowerElec_load5#55:     vPowerSheddingElec_load(5)
                                 - 20 vShedStateElec_load(5) <= 0
 cPositiveShedElec_load6#56:     - 25 vWeightElec_load
                                 + vPowerSheddingElec_load(6) <= 0
 cMaxShedPowerElec_load6#57:     vPowerSheddingElec_load(6)
                                 - 20 vShedStateElec_load(6) <= 0
 cPositiveShedElec_load7#58:     - 18 vWeightElec_load
                                 + vPowerSheddingElec_load(7) <= 0
 cMaxShedPowerElec_load7#59:     vPowerSheddingElec_load(7)
                                 - 20 vShedStateElec_load(7) <= 0
 cPositiveShedElec_load8#60:     - 10 vWeightElec_load
                                 + vPowerSheddingElec_load(8) <= 0
 cMaxShedPowerElec_load8#61:     vPowerSheddingElec_load(8)
                                 - 20 vShedStateElec_load(8) <= 0
 cPositiveShedElec_load9#62:     - 8 vWeightElec_load
                                 + vPowerSheddingElec_load(9) <= 0
 cMaxShedPowerElec_load9#63:     vPowerSheddingElec_load(9)
                                 - 20 vShedStateElec_load(9) <= 0
 cMaxShedTimeElec_load9#64:      - vShedStateElec_load(9) + vShedOnElec_load(0)
                                 + vShedOnElec_load(1) + vShedOnElec_load(2)
                                 + vShedOnElec_load(3) + vShedOnElec_load(4)
                                 + vShedOnElec_load(5) + vShedOnElec_load(6)
                                 + vShedOnElec_load(7) + vShedOnElec_load(8)
                                 + vShedOnElec_load(9) >= 0
 cMinShedStdByElec_load4#65:     vShedStateElec_load(4) + vShedOffElec_load(0)
                                 + vShedOffElec_load(1) + vShedOffElec_load(2)
                                 + vShedOffElec_load(3) + vShedOffElec_load(4)
                                 <= 1
 cMinShedStdByElec_load5#66:     vShedStateElec_load(5) + vShedOffElec_load(1)
                                 + vShedOffElec_load(2) + vShedOffElec_load(3)
                                 + vShedOffElec_load(4) + vShedOffElec_load(5)
                                 <= 1
 cMinShedStdByElec_load6#67:     vShedStateElec_load(6) + vShedOffElec_load(2)
                                 + vShedOffElec_load(3) + vShedOffElec_load(4)
                                 + vShedOffElec_load(5) + vShedOffElec_load(6)
                                 <= 1
 cMinShedStdByElec_load7#68:     vShedStateElec_load(7) + vShedOffElec_load(3)
                                 + vShedOffElec_load(4) + vShedOffElec_load(5)
                                 + vShedOffElec_load(6) + vShedOffElec_load(7)
                                 <= 1
 cMinShedStdByElec_load8#69:     vShedStateElec_load(8) + vShedOffElec_load(4)
                                 + vShedOffElec_load(5) + vShedOffElec_load(6)
                                 + vShedOffElec_load(7) + vShedOffElec_load(8)
                                 <= 1
 cMinShedStdByElec_load9#70:     vShedStateElec_load(9) + vShedOffElec_load(5)
                                 + vShedOffElec_load(6) + vShedOffElec_load(7)
                                 + vShedOffElec_load(8) + vShedOffElec_load(9)
                                 <= 1
 cDefMaxEffectElec_load0#71:     vVarMaxEffectElec_load  = 5
 cFluxGridBoundElec_load0#72:    vVarPowerPeakShavingElec_load(0)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#73:   vVarPowerPeakShavingElec_load(0)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#74: 10 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(0) <= 39
 cFluxGridBoundElec_load0#75:    vVarPowerPeakShavingElec_load(1)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#76:   vVarPowerPeakShavingElec_load(1)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#77: 18 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(1) <= 39
 cFluxGridBoundElec_load0#78:    vVarPowerPeakShavingElec_load(2)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#79:   vVarPowerPeakShavingElec_load(2)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#80: 25 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(2) <= 39
 cFluxGridBoundElec_load0#81:    vVarPowerPeakShavingElec_load(3)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#82:   vVarPowerPeakShavingElec_load(3)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#83: 32 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(3) <= 39
 cFluxGridBoundElec_load0#84:    vVarPowerPeakShavingElec_load(4)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#85:   vVarPowerPeakShavingElec_load(4)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#86: 34 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(4) <= 39
 cFluxGridBoundElec_load0#87:    vVarPowerPeakShavingElec_load(5)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#88:   vVarPowerPeakShavingElec_load(5)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#89: 32 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(5) <= 39
 cFluxGridBoundElec_load0#90:    vVarPowerPeakShavingElec_load(6)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#91:   vVarPowerPeakShavingElec_load(6)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#92: 25 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(6) <= 39
 cFluxGridBoundElec_load0#93:    vVarPowerPeakShavingElec_load(7)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#94:   vVarPowerPeakShavingElec_load(7)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#95: 18 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(7) <= 39
 cFluxGridBoundElec_load0#96:    vVarPowerPeakShavingElec_load(8)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#97:   vVarPowerPeakShavingElec_load(8)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#98: 10 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(8) <= 39
 cFluxGridBoundElec_load0#99:    vVarPowerPeakShavingElec_load(9)
                                 - vVarMaxEffectElec_load <= 0
 cFluxGridBound2Elec_load0#100:  vVarPowerPeakShavingElec_load(9)
                                 + vVarMaxEffectElec_load >= 0
 cFluxGridMaxBoundElec_load0#101: 8 vWeightElec_load
                                 + vVarPowerPeakShavingElec_load(9) <= 39
 cSumsElec_load0#102:            vVarPowerPeakShavingElec_load(0)
                                 + vVarPowerPeakShavingElec_load(1)
                                 + vVarPowerPeakShavingElec_load(2)
                                 + vVarPowerPeakShavingElec_load(3)
                                 + vVarPowerPeakShavingElec_load(4)
                                 + vVarPowerPeakShavingElec_load(5)  = 0
 cSumsElec_load1#103:            vVarPowerPeakShavingElec_load(6)
                                 + vVarPowerPeakShavingElec_load(7)
                                 + vVarPowerPeakShavingElec_load(8)
                                 + vVarPowerPeakShavingElec_load(9)  = 0
 csInstalledElec_source0#104:    vInstalledElec_source  = 1
 csWeightElec_source0#105:       vWeightElec_source  = 1
 csBigMInstalledElec_source0#106: - vInstalledElec_source + vWeightElec_source
                                 <= 0
 csMinSizeInstalledElec_source0#107: vWeightElec_source >= 0
 cMSFxElec_source0#108:          <= 10000
 cMSFxElec_source1#109:          <= 10000
 cMSFxElec_source2#110:          <= 10000
 cMSFxElec_source3#111:          <= 10000
 cMSFxElec_source4#112:          <= 10000
 cMSFxElec_source5#113:          <= 10000
 cMSFxElec_source6#114:          <= 10000
 cMSFxElec_source7#115:          <= 10000
 cMSFxElec_source8#116:          <= 10000
 cMSFxElec_source9#117:          <= 10000
 cSElec_bus0#118:                - 10 vWeightElec_load
                                 + vPowerSheddingElec_load(0)
                                 - vVarPowerPeakShavingElec_load(0)
                                 + 10 vWeightElec_source  = 0
 cSElec_bus1#119:                - 18 vWeightElec_load
                                 + vPowerSheddingElec_load(1)
                                 - vVarPowerPeakShavingElec_load(1)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus2#120:                - 25 vWeightElec_load
                                 + vPowerSheddingElec_load(2)
                                 - vVarPowerPeakShavingElec_load(2)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus3#121:                - 32 vWeightElec_load
                                 + vPowerSheddingElec_load(3)
                                 - vVarPowerPeakShavingElec_load(3)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus4#122:                - 34 vWeightElec_load
                                 + vPowerSheddingElec_load(4)
                                 - vVarPowerPeakShavingElec_load(4)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus5#123:                - 32 vWeightElec_load
                                 + vPowerSheddingElec_load(5)
                                 - vVarPowerPeakShavingElec_load(5)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus6#124:                - 25 vWeightElec_load
                                 + vPowerSheddingElec_load(6)
                                 - vVarPowerPeakShavingElec_load(6)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus7#125:                - 18 vWeightElec_load
                                 + vPowerSheddingElec_load(7)
                                 - vVarPowerPeakShavingElec_load(7)
                                 + 18 vWeightElec_source  = 0
 cSElec_bus8#126:                - 10 vWeightElec_load
                                 + vPowerSheddingElec_load(8)
                                 - vVarPowerPeakShavingElec_load(8)
                                 + 10 vWeightElec_source  = 0
 cSElec_bus9#127:                - 8 vWeightElec_load
                                 + vPowerSheddingElec_load(9)
                                 - vVarPowerPeakShavingElec_load(9)
                                 + 8 vWeightElec_source  = 0
Bounds
 0 <= vInstalledElec_load <= 1
 0 <= vWeightElec_load <= 1
 0 <= vPowerSheddingElec_load(0) <= 20
 0 <= vPowerSheddingElec_load(1) <= 20
 0 <= vPowerSheddingElec_load(2) <= 20
 0 <= vPowerSheddingElec_load(3) <= 20
 0 <= vPowerSheddingElec_load(4) <= 20
 0 <= vPowerSheddingElec_load(5) <= 20
 0 <= vPowerSheddingElec_load(6) <= 20
 0 <= vPowerSheddingElec_load(7) <= 20
 0 <= vPowerSheddingElec_load(8) <= 20
 0 <= vPowerSheddingElec_load(9) <= 20
 0 <= vShedStateElec_load(0) <= 1
 0 <= vShedStateElec_load(1) <= 1
 0 <= vShedStateElec_load(2) <= 1
 0 <= vShedStateElec_load(3) <= 1
 0 <= vShedStateElec_load(4) <= 1
 0 <= vShedStateElec_load(5) <= 1
 0 <= vShedStateElec_load(6) <= 1
 0 <= vShedStateElec_load(7) <= 1
 0 <= vShedStateElec_load(8) <= 1
 0 <= vShedStateElec_load(9) <= 1
 0 <= vShedOnElec_load(0) <= 1
 0 <= vShedOnElec_load(1) <= 1
 0 <= vShedOnElec_load(2) <= 1
 0 <= vShedOnElec_load(3) <= 1
 0 <= vShedOnElec_load(4) <= 1
 0 <= vShedOnElec_load(5) <= 1
 0 <= vShedOnElec_load(6) <= 1
 0 <= vShedOnElec_load(7) <= 1
 0 <= vShedOnElec_load(8) <= 1
 0 <= vShedOnElec_load(9) <= 1
 0 <= vShedOffElec_load(0) <= 1
 0 <= vShedOffElec_load(1) <= 1
 0 <= vShedOffElec_load(2) <= 1
 0 <= vShedOffElec_load(3) <= 1
 0 <= vShedOffElec_load(4) <= 1
 0 <= vShedOffElec_load(5) <= 1
 0 <= vShedOffElec_load(6) <= 1
 0 <= vShedOffElec_load(7) <= 1
 0 <= vShedOffElec_load(8) <= 1
 0 <= vShedOffElec_load(9) <= 1
-5 <= vVarPowerPeakShavingElec_load(0) <= 5
-5 <= vVarPowerPeakShavingElec_load(1) <= 5
-5 <= vVarPowerPeakShavingElec_load(2) <= 5
-5 <= vVarPowerPeakShavingElec_load(3) <= 5
-5 <= vVarPowerPeakShavingElec_load(4) <= 5
-5 <= vVarPowerPeakShavingElec_load(5) <= 5
-5 <= vVarPowerPeakShavingElec_load(6) <= 5
-5 <= vVarPowerPeakShavingElec_load(7) <= 5
-5 <= vVarPowerPeakShavingElec_load(8) <= 5
-5 <= vVarPowerPeakShavingElec_load(9) <= 5
 0 <= vVarMaxEffectElec_load <= 5
 0 <= vInstalledElec_source <= 1
 0 <= vWeightElec_source <= 1
Generals
 vInstalledElec_load  vShedStateElec_load(0)  vShedStateElec_load(1) 
 vShedStateElec_load(2)  vShedStateElec_load(3)  vShedStateElec_load(4) 
 vShedStateElec_load(5)  vShedStateElec_load(6)  vShedStateElec_load(7) 
 vShedStateElec_load(8)  vShedStateElec_load(9)  vShedOnElec_load(0) 
 vShedOnElec_load(1)  vShedOnElec_load(2)  vShedOnElec_load(3) 
 vShedOnElec_load(4)  vShedOnElec_load(5)  vShedOnElec_load(6) 
 vShedOnElec_load(7)  vShedOnElec_load(8)  vShedOnElec_load(9) 
 vShedOffElec_load(0)  vShedOffElec_load(1)  vShedOffElec_load(2) 
 vShedOffElec_load(3)  vShedOffElec_load(4)  vShedOffElec_load(5) 
 vShedOffElec_load(6)  vShedOffElec_load(7)  vShedOffElec_load(8) 
 vShedOffElec_load(9)  vInstalledElec_source 
End
