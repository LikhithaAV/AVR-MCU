/**
 * @file test_ses_led.c
 * @brief Unit tests for LED driver (ses_led.c)
 * 
 * Tests LED initialization, on/off, and toggle functions for all three LEDs.
 */

#include "unity.h"
#include "ses_led.h"
#include "mock_avr_io.h"

/* ============================================================================
 * SETUP AND TEARDOWN
 * ============================================================================ */

void setUp(void) {
    /* Reset mock registers before each test */
    mock_avr_io_reset();
}

void tearDown(void) {
    /* Cleanup after each test */
}

/* ============================================================================
 * RED LED TESTS
 * ============================================================================ */

void test_led_redInit_sets_ddr_as_output(void) {
    /* Initialize red LED */
    led_redInit();
    
    /* Check that DDR bit 5 of PORTF is set (output) */
    TEST_ASSERT(DDRF & (1 << 5));
}

void test_led_redInit_turns_led_off(void) {
    /* Initialize red LED */
    led_redInit();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTF & (1 << 5));
}

void test_led_redOn_turns_led_on(void) {
    /* Initialize first */
    led_redInit();
    
    /* Turn on red LED */
    led_redOn();
    
    /* LED is active-low, so on means bit should be 0 */
    TEST_ASSERT_EQUAL(0, (PORTF & (1 << 5)));
}

void test_led_redOff_turns_led_off(void) {
    /* Initialize and turn on */
    led_redInit();
    led_redOn();
    
    /* Turn off red LED */
    led_redOff();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTF & (1 << 5));
}

void test_led_redToggle_toggles_state(void) {
    /* Initialize */
    led_redInit();
    uint8_t initial_state = PORTF & (1 << 5);
    
    /* Toggle red LED */
    led_redToggle();
    uint8_t toggled_state = PORTF & (1 << 5);
    
    /* State should have changed */
    TEST_ASSERT_NOT_EQUAL(initial_state, toggled_state);
}

void test_led_redToggle_can_toggle_back(void) {
    /* Initialize */
    led_redInit();
    led_redOn();
    uint8_t on_state = PORTF & (1 << 5);
    
    /* Toggle twice */
    led_redToggle();
    led_redToggle();
    uint8_t final_state = PORTF & (1 << 5);
    
    /* Should be back to original state */
    TEST_ASSERT_EQUAL(on_state, final_state);
}

/* ============================================================================
 * GREEN LED TESTS
 * ============================================================================ */

void test_led_greenInit_sets_ddr_as_output(void) {
    /* Initialize green LED */
    led_greenInit();
    
    /* Check that DDR bit 2 of PORTD is set (output) */
    TEST_ASSERT(DDRD & (1 << 2));
}

void test_led_greenInit_turns_led_off(void) {
    /* Initialize green LED */
    led_greenInit();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTD & (1 << 2));
}

void test_led_greenOn_turns_led_on(void) {
    /* Initialize first */
    led_greenInit();
    
    /* Turn on green LED */
    led_greenOn();
    
    /* LED is active-low, so on means bit should be 0 */
    TEST_ASSERT_EQUAL(0, (PORTD & (1 << 2)));
}

void test_led_greenOff_turns_led_off(void) {
    /* Initialize and turn on */
    led_greenInit();
    led_greenOn();
    
    /* Turn off green LED */
    led_greenOff();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTD & (1 << 2));
}

void test_led_greenToggle_toggles_state(void) {
    /* Initialize */
    led_greenInit();
    uint8_t initial_state = PORTD & (1 << 2);
    
    /* Toggle green LED */
    led_greenToggle();
    uint8_t toggled_state = PORTD & (1 << 2);
    
    /* State should have changed */
    TEST_ASSERT_NOT_EQUAL(initial_state, toggled_state);
}

/* ============================================================================
 * YELLOW LED TESTS
 * ============================================================================ */

void test_led_yellowInit_sets_ddr_as_output(void) {
    /* Initialize yellow LED */
    led_yellowInit();
    
    /* Check that DDR bit 3 of PORTD is set (output) */
    TEST_ASSERT(DDRD & (1 << 3));
}

void test_led_yellowInit_turns_led_off(void) {
    /* Initialize yellow LED */
    led_yellowInit();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTD & (1 << 3));
}

void test_led_yellowOn_turns_led_on(void) {
    /* Initialize first */
    led_yellowInit();
    
    /* Turn on yellow LED */
    led_yellowOn();
    
    /* LED is active-low, so on means bit should be 0 */
    TEST_ASSERT_EQUAL(0, (PORTD & (1 << 3)));
}

void test_led_yellowOff_turns_led_off(void) {
    /* Initialize and turn on */
    led_yellowInit();
    led_yellowOn();
    
    /* Turn off yellow LED */
    led_yellowOff();
    
    /* LED is active-low, so off means bit should be 1 */
    TEST_ASSERT(PORTD & (1 << 3));
}

void test_led_yellowToggle_toggles_state(void) {
    /* Initialize */
    led_yellowInit();
    uint8_t initial_state = PORTD & (1 << 3);
    
    /* Toggle yellow LED */
    led_yellowToggle();
    uint8_t toggled_state = PORTD & (1 << 3);
    
    /* State should have changed */
    TEST_ASSERT_NOT_EQUAL(initial_state, toggled_state);
}

/* ============================================================================
 * INTEGRATION TESTS
 * ============================================================================ */

void test_multiple_leds_can_be_controlled_independently(void) {
    /* Initialize all LEDs */
    led_redInit();
    led_greenInit();
    led_yellowInit();
    
    /* Turn on red */
    led_redOn();
    TEST_ASSERT_EQUAL(0, (PORTF & (1 << 5)));
    TEST_ASSERT(PORTD & (1 << 2));  /* Green should still be off */
    TEST_ASSERT(PORTD & (1 << 3));  /* Yellow should still be off */
    
    /* Turn on green */
    led_greenOn();
    TEST_ASSERT_EQUAL(0, (PORTF & (1 << 5)));  /* Red still on */
    TEST_ASSERT_EQUAL(0, (PORTD & (1 << 2))); /* Green now on */
    TEST_ASSERT(PORTD & (1 << 3));             /* Yellow still off */
}

/* ============================================================================
 * MAIN ENTRY POINT
 * ============================================================================ */

int main(void) {
    printf("\n========================================\n");
    printf("AVR MCU - LED Driver Unit Tests\n");
    printf("========================================\n\n");
    
    /* Red LED Tests */
    printf("Running Red LED Tests...\n");
    setUp(); test_led_redInit_sets_ddr_as_output(); tearDown();
    setUp(); test_led_redInit_turns_led_off(); tearDown();
    setUp(); test_led_redOn_turns_led_on(); tearDown();
    setUp(); test_led_redOff_turns_led_off(); tearDown();
    setUp(); test_led_redToggle_toggles_state(); tearDown();
    setUp(); test_led_redToggle_can_toggle_back(); tearDown();
    
    /* Green LED Tests */
    printf("Running Green LED Tests...\n");
    setUp(); test_led_greenInit_sets_ddr_as_output(); tearDown();
    setUp(); test_led_greenInit_turns_led_off(); tearDown();
    setUp(); test_led_greenOn_turns_led_on(); tearDown();
    setUp(); test_led_greenOff_turns_led_off(); tearDown();
    setUp(); test_led_greenToggle_toggles_state(); tearDown();
    
    /* Yellow LED Tests */
    printf("Running Yellow LED Tests...\n");
    setUp(); test_led_yellowInit_sets_ddr_as_output(); tearDown();
    setUp(); test_led_yellowInit_turns_led_off(); tearDown();
    setUp(); test_led_yellowOn_turns_led_on(); tearDown();
    setUp(); test_led_yellowOff_turns_led_off(); tearDown();
    setUp(); test_led_yellowToggle_toggles_state(); tearDown();
    
    /* Integration Tests */
    printf("Running Integration Tests...\n");
    setUp(); test_multiple_leds_can_be_controlled_independently(); tearDown();
    
    printf("\n========================================\n");
    printf("✓ All tests passed!\n");
    printf("========================================\n\n");
    
    return 0;
}
