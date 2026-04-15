[23bcs186@mepcolinux ex8]$cat add.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int n, i;

    printf("Enter the size of the arrays: ");
    scanf("%d", &n);

    int *a = (int*)malloc(n * sizeof(int));
    int *b = (int*)malloc(n * sizeof(int));
    int *c = (int*)malloc(n * sizeof(int));

    printf("Enter elements of Array A:\n");
    for(i = 0; i < n; i++)
        scanf("%d", &a[i]);

    printf("Enter elements of Array B:\n");
    for(i = 0; i < n; i++)
        scanf("%d", &b[i]);

    #pragma omp parallel for schedule(static)
    for(i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
        printf("Thread ID %d processed index %d\n", omp_get_thread_num(), i);
    }

    printf("\nResultant Array C:\n");
    for(i = 0; i < n; i++)
        printf("%d ", c[i]);
    printf("\n");

    free(a);
    free(b);
    free(c);

    return 0;
}

[23bcs186@mepcolinux ex8]$./add
Enter the size of the arrays: 4
Enter elements of Array A:
12 34 21 45
Enter elements of Array B:
43 54 67 88
Thread ID 2 processed index 2
Thread ID 0 processed index 0
Thread ID 3 processed index 3
Thread ID 1 processed index 1

Resultant Array C:
55 88 88 133 
[23bcs186@mepcolinux ex8]$cat const.c 
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int n, i, constant;

    printf("Enter the size of the array: ");
    scanf("%d", &n);

    int *a = (int*)malloc(n * sizeof(int));

    printf("Enter elements of the array:\n");
    for(i = 0; i < n; i++)
        scanf("%d", &a[i]);

    printf("Enter the constant value to add: ");
    scanf("%d", &constant);

    #pragma omp parallel for schedule(dynamic, 2)
    for(i = 0; i < n; i++) {
        a[i] = a[i] + constant;
        printf("Thread ID %d processed index %d\n", omp_get_thread_num(), i);
    }

    printf("\nUpdated Array:\n");
    for(i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");

    free(a);

    return 0;
}

[23bcs186@mepcolinux ex8]$./const 
Enter the size of the array: 4
Enter elements of the array:
12 32 44 55
Enter the constant value to add: 10
Thread ID 6 processed index 2
Thread ID 6 processed index 3
Thread ID 3 processed index 0
Thread ID 3 processed index 1

Updated Array:
22 42 54 65 
