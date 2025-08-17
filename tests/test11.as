; Test values outside 10-bit range
INVALID1: .data 512              ; Just above maximum
INVALID2: .data -513             ; Just below minimum
INVALID3: .data 1000             ; Way above maximum
INVALID4: .data -1000            ; Way below minimum