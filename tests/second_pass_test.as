; test_macro_label_conflict.as
mcro FOO
    mov r0, r1
mcroend

; This label conflicts with the macro name FOO
FOO: .data 5

; Another harmless label
BAR: .string "ok"