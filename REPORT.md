# ECS 150: Project #2 - User-level thread library

Author: Yuhang Wang, Jie Shao

## queue API



I use 2 **struct** to create a **linked list** to represent the **FIFO** queue.

#### Time complexity

- enqueue and dequeue with linked list is simple. For enqueue, I just check and set the new element as the tail of the queue. For dequeue, I just pop the head. All operations are **O(1).**

- **O(1)** `queue_length` is bit tricky with linked list. I add a variable inside the struct to track the length of the queue. Whenever I enqueue or dequeue, I can count++ or count-- respectively.

  Finally, just use one line to return the variable in `queue_length` function.

- Create and destroy are simple **O(1)**. Delete and iterate are out of the scope of **O(1)**.

## uthread API 

### Abstract

We are provided with `context.c` and `private.h` to build a thread mamangement system. Users will use our API to create threads, run threads and yield threads. We can use the queue API to create and store threads, but how do we force a running thread to yield? The secret is using context switch from `context.c.` When you want to run a threads' function, you don't need to actually call the function. After initializing the context of threads' function, once we perform a **context switching,** the functioin will automatically continue.

### Setup

- thread representation

  ```c
  struct TCB {
      uthread_t TID;
      uthread_ctx_t context;
      int state;   // NORMAL, ZOMBIE, BLOCKED
      void* stack; 
      int retval;
      struct TCB* child;  // "child" to collect
      struct TCB* parent; // "parent" which is waiting you
  };
  ```

  - state:  `NORMAL` stands for threads which haven't return. `ZOMBIE` stands for orphaned threads needing to be joined. threads are `BLOCKED` after calling uthread_join();
  - `child` and `parent`: Threads joining someone need to free it after it dies. With the two members being stored, `uthread_join()` can access the "child" thread and free it in **O(1**).  

- I then create two queues as **global variables**, and a **global** `TCB` variable named `running_thread`, the thread currently exectuing.

### uthread_start

Registering the `main` as a thread with `TID = 0.` Noticing that the stack and `context` of `main` do not need to be initialized. Creating the `TCB` object is enough.

### uthread_create

Initializing threads' context and stack via functions from context.c. Seting the state of threads to `NORMAL` and enqueue to `ready_queue`.

### uthread_yield

- If the **length** of `ready_queu`e is 0, we know that it is the only thread which can running. We then don't perform the yield and simply do nothing, otherwise it will result in an **endless loop**.
- Before doing **context switching**, we also check the **state** of the thread which is calling yield. It is becuase both `uthread_join() `and simple thread can call `yield`. For `uthread_join()`, the thread state is `BLOCKED` and we don't enqueue this thread to the `ready_queue`. For `NORMAL`thread, the thread needs to run again after `yield`,thus we directly `enqueue` it to the `ready_queue`.
- When doing **context switching**, just **dequeue** the thread from `ready_queue` and call `uthread_ctx_switch`.

### uthread_exit

This function is called inside the `uthread_ctx_bootstrap` with argument of `func`.

- We have `retval` as a member inside the `TCB` struct. We then store the argument to the retval for joining thread to collect it. Then, setting the state to `ZOMBIE`.
- Then, there are two scenarios. 
  - Someone has already joined this thread, thus the thread's `parent` is not `NULL`.  Then, we can unblock the `parent` and `enqueue` the parent into the `ready_queue`.
  - No one joins it. Enqueueing the thread to `zombie_queue`.

- Finally, **yield**.

### Important: Uthread_join

- We have various error checking in this function, for exmaple: joining oneself, child joining parent, etc.

- Then, we need to find the thread to join from the two queues we created: `ready_queue` and `zombie_queue` by using `queue_iterate`.

  - If find in the ready_queue, the thread is still an active thread, we need to make sure this   thread does not have parent yet. `thread_to_join->parent != NULL`. Then, we register parent and child: `thread_to_join->parent = running_thread` , and 

    `running_thread->child = thread_to_join;`, **block** the parent and then `yield`. Doing collection and free resources after being unblocked later.

  - if find in the zombie_queue, we directly collect the thread and free it. 

## preemption

In this section, we need to set up a **virtual timer** which fire an alarm at **100Hz**, and a signal handler calling `uthread_yield` when receiving the alarm signal. We are required to use `setitimer` and `sigaction` to do so.

We need to have access to the **timer** as well as the **action** within five different functions. In addition, we should be able to restore the previous signal action and timer configuration. Thus, I create 4 global variables inside preempt.c.

```c
struct sigaction new_action;
struct sigaction old_action;
struct itimerval new_timer;
struct itimerval old_timer;
```

### **preempt_start**

- In order to setup sigaction, I include the `<string.h>` to use `memset`. 

- `new_timer` has type of `struct itimerval`. In order to achieve a 100Hz alarm, we need to change the members inside this struct. `it_interval` and `it_value` represents continuous alarm interval and time until the first alarm.  

- `tv_sec` represents whole seconds and `tv_nsec` represents nanoseconds.

  ```c
  new_timer.it_value.tv_sec = 0;
  new_timer.it_value.tv_usec = 10000;    // 100 Hz
  new_timer.it_interval.tv_sec = 0;
  new_timer.it_interval.tv_usec = 10000; // 100 Hz
  ```

- The point for this section is recording previous action and previous timer such that we can restore later.

  ```c
  sigaction(SIGVTALRM, &new_action, &old_action);
  setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);
  ```

### preempt_stop

- If we set the timer to 0 and then call setitimer again, the timer will stop.
  - Then, we need to call setitimer again with the old_timer we previously saved to restore it.
- To uninstall the sigaction, just set sa_handler to **`SIG_DFL`** and call sigaction.
  - Same procedure for sigaction, we need to call it again to restore to previous action.

### preempt enable and disable

The two functions can be realized with one simple line of code respectively.

- We have already set `sigset_t block;` as a global variable and initialize it in the **`preempt_start`** function.

- The following code means set `block` as an empty signal set and add the virtual alarm signal `SIGVTALRM` to it.

  ```c
  sigemptyset(&block);
  sigaddset(&block, SIGVTALRM);
  ```

- Finally, just call sigprocmask to block the signal. Ususally, it's unsafe to call sigprocmask with pthread library, but it's fine since we are building our own.

## preemtion testing

### sigaction and timer

1. I create three identical threads. each of them has an endless loop.

   ```c
   int thread1(void)
   {
       printf("first time 1\n");
       while(1);
       return 1;
   }
   ```

2. I then let main thread to join the thread 1, when it yields to thread1, if the timer and sigaction is working right, it should continuously yielding between the three threads, because they never end. I print a line inside the uthread_yield function, as well as the alarm_handler function.

   ```c
   // first one will be printed inside the uthread_yield function
   
   printf("tid: %hu wants to yield", running_thread->TID);
   ```

3. Here is the result, and I manully press `ctrl + c` to escape the endless loop.

   ```c
   yuhwang@ad3.ucdavis.edu@pc19:~/ecs150/uthread/apps$ ./tester.x
   begin to join thread1
   tid: 0 wants to yield
   signal received time: 1
   tid: 1 wants to yield
   signal received time: 2
   tid: 2 wants to yield
   signal received time: 3
   tid: 3 wants to yield
   signal received time: 4
   tid: 1 wants to yield
   signal received time: 5
   tid: 2 wants to yield
   signal received time: 6
   ...
   tid: 3 wants to yield
   signal received time: 55
   tid: 1 wants to yield
   signal received time: 56
   tid: 2 wants to yield
   ^C
   ```
   
   











