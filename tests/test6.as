; Test various matrix configurations
MAT1:   .mat [1][1] 42           ; 1x1 matrix
MAT2:   .mat [3][4] 1,2,3,4,5,6  ; Partial initialization
MAT3:   .mat [2  ][     2]        ; Spaces and tabs inside brackets
MAT4:   .mat [	2	][	2	]    ; Tabs inside brackets  
MAT5:   .mat [ 2 ][ 2 ]          ; Mixed whitespace

START:  mov MAT1[r0][r0], r1     ; Access 1x1 matrix
        mov MAT2[r1][r2], r3     ; Access partially filled matrix
        mov r4, MAT3[r5][r6]     ; Write to uninitialized matrix
        stop