# Target library
targets:= libuthread
lib := libuthread.a
CC := gcc
CFLAGS := -Wall -Werror -Wextra
objs := queue.o context.o uthread.o preempt.o
AR := ar rcs
all: $(targets)
	
libuthread: $(objs)
	$(AR) $(lib) $^

%.o: %.c 
	$(CC) -c $< -o $@

clean:
	rm -f $(lib) $(objs)
## TODO 
###TEST COMMENT
