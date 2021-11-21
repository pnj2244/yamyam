#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef unsigned long long word; // word: 64비트

#define NON_NEGATIVE 1
#define NEGATIVE -1
#define BASE_64 16
#define TEST 1

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
void str_to_64(word *A, char *input)
{
    char t[16];
    char *endptr = NULL;
    int l = strlen(input);
    int len = ceil((float)(strlen(input) - 2) / (float)BASE_64);

    // printf("문자열 길이: %d\n", l);
    // printf("word 길이: %d\n", len);

    // l : 입력 워드의 길이.

    for (int j = len - 1; j >= 0; j--)
    {
        // 뒤에서 부터 문자열 복사.
        l = l - BASE_64;

        // case: 최상위워드가 64비트가 아닐 경우
        if (l < 1)
        {
            // t 초기화
            for (int i = 0; i < BASE_64; i++)
                t[i] = 0;

            strncpy(t, input + 2, BASE_64 + l - 2);
            A[len - j - 1] = strtoull(t, &endptr, BASE_64);

            // printf("TEST : %#018llx\n", A[len - j - 1]);
            return;
        }
        strncpy(t, input + l, BASE_64);
        A[len - j - 1] = strtoull(t, &endptr, BASE_64);

        // printf("TEST : %#018llx\n", A[len - j - 1]);
    }
}
int get_wordlen(char *input)
{
    return (int)(ceil((float)(strlen(input) - 2) / (float)BASE_64));
}
int get_sign(char *input)
{
    // case: 입력 문자열이 음수일 경우
    if (input[0] == 0x2d)
        return NEGATIVE;

    return NON_NEGATIVE;
}

// 베타
void show64(bigint *A)
{
    // case: 음수면 - 부호 추가
    if (A->sign == NEGATIVE)
        printf("-");

    printf("0x");

    // case: 워드 블록이 0

    printf("%llx", A->a[A->wordlen - 1]);
    for (int j = A->wordlen - 2; j >= 0; j--)
        printf("%016llx", A->a[j]);
    printf("\n");
}

void bi_delete(bigint **x)
{
    if (*x == NULL)
        return;

#ifdef ZERORIZE
array_init((*x)->a,(*x)->wordlen));
#endif
free((*x)->a);
free(*x);
*x = NULL;
}

void bi_new(bigint **x, int wordlen)
{
    if (*x != NULL)
        bi_delete(x);

    // x가 가리키는 값에 bigint byte만큼 동적 할당.
    *x = (bigint *)malloc(sizeof(bigint));
    // x가 가리키는 값에 1로 할당.
    (*x)->sign = NON_NEGATIVE;
    // x가 가리키는 값에 입력받은 wordlen 할당.
    (*x)->wordlen = wordlen;

    (*x)->a = (word *)calloc(wordlen, sizeof(word));
}

void bi_refine(bigint *x)
{
    if (x == NULL)
        return;

    int new_wordlen = x->wordlen;

    while (new_wordlen > 1) // 최소 한개의 워드가 필요함
    {
        // 마지막 워드가 0이 아니면 함수 종료.
        if (x->a[new_wordlen - 1] != 0)
        {
            // printf("real new_wordlen: %d\n", new_wordlen);
            break;
        }
        new_wordlen--;
        // printf("new_wordlen: %d\n", new_wordlen);
    }
    if (x->wordlen != new_wordlen)
    {
        x->wordlen = new_wordlen;
        x->a = (word *)realloc(x->a, sizeof(word) * new_wordlen);
    }

    if ((x->wordlen == 1) && (x->a[0] == 0x0))
        x->sign = NON_NEGATIVE;
}

/* true: 0임,  false: 0이 아님  */
int is_zero(bigint *A)
{
    // case: A가 양수이거나 최상위워드가 0이 아닐 경우
    if (A->sign == NON_NEGATIVE || A->a[0] != 0)
        return false;

    for (int j = A->wordlen - 1; j > 0; j--)
        if (A->a[j] != 0)
            return false;
    return true;
}
void bi_assign(bigint **y, bigint *x)
{
    if (*y != NULL)
        bi_delete(y);
    bi_new(y, x->wordlen);
    (*y)->sign = x->sign;
    memcpy((*y)->a, x->a, x->wordlen * BASE_64);
}

/* 1: A>B, -1: A<B, 0: A=B  */
// 동일부호 워드블록 비교 함수
int compareABS(bigint *A, bigint *B)
{
    // case 1: A > B
    if (A->wordlen > B->wordlen)
        return NON_NEGATIVE;

    // case 2: A > B
    else if (A->wordlen < B->wordlen)
        return NEGATIVE;

    // case 3: A와 B의 워드길이가 동일
    for (int j = A->wordlen - 1; j >= 0; j--)
    {
        if (A->a[j] > B->a[j])
            return NON_NEGATIVE;
        else if (A->a[j] < B->a[j])
            return NEGATIVE;
    }
    // case : A == B
    return 0;
}
// 1:
int compare(bigint *A, bigint *B)
{
    if ((A->sign == 0) && (B->sign == 1))
        return NON_NEGATIVE;
    if ((A->sign == 1) && (B->sign == 0))
        return NEGATIVE;
    int ret = compareABS(A, B);
    if (A->sign == 0)
        return ret;
    else
        return ret * -1;
}

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
    {
        (*C)->wordlen = (*C)->wordlen + 1;
        (*C)->a[A->wordlen] = 0x1;
    }

    bi_refine(*C);
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

// void bi_lshift(bigint **B, bigint *A, int r)
// {
//     int rr = r;
//     // 64비트여서 일단 base: 64
//     int base = BASE_64 * 4;
//     int k = 0;

//     // shift연산 후 비트 길이가 늘어남
//     int n = A->wordlen + (int)ceil(((float)r / (float)(base)));
//     (*B)->wordlen = n;
//     // test 곧 삭제
//     printf("n = %d\n", n);
//     if (r > base)
//     {
//         rr = r % base;
//         k = r / base;
//         // case: r이 워드블록의 배수인 경우
//         if (r % base == 0)
//         {
//             for (int j = 0; j < n; j++)
//                 if (j < k)
//                     (*B)->a[j] = 0;
//                 else
//                     (*B)->a[j] = A->a[j - k];
//             // show64(*B);
//             return;
//         }
//     }

//     // case: r = wk + r'인 경우. 즉, 일반적인 경우
//     for (int j = 0; j < k; j++)
//     {
//         (*B)->a[j] = 0;
//         printf("B[%02d] = %016llx\n", j, (*B)->a[j]);
//     }
//     // case: j = 0

//     (*B)->a[k] = (A->a[0] << rr);
//     printf("B[%02d] = %016llx\n", k, (*B)->a[k]);

//     // case: j = 1, ... , n-1
//     for (int j = 1 + k; j < n; j++)
//     {
//         (*B)->a[j] = (A->a[j - k] << rr) | (A->a[j - 1 - k] >> (base - rr));
//         printf("B[%02d] = %016llx\n", j, (*B)->a[j]);
//     }

//     // case: j = n
//     (*B)->a[n] = A->a[n - 1 - k] >> (base - rr);
//     printf("B[%02d] = %016llx\n", n, (*B)->a[n]);

//     // show64(*B);
// }
// }

void MOD(bigint **A, int x)
{
    // x =  w * q + r, with 0 < r < w.

    // 64비트 기준
    word MASK = 1;

    int w = 64;
    int n = (*A)->wordlen;

    // q: x를 w로 나누었을 때의 몫
    int q = x / w;
    // r: x를 w로 나누었을 때의 나머지
    int r = x % w;

    // case: x가 wn보다 크거나 같은 경우
    if (x >= w * n)
        return;

    // case: x가 w의 배수인 경우 where q < n.
    else if (x % w == 0)
    {
        // k 위로 싹다 잘라내기
        for (int j = q; j < n; j++)
            (*A)->a[j] = 0;
        bi_refine(*A);
        return;
    }
    // case: 그 외
    else
    {
        (*A)->a[q] = (*A)->a[q] & ((MASK << r) - 1);

        // k 위로 싹다 잘라내기
        for (int j = q + 1; j < n; j++)
            (*A)->a[j] = 0;
        bi_refine(*A);
        return;
    }
}

void bi_rshift(bigint **A, int x)
{
    // x =  w * q + r, with 0 < r < w.

    // 64비트 기준
    int w = 64;
    int n = (*A)->wordlen;

    // q: x를 w로 나누었을 때의 몫
    int q = x / w;
    // r: x를 w로 나누었을 때의 나머지
    int r = x % w;

    // case: x가 w의 배수인 경우 where q < n.
    if (x % w == 0)
    {
        // 값 옮겨주기
        for (int j = q; j < n; j++)
            (*A)->a[j - q] = (*A)->a[j];

        // 맨위 지우기
        for (int j = n; j >= n - q; j--)
            (*A)->a[j] = 0;

        bi_refine(*A);
        return;
    }
    // case: 그 외
    else
    {
        printf("q = %d\n", q);
        // k 위로 싹다 잘라내기
        for (int j = q; j < n - 1; j++)
            (*A)->a[j - q] = ((*A)->a[j + 1] << (w - r)) | ((*A)->a[j] >> r);

        (*A)->a[n - 1 - q] = ((*A)->a[n - 1] >> r);

        // 맨위 지우기
        for (int j = n; j >= n - q; j--)
            (*A)->a[j] = 0;
        bi_refine(*A);
        return;
    }
}

void bi_lshift(bigint **A, int x)
{
    // x =  w * q + r, with 0 < r < w.

    // 64비트 기준
    int w = 64;
    int n = (*A)->wordlen;

    // q: x를 w로 나누었을 때의 몫
    int q = x / w;
    // r: x를 w로 나누었을 때의 나머지
    int r = x % w;
    // printf("q = %d\n", q);
    bigint *T = NULL;
    bi_new(&T, n + q + 1);

    // case: x가 wn보다 크거나 같은 경우
    if (x >= w * n)
    {
        for (int j = 0; j < n; j++)
            (*A)->a[j] = 0;
        bi_delete(&T);
        return;
    }

    // case: x가 w의 배수인 경우 where q < n.
    else if (x % w == 0)
    {
        // 초기 설정
        for (int j = 0; j < n; j++)
            T->a[j + q] = (*A)->a[j];

        // 값 옮겨주기
        for (int j = q; j < n; j++)
            T->a[j] = (*A)->a[j - q];

        bi_assign(A, T);
        bi_delete(&T);
        bi_refine(*A);
        return;
    }
    // case: 그 외
    else
    {
        // 초기 설정
        for (int j = 0; j < n; j++)
            T->a[j + q] = 0;

        T->a[q] = (*A)->a[0] << r;

        for (int j = 1; j < n; j++)
            T->a[j + q] = ((*A)->a[j] << r) | ((*A)->a[j - 1] >> (w - r));

        T->a[n + q] = (*A)->a[n - 1] >> (w - r);

        bi_assign(A, T);
        bi_delete(&T);
        bi_refine(*A);
        return;
    }
}
// void MUL_AB(bigint **C, bigint *A, bigint *B)
// {
//     // 64비트 기준
//     int w = 64;
//     word A1 = A->a[0] >> w / 2;
//     word A0 = (A->a[0] << w / 2) >> w / 2;
//     printf("A1 = %llx\n", A1);
//     printf("A0 = %llx\n", A0);

//     word B1 = B->a[0] >> w / 2;
//     word B0 = (B->a[0] << w / 2) >> w / 2;
//     printf("B1 = %llx\n", B1);
//     printf("B0 = %llx\n", B0);

//     word T1 = A1 * B0;
//     word T0 = A0 * B1;

//     printf("T1 = %llx\n", T1);
//     printf("T0 = %llx\n", T0);

//     T0 = T1 + T0;
//     printf("T0 = %llx\n", T0);
//     T1 = 0;
//     // int flag = compare(T0, T1);

//     word C1 = A1 * B1;
//     printf("C1 = %llx\n", C1);
//     word C0 = A0 * B0;
//     printf("C0 = %llx\n\n", C0);

//     word T = C0;
//     printf("T = %llx\n", T);

//     C0 = C0 + (T0 << (w / 2));
//     printf("C0 = %llx\n", C0);

//     C1 = C1 + (T1 << (w / 2)) + (T0 >> (w / 2)) + (C0 < T);
//     printf("C1 = %llx\n", C1);
//     word ret[2] = {
//         0,
//     };

//     ret[0] = C0;
//     ret[1] = C1;
//     printf("RESURT: ");
//     for (int j = 1; j >= 0; j--)
//         printf("%llx", ret[j]);
//     printf("\n");

//     for (int j = 0; j < 2; j++)
//         (*C)->a[j] = ret[1 - j];

//     bi_refine(*C);
//     return;
// }

void MUL_AB(bigint *C, word A, word B)
{
    // C->wordlen = 2;

    // 64비트 기준
    int w = 64;

    word A1 = A >> w / 2;
    word A0 = (A << w / 2) >> w / 2;

    word B1 = B >> w / 2;
    word B0 = (B << w / 2) >> w / 2;

    word T1 = A1 * B0;
    word T0 = A0 * B1;

    T0 = T1 + T0;
    T1 = T0 < T1;

    word C1 = A1 * B1;
    word C0 = A0 * B0;

    word T = C0;

    C0 = C0 + (T0 << (w / 2));
    C1 = C1 + (T1 << (w / 2)) + (T0 >> (w / 2)) + (C0 < T);

    C->a[0] = C0;
    C->a[1] = C1;

    // C워드길이
    // C->wordlen++;
    bi_refine(C);

    return;
}

void MULC(bigint **C, bigint *A, bigint *B)
{
    int w = 64;
    // n: A의 워드길이 , m: B의 워드길이
    int n = A->wordlen;
    int m = B->wordlen;

    bigint *T = NULL;
    bi_new(&T, n + m);

    printf("n = %d\nm = %d\n", n, m);
    (*C)->wordlen = n + m;

    for (int j = 0; j < n; j++)
        for (int i = 0; i < m; i++)
        {
            MUL_AB(T, A->a[j], B->a[i]);
            printf("%d-th T = ", j * m + i);
            show64(T);

            T->wordlen = T->wordlen + (i + j);
            // printf("T wordlen: %d\n", T->wordlen);
            bi_lshift(&T, w * (i + j));
            printf("%d-th T = ", j * m + i);
            show64(T);

            for (int k = T->wordlen + 1; k < (*C)->wordlen; k++)
                (*C)->a[k] = 0;

            // (*C)->wordlen++;
            ADDC(C, *C, T);
            T->wordlen = 2;
            // printf("C wordlen: %d\n", T->wordlen);
            printf("%d-th C = ", j * m + i);
            show64(*C);
            printf("\n");

            // T 초기화
            for (int k = 0; k < n + m; k++)
                T->a[k] = 0;
        }
    // bi_refine(*C);
    bi_delete(&T);
    return;
}

int main()
{
    /*
            " 0x92feee1f99da5da57d9691e6ba9f322a4364df94942ad072c73609845c9b9a9ea5cea42b1a0cdf3a54270fe68a5fbd8fcbdb5f7703ae4eb5552e5962998d1af577797d7c4a26c0e9b7fe9f9ed6c4a401f58d5568dbc1fea166b7090e01e2 ",
            " 0xe4b74f164c1fab66acd8e097cd37cef5f64d534e2dbf76b5395629fe071fb832d3ae00b56986d18cd78ebcc47dc99d85b9a7560fb16f1b9c494a5a1009182e9fcd0c642f535416b771b71ba8a49004d2e67333b41288978f193581ee8c5c ",
            " 0xb0f0140b9d4d26e0ff217a384630184bc30449a3de69633010cf29f769e68f72e38c1754b312b8c439e333013479dce502a901051c43d475d930d4889c9936a5623a98f0d4eb5a3748cc3afb165478de21e9a0904e6a58a8f4aef676ec20 ",
            " 0xea7b2f79ccd41d849101a733fbbb4651d28a97c5d9e5a6709e736a9699c0d46640efb3a02df19656a1ac6829af096621281793bf67b88f802fa2c4df9229c534d71f88f3d20bca2b592b2dcf99d65fe7a336ec5868b569b3b8474e03463c ",
    */
    char *str1[4] = {
        "0xaaaaaaaaa2aaaa1242412222112434231423423142343243214321432432aaaaa", "0xaaaaffffa4352435524334552345342523aaaffff", "0x1234"};
    // char *str1[4] = {
    //     "0x3214321432432aaa", "0xaaaaffffa", "0x1234"};
    //    char *str1[4] = {
    //     "0xaaaaaaaaaaaaa12424112434231423423142343243214321432432aaa", "0xaaaaffffaaaaffff", "0x1234"};

    //*   scanf 사용할 때만 활성화하기 *//
    // char *str1 = NULL;
    // char *str2 = NULL;

    bigint *num1 = NULL;
    bigint *num2 = NULL;
    bigint *ret = NULL;

    //*   scanf 사용할 때만 활성화하기 *//
    //============================================================//
    // str1을 얼마나 메모리공간을 잡아야할지 모르겠음
    // str1 = (char *)malloc(64);
    // str2 = (char *)malloc(64);

    // 입력대상 A와 B를 입력받는 곳
    // scanf("%s", str1);
    // printf("%s\n", str1);

    // scanf("%s", str2);
    // printf("%s\n", str2);
    //============================================================//
    for (int i = 0; i < TEST; i++)
    {
        bi_new(&num1, get_wordlen(str1[2 * i]));
        bi_new(&num2, get_wordlen(str1[2 * i + 1]));

        // 결과값 ret의 워드 길이는 A와 B중 크기가 큰 워드 길이로 설정
        if (get_wordlen(str1[2 * i]) < get_wordlen(str1[2 * i + 1]))
            bi_new(&ret, get_wordlen(str1[2 * i + 1]));
        else
            bi_new(&ret, get_wordlen(str1[2 * i]));

        // 입력값의 부호 알아내기
        num1->sign = get_sign(str1[2 * i]);
        num2->sign = get_sign(str1[2 * i + 1]);

        // 입력값 워드로 바꿔주기
        str_to_64(num1->a, str1[2 * i]);
        str_to_64(num2->a, str1[2 * i + 1]);

        printf("A = ");
        show64(num1);
        printf("B = ");
        show64(num2);
        printf("C = ");
        show64(ret);
        printf("A의 워드길이: %d\n", num1->wordlen);

        // MUL_AB TEST
        // MUL_AB(ret, num1->a[0], num2->a[0]);
        // show64(ret);
        // MULC TEST
        MULC(&ret, num1, num2);
        show64(ret);

        bi_delete(&num1);
        bi_delete(&num2);
        bi_delete(&ret);

        // scanf 사용할 때만 활성화하기
        // free(str1);
        // free(str2);
        return 0;
    }
}
