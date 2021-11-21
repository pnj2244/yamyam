#include "bi_arithmetic.h"

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
