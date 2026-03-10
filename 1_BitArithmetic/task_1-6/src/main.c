#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Function to reverse bits in a uint8_t
uint8_t reverse_bits(uint8_t n) {
    n = (n >> 4) | (n << 4);
    n = ((n & 0xCC) >> 2) | ((n & 0x33) << 2);
    n = ((n & 0xAA) >> 1) | ((n & 0x55) << 1);
    return n;
}

// Function to print an 8-bit number in binary
void print_binary(uint8_t n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
}

int main() {
    int input;
    printf("Enter an 8-bit number (0–255): ");
    scanf("%d", &input);

    if (input < 0 || input > 255) {
        printf("Invalid input! Must be between 0 and 255.\n");
        return 1;
    }

    uint8_t var = (uint8_t)input;
    uint8_t reversed = reverse_bits(var);

    printf("Original: ");
    print_binary(var);
    printf(" (Decimal: %d)\n", var);

    printf("Reversed: ");
    print_binary(reversed);
    printf(" (Decimal: %d)\n", reversed);

    return EXIT_SUCCESS;
}
