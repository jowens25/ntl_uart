CC = gcc
CFLAGS = -Wall -g

# Include your inc directory for header files
INC_DIRS = -I./inc

# Gather source files in main dir and subdirectories (./src)
SOURCES = $(wildcard *.c) $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = driver

PLATFORM ?= linux

ifeq ($(PLATFORM),linux)
CFLAGS += -DLINUX_PLATFORM
else
CFLAGS += -DMCU_PLATFORM
endif

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

.PHONY: clean build_and_clean

clean:
	rm -f $(TARGET) $(OBJECTS)
