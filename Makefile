CC = gcc
CFLAGS = -Wall -Wextra -Werror
OBJ = fileManager.o commands.o helpers.o
TARGET = fileManager

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

all: clean $(TARGET)
