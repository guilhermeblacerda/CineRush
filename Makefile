
CC = C:/raylib/w64devkit/bin/gcc.exe

RAYLIB_PATH = C:/raylib/raylib

CFLAGS = -Wall -Wextra -std=c99 -I$(RAYLIB_PATH)/src

LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(wildcard src/*.c)

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
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean
