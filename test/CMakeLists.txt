cmake_minimum_required(VERSION 3.15)
project(AVR_MCU_Tests C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# ============================================================================
# UNITY FRAMEWORK SETUP
# ============================================================================
set(UNITY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/unity)
add_library(unity
    ${UNITY_DIR}/unity.c
)
target_include_directories(unity PUBLIC ${UNITY_DIR})

# ============================================================================
# COMPILER FLAGS
# ============================================================================
# Enable coverage reporting (gcov for gcc)
add_compile_options(-g -O0 --coverage -fprofile-arcs -ftest-coverage)
add_link_options(--coverage)

# ============================================================================
# DRIVER SOURCE FILES (Exclude main.c)
# ============================================================================
set(DRIVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_fan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_fanspeed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_scheduler.c
)

# ============================================================================
# MOCK FILES (Hardware mocks)
# ============================================================================
set(MOCK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/support/mock_avr_io.c
)

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses
    ${CMAKE_CURRENT_SOURCE_DIR}/support
    ${UNITY_DIR}
)

# ============================================================================
# TEST EXECUTABLES
# ============================================================================

# Test LED Driver
add_executable(test_ses_led
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/test_ses_led.c
    ${DRIVER_SOURCES}
    ${MOCK_SOURCES}
)
target_link_libraries(test_ses_led unity)
add_test(NAME TestLED COMMAND test_ses_led)

# ============================================================================
# CUSTOM TARGETS
# ============================================================================

# Run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_ses_led
    COMMENT "Running all tests..."
)

# Coverage report
add_custom_target(coverage
    COMMAND echo "Generating coverage report..."
    COMMAND gcov -o CMakeFiles/test_ses_led.dir/lib/ses ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ses/ses_led.c
    DEPENDS run_tests
    COMMENT "Coverage report generated"
)

# Clean build
add_custom_target(clean_all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -rf CMakeFiles/ cmake_install.cmake CMakeCache.txt
    COMMENT "Full clean completed"
)
