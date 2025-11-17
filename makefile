CC = gcc
CFLAGS = -Wall -O2
TARGET = w1

all: $(TARGET)

$(TARGET): w1.o
	$(CC) $(CFLAGS) -o $(TARGET) w1.o

w1.o: w1.c
	$(CC) $(CFLAGS) -c w1.c

clean:
	rm -f *.o $(TARGET)

run:
	./w1 stopwords.txt input.txt
