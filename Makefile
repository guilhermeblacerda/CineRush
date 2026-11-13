
CC = gcc

RAYLIB_PATH = C:/raylib/raylib

CFLAGS = -Wall -Wextra -std=c99 -I$(RAYLIB_PATH)/src

LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(shell find src -name "*.c")

OBJ = $(SRC:.c=.o)

TARGET = CineRush.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	del /Q $(OBJ) $(TARGET) 2>NUL

.PHONY: all run clean
