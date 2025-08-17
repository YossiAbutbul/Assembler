; Test various matrix sizes
MAT1:   .mat [1][1] 5                ; 1x1 matrix
MAT2:   .mat [10][10]                ; Large matrix with no values
MAT3:   .mat [2][3] 1,2,3            ; Partial initialization
MAT4:   .mat [2][2] 1,2,3,4,5,6      ; Too many values

; Test edge cases
MAT5:   .mat [0][2] 1,2              ; Zero rows (should error)
MAT6:   .mat [2][0] 1,2              ; Zero columns (should error)
MAT7:   .mat [2,2] 1,2,3,4           ; Wrong bracket syntax (should error)
MAT8:   .mat[2][2] 1,2,3,4           ; No space before brackets