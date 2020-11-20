CFLAGS=-std=c11 -g -static

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.out)

TEST_TOOL_SRCS=$(wildcard test/tool/*.c)
TEST_TOOL_OBJS=$(TEST_TOOL_SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): *.h

test/%.out: 9cc $(TEST_TOOL_OBJS) test/%.c
	./9cc test/$*.c > test/$*.s
	cc -o $@ test/$*.s $(TEST_TOOL_OBJS)

test: $(TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

test-old: 9cc $(TEST_TOOL_OBJS)
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp* test/*.s test/*.out test/tool/*.o

.PHONY: test test-old clean
