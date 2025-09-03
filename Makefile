CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = temple
SRC = temple.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)