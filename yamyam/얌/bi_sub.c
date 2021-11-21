#include "bi_arithmetic.h"

void SUB_AbB(word *C, int *borrow, word A, word B)
{
    int b = 0; // 현재 borrow.

    *C = A - *borrow;

    if (*C < *borrow)
        b = 1;

    if (*C < B)
        b += 1;

    *C = *C - B;
    *borrow = b;
}
// A>B를 가정
void SUBC(bigint **C, bigint *A, bigint *B)
{
    int b = 0;  // current borrow
    word t = 0; // 최상위워드 체크 변수
    int sum = 0;
    /*  ==    ==    ==    ==    C 부호체크    ==    ==    ==    ==  */
    if (A->sign == NON_NEGATIVE)
        (*C)->sign = NON_NEGATIVE;
    else
        (*C)->sign = NEGATIVE;
    /*  ==    ==    ==    ==    C 부호체크    ==    ==    ==    ==  */

    /* Single word Subtraction, C = A - b - B */
    for (int j = 0; j < A->wordlen; j++)
    {
        SUB_AbB(&(*C)->a[j], &b, A->a[j], B->a[j]);
        // borrow 값까지 확인할려면 밑의 코드 체크
        // printf("%#018llx + %#018llx = %#018llx \t next carry: %d\n", A->a[j], B->a[j], (*C)->a[j], c);
    }
    // for (int j = 0; j < TEST; j++)
    // printf("%#018llx + %#018llx = %#018llx \t next carry: %d\n", A->a[j], B->a[j], (*C)->a[j], c);
}

void SUB(bigint **C, bigint *A, bigint *B)
{
    // 입력값 A와 B의 대소 비교 확인용 변수
    // A<B 일 경우 flag = -1
    // A>B 일 경우 flag = 1
    // A=B 일 경우 flag = 0
    int flag = compare(A, B);

    // flag = compare(A, B);

    // case : A = 0
    if (is_zero(A) == true)
    {
        B->sign = NEGATIVE;
        bi_assign(C, B);
        return;
    }
    // case : B = 0
    if (is_zero(B) == true)
    {
        bi_assign(C, B);
        return;
    }

    // A = B -> A-B = 0
    if (is_zero(A) == is_zero(B) == 0)
    {
        bi_assign(C, 0);
        return;
    }

    // case : 0 < B <= A -> A-B
    if ((A->sign == NON_NEGATIVE) && (B->sign == NON_NEGATIVE) && (flag == -1))
    {
        SUBC(C, A, B);
        return;
    }
    // case : 0 < A < B
    else if ((A->sign == NON_NEGATIVE) && (B->sign == NON_NEGATIVE) && (flag == 1))
    {
        SUBC(C, B, A);
        (*C)->sign = NEGATIVE;
        return;
    }
    // case : 0 > A >= B
    if (A->sign == NEGATIVE && B->sign == NEGATIVE && flag == -1)
    {
        // A와 B 절댓값 씌워주기
        A->sign = NON_NEGATIVE;
        B->sign = NON_NEGATIVE;
        SUBC(C, B, A);
        return;
    }
    // case : 0 > B > A
    if (A->sign == NEGATIVE && B->sign == NEGATIVE && flag == 1) //&& A >= B)
    {
        // A와 B 절댓값 씌워주기
        A->sign = NON_NEGATIVE;
        B->sign = NON_NEGATIVE;
        SUBC(C, A, B);
        (*C)->sign = NEGATIVE;
        return;
    }
    // case : A>0 AND B<0
    if (A->sign == NON_NEGATIVE && B->sign == NEGATIVE)
    {
        // B 절댓값 씌워주기
        B->sign = NON_NEGATIVE;
        ADD(C, A, B);
        return;
    }
    else
    {
        // A 절댓값 씌워주기
        A->sign = NON_NEGATIVE;
        ADD(C, A, B);
        (*C)->sign = NEGATIVE;
        return;
    }
}