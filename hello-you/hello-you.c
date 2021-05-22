#include <stdio.h>

int main() {
    char name[100];

    printf("What's your name?\n");
    scanf_s("%s", name);
    printf("Hello, %s!", name);
}
