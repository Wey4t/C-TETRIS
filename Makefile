CC = gcc
CFLAGS = -Wall -O2
LIBS = -lncurses
TARGET = tetris
SOURCES = main.c block.c window.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run