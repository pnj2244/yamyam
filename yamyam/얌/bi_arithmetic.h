#ifndef __BI_ARITHMETIC_H__
#define __BI_ARITHMETIC_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "bi_basic.h"

/* ===== ===== ===== ===== ===== PROTO TYPE ===== ===== ===== ===== ===== */
void SUB(bigint **C, bigint *A, bigint *B);
/* ===== ===== ===== ===== ===== PROTO TYPE ===== ===== ===== ===== ===== */

// ADD_ABc(출력 워드, 입력 carry, 입력워드 A, 입력워드 B)
void ADD_ABc(word *C, int *carry, word A, word B);
// 동일부호 덧셈, ADDC(결과값, 정수1, 정수2)
void ADDC(bigint **C, bigint *A, bigint *B);

// A>B 이어야함
void ADD(bigint **C, bigint *A, bigint *B);
void SUB_AbB(word *C, int *borrow, word A, word B);
// A>B를 가정
void SUBC(bigint **C, bigint *A, bigint *B);
void SUB(bigint **C, bigint *A, bigint *B);

void MUL_AB(bigint *C, word A, word B);
void MULC(bigint **C, bigint *A, bigint *B);

#endif /* __BI__ARITHMETIC_H__ */