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

void str_to_bin(char *input, char *pt)
{
    char t[1];
    int count = 0; // input�� 16���� �� �ڸ��� binary 4�ڸ��� count�ϱ� ����(�迭�� ���)
    char *endptr = NULL;
    int len = strlen(input) - 2;
    // �� �Լ� �����ϱ� ���� �����Լ����� �̸� pt���� �Ҵ� char* pt = (char*)calloc( sizeof(char) , 4*len)
    for (int i = len - 1; i >= 0; i--)
    {
        t[0] = 0;                     // t �ʱ�ȭ
        strncpy(t, input + 2 + i, 1); // strlen(input) - 1 - (i/4)
        for (int j = 0; j <= 3; j++)
        {
            pt[count + j] = (strtoul(t, &endptr, 16) >> j) & 0x01; // ������������ pt�� ����
        }
        count += 4;
    }
}

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
            return;
        }
        strncpy(t, input + l, BASE_64);
        A[len - j - 1] = strtoull(t, &endptr, BASE_64);
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

void bi_assign(bigint **y, bigint *x)
{
    if (*y != NULL)
        bi_delete(y);
    bi_new(y, x->wordlen);
    (*y)->sign = x->sign;
    memcpy((*y)->a, x->a, x->wordlen * BASE_64);
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
        ADD_ABc(&(*C)->a[j], &c, A->a[j], B->a[j]);

    // case 1: next carry 발생 O -> C의 워드 1블록을 추가할당해야함.
    if (c == 1)
    {
        (*C)->wordlen = (*C)->wordlen + 1;
        (*C)->a[A->wordlen] = 0x1;
    }
    bi_refine(*C);
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

void left_shift(bigint *A, int r) // bigint A를 r비트만큼 왼쪽으로 shift하는 함수
{
    int i;
    if (r % w == 0) // r가 w의 배수인 경우
    {
        A->wordlen += r / w;
        A->a = (word *)realloc(A->a, sizeof(word) * get_wordlen(A));
        array_copy(A->a + (r / w), A->a, A->wordlen - (r / w));
        array_init(A->a, r / w);
    }
    else // r이 WORD_BITLEN의 배수가 아닌 경우
    {
        x->wordlen += r / WORD_BITLEN + 1;
        x->a = (word *)realloc(x->a, sizeof(word) * get_wordlen(x));

        int n = get_wordlen(x) - r / WORD_BITLEN; // n은 shift했을 때 0이 아닌 부분의 길이
        x->a[get_wordlen(x) - 1] = (x->a[get_wordlen(x) - 1 - r / WORD_BITLEN - 1] >> (WORD_BITLEN - r % WORD_BITLEN));
        for (i = get_wordlen(x) - 2; i > get_wordlen(x) - n; i--)
            x->a[i] = (x->a[i - r / WORD_BITLEN - 1] >> (WORD_BITLEN - r % WORD_BITLEN)) | (x->a[i - r / WORD_BITLEN] << (r % WORD_BITLEN)); // x->a[i]는 x->a[i-1]의 상위 r%WORD_BITLEN비트와 x->a[i]의 하위 WORD_BITLEN-r%WORD_BITLEN비트로 구성
        x->a[get_wordlen(x) - n] = x->a[get_wordlen(x) - n - r / WORD_BITLEN] << (r % WORD_BITLEN);
        array_init(x->a, r / WORD_BITLEN);
    }
    bi_refine(x);
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
    printf("n: %d, q: %d, 1\n", n, q);
    bi_new(&T, n + q + 1);
    printf("Tlen1: %d\n", T->wordlen);

    // case: x가 w의 배수인 경우 where q < n.
    if (x % w == 0)
    {
        T->wordlen = n + q + 2;
        printf("Tlen2: %d\n", T->wordlen);
        // 초기 설정
        for (int j = 0; j < n; j++)
            T->a[j + q] = (*A)->a[j];

        // 값 옮겨주기
        for (int j = q; j < n; j++)
            T->a[j] = (*A)->a[j - q];

        bi_assign(A, T);
        printf("Tlen3: %d\n", T->wordlen);
        // (*A)->wordlen = T->wordlen + 1;
        // printf("show: \n");
        // show64(*A);
        bi_delete(&T);
        bi_refine(*A);
        // printf("show: \n");
        // show64(*A);
        // printf("Alen %d\n", (*A)->wordlen);
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
    // 워드 단위: 64비트
    int w = 64;

    // n: A의 워드길이 , m: B의 워드길이
    int n = A->wordlen;
    int m = B->wordlen;

    // MUL_AB 연산 결과값 저장을 위해 새로운 bigint* 형 변수 셋팅
    bigint *T = NULL;
    bi_new(&T, 2);

    printf("n = %d\nm = %d\n", n, m);
    (*C)->wordlen = n + m;

    for (int j = 0; j < n; j++)
        for (int i = 0; i < m; i++)
        {
            MUL_AB(T, A->a[j], B->a[i]);

            // printf("%d-th T = ", j * m + i);
            // show64(T);
            printf("check: %d\n", w * (i + j));
            bi_lshift(&T, w * (i + j));

            printf("%d-th T = ", j * m + i);
            show64(T);

            (*C)->wordlen = (*C)->wordlen + T->wordlen;
            // (*C)->wordlen = n + m;
            ADDC(C, *C, T);

            T->wordlen = 2;

            // ADDC(C, *C, T) 후 C값 출력
            // printf("%d-th C = ", j * m + i);
            // show64(*C);
            // printf("\n");

            // T 초기화
            for (int k = 0; k < n + m; k++)
                T->a[k] = 0;
        }

    bi_refine(*C);
    bi_delete(&T);
    return;
}

int main()
{
    char *str1[4] = {
        "0x1111111111111111111111111111111111111111111111111111111111111111", "0x222222222222222222222222222222222222222222222222", "0x1234"};

    bigint *num1 = NULL;
    bigint *num2 = NULL;
    bigint *ret = NULL;

    for (int i = 0; i < TEST; i++)
    {
        bi_new(&num1, get_wordlen(str1[2 * i]));
        bi_new(&num2, get_wordlen(str1[2 * i + 1]));

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

        /* ===== ===== ===== ===== ===== MULC TEST ===== ===== ===== ===== ===== */
        bi_new(&ret, 10);
        MULC(&ret, num1, num2);
        show64(ret);
        /* ===== ===== ===== ===== ===== MULC TEST ===== ===== ===== ===== ===== */

        bi_delete(&num1);
        bi_delete(&num2);
        bi_delete(&ret);

        return 0;
    }
}

/**
 * @void left_shift(bigint* x, int r) // bigint x를 r비트만큼 왼쪽으로 shift하는 함수
{
    int i;
    if (r % WORD_BITLEN == 0) // r가 WORD_BITLEN의 배수인 경우
    {
        x->wordlen += r/WORD_BITLEN;
        x->a = (word*)realloc(x->a, sizeof(word) * get_wordlen(x));
        array_copy(x->a + (r / WORD_BITLEN), x->a, x->wordlen - (r / WORD_BITLEN));
        array_init(x->a, r / WORD_BITLEN);
    }
    else // r이 WORD_BITLEN의 배수가 아닌 경우
    {
        x->wordlen += r / WORD_BITLEN + 1;
        x->a = (word*)realloc(x->a, sizeof(word) * get_wordlen(x));

        int n = get_wordlen(x) - r / WORD_BITLEN; // n은 shift했을 때 0이 아닌 부분의 길이
        x->a[get_wordlen(x)-1] = (x->a[get_wordlen(x) - 1 - r / WORD_BITLEN - 1] >> (WORD_BITLEN - r % WORD_BITLEN));
        for (i = get_wordlen(x) - 2; i > get_wordlen(x) - n; i--)
            x->a[i] = (x->a[i - r/WORD_BITLEN - 1] >> (WORD_BITLEN - r % WORD_BITLEN)) | (x->a[i - r / WORD_BITLEN] << (r % WORD_BITLEN)); // x->a[i]는 x->a[i-1]의 상위 r%WORD_BITLEN비트와 x->a[i]의 하위 WORD_BITLEN-r%WORD_BITLEN비트로 구성
        x->a[get_wordlen(x) - n] = x->a[get_wordlen(x)- n - r / WORD_BITLEN] << (r%WORD_BITLEN);
        array_init(x->a, r / WORD_BITLEN);
    }
    bi_refine(x);
}

void right_shift(bigint* x, int r) // bigint x를 r비트만큼 오른쪽으로 shift하는 함수
{
    if (r >= get_wordlen(x) * WORD_BITLEN) // r이 x->wordlen*WORD_BITLEN보다 크거나 같은 경우
        bi_zeroize(x);
    else if (r % WORD_BITLEN == 0) // r이 WORD_BITLEN의 배수인 경우
    {
        array_copy(x->a, x->a + (r / WORD_BITLEN), get_wordlen(x) - (r / WORD_BITLEN));
        array_init(x->a + (get_wordlen(x) - (r / WORD_BITLEN)), r / WORD_BITLEN);
        bi_refine(x);
    }
    else // r이 WORD_BITLEN의 배수가 아닌 경우
    {
        int i;
        int n = get_wordlen(x) - r / WORD_BITLEN; // n은 shift했을 때 0이 아닌 부분의 길이
        for (i = 0; i < n - 1; i++)
            x->a[i] = (x->a[i + 1 + r/WORD_BITLEN] << (WORD_BITLEN - r % WORD_BITLEN)) | (x->a[i + r / WORD_BITLEN] >> (r % WORD_BITLEN)); // x->a[i]는 x->a[i+1]의 하위 r%WORD_BITLEN비트와 x->a[i]의 상위 WORD_BITLEN-r%WORD_BITLEN비트로 구성
        x->a[n - 1] = x->a[n - 1 + r / WORD_BITLEN] >> (r % WORD_BITLEN);
        array_init(x->a + (get_wordlen(x) - r / WORD_BITLEN), r / WORD_BITLEN);
        bi_refine(x);
    }
}

void reduction_2_r(bigint* x, int r) // bigint x의 x mod 2^r를 출력하는 함수
{
    if (r >= (get_wordlen(x) * WORD_BITLEN)) // r가 x->wordlen*WORD_BITLEN보다 크거나 같은 경우
        return;
    else if (r % WORD_BITLEN == 0) // r가 WORD_BITLEN의 배수인 경우
    {
        array_init(x->a + r / WORD_BITLEN, x->wordlen - r / WORD_BITLEN);
        bi_refine(x);
    }
    else    // r가 WORD_BITLEN의 배수가 아닌 경우
    {
        word k = BITMASK;
         // k = 2^(WORD_BITLEN)-1
        x->a[r / WORD_BITLEN] &= (k >> (WORD_BITLEN - r % WORD_BITLEN)); // x->a[r/WORD_BITLEN]의 하위 r%WORD_BITLEN비트만 남김
        array_init(x->a + r / WORD_BITLEN + 1, x->wordlen - r / WORD_BITLEN - 1);
        bi_refine(x);
    }
}
 *
 */