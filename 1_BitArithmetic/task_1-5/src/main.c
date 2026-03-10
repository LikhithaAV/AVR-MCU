#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/io2313.h>
#include <avr/io90pwm161.h>

void print_binary(uint8_t var) {
    for (int i = 7; i >= 0; i--)
        printf("%d", (var >> i) & 1);
    printf("\n");
}

int main() {
    uint8_t var = 0b00000000; // Start with all bits cleared
    printf("Initial:       ");
    print_binary(var);

    // 1. Set bit 3
    var |= (1 << 3);
    printf("Set bit 3:     ");
    print_binary(var);

    // 2. Set bits 4 and 6
    var |= (1 << 4) | (1 << 6); // or var |= 0x50;
    printf("Set bits 4&6:  ");
    print_binary(var);

    // 3. Clear bit 2
    var &= ~(1 << 2);
    printf("Clear bit 2:   ");
    print_binary(var);

    // 4. Clear bits 2 and 7
    var &= ~((1 << 2) | (1 << 7));
    printf("Clear bits 2&7:");
    print_binary(var);

    // 5. Toggle (invert) bit 3
    var ^= (1 << 3);
    printf("Toggle bit 3:  ");
    print_binary(var);

    // 6. Set bit 2 and clear bits 5 and 7
    var = (var | (1 << 2)) & ~((1 << 5) | (1 << 7));
    printf("Set 2, clr 5&7:");
    print_binary(var);

    // 7. Swap bits 3–5 with bits 0–2
    var = (var & 0b11000000) | ((var & 0b00111000) >> 3) | ((var & 0b00000111) << 3);
    printf("Swap 3–5 & 0–2:");
    print_binary(var);

    return EXIT_SUCCESS;
}
