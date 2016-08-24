CC = cc
#CC = gcc48
CFLAGS  = -g  -Wall -pedantic -O2
INCLUDES=-I/usr/include -I/usr/local/include
LDFLAGS=-L/usr/lib -L/usr/local/lib  

LOCAL_LIB_SRC=
DEFS=
LIBS=

LOCAL_LIB_SRC=

LOCAL_LIB_OBJ=$(LOCAL_LIB_SRC:.c=.o)

BIN_SRC=serialBump.c
BIN_OBJ=$(BIN_SRC:.c=.o)

BIN_EXEC=$(BIN_SRC:.c=)

all:  $(BIN_OBJ) $(LOCAL_LIB_OBJ) $(BIN_EXEC)


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(DEFS)

%: %.o $(LOCAL_LIB_OBJ) 
	$(CC) $(LDFLAGS) $(LIBS) $(DEFS) -o $@ $^

clean:
	rm -f $(BIN_EXEC) $(BIN_OBJ) $(LOCAL_LIB_OBJ)

