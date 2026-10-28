start:
    IN
    POPr Ax          ;ну тип число которое лудик ввел

    PUSHr Ax
    PUSH 0
    JE  exit        ; 0 это выход из игры

    PUSHr Ax
    PUSH 67
    JE  win         ; еслм угадал число 67 то побед

lose:
    PUSH 0
    OUT             ;если не тпхнулся на метку раньше то проиграл
    JNE start     

win:
    PUSH 1          ; лудик победил ему вывелась 1 йоу
    OUT             
    JNE start  

exit:
    HLT