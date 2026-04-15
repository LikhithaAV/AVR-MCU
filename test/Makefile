# Simple Makefile for building and running tests

CC = gcc
CFLAGS = -I../lib/ses -I./support -I./unity -Wall -Wextra -g
LDFLAGS = --coverage

# Source files
SOURCES = unit/test_ses_led.c ../lib/ses/ses_led.c support/mock_avr_io.c unity/unity.c
OUTPUT = test_led

.PHONY: all test clean

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCES) $(LDFLAGS)

test: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o *.gcda *.gcov

.DEFAULT_GOAL := all
