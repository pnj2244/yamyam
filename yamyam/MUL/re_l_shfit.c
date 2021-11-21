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

    printf("%llx ", A->a[A->wordlen - 1]);
    for (int j = A->wordlen - 2; j >= 0; j--)
        printf("%016llx ", A->a[j]);
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

// 워드길이 체크 함수내에서
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

    // case: x가 w의 배수인 경우 where q < n.
    if (x % w == 0)
    {
        printf("T의 길이를 알아보자:%d\n", T->wordlen);
        // 초기 설정
        for (int j = 0; j < n; j++)
            T->a[j + q] = (*A)->a[j];

        // 값 옮겨주기
        for (int j = q; j < n; j++)
            T->a[j] = (*A)->a[j - q];

        bi_assign(A, T);
        printf("T wordlen: %d\n", T->wordlen);
        // (*A)->wordlen = n + q;
        printf("AA wordlen: %d\n", (*A)->wordlen);

        bi_delete(&T);
        bi_refine(*A);
        printf("AA wordlen: %d\n", (*A)->wordlen);

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
            printf("====== %d-th start ======\n", j * m + i);

            MUL_AB(T, A->a[j], B->a[i]);
            printf("%d-th T = ", j * m + i);
            show64(T);
            printf("제발: %d\n", T->wordlen);
            // T->wordlen = T->wordlen + (i + j);
            // printf("T wordlen: %d\n", T->wordlen);
            // 체크
            bi_lshift(&T, w * (i + j));

            printf("%d-th T = ", j * m + i);
            show64(T);
            printf("제에발: %d\n", T->wordlen);

            ADDC(C, *C, T);
            T->wordlen = 2;
            printf("C wordlen: %d\n", T->wordlen);
            printf("%d-th C = ", j * m + i);
            show64(*C);

            // T 초기화
            for (int k = 0; k < n + m; k++)
                T->a[k] = 0;
        }

    bi_delete(&T);
    return;
}
// "0xaaaaaaaaaaaaa12424112434231214323423423142343243214321432432aaa"
int main()
{
    char *str1[4] = {
        "011111111111111111111123412312234131111111111111111111111111111",
        "0x111111111111111111111111111111111111111111111111111111111111111", "0xaaa523aaaffff", "0x1234"};

    bigint *num1 = NULL;
    bigint *num2 = NULL;
    bigint *ret = NULL;

    for (int i = 0; i < TEST; i++)
    {
        printf("str1 wordlen: %d\n", get_wordlen(str1[2 * i]));
        printf("str2 wordlen: %d\n", get_wordlen(str1[2 * i + 1]));

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
        // MULC(&ret, num1, num2);
        // show64(ret);

        // LEFT SHIFT TEST
        // bi_lshift(&num1, 192);
        // show64(num1);
        // printf("A wordlen: %d\n", num1->wordlen);

        bi_delete(&num1);
        bi_delete(&num2);
        bi_delete(&ret);

        return 0;
    }
}
