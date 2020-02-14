SRC = ${wildcard *.c}
OBJ = ${patsubst %.c, %.o, $(SRC)}

all : $(OBJ) 

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c  $^  $(LDFLAGS)

clean:
	rm *.o  

.PHONY:all clean

