TARGET = compiler

SRCS  = $(shell find ./src     -type f -name *.c)
HEADS = $(shell find ./include -type f -name *.h)

CC = gcc
CFLAGS = -I./include -Wall -ansi -pedantic -g

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	rm -rf $(TARGET)
