; Без проверки на линейное уравнение и отриц дискр

start:
    IN
    POPr Ax        ; коэфф a
    IN
    POPr Bx        ; коэфф b
    IN
    POPr Cx        ; коэфф c

    CALL FUNC_calculate_d
    CALL FUNC_calculate_x1
    CALL FUNC_calculate_x2
    HLT

FUNC_calculate_d:
    PUSHr Bx
    PUSHr Bx
    MULT                ; b^2

    PUSH 4
    PUSHr Ax
    MULT                ; 4*a
    PUSHr Cx
    MULT                ; 4*a*c

    SUB                 ; b^2 - 4*a*c = D
    POPr Dx
    RET

FUNC_calculate_x1:
    PUSH 0
    PUSHr Bx
    SUB                 ; -b

    PUSHr Dx
    SQR                 ; sqrt(D)
    ADD                 ; -b + sqrt(D)

    PUSH 2
    PUSHr Ax
    MULT                ; 2*a

    DIV                 ; (-b + sqrt(D)) / (2*a)
    OUT
    RET

FUNC_calculate_x2:
    PUSH 0
    PUSHr Bx
    SUB                 ; -b

    PUSHr Dx
    SQR                 ; sqrt(D)
    SUB                 ; -b - sqrt(D)

    PUSH 2
    PUSHr Ax
    MULT                ; 2*a

    DIV                 ; (-b - sqrt(D)) / (2*a)
    OUT
    RET


    