#include <stdio.h>
#include <math.h>

int add_together(int x, int y) {
    int result = x + y;
    return result;
}

void hello_world(int n) {
    for (; n > 0; n--) {
        puts("Hello World!");
    }
    return;
}

typedef struct {
    float x;
    float y;
} point;

int main(int argc, char** argv) {
    int added = add_together(10, 10);
    printf("Added: %d.\n", added);

    point p;
    p.x = 0.1;
    p.y = 10.0;

    float length = sqrt(p.x * p.x + p.y * p.y);
    printf("Length: %f\n", length);

    // char** - pointer to pointers to characters
    // int* - pointer to an integer
    // char* - C string terminated by null

    int x = 15;
    if (x > 10 && x < 100) {
        puts("10 < x < 100");
    } else {
        puts("x < 11 || x > 99");
    }

    int i = 10;
    while (i > 0) {
        puts("Loop iteration");
        i--;
    }

    for (int i = 0; i < 10; i++) {
        puts("Loop Iteration");
    }

    hello_world(7);

    return 0;
}
