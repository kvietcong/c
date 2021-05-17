#include <stdio.h>

int fib(int n) {
    int prev2 = 0;
    int prev1 = 1;
    int result = 1;

    for (int i = 0; i < n - 1; i++) {
        result = prev1 + prev2;
        prev2 = prev1;
        prev1 = result;
    }

    return result;
}

int main() {
    printf("The 10th Fibonacci number is %d", fib(10));
}
