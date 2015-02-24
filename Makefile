CC=gcc
CC_FLAGS=-Wall
SRC_DIR=src
EXECUTABLE=ush

all:
	$(CC) $(CC_FLAGS) $(SRC_DIR)/ush.c -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
