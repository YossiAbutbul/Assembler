; Test invalid matrix syntax
MAT1: .mat [0][2] 1,2            ; Zero rows
MAT2: .mat [2][0] 1,2            ; Zero columns
MAT3: .mat [2,2] 1,2,3,4         ; Wrong bracket syntax
MAT4: .mat [2][2] 1,2,3,4,5,6,7  ; Too many values