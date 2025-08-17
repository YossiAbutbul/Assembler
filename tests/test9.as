; Test various valid label formats
a:              mov #1, r1       ; Single character label
A123:           mov #2, r2       ; Mixed case with numbers
long_label_name: mov #3, r3      ; Long label with underscores
UPPER_CASE:     mov #4, r4       ; All uppercase
lower_case:     mov #5, r5       ; All lowercase
mixed_Case_123: mov #6, r6       ; Mixed everything

        stop