CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = templeEX
SRC = templeEX.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)