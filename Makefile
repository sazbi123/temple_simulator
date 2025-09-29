CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = templeEX
SRC = templeEX.c
TARGET2 = dat2bin
SRC2 = dat2bin.c
TARGET3 = asm2dat
SRC3 = asm2dat.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

$(TARGET3): $(SRC3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(SRC3)

clean:
	rm -f $(TARGET)
	rm -f $(TARGET2)
	rm -f mem.bin