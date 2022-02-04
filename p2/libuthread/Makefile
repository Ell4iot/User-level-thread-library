# Target library
targets:= libuthread
lib := libuthread.a
CC := gcc
CFLAGS := -Wall -Werror -Wextra
objs := queue.o
AR := ar rcs
all: $(targets)
	
libuthread: $(objs)
	$(AR) $(lib) $^

%.o: %.c %.h
	$(CC) -c $< -o $@

clean:
	rm -f $(lib) $(objs)
## TODO 
