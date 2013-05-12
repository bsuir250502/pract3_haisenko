OBJ = main.o nonogram.o
CFLAGS = -Wall -std=c99 -lpthread `pkg-config --cflags --libs cairo gtk+-3.0`

all: bin

bin: $(OBJ)
	gcc $(CFLAGS) $(OBJ) -o bin

%.o : %.c
	gcc $(CFLAGS) -c $<

clean:
	rm bin $(OBJ)
