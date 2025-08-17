START:  mov #10, #20             ; Immediate to immediate (invalid)
        lea #10, r1              ; Immediate source for lea (invalid)
        lea r1, #20              ; Immediate target for lea (invalid)
        add r1, #30              ; Immediate target for add (invalid)
        stop