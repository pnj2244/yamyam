// 265line assign ( A , t)가 안됩니당
#define _CRT_SECURE_NO_WARNINGS
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
    word* a;     // big integer의 주소.
} bigint;

// 입력 문자열을 64비트 워드로 변환해주는 함수, str_to_64(결과값, 입력스트링, 입력스트링의 워드단위길이)
void str_to_64(word* A, char* input)
{
    char t[16];
    char* endptr = NULL;
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
int get_wordlen(char* input)
{
    return (int)(ceil((float)(strlen(input) - 2) / (float)BASE_64));
}
int get_sign(char* input)
{
    // case: 입력 문자열이 음수일 경우
    if (input[0] == 0x2d)
        return NEGATIVE;

    return NON_NEGATIVE;
}

void show64(bigint* A)
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

void bi_delete(bigint** x)
{
    if (*x == NULL)
        return;

#ifdef ZERORIZE
    array_init((*x)->a, (*x)->wordlen));
#endif
    free((*x)->a);
    free(*x);
    *x = NULL;
}

void bi_new(bigint** x, int wordlen)
{
    if (*x != NULL)
        bi_delete(x);

    // x가 가리키는 값에 bigint byte만큼 동적 할당.
    *x = (bigint*)malloc(sizeof(bigint));
    // x가 가리키는 값에 1로 할당.
    (*x)->sign = NON_NEGATIVE;
    // x가 가리키는 값에 입력받은 wordlen 할당.
    (*x)->wordlen = wordlen;

    (*x)->a = (word*)calloc(wordlen, sizeof(word));
}

void bi_refine(bigint* x)
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
        x->a = (word*)realloc(x->a, sizeof(word) * new_wordlen);
    }

    if ((x->wordlen == 1) && (x->a[0] == 0x0))
        x->sign = NON_NEGATIVE;
}

void bi_assign(bigint** y, bigint* x)
{
    if (*y != NULL)
        bi_delete(y);
    bi_new(y, x->wordlen);
    (*y)->sign = x->sign;
    memcpy((*y)->a, x->a, x->wordlen); // *BASE_64);  //16을 곱한이유?
}

// ADD_ABc(출력 워드, 입력 carry, 입력워드 A, 입력워드 B)
void ADD_ABc(word* C, int* carry, word A, word B)
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
void ADDC(bigint** C, bigint* A, bigint* B)
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
        (*C)->a = (word*)realloc((*C)->a, sizeof(word) * ((*C)->wordlen+1));
        (*C)->a[A->wordlen] = 0x1;
    }
    bi_refine(*C);
}

void bi_rshift(bigint** A, int x)
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

void bi_lshift(bigint** A, int x)  // word가 정해졌을 때 x만큼 왼쪽으로 이동
{
    printf("1 : %llx\n", (*A)->a[0]);
    printf("2 : %llx\n", (*A)->a[1]);
    bigint* T = NULL;
    printf("A wordlen %d \n", (*A)->wordlen);
    for (int i = ((*A)->wordlen) - 1; i >= 0; i--)
        printf("%016llx", (*A)->a[i]);
    printf("::::sdfsdf\n");
    if (x == 0)
    {
        bi_delete(&T);
        bi_refine(*A);
        return;
    }
    else
    {
        bi_new(&T, ((*A)->wordlen) + x);
        for (int i = 0; i < x; i++)
            T->a[i] = 0;
        for (int j = x; j < x + ((*A)->wordlen); j++)
            T->a[j] = (*A)->a[j - x];
        
        /*for (int i = ((*A)->wordlen) + x - 1; i >= 0; i--)
            printf("%016llx", T->a[i]);
            */
        printf("%d\n", T->wordlen);
        T->wordlen = (*A)->wordlen + x;
        printf("%d\n", T->wordlen);z
        bi_assign(A, T);
        /*for (int i = ((*A)->wordlen) + x - 1; i >= 0; i--)
            printf("%016llx", (*A)->a[i]);
            */
        printf("Awordlen : %d\n", (*A)->wordlen);
        bi_delete(&T);
        bi_refine(*A);
        return;
    }
    
}
/*
void bi_lshift(bigint** A, int x)
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
    bigint* T = NULL;
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
*/
void MUL_AB(bigint* C, word A, word B)
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

    bi_new(&C, 2);
    C->a[0] = C0;
    C->a[1] = C1;
    show64(C);
    // C워드길이
    // C->wordlen++;
    bi_refine(C);       

}
void MULC(bigint** C, bigint* A, bigint* B)
{
    int w = 64;
    // n: A의 워드길이 , m: B의 워드길이
    int n = A->wordlen;
    int m = B->wordlen;

    bigint* T = NULL;
    bi_new(&T, n + m);  

    printf("n = %d\nm = %d\n", n, m);
    (*C)->wordlen = n + m;
    printf("C wordlen: %d\n", T->wordlen);

    for (int j = 0; j < n; j++)
        for (int i = 0; i < m; i++)
        {
            MUL_AB(T, A->a[j], B->a[i]);
            printf("%d-th T = ", j * m + i);
            show64(T);
            printf("\n");
            

            //T->wordlen = T->wordlen + (i + j);
            // printf("T wordlen: %d\n", T->wordlen);
            bi_lshift(&T, i + j);
            printf("lshift 출력합니다.\n");
            show64(T);
           
            
            // (*C)->wordlen++;
            ADDC(C, T, *C);
            T->wordlen = 2;
            //printf("C wordlen: %d\n", T->wordlen);
            printf("%d-th C = ", j * m + i);
            show64(*C);
/*
            // T 초기화                 T가 알아서 초기화 될것같다.
            for (int k = 0; k < n + m; k++)
                T->a[k] = 0;
  */      }
    // bi_refine(*C);
    bi_delete(&T);
    return;
}
/*

void MULC(bigint** C, bigint* A, bigint* B)
{
    int w = 64;
    // n: A의 워드길이 , m: B의 워드길이
    int n = A->wordlen;
    int m = B->wordlen;

    bigint* T = NULL;
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
            printf("C wordlen: %d\n", T->wordlen);
            printf("%d-th C = ", j * m + i);
            show64(*C);

            // T 초기화
            for (int k = 0; k < n + m; k++)
                T->a[k] = 0;
        }
    // bi_refine(*C);
    bi_delete(&T);
    return;
}
*/
int main()
{

    char* str1[4] = {
        "0xaaaaaaaaa2aaaa1242412222112434231423423142343243214321432432aaaaa", "0xaaaaffffa4352435524334552345342523aaaffff", "0x1234" };

    bigint* num1 = NULL;
    bigint* num2 = NULL;
    bigint* ret = NULL;

    for (int i = 0; i < TEST; i++)
    {
        bi_new(&num1, get_wordlen(str1[2 * i]));
        bi_new(&num2, get_wordlen(str1[2 * i + 1]));

        bi_new(&ret, get_wordlen(str1[2 * i]) + get_wordlen(str1[2 * i + 1]));
        /*
        // 결과값 ret의 워드 길이는 A와 B중 크기가 큰 워드 길이로 설정
        if (get_wordlen(str1[2 * i]) < get_wordlen(str1[2 * i + 1]))
            bi_new(&ret, get_wordlen(str1[2 * i + 1]));
        else
            bi_new(&ret, get_wordlen(str1[2 * i]));
            */
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
        //printf("C = ");
        //show64(ret);
        //printf("A의 워드길이: %d\n", num1->wordlen);

        // MUL_AB TEST
        // MUL_AB(ret, num1->a[0], num2->a[0]);
        // show64(ret);
        // MULC TEST
        MULC(&ret, num1, num2);
        show64(ret);

        bi_delete(&num1);
        bi_delete(&num2);
        bi_delete(&ret);

        return 0;
    }
}
