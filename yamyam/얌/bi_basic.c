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

    printf("문자열 길이: %d\n", l);
    printf("word 길이: %d\n", len);

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
    // x가 가리키는 값 1로 할당.
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
            break;
        new_wordlen--;
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