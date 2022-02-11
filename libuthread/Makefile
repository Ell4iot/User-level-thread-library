# Target library
targets:= libuthread
lib := libuthread.a
CC := gcc
CFLAGS := -Wall -Werror -Wextra -MMD
objs := queue.o context.o uthread.o preempt.o
AR := ar rcs
all: $(targets)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

ifneq ($(V),1)
Q=@
endif

libuthread: $(objs)
	@echo "AR $@"
	$(Q)$(AR) $(lib) $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps)
## TODO 
###TEST COMMENT
