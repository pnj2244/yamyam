#ifndef __basic_H__
#define __basic_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef unsigned long long word; // word: 64비트

#define NON_NEGATIVE 1
#define NEGATIVE -1
#define BASE_64 16
#define TEST 5

#define IN
#define OUT

/* 메모리 동적 할당, 처리대상 정수의 크기 제한이 없을 경우 워드열을 동적으로 메모리 할당. */
typedef struct Bigint
{
    int sign;    // 부호 음수인지 음이아닌 정수인지 판단.
    int wordlen; // 워드길이를 담는 변수, 0보다 크거나 같다.
    word *a;     // big integer의 주소.
} bigint;

/* ===== ===== ===== ===== ===== PROTO TYPE ===== ===== ===== ===== ===== */
void SUB(bigint **C, bigint *A, bigint *B);
/* ===== ===== ===== ===== ===== PROTO TYPE ===== ===== ===== ===== ===== */

// 입력 문자열을 64비트 워드로 변환해주는 함수, str_to_64(결과값, 입력스트링, 입력스트링의 워드단위길이)
void str_to_64(word *A, char *input);
int get_wordlen(char *input);
int get_sign(char *input);

// 수정해야함
void show64(bigint *A);

void bi_delete(bigint **x);
void bi_new(bigint **x, int wordlen);

void bi_refine(bigint *x);

/* true: 0임,  false: 0이 아님  */
int is_zero(bigint *A);
void bi_assign(bigint **y, bigint *x);

/* 1: A>B, -1: A<B, 0: A=B  */
// 동일부호 워드블록 비교 함수
int compareABS(bigint *A, bigint *B);
int compare(bigint *A, bigint *B);

/* 2**r REDUCTION FUNCTION */
void MOD(bigint **A, int x);

/* WORD SHIFT FUNCTION */
void MOD(bigint **A, int x);
void bi_rshift(bigint **A, int x);
void bi_lshift(bigint **A, int x);

#endif /* __basic_H__ */