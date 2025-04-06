        ; Código gerado pelo compilador Neander Gabriel

        LDA A
        STA TMP_MUL_LHS
        LDA B
        STA TMP_MUL_COUNT
        LDA ZERO
        STA TMP0
MUL_LOOP_0:
        LDA TMP0
        ADD TMP_MUL_LHS
        STA TMP0
        LDA TMP_MUL_COUNT
        SUB ONE
        STA TMP_MUL_COUNT
        JZ MUL_END_0
        JMP MUL_LOOP_0
MUL_END_0:
        LDA TMP0
        ADD 10
        STA C
        LDA C
        STA TMP_MUL_LHS
        LDA D
        STA TMP_MUL_COUNT
        LDA ZERO
        STA RES
MUL_LOOP_1:
        LDA RES
        ADD TMP_MUL_LHS
        STA RES
        LDA TMP_MUL_COUNT
        SUB ONE
        STA TMP_MUL_COUNT
        JZ MUL_END_1
        JMP MUL_LOOP_1
MUL_END_1:

        HLT

ZERO    OCT 0
ONE     OCT 1
TMP_MUL_LHS   SPACE
TMP_MUL_COUNT SPACE
A	SPACE
B	SPACE
C	SPACE
D	SPACE
RES	SPACE
