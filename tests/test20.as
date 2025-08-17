; Test extra text after macro name
mcro my_macro extra_text         ; Error: extra text after macro name
    add r1, r2
    sub r3, r4
mcroend

START:  mov #5, r1
        stop