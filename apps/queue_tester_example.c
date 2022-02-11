#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

<<<<<<< Updated upstream
	TEST_ASSERT(queue_create() != NULL);
=======
    if (*a == 42)
        queue_delete(q, data);
    else
        *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    (void)q; //unused
    if (*a == match)
        return 1;

    return 0;
}

static int printint(queue_t q, void *data, void *arg)
{
    (void)q;
    (void)arg;

    int *a = (int*) data;
    printf("%d\n", *a);
    return 0;
>>>>>>> Stashed changes
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

<<<<<<< Updated upstream
	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
=======
    /* Add value '1' to every item of the queue, delete item '42' */
    queue_iterate(q, inc_item, (void*)1, NULL);
    queue_iterate(q, printint, NULL, NULL);

    assert(data[0] == 2);
    assert(queue_length(q) == 9);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;     // result pointer *must* be reset first
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);

    assert(ptr != NULL);
    assert(*ptr == 5);
    assert(ptr == &data[3]);
>>>>>>> Stashed changes
}

int main(void)
{
	test_create();
	test_queue_simple();

	return 0;
}
