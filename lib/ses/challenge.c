#include <avr/interrupt.h>   
#include <stdint.h>          
/**
 * This is a so-called "naked" interrupt, and thus the compiler
 * creates no additional assembler code when entering/ leaving
 * the function. Note that a "reti" (return from interrupt)
 * becomes necessary.
 *
 */
ISR(TIMER0_COMPA_vect, ISR_NAKED) {
    asm volatile (
        "reti \n\t"
    );
}

//------------------------------

/**
 * In preemptive multi-tasking, each task needs its own stack.
 * This is the size of each stack (in bytes)
 */
#define TASK_STACK_SIZE  256


//------------------------------


/**
 * PCB (stack, stack pointer, next task)
 */
typedef struct context_s {
	uint8_t stack[TASK_STACK_SIZE];
	uint8_t * pstack;
	struct context_s * next;
} context_t;

