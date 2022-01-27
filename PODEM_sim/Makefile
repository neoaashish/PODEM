CC              = cc
SRC             = main.c input.c user.c
TARGET          = project
OBJ             = $(SRC:.c=.o)
DDDEBUG         =
#------------------------------------------------------
$(TARGET): $(OBJ)
	$(CC)  -o $@ $(OBJ) -lm

$(OBJ): $(SRC)
	$(CC) -g -c $(SRC) $(DDDEBUG)

clean:
	rm -f *.o *~ $(TARGET)
