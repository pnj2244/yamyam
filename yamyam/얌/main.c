// #include <stdio.h>
// #include <stdlib.h>
#include "bi_basic.h"
#include "bi_arithmetic.h"

int main()
{
    // char *str1 = "0x123456789abcdef0123456789abcdef0123456789";
    // char *str2 = "0x111122223333444425556666777788883999aaaabbbbcccc4dddeeeeffffabcd";
    // char *str = "0x214c1fe1e0164c89ed64d91ca985c62759cb056cf9e3062136b0f6e5ce14fb241617f442e71349560e";
    // char *str = "0x14c1fe1e016cb056cf9ecf9e30621164c8930621164c89ed64d9ca985c62759cb056cf9e306216cb056cf9ecf9ec1fe1e016cb056cf9ecf9e30621164c8930621164c89ed64d9ca985c62759cb056cf9e306216cb056cf9ecf9e30621164c8930621164c8c1fe1e016cb056cf9ecf9e30621164c8930621164c89ed64d9ca985c62759cb056cf9e306216cb056cf9ecf9ec1fe1e016cb056cf9ecf9e30621164c8930621164c89ed64d9ca985c62759cb056cf9e306216cb056cf9ecf9e30621164c8930621164c89ed64d9ca985c62759cb056cf9e3062136b0f6e5ce14fb241617f442e71349560e";
    // char *str = 0x111122223333444425556666777788883999aaaabbbbcccc4dddeeeeffffabcd

    char *str1 = NULL;
    char *str2 = NULL;

    bigint *num1 = NULL;
    bigint *num2 = NULL;
    bigint *ret = NULL;

    // str1, 2을 얼마나 메모리공간을 잡아야할지 모르겠음
    str1 = (char *)malloc(64);
    str2 = (char *)malloc(64);

    // 입력 대상 A와 B를 입력받는 곳
    printf("A를 입력하세요(16진수 입력 전용 앞에 0x를 붙여주세요.)\n ");
    scanf("%s", str1);
    printf("%s\n", str1);

    printf("B를 입력하세요(16진수 입력 전용 앞에 0x를 붙여주세요.)\n ");
    scanf("%s", str2);
    printf("%s\n", str2);

    // 입력값의 워드길이 알아내기
    // get_wordlen(str1);
    // get_wordlen(str2);

    // 입력 워드길이만큼 동적할당.
    // bi_new(&num1, 1000); // get_wordlen(str1));
    // bi_new(&num2, 1000); // get_wordlen(str2));
    // bi_new(&ret, 1000);  // get_wordlen(str1));

    bi_new(&num1, get_wordlen(str1));
    bi_new(&num2, get_wordlen(str2));

    // 결과값 ret의 워드 길이는 A와 B중 크기가 큰 워드 길이로 설정
    if (get_wordlen(str1) < get_wordlen(str2))
        bi_new(&ret, get_wordlen(str2));
    else
        bi_new(&ret, get_wordlen(str1));

    // printf("str 1 워드길이: %d\n", get_wordlen(str1));
    // printf("str 2 워드길이: %d\n", get_wordlen(str2));

    // 입력값의 부호 알아내기
    num1->sign = get_sign(str1);
    num2->sign = get_sign(str2);
    // printf("num1의 부호: %d\nnum2의 부호: %d\n", num1->sign, num2->sign);
    // printf("워드 길이는 각각 %d, %d입니다.\n", num1->wordlen, num2->wordlen);

    // 입력값 워드로 바꿔주기
    str_to_64(num1->a, str1);
    str_to_64(num2->a, str2);

/* ===== ===== ===== ===== ===== ADD 테스트 ===== ===== ===== ===== =====*/
#if 0
    if (compare(num1, num2) == -1)
        ADD(&ret, num1, num2);
    else
        ADD(&ret, num2, num1);
#endif
/* ===== ===== ===== ===== ===== ADD 테스트 ===== ===== ===== ===== =====*/

/* ===== ===== ===== ===== ===== SUB 테스트 ===== ===== ===== ===== =====*/
#if 0
    SUB(&ret, num1, num2);
#endif
    /* ===== ===== ===== ===== ===== SUB 테스트 ===== ===== ===== ===== =====*/

    printf("A = ");
    show64(num1);
    printf("B = ");
    show64(num2);
    // printf("C = ");
    // show64(ret);

    /* BIGINTEGER LEFT SHIFT TEST */
    bi_new(&ret, get_wordlen(str1));

    /* BIGINTEGER LEFT SHIFT TEST */

/* ===== ===== ===== ===== ===== MUL 테스트 ===== ===== ===== ===== =====*/
#if 1
    MULC(&ret, num1, num2);
    show64(ret);
#endif

    bi_delete(&num1);
    bi_delete(&num2);
    bi_delete(&ret);

    free(str1);
    free(str2);

    return 0;
}