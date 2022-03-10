//
// Created by lpyyxy on 2022/3/7.
//

#include "main.h"

int main(void)
{
    char *a="asdf7";
    char *b="zxcc";
    free(a);
    printf("%d %d ",a,b);
}

typedef struct Test1
{
    char a[10];

    int b;

    unsigned long long c;

    unsigned int d_size;
    void *d;

    struct Test2 *e;
}Test1;

typedef struct Test2
{
    long long f;

    struct Test3 *g;
}Test2;

typedef struct Test3
{
    double h;
}Test3;