#include <stdio.h>

void showArrayElementAssignmentChangesValueAtAddress() {
    int arr[] = { 1, 2 };
    int *p0 = &arr[0];

    printf("BEFORE:\n");
    printf("Pointer address: %i\n", p0);
    printf("Pointer value: %i\n", *p0);
    printf("Array 0: %i\n", arr[0]);

    arr[0] = 10;
    printf("\nAFTER:\n");
    printf("Pointer address: %i\n", p0);
    printf("Pointer value: %i\n", *p0);
    printf("Array 0: %i\n", arr[0]);
}

int main() {
    showArrayElementAssignmentChangesValueAtAddress();
    return 0;
}