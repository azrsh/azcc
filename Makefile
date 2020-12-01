CFLAGS=-std=c11 -g -static -Wall

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/functional/*.c)
TESTS=$(TEST_SRCS:.c=.out)

TEST_TOOL_SRCS=$(wildcard test/tool/*.c)
TEST_TOOL_OBJS=$(TEST_TOOL_SRCS:.c=.o)

azcc: $(OBJS)
	$(CC) -o azcc $(OBJS) $(LDFLAGS)

$(OBJS): *.h

test/%.out: azcc $(TEST_TOOL_OBJS) test/%.c
	./azcc test/$*.c > test/$*.s
	cc -o $@ test/$*.s $(TEST_TOOL_OBJS)

test: $(TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

self: azcc
	cpp -I test/self/dummylib test/self/container.c > test/self/container.i
	./azcc test/self/container.i

test-old: azcc $(TEST_TOOL_OBJS)
	./test.sh

clean:
	rm -f azcc *.o *~ tmp* test/*.s test/*.out test/tool/*.o

.PHONY: test test-old clean
