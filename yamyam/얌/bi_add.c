#include "bi_arithmetic.h"

// ADD_ABc(출력 워드, 입력 carry, 입력워드 A, 입력워드 B)
void ADD_ABc(word *C, int *carry, word A, word B)
{
    int c = 0;

    *C = A + B;

    if (*C < A)
        c = 1;
    *C = *C + *carry;
    if (*C < *carry)
        c += 1;
    *carry = c;
    // printf("==%d\n", *carry); carry 잘 나오나 확인용 라인
}
// 동일부호 덧셈, ADDC(결과값, 정수1, 정수2)
void ADDC(bigint **C, bigint *A, bigint *B)
{
    int c = 0;  // current carry
    word t = 0; // msb 체크 변수
    int sum = 0;
    /*  ==    ==    ==    ==    C 부호체크    ==    ==    ==    ==  */
    if (A->sign == NON_NEGATIVE)
        (*C)->sign = NON_NEGATIVE;
    else
        (*C)->sign = NEGATIVE;
    /*  ==    ==    ==    ==    C 부호체크    ==    ==    ==    ==  */

    /* Single word Addition, C = A + B + c */
    for (int j = 0; j < A->wordlen; j++)
    {
        ADD_ABc(&(*C)->a[j], &c, A->a[j], B->a[j]);
        // carry 값까지 확인할려면 밑의 코드 체크
        // printf("%#018llx + %#018llx = %#018llx \t next carry: %d\n", A->a[j], B->a[j], (*C)->a[j], c);
    }
    // for (int j = 0; j < TEST; j++)
    // printf("%#018llx + %#018llx = %#018llx \t next carry: %d\n", A->a[j], B->a[j], (*C)->a[j], c);

    // ADD_ABc() 후 carry = 1이면 워드 하나 추가해야함.
    // t = c;

    // case 1: next carry 발생 O -> C의 워드 1블록을 추가할당해야함.
    if (c == 1)
        (*C)->a[A->wordlen] = 0x1;
    // bi_refine(*C);
}

// A>B 이어야함
void ADD(bigint **C, bigint *A, bigint *B)
{
    // case 1: A가 0이면 A+B = B이다. 따라서 결과값은 B이다.
    if (is_zero(A) == true)
    {
        bi_assign(C, B);
        return;
    }
    // case 2: B가 0이면 A+B = A이다. 따라서 결과값은 A이다.
    if (is_zero(B) == true)
    {
        bi_assign(C, A);
        return;
    }

    // case 3: A가 양수, B가 음수면 A+B는 A-B와 같다.
    if (A->sign == NON_NEGATIVE && B->sign == NEGATIVE)
    {
        // B에 절댓값 씌워줌
        B->sign = NON_NEGATIVE;
        SUB(C, A, B);
        return;
    }

    // case 4: A가 음수, B가 양수면 -A + B는 B-A와 같다.
    if (A->sign == NEGATIVE && B->sign == NON_NEGATIVE)
    {
        // A에 절댓값 씌워줌
        A->sign = NON_NEGATIVE;
        SUB(C, B, A);
        return;
    }
    // case 5: A의 워드길이 < B의 워드길이 (A와 B의 부호는 동일)
    if (A->wordlen >= B->wordlen)
    {
        ADDC(C, A, B);
        return;
    }
    // case 6: A의 워드길이 < B의 워드길이 (A와 B의 부호는 동일)
    else
    {
        ADDC(C, B, A);
        return;
    }
}




