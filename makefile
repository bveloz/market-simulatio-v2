CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = mSim

OBJ = mSim.o cJSON.o mytime.o mymath.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

mSim.o: mSim.c cJSON.h mytime.h mymath.h
	$(CC) $(CFLAGS) -c mSim.c

cJSON.o: cJSON.c cJSON.h
	$(CC) $(CFLAGS) -c cJSON.c

mytime.o: mytime.c mytime.h
	$(CC) $(CFLAGS) -c mytime.c

mymath.o: mymath.c mymath.h
	$(CC) $(CFLAGS) -c mymath.c

clean: 
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET) customers.json products.json

