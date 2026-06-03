TARGET = chip8

CC = gcc
CC_FLAGS = -c -Wall

C_SRC = $(wildcard *.c)
H_SRC = $(wildcard *.h)
OBJ=$(C_SRC:.c=.o)

all: $(TARGET)

#linking
$(TARGET): $(OBJ)
	$(CC) -o $@ $^
	
%.o: %.c %.h
	$(CC) $(CC_FLAGS) -o $@ $<

clean:
	rm -rf *.o

