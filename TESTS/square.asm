start:
    IN
    POPr Ax        ; коэфф a
    IN
    POPr Bx        ; коэфф b
    IN
    POPr Cx        ; коэфф c

    PUSHr Bx
    PUSHr Bx
    MUL            ; b^2

    PUSH 4
    PUSHr Ax
    MUL            ; 4*a
    PUSHr Cx
    MUL            ; 4*a*c

    SUB            ; b^2 - 4*a*c
    POPr Dx    

    PUSHr Dx 
    PUSH 0   
    JB no_roots    
    
    PUSHr Dx       
    PUSH 0   
    JE one_root    

    CALL FUNC_calculate_x1
    CALL FUNC_calculate_x2
    HLT

one_root:
    PUSH 0
    PUSHr Bx
    SUB            ; -b
    
    PUSH 2
    PUSHr Ax
    MUL            ; 2*a
    
    DIV            ; x = -b / 2a
    OUT
    HLT

no_roots:
    PUSH 68        ; D
    POP [0]
    PUSH 32        ; ' '
    POP [1]
    PUSH 60        ; <
    POP [2]
    PUSH 32        ; ' '
    POP [3]
    PUSH 48        ; 0
    POP [4]
    PUSH 10        ; \n
    POP [5]
    PUSH 0         ; \0
    POP [6]
    
    PRINT
    HLT

FUNC_calculate_x1:
    PUSH 0
    PUSHr Bx
    SUB            ; -b

    PUSHr Dx
    SQR            ; sqrt(D)
    ADD            ; -b + sqrt(D)

    PUSH 2
    PUSHr Ax
    MUL            ; 2*a

    DIV            ; (-b + sqrt(D)) / (2*a)
    OUT
    RET

FUNC_calculate_x2:
    PUSH 0
    PUSHr Bx
    SUB            ; -b

    PUSHr Dx
    SQR            ; sqrt(D)
    SUB            ; -b - sqrt(D)

    PUSH 2
    PUSHr Ax
    MUL            ; 2*a

    DIV            ; (-b - sqrt(D)) / (2*a)
    OUT
    RET
    