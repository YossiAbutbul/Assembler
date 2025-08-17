; Test valid macro syntax
mcro my_macro
    mov r1, r2
    add r3, r4
mcroend

mcro another_macro
    inc r1
    dec r2
mcroend

START:  my_macro
        another_macro
        stop