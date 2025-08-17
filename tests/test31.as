MATRIX: .mat [3][3] 1,2,3,4,5,6,7,8,9

START:  mov MATRIX[r0][r1], r2       ; Valid
        mov MATRIX[ r0 ][ r1 ], r3   ; Spaces in brackets
        mov MATRIX[r0], r4           ; Missing second index (should error)
        mov MATRIX[r8][r1], r5       ; Invalid register (should error)
        mov MATRIX[#1][r1], r6       ; Immediate in index (should error)