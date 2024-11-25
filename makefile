CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = mSim

OBJ = mSim.o cJSON.o 

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

mSim.o: mSim.c cJSON.h 
	$(CC) $(CFLAGS) -c mSim.c

cJSON.o: cJSON.c cJSON.h
	$(CC) $(CFLAGS) -c cJSON.c


clean: 
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

