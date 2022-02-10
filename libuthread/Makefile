# Target library
targets:= libuthread
lib := libuthread.a
CC := gcc
CFLAGS := -Wall -Werror -Wextra
objs := queue.o context.o uthread.o preempt.o
AR := ar rcs
all: $(targets)
	
libuthread: $(objs)
	@echo "AR $@"
	@$(AR) $(lib) $^

%.o: %.c
	@echo "CC $@"
	@$(CC) -c $< -o $@

clean:
	rm -f $(lib) $(objs)
## TODO 
###TEST COMMENT
